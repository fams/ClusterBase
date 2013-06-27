//
// This program is free software: you can redistribute it and/or modify
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
//,
#include <vector>
#include <string.h>
#include "GameForm.h"
#include <AddressingInterface.h>
#include <cassert>
#include <Packet.h>
#include "FindModule.h"
#include <BaseNetwLayer.h>
#include <GPSInfo.h>



Define_Module(GameForm);


void GameForm::initialize(int stage)
{
	ClusterManager::initialize(stage);

	if(stage == 0) {

	    //Node State Ž relativo ao LowestID
        clusterNodeState = INITIALIZING;
        //Inicia o node UNDEFINED
        // setCurrentRole(UNDEFINED_NODE);

		//Messages
		delayTimer 		= new cMessage("LID-Timer", DEFINE_HEAD);



		pollingTimer	= new cMessage("polling-timer", POLLING);

		publishTimer    = new cMessage("publish-timer", SEND_MYID);


		//Parameters
		electionTime        = par("electionTime");

		heloTime            = par("heloTime");

        pollingTime         = par("pollingTime");

		headJoinTime 		= par("headJoinTime");

		publishTime         = par("publishTime");

		setNodeTimeout((int)par("resetTime"));

		retransmissionTime	= par("retransmissionTime");

		neighInqTime        = par("neighInqTime");

        gpsInterval         = par("gpsInterval");

        optimalDegree       = par("optimalDegree");


       //Utils
		maxChilds = 0;

		nbPacketDropped = 0;

		///Inicializar CONTAGEM FIXME
/*
    Emitir sinal para contabilizar o node UNDEFINED
*/
		joinAttempts_cur = 0;
		head_candidate = -1;
		pollAttempt = 0;

		createReset(DO_RESET);


	} else if (stage == 1) {

		debugEV << "Meu App Addr e:" << myAddress << endl;
		double schedtime = simTime().dbl() + dblrand() + heloTime;
		cancelEvent(delayTimer);
		//scheduleAt( schedtime , delayTimer);
		scheduleAt(schedtime , publishTimer);
		addNeighbour(myAddress,999);
		debugEV << "Agendando info para" << schedtime <<endl;
        /*Criando GPS */
        BaseMobility* mob = FindModule<BaseMobility*>::findSubModule(
                findHost());
        Coord HostCoor = mob->getCurrentPosition();
        gps.updatePos(&HostCoor);
        gps.setDistance(0);
        debugEV << "DISTANCIA " << gps.getTotalDistance();
        sendMobTimer = new cMessage("sendMob-timer", UPDATE_POSITION);
        scheduleAt(simTime() + 1 + dblrand()*2, sendMobTimer);

	} else {

	}
}

GameForm::~GameForm() {
	//if(delayTimer)
		//cancelAndDelete(delayTimer);
	if(pollingTimer)
		cancelAndDelete(pollingTimer);
	if(resetTimer)
	    cancelAndDelete(resetTimer);
}

void GameForm::finish()
{
	//recordScalar("dropped", nbPacketDropped);
}

int GameForm::isHeadValid(int TotalChilds, int ActiveChilds){

    if (clusterNodeState < HEAD_NEIGH || TotalChilds == 0){
        return 1;
    }
    return (ActiveChilds > (childLostPercentage * TotalChilds));
}

