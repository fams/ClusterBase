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
// 

#ifndef MCFAAPPLLAYER_H_
#define MCFAAPPLLAYER_H_

#include <vector>
#include <map>
#include <Move.h>
#include "NetwControlInfo.h"
//#include "MCFANode.h"
#include <omnetpp.h>
#include "GPSInfo.h"
#include "MCFAAutomata.h"
#include "ClusterMCFAPkt_m.h"
#include "ClusterManager.h"
#include "ClusterPkt_m.h"

//Lista de Filhos
typedef std::list<int> NodeList;


class ClusterMCFA : public ClusterManager
{

	MCFAAutomata *Automata;
	std::map<int,double> ERMi; //ERM dos nodes

	/** Statistics */
	//simsignal_t changeTypeSignal;
	simsignal_t rxMessageSignal;
	simsignal_t txMessageSignal;

public:
	/** @brief The message kinds used by this layer.*/
	enum MCFAApplLayerMessageKinds {
		/** self message kinds */
		/** @brief Schedules sending init Cluster.**/
		SEND_ASFREQ = LAST_BASE_CLUSTER_MESSAGE_KIND, //Send Action Set Formation Cluster

		INIT_MCFA,	//Init Cluster Formation

		GET_RERM,

		PROC_MCFA,	//Processa Formacao

		PROC_MCFA2, //Reset MCFA e recomeca
		/** inter layer message kinds */
		MCFA_CTRL, //Protocol Control Messages

		CLUSTER_FORMATION_PACKET,

		UPDATE_POSITION,

		RESET,

		REPLY_JOIN,

		/** @brief Polling */
		POLL,

		LAST_CLUSTER_MESSAGE_KIND,
	};

	virtual void initialize(int stage);

	virtual ~ClusterMCFA();

	/** @brief Called at the end of the simulation to record statistics.*/
	virtual void finish();

protected:
	//Intervalo do GPS na versao MiXiM 2.0
	double gpsInterval;

	//BaseUtility utility;
	/** @brief Estagio K da formacao */
	int stageK;

	/* @brief currStage mostra se o algoritmo esta 0: iterando a procura de um ch 1: voltando de um getERM
	 *  tem um ch selecionado
	 */
	int currStage;

	double pollingTime;
	/** @brief time to reset entire thing*/
	double resetTime;
	/** @brief time to restransmit message */
	double retransmissionTime;

	/** @brief Timer message to schedule next packet send.*/
	cMessage *delayTimer;

	cMessage *pollingTimer;

	cMessage *resetTimer;

	cMessage *sendMobTimer;

	cMessage *reinitTimer;

	/* @brief tempo de vida do cluster (tempo de vida do node) */
    simtime_t clusterLifeTime;

	/** @brief SHortcut to blackboards Packet category.*/
	int catPacket;

	/** @brief SHortcut to blackboards ClusterState category.*/
	int catClusterNode;

	/* Tracking GPS */
	Move HostMove;

	/*GPS Category */
	int catHostMove;

    /** @brief Tentativas de contactar o head */
    int pollAttempt;

	/* Save GPS Info */
	GPSInfo gps;


	/** @brief Estado do Node na formacao */
	ClusterNodeStates clusterNodeState;

    /** @brief ChildList */
    NodeList childs;

	/** @brief percentage of childs lost accepted */
	double childLostPercentage;

    /** @brief HeadId */
    int headAddr;

    /* mudanca de dados de GPS */
    int novoGPS;

    /* Tempo de espera para formar o ActionSet */
    double asfreqTime;

    /* Tempo de espera para um RERM */
    double rermTimeout;

    //MCFA Selected ch
    int ch;

    //Curr Weight
    double w;

    //Dynamic Threshold
    double T;

    //configured probability P
    double P;

protected:

	/** @brief Handle self messages such as timer... */
	virtual void handleSelfMsg(cMessage *msg);

	/** @brief Handle messages from lower layer */
	virtual void handleNetlayerMsg(cMessage *);

	/** @brief Handle event RESET */
	virtual void handleReset(cMessage *msg);

	virtual void HeadPolling(cMessage *msg);
	virtual void ChildPolling(cMessage *msg);
    virtual void UndefinedPolling(cMessage *msg);


	int MCF();

	void handleMCFAControl(ClusterMCFAPkt *m);

	//void handleClusterMessage(ClusterMCFAPkt* m);

	MobInfo* getMobInfo();

	void sendCHSEL(int NodeAddr);

	//*Broadcast MobInfo */
	void sendMobInfo();

	void nodeGarbageCollector();

};

#endif /* MCFAAPPLLAYER_H_ */
