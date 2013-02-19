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
//#include "utilities.h"

Define_Module(ClusterMCFA);

void ClusterMCFA::initialize(int stage) {

    ClusterManager::initialize(stage);
    if (stage == 0) {
        //Event System timer
        delayTimer = new cMessage("delay-timer", SEND_ASFREQ);

        resetTimer = new cMessage("reset-timer", RESET);

        reinitTimer = new cMessage("reinit-MFC", PROC_MCFA2);

        //Parameters

        //pollingTimer = new cMessage("polling-timer", CLUSTER_BASE_POLL);
        pollingTimer = new cMessage("polling-timer", POLL);
        pollingTime = par("pollingTime");

        //Node Timeout
        int timeout = par("timeout");
        setNodeTimeout(timeout);
        //Tempo para esperar por formar o ActionSet
        asfreqTime  = par("asfreqTime");

        //Receive Asfreq
        resetTime   = par("resetTime");

        rermTimeout = par("rermTimeout");

        gpsInterval = par("gpsInterval");

        //Se chegar nessa probabilidade para o cluster
        P           = par("ChooseProb"); //Receive Probability Threshold to stop Cluster Formation

        // Parametro de Reforco
        double rewardP = par("rewardP");

        // Parametro de Punicao
        double penaltyP = par("penaltyP");

        /**
         * MERGE
         */

        mergeMobility = par("MergeMobility");

        mergeTime = par("MergeTime");

        mergeEnable = par("MergeEnable");



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
        double schedtime = 3.5 + dblrand()*2;

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
        scheduleAt(simTime() + 1 + dblrand()*2, sendMobTimer);


    }
}


void ClusterMCFA::finish() {
}

ClusterMCFA::~ClusterMCFA() {
    cancelAndDelete(reinitTimer);
    cancelAndDelete(delayTimer);
    cancelAndDelete(pollingTimer);
    cancelAndDelete(resetTimer);
}


/* **********************************************************
 *
 *  Tratamento de mensagens do proprio modulo
 *
 ***********************************************************/