void GameForm::migrate(int hc){
    /*if(getCurrentRole() != HEAD_NODE)
        return;
    Neighbor2 n = *(findCandidate(hc));
    //Se o candidato n‹o eh um head nao migra
    if(n.isChild())
        return;
    //Se o head atual eh o menor nao migra
    if(getHeadAddress() <= hc)
        return;
    switch(getCurrentRole()){
    case CHILD_NODE:{
        NotifyLeave();
    };
    break;
    case HEAD_NODE:{
        //Reseting all
        SendLeave();
        clearChilds();
    }
    break;
    }*/
    //Reseting all
    debugEV << "Migrando..." << endl;
    setCurrentRole(UNDEFINED_NODE);
    findHost()->bubble("Migrando");
    //Display String
    char* tt = new char(20);
    sprintf(tt, "MyAddr is %i" , myAddress);
    setTTString(tt);

    //Tenta o Rejoin
    debugEV << "Tentando me afiliar a outro head." << endl;
    clusterNodeState = CHILD_JOIN;
    debugEV << "Node: " <<myAddress << " tentando se afiliar ao no " << hc << endl;

    //Envia Mensagem de Join para o HEAD
    GameFormPkt *pkt = new GameFormPkt("DIRECT: CLUSTER_JOIN", CLUSTER_FORMATION_PACKET);
    setPktValues(pkt,CLUSTER_JOIN, hc, myAddress);
    sendDirectMessage(pkt, hc);

    //Agendando Eleicao
    delayTimer = new cMessage("LID-timer", DEFINE_HEAD);
    cancelEvent(delayTimer);
    double schedtime = simTime().dbl() + electionTime;
    debugEV << "Agendando eleicao para " << schedtime <<endl;
    scheduleAt( schedtime , delayTimer);
}
void GameForm::handleReset(cMessage *msg){
	//Reseting all
	debugEV << "LIC Reseting  " << endl;

	setCurrentRole(UNDEFINED_NODE);

	/** Init Node State */
	clusterNodeState = INITIALIZING;

	findHost()->bubble("Reseting");

	//Display String
	char* tt = new char(20);
	sprintf(tt, "MyAddr is %i" , myAddress);
	setTTString(tt);
/*
	//Tenta o Rejoin
	debugEV << "Tentando me afiliar a outro head." << endl;
	clusterNodeState = CHILD_JOIN;
	GameFormPkt *pkt = new GameFormPkt("BROADCAST: CLUSTER_JOIN", CLUSTER_FORMATION_PACKET);
	setPktValues(pkt , CLUSTER_JOIN, myAddress, myAddress);
	sendBroadcast(pkt);
*/
	//Agendando Eleicao

	delayTimer = new cMessage("LID-timer", DEFINE_HEAD);
    cancelEvent(delayTimer);
    double schedtime = simTime().dbl() + electionTime;
    debugEV << "Agendando eleicao para " << schedtime <<endl;
	scheduleAt( schedtime , delayTimer);
}

/*
 * Mensagens locais
 *
 */

