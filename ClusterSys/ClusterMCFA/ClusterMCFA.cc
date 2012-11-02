//
// This program is free software: you can redistribute it and/or
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 
/** STL */
#include <cmath>
#include <string.h>

/** Omnet MiXiM */
#include <Coord.h>
#include <SimpleNetwLayer.h>
#include <BaseNetwLayer.h>
#include <Packet.h>
#include <BaseMobility.h>
#include <AddressingInterface.h>

/** My Application */
#include "ClusterMCFA.h"
//#include "MCFANode.h"
#include "MCFAAutomata.h"

Define_Module(ClusterMCFA);

void ClusterMCFA::initialize(int stage) {

    ClusterManager::initialize(stage);
    if (stage == 0) {
        //Event System timer
        delayTimer = new cMessage("delay-timer", SEND_ASFREQ);

        resetTimer = new cMessage("reset-timer", RESET);

        pollingTimer = new cMessage("polling-timer", POLL);

        //Parameters
        //Tempo para esperar por formar o ActionSet
        asfreqTime  = par("asfreqTime"); //Receive Asfreq

        resetTime   = par("resetTime");

        rermTimeout = par("rermTimeout");

        pollingTime = par("pollingTime");

        gpsInterval = par("gpsInterval");

        //Se chegar nessa probabilidade para o cluster
        P           = par("ChooseProb"); //Receive Probability Threshold to stop Cluster Formation

        // Parametro de Reforco
        double rewardP = par("rewardP");

        // Parametro de Punicao
        double penaltyP = par("penaltyP");

        //Parametro T dinamico
        T = 0;

        clusterNodeState = UNDEFINED;

        /** Init Node Type */
        setCurrentRole(UNDEFINED_NODE);
        currStage = 0;

        //Inicializa o Automato
        Automata = new MCFAAutomata(rewardP, penaltyP);

        ch = 0;
        WATCH(ch);
        WATCH(P);
        WATCH(T);
        WATCH(w);

    } else if (stage == 1) {

        debugEV << "Meu App Addr e:" << myAddress << endl;
        //seta o owner do automata
        Automata->setMyID(myAddress);
        //DelayTimer inicial
        double schedtime = dblrand() + 4;
        scheduleAt(simTime() + schedtime, delayTimer);
        WATCH(myAddress);

        char tt[20];
        sprintf(tt, "MyID: %d", myAddress);
        setTTString(tt);

        //sendMobInfo
        /*Criando GPS */
        BaseMobility* mob = FindModule<BaseMobility*>::findSubModule(
                findHost());
        Coord HostCoor = mob->getCurrentPosition();
        gps.updatePos(&HostCoor);

        novoGPS = 0;
        sendMobTimer = new cMessage("sendMob-timer", UPDATE_POSITION);
        scheduleAt(simTime() + 2, sendMobTimer);

    }
}

ClusterMCFA::~ClusterMCFA() {

    cancelAndDelete(delayTimer);
    cancelAndDelete(pollingTimer);
    cancelAndDelete(resetTimer);
}

void ClusterMCFA::handleReset(cMessage *msg) {
    //Reseting all
    debugEV << "Reseting  " << endl;

    /** Init Node Type */
    setCurrentRole(UNDEFINED_NODE);
    clusterNodeState = UNDEFINED;
    currStage = 0;

    bubble("Reseting");
    //Display String
    char* tt = new char(20);
    sprintf(tt, "MyAddr is %i", myAddress);
    setTTString(tt);


}

void ClusterMCFA::finish() {
}

