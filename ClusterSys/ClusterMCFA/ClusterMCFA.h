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
private: //types
	MCFAAutomata *Automata;
	//std::map<int,double> ERMi; //ERM dos nodes

	/** Statistics */
	//simsignal_t changeTypeSignal;
	//simsignal_t rxMessageSignal;
	//simsignal_t txMessageSignal;

public: //Types
	/** @brief The message kinds used by this layer.*/
	enum MCFAApplLayerMessageKinds {
		/** self message kinds */
		/** @brief Schedules sending init Cluster.**/
		SEND_ASFREQ = LAST_BASE_CLUSTER_MESSAGE_KIND, //Send Action Set Formation Cluster

		INIT_MCFA,	//Init Cluster Formation

		GET_RERM, //Atualiza todo o RERM

		SEND_JREQ, // pede o Join para os proximos

		PROC_MCFA,	//Processa Formacao

		PROC_MCFA2, //Reset MCFA e recomeca
		/** inter layer message kinds */
		MCFA_CTRL, //Protocol Control Messages

		CLUSTER_FORMATION_PACKET,

		UPDATE_POSITION,

		RESET,

		REPLY_JOIN,

		RESOLV_JOIN,

		/** @brief Polling */
		POLL,

		LAST_CLUSTER_MESSAGE_KIND,
	};

public: //Methods

	virtual void initialize(int stage);

	virtual ~ClusterMCFA();

	/** @brief Called at the end of the simulation to record statistics.*/
	virtual void finish();

protected: //Types
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

    /* ultimo envio de anuncio  */
    simtime_t lastsend;

	/** @brief SHortcut to blackboards Packet category.*/
	int catPacket;

	/** @brief SHortcut to blackboards ClusterState category.*/
	int catClusterNode;

	/* Tracking GPS */
	Move HostMove;

	/*GPS Category */
	int catHostMove;

    /* Save GPS Info */
    GPSInfo gps;

    /** @brief Tentativas de contactar o head */
    int pollAttempt;




	/** @brief Estado do Node na formacao */
	ClusterNodeStates clusterNodeState;

    /** @brief ChildList */
    NodeList childs;
    //parametros para join
    int headCandidate;
    double candidateERM;

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

    /**
     * MERGE
     */
    double mergeMobility;

    double mergeTime;

    bool   mergeEnable;
protected: //Methods
    /*
     * Handlers
     */
    /* Externos */
    /** @brief Handle Cluster Control Messages */
    void handleMCFAControl(ClusterMCFAPkt *m);
    /** @brief Handle messages from lower layer */
    virtual void handleNetlayerMsg(cMessage *);

    void handleMerge(ClusterMCFAPkt *);

    /* Locais */
	/** @brief Handle self messages such as timer... */
	virtual void handleSelfMsg(cMessage *msg);
	/** @brief Handle event RESET */
	virtual void handleReset(cMessage *msg);
    //virtual void handlePolling(cMessage*);
    virtual void handlePingMsg(ClusterPkt *msg);

    /* Polling */
	virtual void HeadPolling(cMessage *msg);
	virtual void ChildPolling(cMessage *msg);
    virtual void UndefinedPolling(cMessage *msg);

/*
 * Algoritmo de formacao
 */
	int MCF();
	void clusterMerge(int);
/*
 * Envio de mensagens
 */

	void sendCHSEL(int NodeAddr);

	void sendLREQ(LAddress::L3Type NodeAddr);

	//*Broadcast MobInfo */
	void sendMobInfo();

/*
 *  Utilitarios
 */
	//Adiciona Informacaod e merge no pacote
	void addMergeInfo(ClusterMCFAPkt *pkt);

	//void updatePosition()

	MobInfo* getMobInfo();

    virtual void updateSeen(LAddress::L3Type);

	void nodeGarbageCollector();
	std::vector<int> str2intList(const char *);
	bool checkOverlap(std::vector<int>);
};

#endif /* MCFAAPPLLAYER_H_ */