void GameForm::handleSelfMsg(cMessage *msg)
{

	switch( msg->getKind()){
	case SEND_MYID:{
	    sendMyID();
	    cancelEvent(publishTimer);
	    //scheduleAt(simTime() + publishTime, publishTimer);
	}
	break;
	case SEND_ELECTION:{
	   sendElection();
       //cancelEvent(publishTimer);
       //scheduleAt(simTime() + electionTime, publishTimer);
	}
	break;
	case POLLING:{
		handlePolling(msg);
        debugEV << "Reagendando polling para " << simTime().dbl() + pollingTime << endl;
        cancelEvent(pollingTimer);
        scheduleAt( simTime() + pollingTime , pollingTimer);
		}
	break;

	case DO_RESET:{
		handleReset(msg);
	}
	break;

/* HEAD */
	case DEFINE_HEAD:{
	    debugEV << "Definindo Heads" << endl;
	    int hc = getElected();
	    if(hc == myAddress){
	        sendHeadInit();
	        //Agenda Mudanca de estado
	        debugEV << "Iniciando Pooling em :"<< pollingTime << endl;
	        cancelAndDelete(delayTimer);
	        cancelEvent(pollingTimer);
	        scheduleAt( simTime() + pollingTime , pollingTimer);
	    }else{
	        //Agenda Mudanca de estado
            clusterNodeState = CHILD_JOIN;
	        debugEV << "Agendando troca de estado para SEND_JOIN em :"<< (simTime() + headJoinTime) << endl;
	        cancelAndDelete(delayTimer);
	        delayTimer = new cMessage("game-Timer", SEND_JOIN);
	        scheduleAt( simTime() + headJoinTime , delayTimer);
	        cancelEvent(pollingTimer);
	        scheduleAt( simTime() + pollingTime , pollingTimer);
	    }

	}
	break;
	case SEND_JOIN:{
	    int hc = getHeadCandidate();
	    if (hc < 0){
	        debugEV << "NAO RECEBI HEADS RESET!!!!" << hc << endl;
	        //Agenda reset
	        cancelAndDelete(delayTimer);
	        delayTimer = new cMessage("reseting", DO_RESET);
	        scheduleAt( simTime()  , delayTimer);
	    }else{
            debugEV << "Meu endereco e " << myAddress << " o eleito foi " << hc << endl;
            debugEV << "Tentando associar a um HEAD" <<endl;
            clusterNodeState = CHILD_JOIN;
            debugEV << "Node: " <<myAddress << " tentando se afiliar ao no " << hc << endl;

            //Envia Mensagem de Join para o HEAD
            GameFormPkt *pkt = new GameFormPkt("DIRECT: CLUSTER_JOIN", CLUSTER_FORMATION_PACKET);
            setPktValues(pkt,CLUSTER_JOIN, hc, myAddress);
            sendDirectMessage(pkt, hc);

            char tt[50];
            sprintf(tt, "Tentando associar a Head %i" ,  hc );
            setTTString(tt);

            //Agenda reset
            cancelAndDelete(delayTimer);
            delayTimer = new cMessage("reseting", DO_RESET);
            scheduleAt( simTime() + headJoinTime , delayTimer);
	    }
	}
	break;
	case SEND_NEIGH_INQ:{
		//Muda estado para procura de vizinhos
		//assert(msg == delayTimer);
		clusterNodeState = HEAD_NEIGH;

		char tt[50];
		sprintf(tt, "HEAD, MyID:  %i - HEAD_NEIGH" , myAddress);
		setTTString(tt);
		GameFormPkt *pkt;
		for (std::list<int>::iterator it = childs.begin(); it!=childs.end(); ++it) {
		    debugEV << "Sending INQ to: " << *it << endl;
		    pkt = new GameFormPkt("BROADCAST: NEIGH_INQ", CLUSTER_FORMATION_PACKET);
		    setPktValues(pkt, CLUSTER_NEIGH_INQ, myAddress, myAddress);
		    sendDirectMessage(pkt, *it);
		}
		//Agenda estado de envio de mensagens
		debugEV << "Agendando troca de estado para SEND_PROCESS em :"<< headJoinTime + simTime() << endl;
		cancelAndDelete(delayTimer);
		delayTimer = new cMessage("LID-Timer", SEND_PROCESS);
		scheduleAt(simTime() + neighInqTime , delayTimer);
		/*
		//Schedule Polling
		debugEV << "Broadcast a HEAD POLLING " << endl;
		pkt = new GameFormPkt("DIRECT: HEAD POLLING", CLUSTER_FORMATION_PACKET);
		setPktValues(pkt,CLUSTER_PING, getHeadAddress(), myAddress);
		sendBroadcast(pkt); */
	}
	break;
	  //Atualize sua posicao
	    case UPDATE_POSITION: {

	        //Obtem o host
	        BaseMobility* mob = FindModule<BaseMobility*>::findSubModule(
	                findHost());
	        //Obtem a posicao
	        Coord HostCoor = mob->getCurrentPosition();

	        debugEV << "Minha nova posicao e " << HostCoor.x << "," << HostCoor.y
	                << "\n";
	        debugEV << "Minha ultima posicao e " << gps.getCoor().x << ","
	                << gps.getCoor().y << "\n";
	        /** Verificando se temos um EPOCH */
	        double lastSpeed = gps.getSpeed();
	        double lastDirection = gps.getDirection();
	        gps.updatePos(&HostCoor);
	        debugEV << "DISTANCIA " << gps.getTotalDistance();
	        debugEV << "Meu estado atual" << clusterNodeState << endl;
	        debugEV << "LastCourse: " << lastDirection << " currentCourse " << gps.getDirection() << "\n";
	        debugEV << "LastSpeed: " << lastSpeed << " currentSpeed: " << gps.getSpeed() << "\n";
	        debugEV << "Minha Velcidade e " << gps.getSpeed() << "\n";
	        debugEV << "Minha Direcao e " << gps.getDirection() << "\n";

	        cancelEvent(sendMobTimer);
	        scheduleAt(simTime() + gpsInterval, sendMobTimer);

	        //cancelAndDelete(sendMobTimer);
	    }
	        break;
	case SEND_PROCESS:{
		//Em estado de processamento
		debugEV << "Change to SEND: " << endl;
		clusterNodeState = HEAD_MESSAGE;

		char tt[50];
		sprintf(tt, "HEAD, MyID:  %i\n HEAD_SEND" , myAddress);
		setTTString(tt);
	}
	break;
	default:
		EV << "Unkown selfmessage! -> delete, kind: "<<msg->getKind() <<endl;
		delete msg;
		break;
	}
}