//Own App Self Message
void ClusterMCFA::handleSelfMsg(cMessage *msg) {
    switch (msg->getKind()) {
    case POLL: {
        handlePolling(msg);
    }
        break;

    case RESET: {
        handleReset(msg);
    }
    case UPDATE_POSITION: {

        BaseMobility* mob = FindModule<BaseMobility*>::findSubModule(
                findHost());

        Coord HostCoor = mob->getCurrentPosition();

        debugEV << "Minha nova posicao e " << HostCoor.x << "," << HostCoor.y
                << "\n";
        debugEV << "Minha ultima posicao e " << gps.getCoor().x << ","
                << gps.getCoor().y << "\n";
        /** Verificando se temos um EPOCH */
        double lastSpeed = gps.getSpeed();
        double lastDirection = gps.getDirection();
        gps.updatePos(&HostCoor);
        if (((lastSpeed != gps.getSpeed())
                || (lastDirection != gps.getDirection()))
                && clusterNodeState > ACTSETFORM) {
            debugEV << "Mudei de Direcao ou velocidade " << endl;
            novoGPS = 1;
        }
        debugEV << "Minha Velcidade e " << gps.getSpeed() << "\n";
        debugEV << "Minha Direcao e " << gps.getDirection() << "\n";

        if (novoGPS > 0) {
            novoGPS = 0;
            debugEV << "Enviando Mobile Info" << endl;
            sendMobInfo();
        }
        cancelEvent(sendMobTimer);
        scheduleAt(simTime() + 4, sendMobTimer);
        //cancelAndDelete(sendMobTimer);
    }
        break;
    case SEND_ASFREQ: {
        debugEV << "Enviando ASFREQ:" << myAddress << endl;
        //Criando Pacote ASFREQ
        ClusterMCFAPkt *pkt = new ClusterMCFAPkt("BROADCAST: ASFREQ",
                MCFA_CTRL);
        //ASFREQ Message
        pkt->setMsgtype(MCFA_ASFREQ);
        pkt->setOriginId(myAddress);
        sendBroadcast(pkt);
        //Limpando AS
        Automata->clearAS();
        //Agenda Mudanca de estado
        debugEV << "Agendando Formacao em :" << asfreqTime << endl;
        cancelAndDelete(delayTimer);
        delayTimer = new cMessage("delay-timer", INIT_MCFA);
        clusterNodeState = INITIALIZING;
        scheduleAt(simTime() + asfreqTime, delayTimer);
    }
        break;
    case INIT_MCFA: {
        //Append me as an action
        double ret = Automata->addAction(myAddress, getMobInfo(), getMobInfo());
        debugEV << "RM:" << ret << endl;
        cancelAndDelete(delayTimer);
        //Call Cluster Formation at stage and wait for next stage
        currStage = 0;
        clusterNodeState = HEADSELECT;
        double myp = Automata->initProb();
        debugEV << "P inicial de: " << myp <<  " Nodes " << Automata->getDegree() << endl;
        MCF();
        delayTimer = new cMessage("PROC_MCFA", PROC_MCFA2);
        scheduleAt(simTime() + rermTimeout, delayTimer);
    }
        break;
    case PROC_MCFA:{
        debugEV << "PROC_MCFA" << endl;
        MCF();
        if (currStage < 2)
            scheduleAt(simTime() + rermTimeout, delayTimer);
        }
    break;
    case PROC_MCFA2:{
        debugEV << "------RESET RERM ---" <<endl;
        currStage=0;
        cancelAndDelete(delayTimer);
        delayTimer = new cMessage("PROC_MCFA2", PROC_MCFA);
        scheduleAt(simTime() , delayTimer);
    }
    break;
    }
}

// Message from lower layers

void ClusterMCFA::handleNetlayerMsg(cMessage *msg) {

    ClusterManager::handleNetlayerMsg(msg);
    switch (msg->getKind()) {
    case MCFA_CTRL: {
        ClusterMCFAPkt *m;
        m = static_cast<ClusterMCFAPkt *>(msg);
        handleMCFAControl(m);
        EV << "Publishing Handle MEssage" << endl;
        //Contabilizando pacotes enviados
        emit(rxMessageSignal, 1);
        delete msg;
        msg = 0;

    }
        break;
    }

}

void ClusterMCFA::handleMCFAControl(ClusterMCFAPkt *m) {
    int msgType = m->getMsgtype();
    debugEV << "Received Msg Type " << msgType << " From: " << m->getSrcAddr()
            << endl;
    switch (msgType) {
    case MCFA_ASFREQ:
        debugEV << "Recebi uma ASFREQ de " << m->getSrcAddr() << endl;
        if (m->getDestAddr() != myAddress) {
            debugEV << "Respondendo ASFREQ: De:" << myAddress << " --> "
                    << m->getSrcAddr() << endl;
            ClusterMCFAPkt *pkt = new ClusterMCFAPkt("DirectCast ASFREP",
                    MCFA_CTRL);
            pkt->setMsgtype(MCFA_ASFREP);
            pkt->setDirection(gps.getDirection());
            pkt->setSpeed(gps.getSpeed());
            sendDirectMessage(pkt, m->getSrcAddr());
        }
        break;
    case MCFA_ASFREP: {
        debugEV << "Recebi um ASFREP de " << m->getSrcAddr() << endl;
        MobInfo *mi = new MobInfo();
        mi->direction = m->getDirection();
        mi->speed = m->getSpeed();
        debugEV << "Adicionando Action set do host " << m->getSrcAddr() << endl;
        double ret = Automata->addAction(m->getSrcAddr(), mi, getMobInfo());
        debugEV << "RM: " << ret << endl;
        //Atualizando nome
        char buf[20];
        sprintf(buf, "MyAddr: %i\nERMt: %f", getAddress(), Automata->getERMt());
        setTTString(buf);
    }
        break;
    case MCFA_RERM: {
        debugEV << "Recebi um RERM de " << m->getSrcAddr() << " Meu ch eh "
                << ch << " w: " << m->getERM() << endl;
        if (m->getSrcAddr() == ch && currStage == 1) { //se veio do meu ch eu volto para a construcao do cluster
            debugEV << "Retorno de RERM" <<endl;
            cancelAndDelete(delayTimer);
            w = m->getERM();
            MCF();
            delayTimer = new cMessage("delay-timer", PROC_MCFA2);
            scheduleAt(simTime() + 2, delayTimer);
        } else {
            debugEV << "Respondendo RERM: De:" << myAddress << " --> "
                    << m->getSrcAddr() << endl;
            ClusterMCFAPkt *pkt = new ClusterMCFAPkt("DirectCast RERM",
                    MCFA_CTRL);
            pkt->setMsgtype(MCFA_RERM);
            pkt->setERM(Automata->getERMt());
            sendDirectMessage(pkt, m->getSrcAddr());
        }
    }
        break;
        //Se recebe um CHSEL tranforma-se em HEAD e adiciona o sender ao childlist
    case MCFA_CHSEL: {
        debugEV << "Recebi um CHSEL de " << m->getSrcAddr() << endl;
        childs.push_back(m->getSrcAddr());
        setHeadAddress(myAddress);
        setCurrentRole(HEAD_NODE);
        cancelEvent(delayTimer);
    }
        break;
    case MCFA_MOBINFO: {
        debugEV << "Recebi um MOBINFO de " << m->getSrcAddr() << endl;
        MobInfo *mi = new MobInfo();
        mi->direction = m->getDirection();
        mi->speed = m->getSpeed();
        debugEV << "Atualizando Action set do host " << m->getSrcAddr() << endl;
        Automata->newEpoch(m->getSrcAddr(), mi, getMobInfo());
    }
        break;
    default:
        EV << "MESSAGE NOT HANDLED !!!!!!!!" << endl;
        break;

    }
}

