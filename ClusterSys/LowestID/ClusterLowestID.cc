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
#include "ClusterLowestID.h"
#include <AddressingInterface.h>
#include <cassert>
#include <Packet.h>
#include "FindModule.h"
#include <BaseNetwLayer.h>
#include "ClusterLowestIdPkt_m.h"



Define_Module(ClusterLowestID);


void ClusterLowestID::initialize(int stage)
{
	ClusterManager::initialize(stage);

	if(stage == 0) {

	    //Node State Ž relativo ao LowestID
        clusterNodeState = INITIALIZING;
        //Inicia o node UNDEFINED
        // setCurrentRole(UNDEFINED_NODE);

		//Messages
		delayTimer 		= new cMessage("LID-Timer", SEND_HEAD_INIT);

		resetTimer 		= new cMessage("reset-timer", DO_RESET);

		pollingTimer	= new cMessage("polling-timer", POLLING);

		//Parameters


        pollingTime         = par("pollingTime");

		headJoinTime 		= par("headJoinTime");

		setNodeTimeout((int)par("resetTime"));

		retransmissionTime	= par("retransmissionTime");

		childLostPercentage = par("childLostPercentage");

		joinAttempts	    = par("joinAttempts");

		neighInqTime        = par("neighInqTime");



       //Utils
		maxChilds = 0;

		nbPacketDropped = 0;

		///Inicializar CONTAGEM FIXME
/*
    Emitir sinal para contabilizar o node UNDEFINED
*/
		joinAttempts_cur = 0;
		pollAttempt = 0;



	} else if (stage == 1) {

		debugEV << "Meu App Addr e:" << myAddress << endl;
		double schedtime = dblrand() * 100;
		cancelEvent(delayTimer);
		scheduleAt( schedtime , delayTimer);
		debugEV << "Agendando  init para" << schedtime <<endl;

	} else {

	}
}

ClusterLowestID::~ClusterLowestID() {
	if(delayTimer)
		cancelAndDelete(delayTimer);
	if(pollingTimer)
		cancelAndDelete(pollingTimer);
}

void ClusterLowestID::finish()
{
	recordScalar("dropped", nbPacketDropped);
}

int ClusterLowestID::isHeadValid(int TotalChilds, int ActiveChilds){

    if (clusterNodeState < HEAD_NEIGH){
        return 1;
    }
    return (ActiveChilds > (childLostPercentage * TotalChilds));
}

void ClusterLowestID::handleReset(cMessage *msg){
	//Reseting all
	debugEV << "Reseting  " << endl;

	setCurrentRole(UNDEFINED_NODE);

	/** Init Node State */
	clusterNodeState = INITIALIZING;

	bubble("Reseting");

	//Display String
	char* tt = new char(20);
	sprintf(tt, "MyAddr is %i" , myAddress);
	setTTString(tt);

	//Tenta o Rejoin
	ClusterLowestIdPkt *pkt = new ClusterLowestIdPkt("BROADCAST: CLUSTER_REJOIN", CLUSTER_FORMATION_PACKET);
	setPktValues(pkt , CLUSTER_REJOIN, myAddress, myAddress);
	sendBroadcast(pkt);

	delayTimer = new cMessage("LID-timer", SEND_HEAD_INIT);
    cancelEvent(delayTimer);
	scheduleAt( simTime() + dblrand() * myAddress , delayTimer);
}