void GameForm::handleNetlayerMsg(cMessage *msg)
{

    ClusterManager::handleNetlayerMsg(msg);
	switch (msg->getKind()) {
		case CLUSTER_FORMATION_PACKET:
			GameFormPkt *m;
			m = static_cast<GameFormPkt *> (msg);
			handleClusterMessage(m);
			delete msg;
			msg = 0;
		break;
	}
}
/*
 *  Mensagens recebidas dos NODES
 *
 */
void GameForm::handleClusterMessage(GameFormPkt* m){
	//Acao depende do estado do cluster e tipo de mensagem

    int msgType = m->getMsgtype();
	EV << "Received Msg Type " << msgType << " From: " <<  m->getSrcAddr() << endl;

	switch(msgType){

	/*********************
	 *  HEAD Messages
	 *********************/
	case CLUSTER_JOIN:{
        //Recebi um Cluster Join
        debugEV << "Received CLUSTER_JOIN from:" << m->getSrcAddr() << endl;
        if(getCurrentRole() == HEAD_NODE){
            //Adicionando na lista de filhotes
            addChild(m->getSrcAddr());
            //Envia Mensagem de JoinAccept
            GameFormPkt *pkt = new GameFormPkt("DIRECT: CLUSTER_ACCEPTED:", CLUSTER_FORMATION_PACKET);
            setPktValues(pkt, CLUSTER_ACCEPTED, m->getSrcAddr(), myAddress);
            sendDirectMessage(pkt,m->getSrcAddr());
        }
	}
	break;

	/*********************
	 *  UNDEFINED Messages
	 *********************/
	case CLUSTER_HELO:{
	    debugEV << "Received node ID\n";
	    std::stringstream ss;
	    std::string buf;
	    ss << m->getListeningNodes();
	    ss >> buf;
	    if(m->getHeadId() == m->getOriginId()){
	        addCandidate(m->getHeadId(),m->getW());
	    }else{
	        addNeighbour( m->getSrcAddr(),m->getHeadId(),m->getW());
	    }
	    //migrate(m->getSrcAddr());
	}
	break;
	case CLUSTER_ELECTION:{
	    debugEV << "Election\n";
        std::stringstream ss;
        std::string buf;
        ss << m->getListeningNodes();
        ss >> buf;
	    addNeighbour( m->getSrcAddr(),m->getHeadId(),m->getW());
	}
	break;
	case CLUSTER_INIT:{
	    addCandidate(m->getSrcAddr(),m->getW());
	}
	break;
	/*********************
	*  CHILD Messages
	*********************/
	case CLUSTER_ACCEPTED:
		if(clusterNodeState == CHILD_JOIN){
			debugEV << "Becaming child of " << m->getSrcAddr() << endl;
			clusterNodeState = CHILD_MESSAGE;
			setCurrentRole(CHILD_NODE);
			//cancelEvent(publishTimer);

			char tt[20];
			sprintf(tt, "My HEAD: %l" , m->getSrcAddr());
			setTTString(tt);
			setHeadAddress( m->getSrcAddr());
			updateSeen();
            cancelEvent(pollingTimer);
            scheduleAt( simTime() + pollingTime , pollingTimer);
			//cancelEvent(publishTimer);
			cancelEvent(delayTimer);
		}else{
			debugEV << "received CLUSTER_ACCEPTED but not in CHILD_JOIN actual state: " << clusterNodeState << endl;
		}
	break;
	/*********************
	*  BOTH Messages
	*********************/
	case CLUSTER_NEIGH_INQ:
		debugEV << "Received CLUSTER_NEIGH_INQ from:" << m->getSrcAddr() << endl;
		switch(clusterNodeState){
		case CHILD_MESSAGE:
		case HEAD_MESSAGE:
		case HEAD_NEIGH:
			if(m->getHeadId() != getHeadAddress()){
				debugEV << "Received INQ from other HEAD " << endl;
				GameFormPkt *pkt = new GameFormPkt("DIRECT: NEIGH_RESP", CLUSTER_FORMATION_PACKET);
				setPktValues(pkt,CLUSTER_NEIGH_RESP, getHeadAddress(), myAddress);
				sendDirectMessage(pkt,m->getSrcAddr());
			}else if(m->getSrcAddr()== getHeadAddress()){
			    updateSeen();
				debugEV << "Searching fror NEIGHBOHRS, resending message" << endl;
				GameFormPkt *pkt = new GameFormPkt("BROADCAST: NEIGH_INQ", CLUSTER_FORMATION_PACKET);
				setPktValues(pkt, CLUSTER_NEIGH_INQ, m->getHeadId(), m->getOriginId());
				sendBroadcast(pkt);
			}
			break;
		}
	break;
	case CLUSTER_NEIGH_RESP:
		debugEV << "Received CLUSTER_NEIGH_RESP from:" << m->getSrcAddr() << endl;
		//Popula a tabela de rotas, se for um filhote, repassa para o head
		switch(clusterNodeState){
		case CHILD_MESSAGE:
			if(m->getHeadId() != getHeadAddress()){
				debugEV << "Received RESP to My HEAD " << endl;
				GameFormPkt *pkt = new GameFormPkt("DIRECT: NEIGH_RESP", CLUSTER_FORMATION_PACKET);
				setPktValues(pkt, CLUSTER_NEIGH_RESP, m->getHeadId(), m->getOriginId());
				sendDirectMessage(pkt, getHeadAddress());
			}
			break;
		case HEAD_NEIGH:{
			RouteEntry re;
			re.gw 	= m->getSrcAddr();
			re.head = m->getHeadId();
			Neighbor2s.push_back(re);
		}
		break;
		default:
				EV << "STATE NOT HANDLED" << endl;
		break;
		}
	break;
	default:
		EV << "MSG NOT HANDLED" << endl;
		break;
	}
}
/*
 * Metodos especificos da implementacao
 * */
