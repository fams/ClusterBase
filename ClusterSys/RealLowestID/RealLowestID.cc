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
#include "RealLowestID.h"
#include <AddressingInterface.h>
#include <cassert>
#include <Packet.h>
#include "FindModule.h"
#include <BaseNetwLayer.h>
#include "RealLowestIDPkt_m.h"



Define_Module(RealLowestID);


void RealLowestID::initialize(int stage)
{
	ClusterManager::initialize(stage);

	if(stage == 0) {

	    //Node State Ž relativo ao LowestID
        clusterNodeState = INITIALIZING;
        //Inicia o node UNDEFINED
        // setCurrentRole(UNDEFINED_NODE);

		//Messages
		delayTimer 		= new cMessage("LID-Timer", DEFINE_HEAD);

		resetTimer 		= new cMessage("reset-timer", DO_RESET);

		pollingTimer	= new cMessage("polling-timer", POLLING);

		publishTimer    = new cMessage("publish-timer", SEND_MYID);


		//Parameters
		electionTime        = par("electionTime");

        pollingTime         = par("pollingTime");

		headJoinTime 		= par("headJoinTime");

		publishTime         = par("publishTime");

		setNodeTimeout((int)par("resetTime"));

		retransmissionTime	= par("retransmissionTime");

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
		double schedtime = simTime().dbl() + electionTime;
		cancelEvent(delayTimer);
		scheduleAt( schedtime , delayTimer);
		scheduleAt(simTime() + (dblrand() *2 ), publishTimer);
		debugEV << "Agendando eleicao para" << schedtime <<endl;

	} else {

	}
}

RealLowestID::~RealLowestID() {
	if(delayTimer)
		cancelAndDelete(delayTimer);
	if(pollingTimer)
		cancelAndDelete(pollingTimer);
}

void RealLowestID::finish()
{
	//recordScalar("dropped", nbPacketDropped);
}

int RealLowestID::isHeadValid(int TotalChilds, int ActiveChilds){

    if (clusterNodeState < HEAD_NEIGH){
        return 1;
    }
    return (ActiveChilds > (childLostPercentage * TotalChilds));
}

void RealLowestID::handleReset(cMessage *msg){
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
	RealLowestIDPkt *pkt = new RealLowestIDPkt("BROADCAST: CLUSTER_REJOIN", CLUSTER_FORMATION_PACKET);
	setPktValues(pkt , CLUSTER_REJOIN, myAddress, myAddress);
	sendBroadcast(pkt);

	delayTimer = new cMessage("LID-timer", DEFINE_HEAD);
    cancelEvent(delayTimer);
	scheduleAt( simTime() + dblrand() * myAddress , delayTimer);
}

/*
 * Mensagens locais
 *
 */

