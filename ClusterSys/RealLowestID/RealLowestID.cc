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
		head_candidate = -1;
		pollAttempt = 0;

		createReset(DO_RESET);


	} else if (stage == 1) {

		debugEV << "Meu App Addr e:" << myAddress << endl;
		double schedtime = simTime().dbl() + electionTime;
		cancelEvent(delayTimer);
		scheduleAt( schedtime , delayTimer);
		scheduleAt(simTime() + (dblrand() *2 ), publishTimer);
		addCandidate(myAddress,"");
		debugEV << "Agendando eleicao para" << schedtime <<endl;

	} else {

	}
}

RealLowestID::~RealLowestID() {
	if(delayTimer)
		cancelAndDelete(delayTimer);
	if(pollingTimer)
		cancelAndDelete(pollingTimer);
	if(resetTimer)
	    cancelAndDelete(resetTimer);
}

void RealLowestID::finish()
{
	//recordScalar("dropped", nbPacketDropped);
}

int RealLowestID::isHeadValid(int TotalChilds, int ActiveChilds){

    if (clusterNodeState < HEAD_NEIGH || TotalChilds == 0){
        return 1;
    }
    return (ActiveChilds > (childLostPercentage * TotalChilds));
}

void RealLowestID::migrate(int hc){
    if(getCurrentRole() != HEAD_NODE)
        return;
    Neighbor n = *(findCandidate(hc));
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
    }
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
    RealLowestIDPkt *pkt = new RealLowestIDPkt("DIRECT: CLUSTER_JOIN", CLUSTER_FORMATION_PACKET);
    setPktValues(pkt,CLUSTER_JOIN, hc, myAddress);
    sendDirectMessage(pkt, hc);

    //Agendando Eleicao
    delayTimer = new cMessage("LID-timer", DEFINE_HEAD);
    cancelEvent(delayTimer);
    double schedtime = simTime().dbl() + electionTime;
    debugEV << "Agendando eleicao para " << schedtime <<endl;
    scheduleAt( schedtime , delayTimer);
}
void RealLowestID::handleReset(cMessage *msg){
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
	RealLowestIDPkt *pkt = new RealLowestIDPkt("BROADCAST: CLUSTER_JOIN", CLUSTER_FORMATION_PACKET);
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

        //vou tentar associar a esse head'

	    int hc = getElected();
	    //Se eh a primeira tentativa limpa o contador
	    if (hc != head_candidate){
	        joinAttempts_cur = 0;
	    }else{
	        if(joinAttempts_cur < 2){
	            debugEV << "Tentativa " << joinAttempts_cur << endl;
	        }else{
	            debugEV << "Tentativa " << joinAttempts_cur << ". removendo " << hc << endl;
	            joinAttempts_cur = 0;
	            removeCandidate(hc);
	            hc = getElected();
	        }
	    }
	    head_candidate = hc;
	    debugEV << "Meu endereco e " << myAddress << " o eleito foi " << hc << endl;
        if (hc == myAddress){
            joinAttempts_cur = 0;
            debugEV << "Tornando-me HEAD:" << myAddress << endl;

            char tt[50];
            sprintf(tt, "I'M HEAD, MyID: %i\n" , myAddress);
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
            debugEV << "Node: " <<myAddress << " tentando se afiliar ao no " << hc << endl;

            //Envia Mensagem de Join para o HEAD
            RealLowestIDPkt *pkt = new RealLowestIDPkt("DIRECT: CLUSTER_JOIN", CLUSTER_FORMATION_PACKET);
            setPktValues(pkt,CLUSTER_JOIN, hc, myAddress);
            sendDirectMessage(pkt, hc);

            char tt[50];
            sprintf(tt, "Tentando associar a Head %i" ,  hc );
            setTTString(tt);
            joinAttempts_cur++;

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
		sprintf(tt, "HEAD, MyID:  %i - HEAD_NEIGH" , myAddress);
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
        if(getCurrentRole() == HEAD_NODE){
            //Adicionando na lista de filhotes
            addChild(m->getSrcAddr());
            //Envia Mensagem de JoinAccept
            RealLowestIDPkt *pkt = new RealLowestIDPkt("DIRECT: CLUSTER_ACCEPTED:", CLUSTER_FORMATION_PACKET);
            setPktValues(pkt, CLUSTER_ACCEPTED, m->getSrcAddr(), myAddress);
            sendDirectMessage(pkt,m->getSrcAddr());
        }
	}
	break;

	/*********************
	 *  UNDEFINED Messages
	 *********************/
	case CLUSTER_INFO:{
	    debugEV << "Received node ID\n";
	    std::stringstream ss;
	    std::string buf;
	    ss << m->getListeningNodes();
	    ss >> buf;
	    addCandidate( m->getSrcAddr(),m->getHeadId(),buf);
	    migrate(m->getSrcAddr());
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
			sprintf(tt, "My HEAD: %i" , m->getSrcAddr());
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
    //Avisa se Ž child no sendID
    int head = myAddress;
    if(getCurrentRole() == CHILD_NODE)
        head = getHeadAddress();

    setPktValues(pkt , CLUSTER_INFO, head, myAddress);
    //pkt->setl
    sendBroadcast(pkt);
}

std::vector<Neighbor>::iterator RealLowestID::findCandidate(int c){
    return std::find(listenList.begin(), listenList.end(), Neighbor(c,0,"")) ;
}
std::vector<Neighbor>::iterator RealLowestID::findCandidate(Neighbor c){
    return std::find(listenList.begin(), listenList.end(), c) ;
}

void RealLowestID::addCandidate(int c, std::string nl) {
    std::vector<Neighbor>::iterator it = findCandidate(c);
    if(it!=listenList.end()){
        it->setlastseen(simTime().dbl());
        it->setneighlist(nl);
    }else{
        listenList.push_back(Neighbor(c, simTime().dbl(), ""));
        std::sort(listenList.begin(),listenList.end(),Neighbor::cmp_neigh);
        std::reverse(listenList.begin(),listenList.end());
    }

}
void RealLowestID::addCandidate(int c, int h, std::string nl) {
    std::vector<Neighbor>::iterator it = findCandidate(c);
    if(it!=listenList.end()){
        debugEV << "Atualizando info do node:" << c << " head agora eh: " << h << endl;
        it->setlastseen(simTime().dbl());
        it->sethead(h);
        it->setneighlist(nl);
    }else{
        listenList.push_back(Neighbor(c, h , simTime().dbl(), ""));
        std::sort(listenList.begin(),listenList.end(),Neighbor::cmp_neigh);
        std::reverse(listenList.begin(),listenList.end());
    }

}

void RealLowestID::removeCandidate(int c){
    std::vector<Neighbor>::iterator nc = findCandidate(c);
    if(nc != listenList.end())
        listenList.erase(nc);
}
void RealLowestID::removeCandidate(Neighbor c){
    std::vector<Neighbor>::iterator nc = findCandidate(c);
    if(nc != listenList.end())
        listenList.erase(nc);
}

void RealLowestID::flushCandidates(){
    listenList.clear();
}
/*
 * A eleicao funciona ordenando os n—s vizinhos que est‹o viziveis dentro do tempo de timeout e n‹o s‹o publicamente childs
 * FIXME: Adicionar lastseen
 */
int RealLowestID::getElected(){
    std::sort(listenList.begin(),listenList.end(),Neighbor::cmp_neigh);
    Neighbor *hc = &listenList.back();
    std::vector<Neighbor>::iterator itt = listenList.begin();
    while(itt != listenList.end()){
        if(((itt->getLastseen()+ getNodeTimeout()) < simTime().dbl())&&(itt->getNode() != myAddress)){
            listenList.erase(itt);
        }else{
            itt++;
        }
    }

    for(std::vector<Neighbor>::iterator it = listenList.begin();it != listenList.end(); it++){
        if(it->getNode() == myAddress){
            debugEV << "Selecionado " << it->getNode() << endl;
            hc = &(*it);
            break;
        }
        if(it->isChild())
            continue;
        debugEV << "Selecionado " << it->getNode() << endl;
        hc = &(*it);
        break;
    }
    /*std::reverse(listenList.begin(),listenList.end());
    Neighbor hc = listenList.back();
    while(hc != listenList.front()){
        Neighbor hc = listenList.back();
         if
    */
    debugEV << "Vizinhos: ";
    for(std::vector<Neighbor>::iterator it = listenList.begin(); it != listenList.end(); it++){
        debugEV << it->getNode() << ", ";
    }
    debugEV << endl;
    //removeCandidate(hc);
    return hc->getNode();
}