void GameForm::sendElection(){
    // Acerta o Meu W
    addNeighbour(myAddress,getW());
    //Envia info de ID
    GameFormPkt *pkt = new GameFormPkt("BROADCAST: CLUSTER_INFO", CLUSTER_FORMATION_PACKET);
    //Avisa Coloca o ID
    pkt->setMsgtype(CLUSTER_ELECTION);
    pkt->setOriginId(myAddress);
    pkt->setW(getW());
    sendBroadcast(pkt);
//////
    debugEV << "Enviando W:" << getW() << endl;
     //Agenda Mudanca de estado
    double schedat = simTime().dbl() + electionTime;
    debugEV << "Agendando Eleicao em :" << schedat << endl;
    cancelAndDelete(delayTimer);
    delayTimer = new cMessage("delay-timer", DEFINE_HEAD);
    clusterNodeState = ELECTION;
    scheduleAt(schedat, delayTimer);

}
void GameForm::sendHeadID(){
    //Envia info de ID
    GameFormPkt *pkt = new GameFormPkt("BROADCAST: CLUSTER_INFO", CLUSTER_FORMATION_PACKET);
    //Avisa Coloca o ID
    pkt->setMsgtype(CLUSTER_HELO);
    pkt->setOriginId(myAddress);
    pkt->setHeadId(myAddress);
    pkt->setW(getW());
    sendBroadcast(pkt);
}
void GameForm::sendMyID(){
    //Envia info de ID
    GameFormPkt *pkt = new GameFormPkt("BROADCAST: CLUSTER_INFO", CLUSTER_FORMATION_PACKET);
    //Avisa Coloca o ID
    pkt->setMsgtype(CLUSTER_HELO);
    pkt->setOriginId(myAddress);
    pkt->setW(getW());
    sendBroadcast(pkt);
//////
    debugEV << "Enviando HELO:" << myAddress << endl;
     //Agenda Mudanca de estado
    if(clusterNodeState == INITIALIZING){
        debugEV << "Agendando Formacao em :" << (simTime() + heloTime) << endl;
        cancelAndDelete(delayTimer);
        delayTimer = new cMessage("delay-timer", SEND_ELECTION);
        clusterNodeState = DISCOVER;
        scheduleAt(simTime() + heloTime, delayTimer);
    }

}