int ClusterMCFA::MCF() {
    if (currStage == 0) {
        debugEV << "----------INICIANDO MCF------------" << endl;
        ch = Automata->randNeigh();
        //Selected CH is me, set w as my ERM and go to stage 2
        if (ch == myAddress) {
            w = Automata->getERMt();
            currStage = 1;
            debugEV << "Eu sou o CH" << endl;
        } else { //Selected CH is another host, ask for ERM and go to stage 1
            debugEV << "Enviando RERM para" << ch << endl;
            ClusterMCFAPkt *pkt = new ClusterMCFAPkt("DirectCast RERM",
                    MCFA_CTRL);
            pkt->setMsgtype(MCFA_RERM);
            sendDirectMessage(pkt, ch);
            currStage = 1;
            return 1;
        }
    }
    debugEV << "w de ch(" << ch << "):" << w << " T: " << T << endl;
    if (w <= T) {
        debugEV << "Reforco em ch " << ch << endl;
        debugEV << "Atual " << Automata->getProbability(ch) << endl;
        Automata->reward(ch);
        debugEV << "Novo " << Automata->getProbability(ch) << endl;
    } else {
        debugEV << "Punicao em ch " << ch << endl;
        debugEV << "Atual " << Automata->getProbability(ch) << endl;
        Automata->penalize(ch);
        debugEV << "Novo " << Automata->getProbability(ch) << endl;
    }
    ERMi[ch] = w;
    double ERMk = 0;
    debugEV << "ERMi[ch]" << ERMi[ch] << endl;
    for (std::map<int, double>::iterator it = ERMi.begin(); it != ERMi.end();
            it++) {
        debugEV << "it = " << (*it).second << endl;
        ERMk += (*it).second;
    }
    T = ERMk / (Automata->getDegree() + 1);
    debugEV << "Novo ERMk eh: " << ERMk << endl;

    //T = ((T*stageK)+Automata->getERMt()/stageK);
    stageK++;

    if (Automata->getProbability(ch) < P) {
        debugEV << "Probabilidade de ch(" << ch << "): "
                << Automata->getProbability(ch) << " P: " << P << endl;
        currStage = 0;
    } else {
        debugEV << "Probability" << Automata->getProbability(ch) << "Threshold"
                << P << endl;
        if (ch == myAddress) {
            setCurrentRole(HEAD_NODE);
            setHeadAddress(ch);
        } else {
            sendCHSEL(ch);
            setCurrentRole(CHILD_NODE);
            setHeadAddress(ch);

        }
        currStage = 2;
    }
    return 1;
}

MobInfo* ClusterMCFA::getMobInfo() {
    MobInfo *mi = new MobInfo;
    mi->direction = gps.getDirection();
    mi->speed = gps.getSpeed();
    return mi;
}

void ClusterMCFA::sendMobInfo() {
    debugEV << "Enviando novo MOBINFO" << endl;
    ClusterMCFAPkt *pkt = new ClusterMCFAPkt("BROADCAST: MOBINFO", MCFA_CTRL);
    pkt->setMsgtype(MCFA_MOBINFO);
    pkt->setDirection(gps.getDirection());
    pkt->setSpeed(gps.getSpeed());
    pkt->setOriginId(myAddress);
    sendBroadcast(pkt);

}
void ClusterMCFA::sendCHSEL(int NodeAddr) {
    debugEV << "ENVIANDO CHSEL: De:" << myAddress << " --> " << NodeAddr
            << endl;
    ClusterMCFAPkt *pkt = new ClusterMCFAPkt("DirectCast CHSEL", MCFA_CTRL);
    pkt->setMsgtype(MCFA_CHSEL);
    pkt->setERM(Automata->getERMt());
    sendDirectMessage(pkt, NodeAddr);
}

