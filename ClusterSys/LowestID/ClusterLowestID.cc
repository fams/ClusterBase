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
#include "ClusterApplLayer.h"
#include <AddressingInterface.h>
#include <cassert>
#include <Packet.h>
#include <BaseNetwLayer.h>
//#include "ClusterInit_m.h"


Define_Module(ClusterLowestID);


void ClusterLowestID::initialize(int stage)
{
	BaseApplLayer::initialize(stage);


	if(stage == 0) {
		/** Init Node Type */
		nodeType = UNDEFINED_NODE;
//emit RESET
		/** Init Node State */
		clusterNodeState = INITIALIZING;

		world = FindModule<BaseWorldUtility*>::findGlobalModule();


		//Messages
		delayTimer 		= new cMessage("delay-timer", SEND_HEAD_INIT);

		resetTimer 		= new cMessage("reset-timer", RESET);

		pollingTimer	= new cMessage("polling-timer", POLL);

		//Parameters

		headJoinTime 		= par("headJoinTime");

		resetTime			= par("resetTime");

		pollingTime = par("pollingTime");

		retransmissionTime	= par("retransmissionTime");

		childLostPercentage = par("childLostPercentage");

		joinAttempts	= par("joinAttempts");

		neighInqTime = par("neighInqTime");



//Utils
		maxChilds = 0;

		nbPacketDropped = 0;
		Packet p(1);
		ClusterNode	cn( UNDEFINED_NODE, UNDEFINED_NODE, myAppAddr);
		catPacket = world->getCategory(&p);
		catClusterNode = world->getCategory(&cn);
		world->publishBBItem(catClusterNode, &cn, -1);

		joinAttempts_cur = 0;
		pollAttempt = 0;

		clusterLifeTime = 0;

		// dispStr = getParentModule()->getDisplayString();
	} else if (stage == 1) {
		// Application address configuration: equals to host address
		//myAppAddr = myApplAddr();
		// Application address configuration: equals to host address

		cModule *netw = FindModule<BaseNetwLayer*>::findSubModule(findHost());
		if(!netw) {
			netw = findHost()->getSubmodule("netw");
			if(!netw) {
				opp_error("Could not find network layer module. This means "
						  "either no network layer module is present or the "
						  "used network layer module does not subclass from "
						  "BaseNetworkLayer.");
			}
		}
		AddressingInterface* addrScheme = FindModule<AddressingInterface*>
													::findSubModule(findHost());
		if(addrScheme) {
			myAppAddr = addrScheme->myNetwAddr(netw);
		} else {
			myAppAddr = netw->getId();
		}
		debugEV << "Meu App Addr e:" << myAppAddr << endl;
		double schedtime = dblrand() * 100;
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

void ClusterLowestID::handlePolling(cMessage *msg){
	debugEV << "Received polling event!!!!\n" << endl;
		switch(nodeType){
		case CHILD_NODE:{
			debugEV << "Handle child Polling" << endl;
			//Verifica se o ping retornou, se nao reset
			if(pong == 1 ){
				pollAttempt=0;
				ClusterInit *pkt = new ClusterInit("DIRECT: CHILD POLLING", CLUSTER_INIT_PACKET);
				setPktValues(pkt,CLUSTER_PING, headAddr, myAppAddr);
				sendDirectMessage(pkt,headAddr);
				pong = 0;
				if(resetTimer)
					cancelEvent(resetTimer);
				//resetTimer = new cMessage("reset-timer", RESET);
				simtime_t resetAt = (simTime() + resetTime + (dblrand() * myAppAddr));
				scheduleAt( resetAt , resetTimer);
			}else if(pollAttempt < 3){
				pollAttempt++;
				ClusterInit *pkt = new ClusterInit("DIRECT: CHILD POLLING", CLUSTER_INIT_PACKET);
				setPktValues(pkt,CLUSTER_PING, headAddr, myAppAddr);
				sendDirectMessage(pkt,headAddr);
			}
			scheduleAt( simTime() + pollingTime , pollingTimer);
		}
		break;
		case HEAD_NODE:{
			debugEV << "Handle head Polling" << endl;
			childs = childs_pre;
			childs_pre.clear();
			//Se perder mais de childLostPercentage, reset
			if( childs.size() < (maxChilds * childLostPercentage )) {
				debugEV "Resetting Head,  max childLost " << maxChilds << " atual " << childs.size() << endl;
				cancelEvent(resetTimer);
				//resetTimer = new cMessage("reset-timer", RESET);
				scheduleAt( simTime() + 1 , resetTimer);
			}else{
				ClusterInit *pkt = new ClusterInit("DIRECT: HEAD POLLING", CLUSTER_INIT_PACKET);
				setPktValues(pkt,CLUSTER_PING, headAddr, myAppAddr);
				sendBroadcast(pkt);
				//pollingTimer = new cMessage("polling-timer", POLL);
				cancelEvent(resetTimer);
				int rndTime = (dblrand() * myAppAddr);
				//resetTimer = new cMessage("reset-timer", RESET);


				scheduleAt( simTime() + resetTime + rndTime , resetTimer);
				scheduleAt( simTime() + pollingTime , pollingTimer);
			}
		}
		break;
		default:
			debugEV << "Type Not Handled" << endl;
		}//switch(nodeType){

}

void ClusterLowestID::handleReset(cMessage *msg){
	//Reseting all
	debugEV << "Reseting  " << endl;
	/** Init Node Type */
	/*
	ClusterNode cn(UNDEFINED_NODE,  nodeType, myAppAddr);
	world->publishBBItem(catClusterNode, &cn, -1);
	nodeType = UNDEFINED_NODE;
	*/
	changeNodeType(UNDEFINED_NODE, -1);

	/** Init Node State */
	clusterNodeState = INITIALIZING;

	bubble("Reseting");
	//Display String
	cDisplayString& dispStr = getParentModule()->getDisplayString();
	/*char* t = new char(20);
	dispStr.insertTag("t",0);
	sprintf(t, "reset %f " , simTime().dbl() + resetTime );
	dispStr.setTagArg("t",  0, t);*/
	//sprintf(t, "Reset at %f", simTime().dbl() + 1);
	char* tt = new char(20);
	sprintf(tt, "MyAddr is %i" , myAppAddr);
	dispStr.setTagArg("tt", 0, tt);
	dispStr.setTagArg("i2", 0, "status/gray");
//	if(delayTimer->isSelfMessage() && delayTimer->isScheduled())
	//	cancelEvent(delayTimer);
	//Tenta o Rejoin
	ClusterInit *pkt = new ClusterInit("BROADCAST: CLUSTER_REJOIN", CLUSTER_INIT_PACKET);
	setPktValues(pkt , CLUSTER_REJOIN, myAppAddr, myAppAddr);
	sendBroadcast(pkt);

	delayTimer = new cMessage("delay-timer", SEND_HEAD_INIT);
	scheduleAt( simTime() + dblrand() * myAppAddr , delayTimer);
}


void ClusterLowestID::handleSelfMsg(cMessage *msg)
{

	switch( msg->getKind()){
	case POLL:{
		handlePolling(msg);
		}
	break;

	case RESET:{
		handleReset(msg);
	}
	break;
	case REPLY_JOIN:{
		debugEV << "Reply Attempt: " << joinAttempts_cur << " de " << joinAttempts << endl;
		//Show Reply Attempt on popup
		cDisplayString& dispStr = getParentModule()->getDisplayString();
		char tt[50];
		sprintf(tt, "Reply attempt %i to H: %i" , joinAttempts_cur, msg->par("headAddr").doubleValue());
		dispStr.setTagArg("tt",0, tt);

		changeNodeType(CHILD_NODE, msg->par("headAddr").doubleValue());
		//Muda estado para CHILD_NOIN
		clusterNodeState = CHILD_JOIN;
		//Envia Mensagem de Join para o HEAD
		ClusterInit *pkt = new ClusterInit("DIRECT: CLUSTER_JOIN", CLUSTER_INIT_PACKET);
		setPktValues(pkt,CLUSTER_JOIN, msg->par("reply-to").doubleValue(), myAppAddr);
		sendDirectMessage(pkt,msg->par("reply-to").doubleValue());
		//FIXME
		//Agenda Mudanca de estado
		debugEV << "Agendando reenvio para " << (simTime()+resetTime) << endl;
		//Se acabaram as tentativas, reset
		if(joinAttempts_cur > joinAttempts){
			cancelAndDelete(delayTimer);
			debugEV << "Agendando Reset para " << (simTime() + 1) << endl;
			delayTimer = new cMessage("delay-timer", RESET);
			scheduleAt(simTime() + 1, delayTimer);
		}else{
			joinAttempts_cur++;
			debugEV << "Msg Type>  " << delayTimer->getKind() <<endl;
			scheduleAt( simTime() + retransmissionTime + (dblrand() * 2) , delayTimer);
		}
	}
	break;
/* HEAD */
	case SEND_HEAD_INIT:{
		debugEV << "Tornando-me HEAD:" << myAppAddr << endl;
		cDisplayString& dispStr = getParentModule()->getDisplayString();
		char tt[50];
		sprintf(tt, "HEAD, MyID:  %i\n HEAD_JOIN" , myAppAddr);
		dispStr.setTagArg("tt",0, tt);
		dispStr.setTagArg("i2",0,"status/green");
		//Se esta enviando, atingiu timeout vira HeadNode
		//assert(msg == delayTimer);
		changeNodeType(HEAD_NODE,myAppAddr);
		//Muda estado para HEAD_NOIN
		clusterNodeState = HEAD_JOIN;
		//Envia Mensagem de join para os filhos
		ClusterInit *pkt = new ClusterInit("BROADCAST: CLUSTER_INIT", CLUSTER_INIT_PACKET);
		setPktValues(pkt , CLUSTER_INIT, myAppAddr, myAppAddr);
		sendBroadcast(pkt);
		//Agenda Mudanca de estado
		debugEV << "Agendando troca de estado para NEIGH_INQ em :"<< headJoinTime << endl;
		cancelAndDelete(delayTimer);
		delayTimer = new cMessage("delay-timer", SEND_NEIGH_INQ);
		scheduleAt( simTime() + headJoinTime , delayTimer);
	}
	break;
	case SEND_NEIGH_INQ:{
		//Muda estado para procura de vizinhos
		//assert(msg == delayTimer);
		clusterNodeState = HEAD_NEIGH;
		cDisplayString& dispStr = getParentModule()->getDisplayString();
		char tt[50];
		sprintf(tt, "HEAD, MyID:  %i\n HEAD_NEIGH" , myAppAddr);
		dispStr.setTagArg("tt",0, tt);
		ClusterInit *pkt;
		for (std::list<int>::iterator it = childs.begin(); it!=childs.end(); ++it) {
		    debugEV << "Sending INQ to: " << *it << endl;
		    pkt = new ClusterInit("BROADCAST: NEIGH_INQ", CLUSTER_INIT_PACKET);
		    setPktValues(pkt, CLUSTER_NEIGH_INQ, myAppAddr, myAppAddr);
		    sendDirectMessage(pkt, *it);
		}
		//Agenda estado de envio de mensagens
		debugEV << "Agendando troca de estado para SEND_PROCESS em :"<< headJoinTime + simTime() << endl;
		cancelEvent(delayTimer);
		delayTimer = new cMessage("delay-timer", SEND_PROCESS);
		scheduleAt(simTime() + neighInqTime , delayTimer);

		//Schedule Polling
		debugEV << "Broadcast a HEAD POLLING " << endl;
		pkt = new ClusterInit("DIRECT: HEAD POLLING", CLUSTER_INIT_PACKET);
		setPktValues(pkt,CLUSTER_PING, headAddr, myAppAddr);
		sendBroadcast(pkt);
		scheduleAt( simTime() + pollingTime , pollingTimer);
	}
	break;
	case SEND_PROCESS:{
		//Em estado de processamento
		debugEV << "Change to SEND: " << endl;
		clusterNodeState = HEAD_MESSAGE;
		cDisplayString& dispStr = getParentModule()->getDisplayString();
		char tt[50];
		sprintf(tt, "HEAD, MyID:  %i\n HEAD_SEND" , myAppAddr);
		dispStr.setTagArg("tt",0, tt);
	}
	break;
	default:
		EV << "Unkown selfmessage! -> delete, kind: "<<msg->getKind() <<endl;
		delete msg;
	}
}


void ClusterLowestID::handleLowerMsg(cMessage *msg)
{


	switch (msg->getKind()) {
		case CLUSTER_INIT_PACKET:
			ClusterInit *m;
			m = static_cast<ClusterInit *> (msg);
			cPacket* pkt = static_cast<cPacket*>(msg);
			Packet p(pkt->getBitLength(), 1, 0);
			world->publishBBItem(catPacket, &p, -1);
			EV << "Publishing Handle MEssage" << endl;
			//Contabilizando pacotes enviados
			emit(rxMessageSignal,1);
			handleClusterMessage(m);
			delete msg;
			msg = 0;
		break;
	}
}

void ClusterLowestID::handleClusterMessage(ClusterInit* m){
	//Acao depende do estado do cluster e tipo de mensagem

	int msgType = m->getMsgtype();
	EV << "Received Msg Type " << msgType << " From: " <<  m->getSrcAddr() << endl;

	switch(msgType){
	case CLUSTER_REJOIN:{
		if(nodeType == HEAD_NODE){
			ClusterInit *pkt = new ClusterInit("BROADCAST: CLUSTER_INIT", CLUSTER_INIT_PACKET);
			setPktValues(pkt , CLUSTER_INIT, myAppAddr, myAppAddr);
			sendDirectMessage(pkt,m->getSrcAddr());
			//sendBroadcast(pkt);
		}
	}
	break;
	//Recebi mensagem de associac‹o
	case CLUSTER_INIT:{
		debugEV << "Received CLUSTER_INIT" <<endl;
		/*
		 * Se esta inicializando aceita a mensagem e tenta tornar-se filhote
		 */
		if(clusterNodeState == INITIALIZING ) {
			debugEV << "Stopping counter," << myAppAddr << " tring to becoming child of " << m->getHeadId() << endl;
			cDisplayString& dispStr = getParentModule()->getDisplayString();
			char tt[50];
			sprintf(tt, "Received INIT from Head %i \n" ,  m->getHeadId() );
			dispStr.setTagArg("tt",0, tt);

			//Para o proprio timer
			cancelAndDelete(delayTimer);
			delayTimer = new cMessage("delay-timer", REPLY_JOIN);
			delayTimer->addPar("reply-to").setDoubleValue(m->getSrcAddr());
			delayTimer->addPar("headAddr").setDoubleValue(m->getHeadId());
			//Agenda um reply Join
			scheduleAt( simTime()  , delayTimer);
		}
/*
 * Nao sei porque fiz assim, vamos testar de outro jeito
 *	}else if(headAddr == m->getSrcAddr()){
			//
			ClusterInit *pkt = new ClusterInit("DIRECT: CLUSTER_JOIN", CLUSTER_INIT_PACKET);
			setPktValues(pkt,CLUSTER_JOIN, m->getSrcAddr(), myAppAddr);
			sendDirectMessage(pkt,m->getSrcAddr());
			//Agenda Mudanca de estado
			debugEV << "Agendando reset para " << (simTime()+headJoinTime) << endl;
			delayTimer = new cMessage("delay-timer", RESET);
			scheduleAt( simTime() + resetTime + (dblrand() * myAppAddr) , delayTimer);
		}
*/
	}//case CLUSTER_INIT
	break;
	case CLUSTER_JOIN:{
		//Recebi um Cluster Join
		debugEV << "Received CLUSTER_JOIN from:" << m->getSrcAddr() << endl;
		switch(clusterNodeState){
		case HEAD_JOIN:
		case HEAD_MESSAGE:
				if(msgType== CLUSTER_JOIN){
					//Adicionando na lista de filhotes
					childs.push_back(m->getSrcAddr());
					maxChilds = childs.size();
					//Envia Mensagem de JoinAccept
					ClusterInit *pkt = new ClusterInit("DIRECT: CLUSTER_ACCEPTED:", CLUSTER_INIT_PACKET);
					setPktValues(pkt, CLUSTER_ACCEPTED, m->getSrcAddr(), myAppAddr);
					sendDirectMessage(pkt,m->getSrcAddr());
				}
		break;
		default:
			EV << "CLUSTER_JOIN in state" << clusterNodeState << endl;
		}
	}
	break;
	case CLUSTER_ACCEPTED:
		if(clusterNodeState == CHILD_JOIN){
			debugEV << "Becaming child of " << m->getSrcAddr() << endl;
			clusterNodeState = CHILD_MESSAGE;
			cDisplayString& dispStr = getParentModule()->getDisplayString();
			char tt[20];
			sprintf(tt, "Head of: %i" , m->getSrcAddr());
			dispStr.setTagArg("tt",0, tt);
			dispStr.setTagArg("i2",0,"status/yellow");
			headAddr = m->getSrcAddr();
			pong = 1;
			joinAttempts_cur = 0;
			cancelEvent(delayTimer);
		}else{
			debugEV << "received CLUSTER_ACCEPTED but not in CHILD_JOIN actual state: " << clusterNodeState << endl;
		}
	break;
	case CLUSTER_NEIGH_INQ:
		debugEV << "Received CLUSTER_NEIGH_INQ from:" << m->getSrcAddr() << endl;
		switch(clusterNodeState){
		case CHILD_MESSAGE:
		case HEAD_MESSAGE:
		case HEAD_NEIGH:
			if(m->getHeadId() != headAddr){
				debugEV << "Received INQ from other HEAD " << endl;
				ClusterInit *pkt = new ClusterInit("DIRECT: NEIGH_RESP", CLUSTER_INIT_PACKET);
				setPktValues(pkt,CLUSTER_NEIGH_RESP, headAddr, myAppAddr);
				sendDirectMessage(pkt,m->getSrcAddr());
			}else if(m->getSrcAddr()== headAddr){
				debugEV << "Searching fror NEIGHBOHRS, resending message" << endl;
				ClusterInit *pkt = new ClusterInit("BROADCAST: NEIGH_INQ", CLUSTER_INIT_PACKET);
				setPktValues(pkt, CLUSTER_NEIGH_INQ, m->getHeadId(), m->getOriginId());
				sendBroadcast(pkt);
			}
		}
	break;
	case CLUSTER_NEIGH_RESP:
		debugEV << "Received CLUSTER_NEIGH_RESP from:" << m->getSrcAddr() << endl;
		//Popula a tabela de rotas, se for um filhote, repassa para o head
		switch(clusterNodeState){
		case CHILD_MESSAGE:
			if(m->getHeadId() != headAddr){
				debugEV << "Received RESP to My HEAD " << endl;
				ClusterInit *pkt = new ClusterInit("DIRECT: NEIGH_RESP", CLUSTER_INIT_PACKET);
				setPktValues(pkt, CLUSTER_NEIGH_RESP, m->getHeadId(), m->getOriginId());
				sendDirectMessage(pkt, headAddr);
			}
		case HEAD_NEIGH:{
			RouteEntry re;
			re.gw 	= m->getSrcAddr();
			re.head = m->getHeadId();
			neighbors.push_back(re);
		}
		break;
		default:
				EV << "STATE NOT HANDLED" << endl;
		}
	break;
	case CLUSTER_PING:{
		debugEV << "Recebi um PING! de " << m->getSrcAddr() << " Meu headAddr eh: " << headAddr << endl;
		if(m->getHeadId() == headAddr) {
			if(nodeType == HEAD_NODE ){
				ClusterInit *pkt = new ClusterInit("DIRECT: HEAD PONG", CLUSTER_INIT_PACKET);
				setPktValues(pkt,CLUSTER_PONG, headAddr, myAppAddr);
				sendDirectMessage(pkt,m->getSrcAddr());
			}else if(nodeType == CHILD_NODE){
				pong=1;
				ClusterInit *pkt = new ClusterInit("DIRECT: CHILD PONG", CLUSTER_INIT_PACKET);
				setPktValues(pkt,CLUSTER_PONG, headAddr, myAppAddr);
				sendDirectMessage(pkt,m->getSrcAddr());
			}
		}
	}
	break;
	case CLUSTER_PONG:{
		debugEV << "Recebi um PONG! de " << m->getSrcAddr() << " Meu headAddr eh: " << headAddr << endl;
		if(m->getHeadId() == headAddr) {
			if(nodeType == HEAD_NODE ){
				childs_pre.push_back(m->getSrcAddr());
			}else if(nodeType == CHILD_NODE){
				pong = 1;
			}
		}
	}
	break;
	default:
		EV << "MSG NOT HANDLED" << endl;
	}
}

void ClusterLowestID::sendBroadcast(ApplPkt* pkt)
{
	pkt->setDestAddr(-1);
	// we use the host modules getIndex() as a appl address
	pkt->setSrcAddr( myAppAddr );
	pkt->setBitLength(headerLength);

	// set the control info to tell the network layer the layer 3
	// address;
	pkt->setControlInfo( new NetwControlInfo(L3BROADCAST) );

	debugEV << "Sending broadcast packet!!\n";
	//Contabilizando pacotes enviados
	emit(txMessageSignal,1);
	sendDown( pkt );
}

void ClusterLowestID::sendDirectMessage(ApplPkt* pkt, int destAddr){
	if(destAddr == myAppAddr){
		return;
	}
	pkt->setDestAddr(destAddr);
	// we use the host modules getIndex() as a appl address
	pkt->setSrcAddr( myAppAddr );
	pkt->setBitLength(headerLength);

	// set the control info to tell the network layer the layer 3
	// address;
	pkt->setControlInfo( new NetwControlInfo(pkt->getDestAddr()) );

	debugEV << "Mensage direta" <<endl;
	debugEV << "Enviando Mensagem direta para " << destAddr << "!!" << endl;
	//Contabilizando pacotes enviados
	emit(txMessageSignal,1);
	sendDown( pkt );
}

void ClusterLowestID::changeNodeType(ClusterNodeType t, int h){
	simtime_t lifetime = 0;
	if(t == HEAD_NODE){
		clusterLifeTime = simTime();
	}else if(nodeType == HEAD_NODE){
		lifetime = clusterLifeTime - simTime();
	}

	ClusterNode cn( t,  nodeType, myAppAddr, lifetime.dbl());
	world->publishBBItem(catClusterNode, &cn, -1);
//Alsterando Tipo de Node
	nodeType = t;
	headAddr = h;
	cancelEvent(pollingTimer);

	if(nodeType == CHILD_NODE){
		debugEV << "Agendando Polling Child";
		scheduleAt( simTime() + pollingTime , pollingTimer);
	}

}
void ClusterLowestID::setPktValues(ClusterInit *pkt, int msgType = 0, int headAddr = 0 , int origAddr = 0 ){
	pkt->setMsgtype(msgType);
	pkt->setHeadId(headAddr);
	pkt->setOriginId(origAddr);
}