void ClusterLowestID::handleSelfMsg(cMessage *msg)
{

	switch( msg->getKind()){
	case POLLING:{
		handlePolling(msg);
		}
	break;

	case DO_RESET:{
		handleReset(msg);
	}
	break;

	case REPLY_JOIN:{
		debugEV << "Reply Attempt: " << joinAttempts_cur << " de " << joinAttempts << endl;
		//Show Reply Attempt on popup

		char tt[50];
		sprintf(tt, "Reply attempt %i to H: %i" , joinAttempts_cur, msg->par("headAddress").doubleValue());
		setTTString(tt);
		setCurrentRole(CHILD_NODE);
		/* O Polling no CHILD verifica a quanto tempo o HEAD nao Ž visto */
	    debugEV << "Agendando Polling Child";
	    cancelEvent(pollingTimer);
	    scheduleAt( simTime() + pollingTime , pollingTimer);

		setHeadAddress(msg->par("headAddress").doubleValue());

		//Muda estado para CHILD_NOIN
		clusterNodeState = CHILD_JOIN;
		//Envia Mensagem de Join para o HEAD
		ClusterLowestIdPkt *pkt = new ClusterLowestIdPkt("DIRECT: CLUSTER_JOIN", CLUSTER_FORMATION_PACKET);
		setPktValues(pkt,CLUSTER_JOIN, msg->par("reply-to").doubleValue(), myAddress);
		sendDirectMessage(pkt,msg->par("reply-to").doubleValue());

		//Agenda Mudanca de estado
		debugEV << "Agendando reenvio para " << (simTime()+getNodeTimeout()) << endl;
		//Se acabaram as tentativas, reset
		if(joinAttempts_cur > joinAttempts){
			debugEV << "Agendando Reset para " << (simTime() + 1) << endl;
            cancelAndDelete(delayTimer);
			delayTimer = new cMessage("LID-timer", DO_RESET);
			scheduleAt(simTime() + 1, delayTimer);
		}else{
			joinAttempts_cur++;
			debugEV << "Msg Type>  " << delayTimer->getKind() <<endl;
			cancelEvent(delayTimer);
			scheduleAt( simTime() + retransmissionTime + (dblrand() * 2) , delayTimer);
		}
	}
	break;
/* HEAD */
	case SEND_HEAD_INIT:{
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
		ClusterLowestIdPkt *pkt = new ClusterLowestIdPkt("BROADCAST: CLUSTER_INIT", CLUSTER_FORMATION_PACKET);
		setPktValues(pkt , CLUSTER_INIT, myAddress, myAddress);
		sendBroadcast(pkt);
		//Agenda Mudanca de estado
		debugEV << "Agendando troca de estado para NEIGH_INQ em :"<< headJoinTime << endl;
		cancelAndDelete(delayTimer);
		delayTimer = new cMessage("LID-Timer", SEND_NEIGH_INQ);
		scheduleAt( simTime() + headJoinTime , delayTimer);
		cancelEvent(pollingTimer);
        scheduleAt( simTime() + pollingTime , pollingTimer);
	}
	break;
	case SEND_NEIGH_INQ:{
		//Muda estado para procura de vizinhos
		//assert(msg == delayTimer);
		clusterNodeState = HEAD_NEIGH;

		char tt[50];
		sprintf(tt, "HEAD, MyID:  %i\n HEAD_NEIGH" , myAddress);
		setTTString(tt);
		ClusterLowestIdPkt *pkt;
		for (std::list<int>::iterator it = childs.begin(); it!=childs.end(); ++it) {
		    debugEV << "Sending INQ to: " << *it << endl;
		    pkt = new ClusterLowestIdPkt("BROADCAST: NEIGH_INQ", CLUSTER_FORMATION_PACKET);
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
		pkt = new ClusterLowestIdPkt("DIRECT: HEAD POLLING", CLUSTER_FORMATION_PACKET);
		setPktValues(pkt,CLUSTER_PING, getHeadAddress(), myAddress);
		sendBroadcast(pkt); */
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


void ClusterLowestID::handleNetlayerMsg(cMessage *msg)
{

    ClusterManager::handleNetlayerMsg(msg);
	switch (msg->getKind()) {
		case CLUSTER_FORMATION_PACKET:
			ClusterLowestIdPkt *m;
			m = static_cast<ClusterLowestIdPkt *> (msg);

			//Contabilizando pacotes enviados
			emit(rxMessageSignal,1);
			handleClusterMessage(m);
			delete msg;
			msg = 0;
		break;
	}
}

void ClusterLowestID::handleClusterMessage(ClusterLowestIdPkt* m){
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
	        switch(clusterNodeState){
	        case HEAD_JOIN:
	        case HEAD_MESSAGE:
	                if(msgType== CLUSTER_JOIN){
	                    //Adicionando na lista de filhotes
	                    addChild(m->getSrcAddr());
	                    //childs.push_back(m->getSrcAddr());
	                    //axChilds = childs.size();
	                    //Envia Mensagem de JoinAccept
	                    ClusterLowestIdPkt *pkt = new ClusterLowestIdPkt("DIRECT: CLUSTER_ACCEPTED:", CLUSTER_FORMATION_PACKET);
	                    setPktValues(pkt, CLUSTER_ACCEPTED, m->getSrcAddr(), myAddress);
	                    sendDirectMessage(pkt,m->getSrcAddr());
	                }
	        break;
	        default:
	            EV << "CLUSTER_JOIN in state" << clusterNodeState << endl;
	        }
	    }
	    break;

	case CLUSTER_REJOIN:{
		if(getCurrentRole() == HEAD_NODE){
			ClusterLowestIdPkt *pkt = new ClusterLowestIdPkt("BROADCAST: CLUSTER_INIT", CLUSTER_FORMATION_PACKET);
			setPktValues(pkt , CLUSTER_INIT, myAddress, myAddress);
			sendDirectMessage(pkt,m->getSrcAddr());
			//sendBroadcast(pkt);
		}
	}
	break;
	/*********************
	 *  UNDEFINED Messages
	 *********************/
	//Recebi mensagem de associac‹o
	case CLUSTER_INIT:{
		debugEV << "Received CLUSTER_INIT" <<endl;
		/*
		 * Se esta inicializando aceita a mensagem e tenta tornar-se filhote
		 */
		if(clusterNodeState == INITIALIZING ) {
			debugEV << "Stopping counter," << myAddress << " tring to becoming child of " << m->getHeadId() << endl;

			char tt[50];
			sprintf(tt, "Received INIT from Head %i \n" ,  m->getHeadId() );
			setTTString(tt);
			//Para o proprio timer
			updateSeen();
			cancelAndDelete(delayTimer);
			delayTimer = new cMessage("LID-Timer", REPLY_JOIN);
			delayTimer->addPar("reply-to").setDoubleValue(m->getSrcAddr());
			delayTimer->addPar("headAddress").setDoubleValue(m->getHeadId());
			//Agenda um reply Join
			scheduleAt( simTime()  , delayTimer);
		}

	}//case CLUSTER_INIT
	break;
	/*********************
	*  CHILD Messages
	*********************/
		case CLUSTER_ACCEPTED:
		if(clusterNodeState == CHILD_JOIN){
			debugEV << "Becaming child of " << m->getSrcAddr() << endl;
			clusterNodeState = CHILD_MESSAGE;

			char tt[20];
			sprintf(tt, "Head of: %i" , m->getSrcAddr());
			setTTString(tt);
			setHeadAddress( m->getSrcAddr());
			updateSeen();
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
				ClusterLowestIdPkt *pkt = new ClusterLowestIdPkt("DIRECT: NEIGH_RESP", CLUSTER_FORMATION_PACKET);
				setPktValues(pkt,CLUSTER_NEIGH_RESP, getHeadAddress(), myAddress);
				sendDirectMessage(pkt,m->getSrcAddr());
			}else if(m->getSrcAddr()== getHeadAddress()){
			    updateSeen();
				debugEV << "Searching fror NEIGHBOHRS, resending message" << endl;
				ClusterLowestIdPkt *pkt = new ClusterLowestIdPkt("BROADCAST: NEIGH_INQ", CLUSTER_FORMATION_PACKET);
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
				ClusterLowestIdPkt *pkt = new ClusterLowestIdPkt("DIRECT: NEIGH_RESP", CLUSTER_FORMATION_PACKET);
				setPktValues(pkt, CLUSTER_NEIGH_RESP, m->getHeadId(), m->getOriginId());
				sendDirectMessage(pkt, getHeadAddress());
			}
			break;
		case HEAD_NEIGH:{
			RouteEntry re;
			re.gw 	= m->getSrcAddr();
			re.head = m->getHeadId();
			neighbors.push_back(re);
		}
		break;
		default:
				EV << "STATE NOT HANDLED" << endl;
		break;
		}
	break;
	/* Nao vou mais usar  */
	case CLUSTER_PING:{
		debugEV << "Recebi um PING! de " << m->getSrcAddr() << " Meu headAddress eh: " << getHeadAddress() << endl;
		if(m->getHeadId() == getHeadAddress()) {
			if(getCurrentRole() == HEAD_NODE ){
				ClusterLowestIdPkt *pkt = new ClusterLowestIdPkt("DIRECT: HEAD PONG", CLUSTER_FORMATION_PACKET);
				setPktValues(pkt,CLUSTER_PONG, getHeadAddress(), myAddress);
				sendDirectMessage(pkt,m->getSrcAddr());
			}else if(getCurrentRole() == CHILD_NODE){
				pong=1;
				ClusterLowestIdPkt *pkt = new ClusterLowestIdPkt("DIRECT: CHILD PONG", CLUSTER_FORMATION_PACKET);
				setPktValues(pkt,CLUSTER_PONG, getHeadAddress(), myAddress);
				sendDirectMessage(pkt,m->getSrcAddr());
			}
		}
	}
	break;
	case CLUSTER_PONG:{
		debugEV << "Recebi um PONG! de " << m->getSrcAddr() << " Meu headAddress eh: " << getHeadAddress() << endl;
		if(m->getHeadId() == getHeadAddress()) {
			if(getCurrentRole() == HEAD_NODE ){
				childs_pre.push_back(m->getSrcAddr());
			}else if(getCurrentRole() == CHILD_NODE){
				pong = 1;
			}
		}
	}
	break;
	default:
		EV << "MSG NOT HANDLED" << endl;
		break;
	}
}


void ClusterLowestID::setPktValues(ClusterPkt *pkt, int msgType = 0, int h = 0 , int origAddr = 0 ){
	pkt->setMsgtype(msgType);
    pkt->setHeadId(h);
	pkt->setOriginId(origAddr);
}