void RealLowestID::handleSelfMsg(cMessage *msg)
{

	switch( msg->getKind()){
	case SEND_MYID:{
	    sendMyID();
	    cancelEvent(publishTimer);
	    scheduleAt(simTime() + publishTime, publishTimer);
	}
	break;
	case POLLING:{
		handlePolling(msg);
		}
	break;

	case DO_RESET:{
		handleReset(msg);
	}
	break;

/* HEAD */
	case DEFINE_HEAD:{

        //vou tentar associar a esse head
        int hc = getElected();
        if (hc == myAddress){
            debugEV << "Tornando-me HEAD:" << myAddress << endl;

            char tt[50];
            sprintf(tt, "HEAD, MyID:  %i\n HEAD_JOIN" , myAddress);
            setTTString(tt);


            //Se esta enviando, atingiu timeout vira HeadNode
            setCurrentRole(HEAD_NODE);
            setHeadAddress(myAddress);

            //Muda estado para HEAD_NOIN
            clusterNodeState = HEAD_JOIN;
            //Agenda Mudanca de estado
            debugEV << "Agendando troca de estado para NEIGH_INQ em :"<< headJoinTime << endl;
            cancelAndDelete(delayTimer);
            delayTimer = new cMessage("LID-Timer", SEND_NEIGH_INQ);
            scheduleAt( simTime() + headJoinTime , delayTimer);
            cancelEvent(pollingTimer);
            scheduleAt( simTime() + pollingTime , pollingTimer);

        }else{
            debugEV << "Tentando associar a um HEAD" <<endl;
            clusterNodeState = CHILD_JOIN;
            debugEV << "Node: " <<myAddress << " tring to becoming child of " << hc << endl;

            //Envia Mensagem de Join para o HEAD
            RealLowestIDPkt *pkt = new RealLowestIDPkt("DIRECT: CLUSTER_JOIN", CLUSTER_FORMATION_PACKET);
            setPktValues(pkt,CLUSTER_JOIN, hc, myAddress);
            sendDirectMessage(pkt, hc);

            char tt[50];
            sprintf(tt, "Tentando associar a Head %i \n" ,  hc );
            setTTString(tt);

            //Agenda nova tentativa
            debugEV << "Msg Type>  " << delayTimer->getKind() <<endl;
            cancelEvent(delayTimer);
            scheduleAt( simTime() + retransmissionTime , delayTimer);
        }
	}
	break;
	case SEND_NEIGH_INQ:{
		//Muda estado para procura de vizinhos
		//assert(msg == delayTimer);
		clusterNodeState = HEAD_NEIGH;

		char tt[50];
		sprintf(tt, "HEAD, MyID:  %i\n HEAD_NEIGH" , myAddress);
		setTTString(tt);
		RealLowestIDPkt *pkt;
		for (std::list<int>::iterator it = childs.begin(); it!=childs.end(); ++it) {
		    debugEV << "Sending INQ to: " << *it << endl;
		    pkt = new RealLowestIDPkt("BROADCAST: NEIGH_INQ", CLUSTER_FORMATION_PACKET);
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
		pkt = new RealLowestIDPkt("DIRECT: HEAD POLLING", CLUSTER_FORMATION_PACKET);
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


void RealLowestID::handleNetlayerMsg(cMessage *msg)
{

    ClusterManager::handleNetlayerMsg(msg);
	switch (msg->getKind()) {
		case CLUSTER_FORMATION_PACKET:
			RealLowestIDPkt *m;
			m = static_cast<RealLowestIDPkt *> (msg);
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
void RealLowestID::handleClusterMessage(RealLowestIDPkt* m){
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
	        if(getCurrentRole() != CHILD_NODE){
	                setCurrentRole(HEAD_NODE);
	                if(msgType== CLUSTER_JOIN){
	                    //Adicionando na lista de filhotes
	                    addChild(m->getSrcAddr());
	                    //Envia Mensagem de JoinAccept
	                    RealLowestIDPkt *pkt = new RealLowestIDPkt("DIRECT: CLUSTER_ACCEPTED:", CLUSTER_FORMATION_PACKET);
	                    setPktValues(pkt, CLUSTER_ACCEPTED, m->getSrcAddr(), myAddress);
	                    sendDirectMessage(pkt,m->getSrcAddr());
	                }
	        }
	    }
	    break;

	case CLUSTER_REJOIN:{
		if(getCurrentRole() == HEAD_NODE){

			RealLowestIDPkt *pkt = new RealLowestIDPkt("BROADCAST: CLUSTER_INIT", CLUSTER_FORMATION_PACKET);
			setPktValues(pkt , CLUSTER_INIT, myAddress, myAddress);
			sendDirectMessage(pkt,m->getSrcAddr());
			//sendBroadcast(pkt);
		}
	}
	break;
	/*********************
	 *  UNDEFINED Messages
	 *********************/
	case CLUSTER_INFO:{
	    debugEV << "Received node ID\n";
	    addCandidate( m->getSrcAddr());
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
			cancelEvent(publishTimer);

			char tt[20];
			sprintf(tt, "Head of: %i" , m->getSrcAddr());
			setTTString(tt);
			setHeadAddress( m->getSrcAddr());
			updateSeen();
			cancelEvent(publishTimer);
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
				RealLowestIDPkt *pkt = new RealLowestIDPkt("DIRECT: NEIGH_RESP", CLUSTER_FORMATION_PACKET);
				setPktValues(pkt,CLUSTER_NEIGH_RESP, getHeadAddress(), myAddress);
				sendDirectMessage(pkt,m->getSrcAddr());
			}else if(m->getSrcAddr()== getHeadAddress()){
			    updateSeen();
				debugEV << "Searching fror NEIGHBOHRS, resending message" << endl;
				RealLowestIDPkt *pkt = new RealLowestIDPkt("BROADCAST: NEIGH_INQ", CLUSTER_FORMATION_PACKET);
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
				RealLowestIDPkt *pkt = new RealLowestIDPkt("DIRECT: NEIGH_RESP", CLUSTER_FORMATION_PACKET);
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
				RealLowestIDPkt *pkt = new RealLowestIDPkt("DIRECT: HEAD PONG", CLUSTER_FORMATION_PACKET);
				setPktValues(pkt,CLUSTER_PONG, getHeadAddress(), myAddress);
				sendDirectMessage(pkt,m->getSrcAddr());
			}else if(getCurrentRole() == CHILD_NODE){
				pong=1;
				RealLowestIDPkt *pkt = new RealLowestIDPkt("DIRECT: CHILD PONG", CLUSTER_FORMATION_PACKET);
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
/*
 * Metodos especificos da implementacao
 * */
void RealLowestID::sendMyID(){
    //Envia info de ID
    RealLowestIDPkt *pkt = new RealLowestIDPkt("BROADCAST: CLUSTER_INFO", CLUSTER_FORMATION_PACKET);
    setPktValues(pkt , CLUSTER_INFO, myAddress, myAddress);
    sendBroadcast(pkt);
}

void RealLowestID::addCandidate(int c) {
    head_candidate.push_back(c);
    head_candidate.sort();
    head_candidate.reverse();
    head_candidate.unique();
}

void RealLowestID::removeCandidate(int c){
    head_candidate.remove(c);
}

void RealLowestID::flushCandidates(){
    head_candidate.clear();
}

int RealLowestID::getElected(){
    head_candidate.sort();
    int hc = head_candidate.back();
    removeCandidate(hc);
    return hc;
}