//Get W
double GameForm::getW()
{

    int number = listenList.size();
    //Calculo de W <<
    debugEV << "DADOS totalDistance " << gps.getTotalDistance() << " time " << gps.getTime()  << " Speed " << gps.getSpeed() << endl;
    debugEV << "0.5 * " << abs(number - optimalDegree) << "+ 0.5 * " << gps.getAvgSpeed() << endl;
    return (0.5 * abs(number - optimalDegree) + 0.5 * gps.getAvgSpeed());
}

void GameForm::sendHeadInit()
{
    debugEV << "Tornando-me HEAD:" << myAddress << endl;

    char tt[50];
    sprintf(tt, "HEAD, MyID:  %i\n HEAD_JOIN" , myAddress);
    setTTString(tt);


    //Se esta enviando, atingiu timeout vira HeadNode
    setCurrentRole(HEAD_NODE);
    setHeadAddress(myAddress);

    //Muda estado para HEAD_NOIN
    clusterNodeState = HEAD_JOIN;

    //Envia Mensagem de join para os filhos
    GameFormPkt *pkt = new GameFormPkt("BROADCAST: CLUSTER_INIT", CLUSTER_FORMATION_PACKET);

    setPktValues(pkt , CLUSTER_INIT, myAddress, myAddress);
    pkt->setW(getW());
    sendBroadcast(pkt);
}

void GameForm::addCandidate(int c, double weight){

    std::vector<Neighbor2>::iterator it = findCandidate2(c);
    if(it!=HeadCandidates.end()){
        debugEV << "Atualizando candidato " << c <<endl;
        it->setlastseen(simTime().dbl());
        it->setW(weight);
    }else{
        debugEV << "Adicionando como candidato " << c <<endl;
        HeadCandidates.push_back(Neighbor2(c, simTime().dbl(),weight));
        std::sort(HeadCandidates.begin(),HeadCandidates.end(),Neighbor2::cmp_neigh);
        std::reverse(HeadCandidates.begin(),HeadCandidates.end());
    }
}
std::vector<Neighbor2>::iterator GameForm::findCandidate2(int c){
    return std::find(HeadCandidates.begin(), HeadCandidates.end(), Neighbor2(c,0,0)) ;
}

std::vector<Neighbor2>::iterator GameForm::findCandidate(Neighbor2 c){
    return std::find(listenList.begin(), listenList.end(), c) ;
}
std::vector<Neighbor2>::iterator GameForm::findCandidate(int c){
    return std::find(listenList.begin(), listenList.end(), Neighbor2(c,0,0)) ;
}

void GameForm::addNeighbour(int c, double weight) {
    std::vector<Neighbor2>::iterator it = findCandidate(c);
    if(it!=listenList.end()){
        it->setlastseen(simTime().dbl());
        it->setW(weight);
    }else{
        listenList.push_back(Neighbor2(c, simTime().dbl(),weight));
        std::sort(listenList.begin(),listenList.end(),Neighbor2::cmp_neigh);
        std::reverse(listenList.begin(),listenList.end());
    }

}
void GameForm::addNeighbour(int c, int h, double weight) {
    std::vector<Neighbor2>::iterator it = findCandidate(c);
    if(it!=listenList.end()){
        debugEV << "Atualizando info do node:" << c << " W agora: " << weight << endl;
        it->setlastseen(simTime().dbl());
        it->setW(weight);
        it->sethead(h);
    }else{
        debugEV << "Adicionando info do node:" << c << " W: " << weight << endl;
        listenList.push_back(Neighbor2(c, h , simTime().dbl(),weight));
        std::sort(listenList.begin(),listenList.end(),Neighbor2::cmp_neigh);
        std::reverse(listenList.begin(),listenList.end());
    }
    debugEV << "Ordem:" << listenList.size() <<endl;

}

void GameForm::removeCandidate(int c){
    std::vector<Neighbor2>::iterator nc = findCandidate(c);
    if(nc != listenList.end())
        listenList.erase(nc);
}
void GameForm::removeNeighbour(Neighbor2 c){
    std::vector<Neighbor2>::iterator nc = findCandidate(c);
    if(nc != listenList.end())
        listenList.erase(nc);
}

