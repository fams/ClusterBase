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

#ifndef ClusterApplLayer_H_
#define ClusterApplLayer_H_

#include <vector>
#include <String>
#include <algorithm>
#include <sstream>
#include <Move.h>
#include <GPSInfo.h>

#include "BaseApplLayer.h"
#include <BaseMobility.h>
#include "NetwControlInfo.h"
#include <omnetpp.h>
#include "GameFormPkt_m.h"



#include "ClusterManager.h"
#include "ClusterPkt_m.h"



typedef struct { int gw ; int head; } RouteEntry;

class Neighbor2{
private:
    int node;
    double lastseen;
    int head;
    double W;
public:
    Neighbor2(int n, double l, double weight): node(n),lastseen(l),  W(weight){};
    Neighbor2(int n, int h, double l,  double weight): node(n),lastseen(l),head(h), W(weight){};
public:
    void setlastseen(double l){
        lastseen = l;
    };
    void setW(double weight){
        W = weight;
    };
    void sethead(int h){
        head = h;
    }
    static bool cmp_neigh(const Neighbor2 a, const Neighbor2 b){
        return (a.W < b.W);
    }
    static bool same(const Neighbor2 a, const Neighbor2 b){
        return (a.node == b.node );
    }
    bool operator==(const Neighbor2 &other) {
        return (node == other.node);
    }
    bool operator!=( const Neighbor2 &other) {
        return !(*this == other);
    }
    int getNode(){
        return node;
    }
    double getLastseen(){
        return lastseen;
    }
    double getW(){
        return W;
    }
    //Verifica se Ž child
    bool isChild(){
        return (node != head);
    }

};

typedef std::vector<Neighbor2> NeighList2;
typedef std::list<int> NodeList;
typedef std::vector<RouteEntry> RouteList;


enum ClusterNodeStates{
    UNDEFINED       = 0,
    INITIALIZING    = 1,
    HEAD_JOIN       = 2,
    HEAD_NEIGH      = 3,
    HEAD_MESSAGE    = 4,
    CHILD_JOIN      = 5,
    CHILD_MESSAGE   = 6,
    ACTSETFORM      = 7,
    ELECTION        = 8,
    DISCOVER        = 9,
    RUNNING         = 10,
};
/**
 * @brief A module to generate a certain rate of traffic.
 */
class GameForm : public ClusterManager
{
private:

public:

	/** @brief The message kinds used by this layer.*/
	enum ClusterLowestIDSelfMsg {
	    /*
	     * Self Messages
	     *
	     */
		/** @brief Schedules sending init Cluster.*/
		DEFINE_HEAD = LAST_BASE_CLUSTER_MESSAGE_KIND,

		/** @brief Schedules sending Neighbor2hood Inquire*/
		SEND_NEIGH_INQ,

		/** @brief Schedules init Process */
		SEND_PROCESS,

		/** @brief Reset to initial state */
		DO_RESET,

		/** @brief Responde ao pedido de ingresso */
		REPLY_JOIN,

		/** @brief Polling */
		POLLING,

		/** envia o id inicial*/
		SEND_MYID,

		/** INICIA a Eleicao */
		SEND_ELECTION,

		SEND_HEAD_INIT,

        UPDATE_POSITION,

        SEND_JOIN,

		//Inter Clusters
        /** @brief Cluster Init */
        CLUSTER_FORMATION_PACKET,

		/** @brief Sub classing layers shoudl begin their own kinds at this value.*/
		LAST_CLUSTER_MESSAGE_KIND,
	};

protected:
	/*UpdateGPS*/
    double gpsInterval;

    /* Tracking GPS */
    Move HostMove;

    /*GPS Category */
    int catHostMove;

    /* Save GPS Info */
    GPSInfo gps;

	/** @brief The time it takes to transmit a packet.
	 * Bit length divided by bitrate.*/
	simtime_t packetTime;


	/** @brief Time to stay in HEAD_JOIN state.*/
	double headJoinTime;


	/** @brief time to restransmit message */
	double retransmissionTime;

	/** @brief Time to stay in HEAD_NEIGH state..*/
	double neighInqTime;

	double publishTime;

	/**@brief time waiting helo */
	double heloTime;

	/** @brief SHortcut to blackboards Packet category.*/
	int catPacket;

	/** @brief SHortcut to blackboards Packet category.*/
	int catClusterNode;

	/** @brief Tracks the number of packets dropped so far.*/
	long nbPacketDropped;

	int optimalDegree;


	/** @brief Timer message to schedule next packet send.*/
	cMessage *delayTimer;

	cMessage *publishTimer;

    cMessage *sendMobTimer;


	/** @brief Pointer to world utility module.*/
//	BaseWorldUtility* world;

	/** @lista de filhotes desde o ultimo pool */
	std::vector<NodeEntry> ChildLast;


    /** Maximo de filhotes atingidos */
    int maxChilds;

    /** Lista de childs temporaria */
    /** @brief ChildList */
    NodeList childs;

    NodeList childs_pre;

    NeighList2 listenList;

    NeighList2 HeadCandidates;

    int head_candidate;


	/** @brief percentage of childs lost accepted */
	double childLostPercentage;

    /* @brief Neighbor2s list */
    RouteList Neighbor2s;

    int joinAttempts;

    /** @brief Tentativas de contactar o head */
    int pollAttempt;

    /** @brief verifica se o ping retornou ok */
    int pong;

    // @brief Tentativa de Join corrente
    int joinAttempts_cur ;

    int electionTime ;

    int lowestID;

    /** @brief ClusterState */
    ClusterNodeStates clusterNodeState;


public:
	virtual ~GameForm();
	/** @brief Omnet++ Initialisation.*/
	virtual void initialize(int stage);

	/** @brief Called at the end of the simulation to record statistics.*/
	virtual void finish();

protected:

	/** @brief Handle self messages such as timer... */
	virtual void handleSelfMsg(cMessage *msg);


	/** @brief clusterInit */
	virtual void handleClusterMessage(GameFormPkt*);

	/** @brief Handle event RESET */
	virtual void handleReset(cMessage *msg);
	//virtual void handlePingMsg(ClusterPkt *msg);

	/**/
	virtual void ChildPolling(cMessage *msg);

	virtual void sendMyID();

	virtual void sendHeadID();

	virtual void BroadPing();

	virtual void sendElection();

	virtual void sendHeadInit();
	/** @brief Handle event  Polling */
	//virtual void ahandlePolling(cMessage *msg);

	/** @brief Trata mensagens vindas da netlayer */
	virtual void handleNetlayerMsg(cMessage*);


	int isHeadValid(int TotalChilds, int ActiveChilds);

	//Metodos de selecao de candidato
	void addNeighbour(int, double );
	void addNeighbour(int, int ,double);
	void removeNeighbour(int);
	void removeNeighbour(Neighbor2);

    void addCandidate(int,double);
    void removeCandidate(int);
    void removeCandidate(Neighbor2);
    int getHeadCandidate();



	void  flushCandidates();

	int  getElected();

	void migrate(int);

	double getW();

	std::vector<Neighbor2>::iterator findCandidate(int);
    std::vector<Neighbor2>::iterator findCandidate2(int);

    std::vector<Neighbor2>::iterator findCandidate(Neighbor2);


};

#endif