void ClusterMCFA::handleSelfMsg(cMessage *msg) {
    switch (msg->getKind()) {
    case POLL: {
        handlePolling(msg);
        debugEV << "Reagendando polling para " << simTime().dbl() + pollingTime << endl;
        scheduleAt( simTime() + pollingTime , pollingTimer);
    }
    break;
    case RESET: {
        handleReset(msg);
    }
    break;
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
        debugEV << "Meu estado atual" << clusterNodeState << endl;
        debugEV << "LastCourse: " << lastDirection << " currentCourse " << gps.getDirection() << "\n";
        debugEV << "LastSpeed: " << lastSpeed << " currentSpeed: " << gps.getSpeed() << "\n";
        if (((lastSpeed != gps.getSpeed())
                || (lastDirection != gps.getDirection()))
                && clusterNodeState > ACTSETFORM) {
            debugEV << "Mudei de Direcao ou velocidade " << endl;
            novoGPS = 1;
        }
        debugEV << "Minha Velcidade e " << gps.getSpeed() << "\n";
        debugEV << "Minha Direcao e " << gps.getDirection() << "\n";

        if ((novoGPS > 0) || (lastsend < (simTime() - (getNodeTimeout()/2)))) {
            novoGPS = 0;
            debugEV << "Enviando Mobile Info" << endl;
            sendMobInfo();
            /*
            if(getCurrentRole() == HEAD_NODE && mergeEnable){
                sendCLINFO();
            }else
            {
                debugEV << "SEMHEAD" <<endl;
            }*/
            if(clusterNodeState==HEADSELECT and currStage == 0){
                MCF();
            }

            //Envio Update para todo mundo de tempos em tempos, a menos que envie aqui
            //cancelEvent(pollingTimer);
            //scheduleAt(simTime()+pollingTime,pollingTimer);
        }
        cancelEvent(sendMobTimer);
        scheduleAt(simTime() + gpsInterval, sendMobTimer);


        //cancelAndDelete(sendMobTimer);
    }
        break;
    case SEND_JREQ:{
        headCandidate = -1;
        candidateERM = 16000;
        debugEV << "Enviando JREQ:" << myAddress << endl;
        //Criando Pacote ASFREQ
        ClusterMCFAPkt *pkt = new ClusterMCFAPkt("BROADCAST: JREQ",
                MCFA_CTRL);
        //ASFREQ Message
        pkt->setMsgtype(MCFA_JREQ);
        pkt->setOriginId(myAddress);
        sendBroadcast(pkt);
        //Limpando AS
        cancelEvent(reinitTimer);
        cancelAndDelete(delayTimer);
        cancelEvent(pollingTimer);
        Automata->clearAS();
        debugEV << "ClearAS" << endl;
        //Agenda Mudanca de estado
        debugEV << "Agendando Definicao Join para : " << simTime()+asfreqTime << endl;

        delayTimer = new cMessage("delay-timer", RESOLV_JOIN);
        clusterNodeState = JOIN;
        scheduleAt(simTime() + asfreqTime, delayTimer);
    }
    break;
    case RESOLV_JOIN:{
        if(headCandidate < 0){
            debugEV << "Nao tenho candidatos, agendando formacao  " << endl;
            cancelAndDelete(delayTimer);
            delayTimer = new cMessage("delay-timer", GET_RERM);
            clusterNodeState = ACTSETFORM;
            scheduleAt(simTime(), delayTimer);
        }else{
            debugEV << "Encontrei um Head  " << endl;
            sendCHSEL(headCandidate);
            setCurrentRole(CHILD_NODE);
            setHeadAddress(ch);
            clusterNodeState = RUNNING;
        }
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
        debugEV << "ClearAS" <<endl;
         //Agenda Mudanca de estado
        debugEV << "Agendando Formacao em :" << asfreqTime << endl;
        cancelAndDelete(delayTimer);
        delayTimer = new cMessage("delay-timer", GET_RERM);
        clusterNodeState = ACTSETFORM;
        scheduleAt(simTime() + asfreqTime, delayTimer);
    }
        break;
    case INIT_MCFA: {
        debugEV << "INIT_MCFA" << endl;
        //Append me as an action
        MobInfo *mi = getMobInfo();
        MobInfo *mj = getMobInfo();
        double ret = Automata->addAction(myAddress, mi, mj);
        delete mi;
        delete mj;
        //FIXME
        //ERMi[myAddress] = Automata->getERMt();
        Automata->setERMi(myAddress,Automata->getERMt());
        debugEV << "myAddress " << myAddress << " ERMt " << Automata->getERMi(myAddress) << endl;
        debugEV << "RM:" << ret << " ERMt:" << Automata->getERMt() << endl;
        cancelAndDelete(delayTimer);
        //Call Cluster Formation at stage and wait for next stage
        currStage = 0;
        clusterNodeState = HEADSELECT;
        double myp = Automata->initProb();
        debugEV << "P inicial de: " << myp <<  " Nodes " << Automata->getDegree() << endl;

        T = Automata->getT();
/*
 * Agendamento de PROC_MCFA
 */
        delayTimer = new cMessage("PROC_MCFA", PROC_MCFA);
        scheduleAt(simTime(), delayTimer);
        cancelEvent(pollingTimer);
        debugEV << "Agendando polling para " << simTime().dbl() + pollingTime << endl;
        scheduleAt(simTime() + pollingTime, pollingTimer);

    }
    break;
    case GET_RERM:{
        //Monta o ERMt
            lastsend = simTime();
            debugEV << myAddress <<" enviando RERM BroadCAST" << endl;
            ClusterMCFAPkt *pkt = new ClusterMCFAPkt("DirectCast RERM",
                    MCFA_CTRL);
            pkt->setQuestion(1);
            pkt->setMsgtype(MCFA_RERM);
            sendBroadcast(pkt);
            cancelAndDelete(delayTimer);
            delayTimer = new cMessage("delay-timer", INIT_MCFA);
            clusterNodeState = INITIALIZING;
            scheduleAt(simTime() + 4, delayTimer);
    }
    break;
    case PROC_MCFA:{
        debugEV << "PROC_MCFA" << endl;
        MCF();

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

/*
 * RESET
 */
void ClusterMCFA::handleReset(cMessage *msg) {
    //Reseting all
    debugEV << "Reseting  " << endl;

    /** Init Node Type */
    setCurrentRole(UNDEFINED_NODE);
    clusterNodeState = UNDEFINED;
    currStage = 0;
    if (ev.isGUI()) {
    findHost()->bubble("Reseting");
    }
    //Display String
    char* tt = new char(20);
    sprintf(tt, "MyAddr is %i", myAddress);
    setTTString(tt);


    cancelAndDelete(delayTimer);
    delayTimer = new cMessage("Join", SEND_JREQ);
    scheduleAt(simTime(),delayTimer);

}

/*
 * Pollings
 */
void ClusterMCFA::HeadPolling(cMessage *msg){
    BroadPing();
    //sendMobInfo();
    nodeGarbageCollector();
}
//Tratamento de polling Child, vou ver se tenho Pai, quem s‹o os meus vizinhos
void ClusterMCFA::ChildPolling(cMessage *msg){
    debugEV << "Handle child Polling" << endl;
    //sendMobInfo();
    nodeGarbageCollector();

}
void ClusterMCFA::UndefinedPolling(cMessage *msg){
    //sendMobInfo();
    nodeGarbageCollector();
}



/* **********************************************************
 *
 *  Tratamento de mensagens de outros modulos
 *
 ***********************************************************/

void ClusterMCFA::handleNetlayerMsg(cMessage *msg) {

    ClusterManager::handleNetlayerMsg(msg);
    switch (msg->getKind()) {
    case MCFA_CTRL: {
        ClusterMCFAPkt *m;
        m = static_cast<ClusterMCFAPkt *>(msg);
        handleMCFAControl(m);
        EV << "Publishing Handle MEssage" << endl;
        //Contabilizando pacotes enviados
        //emit(rxMessageSignal, 1);
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
    case MCFA_JREP:{
        debugEV << "Recebi um JREP" << endl;
        if (m->getERM() < candidateERM){
            headCandidate = m->getSrcAddr();
            candidateERM = m->getERM();
            debugEV << "Novo candidado" << headCandidate << endl;
        }
    }
    break;
    case MCFA_JREQ:{
        debugEV << "Recebi um MCFA_JREQ de " << m->getSrcAddr() << endl;
        if(getCurrentRole() == HEAD_NODE){
            debugEV << "Respondendo JREQ: De:" << myAddress << " --> "
                    << m->getSrcAddr() << endl;
            ClusterMCFAPkt *pkt = new ClusterMCFAPkt("DirectCast JREP",
                    MCFA_CTRL);
            pkt->setMsgtype(MCFA_JREP);
            pkt->setERM(Automata->getERMt());
            sendDirectMessage(pkt, m->getSrcAddr());
        }
    }
    break;
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
        debugEV << "Recebi speed:" << mi->speed << " direction:" << mi->direction <<endl;
        debugEV << "Eu tenho speed:" << getMobInfo()->speed << " direction:" << getMobInfo()->direction <<endl;
        MobInfo *mj = getMobInfo();
        double ret = Automata->addAction(m->getSrcAddr(), mi, mj);
        delete mi;
        delete mj;
        debugEV << "RM: " << ret <<  " ERM" << Automata->getERMt() << endl;
        //Atualizando nome
        char buf[20];
        sprintf(buf, "MyAddr: %i\nERMt: %f", getAddress(), Automata->getERMt());
        setTTString(buf);
        //delete mi;
    }
        break;
    case MCFA_RERM: {
        debugEV << "Recebi um RERM de " << m->getSrcAddr() << " Meu ch eh "
                << ch << " w da mensagem: " << m->getERM() << endl;

        if (m->getQuestion() == 1) {
            //Isso é um pedido de RERM

            debugEV << "Respondendo RERM: De:" << myAddress << " --> "
                    << m->getSrcAddr() << endl;
            ClusterMCFAPkt *pkt = new ClusterMCFAPkt("DirectCast RERM",
                    MCFA_CTRL);
            updateSeen(m->getSrcAddr());
            pkt->setMsgtype(MCFA_RERM);
            pkt->setQuestion(0);
            //Se eu for CHILD eu saio fora
            if(getCurrentRole() == CHILD_NODE){
                debugEV << "Informando desistencia de HEAD " << endl;
                pkt->setQuestion(2);
            }
            debugEV << "Enviando ERM de " << Automata->getERMt() << endl;
            pkt->setERM(Automata->getERMt());

            sendDirectMessage(pkt, m->getSrcAddr());
        } else {
            //Isso é um retorno

            if (m->getSrcAddr() == ch && currStage == 1) { //se veio do meu ch eu volto para a construcao do cluster
                debugEV << "Retorno de RERM" << endl;
                //cancelAndDelete(delayTimer);
                cancelEvent(reinitTimer);

                //SE chegou mensagem, o sujeito está vivo
                updateSeen(m->getSrcAddr());

                //Pego o ERMt da mensagem
                w = m->getERM();

                /*************
                 *
                 * RETORNO para o estagio 1 do MCFA
                 *
                 *************/
                MCF();
            }
            else {
                debugEV << "Atualizando ERMt\n";
                int id = m->getSrcAddr();
                w = m->getERM();
                //ERMi[id] = w;
                Automata->setERMi(id, w);
            }
        }
    }
        break;
        //Se recebe um CHSEL tranforma-se em HEAD e adiciona o sender ao childlist
    case MCFA_CHSEL: {
        debugEV << "Recebi um CHSEL de " << m->getSrcAddr() << endl;
        addChild(m->getSrcAddr());
        setHeadAddress(myAddress);
        setCurrentRole(HEAD_NODE);
        clusterNodeState = RUNNING ;
        cancelEvent(delayTimer);
    }
        break;
    case MCFA_MOBINFO: {
        debugEV << "Recebi um MOBINFO de " << m->getSrcAddr() << endl;
        MobInfo *mi = new MobInfo();
        MobInfo *mj = getMobInfo();
        mi->direction = m->getDirection();
        mi->speed = m->getSpeed();
        debugEV << "Atualizando Action set do host " << m->getSrcAddr() << endl;
        if(getCurrentRole() == HEAD_NODE && mergeEnable){
            updateSeen(m->getSrcAddr() );
            if(m->getHeadId() == m->getSrcAddr()){
                debugEV << "Recebi uma mensagem de HEAD" <<endl;
                MobInfo *mi = new MobInfo();
                MobInfo *mj = getMobInfo();
                mi->direction = m->getDirection();
                mi->speed = m->getSpeed();
                double ClusterRM = MCFAAutomata::RM(mi,mj);
                debugEV << "MERGE: RM: " <<ClusterRM <<endl;
                if(ClusterRM < mergeMobility){
                    debugEV << "MERGE: RMOK " <<endl;
                    if(checkOverlap(str2intList(m->getChildinfo()))){
                        debugEV << "MERGE: OVERLAP"<<endl;
                       debugEV << "Meu ERM: " << Automata->getERMt() << " ERM Remoto: " <<m->getERM() << endl;
                       if (Automata->getERMt() >  m->getERM())
                           clusterMerge(m->getSrcAddr());
                    }
                }
            }
        }

        /* Modo original do MCF
         *
         */
        debugEV << "ERM atual " << Automata->getERMt() <<endl;
        if(clusterNodeState == HEADSELECT ) {
            debugEV << "Estou em HEADSELECT atualizando EPOCH" << endl;
            MCF();
            if(Automata->ActionExists(m->getSrcAddr())){
                debugEV << "Existe a Acao, vou atualizar" <<endl;
                Automata->newEpoch(m->getSrcAddr(), mi, mj);
            }
        }else{
            debugEV << "Estou em ASFREQ criando EPOCH" << endl;
            Automata->newEpoch(m->getSrcAddr(), mi, mj);
        }
        debugEV << "ERM novo " << Automata->getERMt() <<endl;
        delete mi;
        delete mj;

    }
        break;
    case MCFA_LREQ:{
        debugEV << "RECEBI UM LREQ!!! RESETING " <<endl;
        Automata->removeAction(m->getSrcAddr());
        if(m->getHeadId() == getHeadAddress()){
            cancelAndDelete(delayTimer);
            delayTimer = new cMessage("reseting", RESET);
            handleReset(delayTimer);
        }
    }
    break;
    default:
        EV << "MESSAGE NOT HANDLED !!!!!!!!" << endl;
        break;

    }
}
bool ClusterMCFA::checkOverlap(std::vector<int> rChilds){
    for(std::vector<int>::iterator it = rChilds.begin(); it != rChilds.end();it++){
        if(! Automata->ActionExists(*it))
            return false;
    }
    return true;
}

void ClusterMCFA::handlePingMsg(ClusterPkt *msg) {
    int msgType = msg->getMsgtype();
    updateSeen(msg->getSrcAddr());
    switch (getCurrentRole()) {
    case CHILD_NODE: {
        if (msg->getHeadId() != getHeadAddress())
            return;
        if (msgType == CLUSTER_PING) {
            updateSeen(msg->getSrcAddr());
            debugEV << "Recebi um PING! de " << msg->getSrcAddr()
                    << " Meu headAddress eh: " << getHeadAddress() << endl;
            Pong(msg->getSrcAddr());
        }
    }
        break;
    case HEAD_NODE: {
        if (msgType == CLUSTER_PONG) {
            debugEV << "Recebi um PONG!! de " << msg->getSrcAddr()
                    << " Meu headAddress eh: " << getHeadAddress() << endl;
            /*Verifica se recebeu um pong de um filhote */

            }
        }

        break;

    }
}


/*
 *
 * ---------------------------------------MCF-------------------------------------
 *
 *
 */
int ClusterMCFA::MCF() {
    debugEV << "----------INICIANDO MCF------------" << endl;
    /*
     *
     * STAGE 0
     *
     */
    if (currStage == 0) {
        debugEV << "----------STAGE 0------------" << endl;
        debugEV << "Vou escolher" <<endl;
        debugEV << Automata->prtProb();
        ch = Automata->randNeigh();
        //Selected CH is me, set w as my ERM and go to stage 2
        if(ch < 0){
            debugEV << "Recebi um vizinho vazio! " << ch << endl;
            exit(ch);
        }
/*
 * Se o o CH e outro , envia um pedido de ERM (RERM) e espera pela mensagem que envia ao estagio 1,
 *  senao passa diretamente para o estagio 1
 */
        if (ch == myAddress) {
            w = Automata->getERMt();
            currStage = 1;
            debugEV << "Eu sou o CH" << endl;
        } else {
            debugEV << "Enviando RERM para" << ch << endl;
            ClusterMCFAPkt *pkt = new ClusterMCFAPkt("DirectCast RERM",
                    MCFA_CTRL);
            pkt->setMsgtype(MCFA_RERM);
            // Marca o pacote como um pedido de ERM
            pkt->setQuestion(1);
            sendDirectMessage(pkt, ch);
            cancelEvent(reinitTimer);
            scheduleAt(simTime() + 4 , reinitTimer);
            currStage = 1;
            return 1; //Espero voltar para o estagio 1
        }
    }
    /*
     *
     * STAGE 1
     *
     */
    debugEV << "----------STAGE 1------------" << endl;
    debugEV << "w de ch(" << ch << "):" << w << " T: " << T << endl;
    /*
     * Se o ERMi e menor que a media dos ERMi[] reforca ch, senao pune ch
     */
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

    /*
     * Atualiza ERM de ch para calculo do T
     */
    Automata->setERMi(ch,w);

    debugEV << Automata->prtProb();
    T = Automata->getT();
    stageK++;

    if (Automata->getProbability(ch) < P) {
        debugEV << "Probabilidade de ch(" << ch << "): "
                << Automata->getProbability(ch) << " P: " << P << endl;
        currStage = 0;
    } else {
        debugEV << "Probability: " << Automata->getProbability(ch) << " Threshold:"
                << P << endl;
        if (ch == myAddress) {
            setCurrentRole(HEAD_NODE);
            setHeadAddress(ch);
            debugEV <<"CHSEL: Eu sou HEAD" <<endl;
            clusterNodeState = RUNNING;
        } else {
            sendCHSEL(ch);
            setCurrentRole(CHILD_NODE);
            setHeadAddress(ch);
            clusterNodeState = RUNNING;
        }
        debugEV << "Encontrei o Meu HEAD:" << ch <<endl;
        currStage = 2;
        debugEV << " currStage agora e" << currStage  <<endl;
    }
    ch = 0;
    return 1;
}

void ClusterMCFA::clusterMerge(int h){
    debugEV << "Pronto para o Merge com " << h <<endl;
    //Gera lista de Afiliados
    std::map<LAddress::L3Type, NodeEntry> cl = getChildList();
    for(std::map<LAddress::L3Type, NodeEntry>::iterator it = cl.begin(); it != cl.end(); it++) {
        debugEV << "Enviando LREQ para" << it->first <<endl;
        sendLREQ(it->first);
    }
    debugEV << "Encontrei um Head  " << endl;
    sendCHSEL(h);
    setCurrentRole(CHILD_NODE);
    setHeadAddress(ch);
    clusterNodeState = RUNNING;
}

/************************************************
 *
 * Utilitarios
 *
 ************************************************/
void ClusterMCFA::nodeGarbageCollector(){
    int i ;
    std::vector<int> removidos = Automata->garbageCollector(simTime() - getNodeTimeout());
    //debugEV << Automata->msg << endl;;
    for(i=0;i<removidos.size();i++){
        debugEV << "Removi da lista de proximos: " << removidos[i] <<endl;
    }
    switch(getCurrentRole()){
    case HEAD_NODE:{
        int ActiveChilds, TotalChilds;
        std::map<LAddress::L3Type, NodeEntry> ChildList = getChildList();
        debugEV << "Temos: " << ChildList.size() << endl;
        ActiveChilds = TotalChilds = ChildList.size();
        for(i=0;i<removidos.size();i++){
            debugEV << "Removi da lista de Filhotes " << removidos[i] <<endl;
            if(ChildList.find(removidos[i]) != ChildList.end() ){
                removeChild(removidos[i]);
            }
        }
    }
        break;
    case CHILD_NODE:{
        if(std::find(removidos.begin(), removidos.end(), getHeadAddress())!=removidos.end()){
            /*
            setCurrentRole(UNDEFINED_NODE);
            currStage = 0;
            MCF();
            */
            cancelAndDelete(delayTimer);
            delayTimer = new cMessage("reseting", RESET);
            handleReset(delayTimer);
        }
    }
        break;

    }

    debugEV << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!REMOCAO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    debugEV << "Removi " << Automata->removido << endl;
    debugEV << "nao vistos desde " << simTime().dbl() - getNodeTimeout() << endl;

}
std::vector<int> ClusterMCFA::str2intList(const char * str){
    std::string s = std::string(str,strlen(str));
    std::vector<int> cl ;
    std::string delimiters = ",";
    size_t current;
    size_t next = -1;
    int id;
    do
    {
      current = next + 1;
      next = s.find_first_of( delimiters, current );
      std::stringstream ss(s.substr( current, next - current ));
      ss >> id;
      cl.push_back(id) ;
    }
    while (next != std::string::npos);

    return cl;

 }

MobInfo* ClusterMCFA::getMobInfo() {
    MobInfo *mi = new MobInfo;
    mi->direction = gps.getDirection();
    mi->speed = gps.getSpeed();
    return mi;
}

void ClusterMCFA::updateSeen(LAddress::L3Type node){
    ClusterManager::updateSeen(node);
    Automata->updateSeen(node);
}

void ClusterMCFA::sendMobInfo() {
    debugEV << "Enviando novo MOBINFO" << endl;
    lastsend = simTime();
    ClusterMCFAPkt *pkt = new ClusterMCFAPkt("BROADCAST: MOBINFO", MCFA_CTRL);
    pkt->setMsgtype(MCFA_MOBINFO);
    pkt->setDirection(((gps.getSpeed()==0)?0:gps.getDirection()));
    pkt->setSpeed(gps.getSpeed());
    pkt->setOriginId(myAddress);
    if (getCurrentRole() == HEAD_NODE  && mergeEnable){
        //Gera lista de Afiliados
        std::map<LAddress::L3Type, NodeEntry> cl = getChildList();
        std::ostringstream out;
        for(std::map<LAddress::L3Type, NodeEntry>::iterator it = cl.begin(); it != cl.end(); it++) {
            out << it->first << ",";
        }
        pkt->setHeadId(myAddress);
        pkt->setChildinfo(out.str().c_str());
        pkt->setERM(Automata->getERMt());
    }

    sendBroadcast(pkt);

}/*
void ClusterMCFA::sendCLINFO(){
    debugEV << "ENVIANDO CLUSTER INFO: De:" << myAddress << endl;
    ClusterMCFAPkt *pkt = new ClusterMCFAPkt("BROADCAST CLUSTINFO", MCFA_CTRL);
    //Gera lista de Afiliados
    std::map<LAddress::L3Type, NodeEntry> cl = getChildList();
    std::ostringstream out;
    for(std::map<LAddress::L3Type, NodeEntry>::iterator it = cl.begin(); it != cl.end(); it++) {
        debugEV << it->first << ",";
        out << it->first << ",";
    }
    debugEV <<endl;
    pkt->setMsgtype(MCFA_CLINFO);

    pkt->setChildinfo(out.str().c_str());
    pkt->setERM(Automata->getERMt());

    pkt->setDirection(((gps.getSpeed()==0)?0:gps.getDirection()));
    pkt->setSpeed(gps.getSpeed());
    pkt->setOriginId(myAddress);
    sendBroadcast(pkt);
}*/
void ClusterMCFA::sendCHSEL(int NodeAddr) {
    debugEV << "ENVIANDO CHSEL: De:" << myAddress << " --> " << NodeAddr
            << endl;
    ClusterMCFAPkt *pkt = new ClusterMCFAPkt("DirectCast CHSEL", MCFA_CTRL);
    pkt->setMsgtype(MCFA_CHSEL);
    pkt->setERM(Automata->getERMt());
    sendDirectMessage(pkt, NodeAddr);
}
void ClusterMCFA::sendLREQ(LAddress::L3Type NodeAddr) {
    debugEV << "ENVIANDO LREQ: De:" << myAddress  << " para:" << NodeAddr << endl;
    ClusterMCFAPkt *pkt = new ClusterMCFAPkt("DirectCast LREQ", MCFA_CTRL);
    pkt->setMsgtype(MCFA_LREQ);
    sendDirectMessage(pkt, NodeAddr);


}