void GameForm::flushCandidates(){
    debugEV << "FlushCandidates" <<endl;
    listenList.clear();
    HeadCandidates.clear();
}
int GameForm::getHeadCandidate(){
    std::vector<Neighbor2>::iterator itt = HeadCandidates.begin();
    debugEV << " Timeout:" << getNodeTimeout() << endl;
    while(itt != HeadCandidates.end()){
        debugEV << " Node:" << itt->getNode() << " Weight:" << itt->getW() << endl;
        if(((itt->getLastseen()+ getNodeTimeout()) < simTime().dbl())&&(itt->getNode() != myAddress)){
            debugEV<< " Timeout At: " << itt->getLastseen()+ getNodeTimeout() << " Estou em: " << simTime().dbl() << endl;
            HeadCandidates.erase(itt);
        }else{
            itt++;
        }
    }
    if(HeadCandidates.size()< 1 ){
        return -1;
    }
    std::sort(HeadCandidates.begin(),HeadCandidates.end(),Neighbor2::cmp_neigh);
    Neighbor2 *hc =  &HeadCandidates.back();
    return hc->getNode();
}
/*
 * A eleicao funciona ordenando os n—s vizinhos que est‹o viziveis dentro do tempo de timeout e n‹o s‹o publicamente childs
 * FIXME: Adicionar lastseen
 */
int GameForm::getElected(){

    std::sort(listenList.begin(),listenList.end(),Neighbor2::cmp_neigh);
    Neighbor2 *hc;// = &listenList.back();
    std::vector<Neighbor2>::iterator itt = listenList.begin();
    debugEV << " Timeout:" << getNodeTimeout() <<endl;
    while(itt != listenList.end()){
        debugEV << " Node:" << itt->getNode() << " Weight:" << itt->getW() << endl;
        if(((itt->getLastseen()+ getNodeTimeout()) < simTime().dbl())&&(itt->getNode() != myAddress)){
            debugEV<< " Timeout At: " << itt->getLastseen()+ getNodeTimeout() << " Estou em: " << simTime().dbl() << endl;
            listenList.erase(itt);
        }else{
            itt++;
        }
    }
    debugEV << endl;
    std::sort(listenList.begin(),listenList.end(),Neighbor2::cmp_neigh);
    for(std::vector<Neighbor2>::iterator it = listenList.begin();it != listenList.end(); it++){
        if(it->getNode() == myAddress){
            debugEV << "Selecionado " << it->getNode() << endl;
            hc = &(*it);
            break;
        }
        //if(it->isChild())
          //  continue;
        debugEV << "Selecionado " << it->getNode() << endl;
        hc = &(*it);
        break;
    }
    /*std::reverse(listenList.begin(),listenList.end());
    Neighbor2 hc = listenList.back();
    while(hc != listenList.front()){
        Neighbor2 hc = listenList.back();
         if
    */
    debugEV << "Vizinhos: ";
    for(std::vector<Neighbor2>::iterator it = listenList.begin(); it != listenList.end(); it++){
        debugEV << it->getNode() << "W: " << it->getW() << ", ";
    }
    debugEV << endl;
    //removeNeighbour(hc);
    return hc->getNode();
}
void GameForm::BroadPing(){
    sendHeadID();
    ClusterPkt *pkt = new ClusterPkt("DIRECT: HEAD POLLING", CLUSTER_BASE_PING);
    setPktValues(pkt,CLUSTER_PING, getHeadAddress(), myAddress);
    sendBroadcast(pkt);
}

void GameForm::ChildPolling(cMessage *msg){
    debugEV << "Handle child Polling" << endl;
    if ((getTimeoutAt() )  < simTime().dbl()){
        debugEV << "Estourou o timer, vou resetar" << endl;
        int hc = getHeadCandidate();
        if(hc<0){
            cancelEvent(resetTimer);
            scheduleAt( simTime() + 1, resetTimer);
        }else{
            migrate(hc);
        }
    }else{
        debugEV << "Vi meu Head em: " << getHeadLastSeen() << " timeout em: " << getTimeoutAt() << endl;
        cancelEvent(resetTimer);
        scheduleAt( simTime() + getTimeoutAt()  , resetTimer);
    }
}
