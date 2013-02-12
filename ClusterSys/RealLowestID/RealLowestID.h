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

#include "BaseApplLayer.h"
#include "NetwControlInfo.h"
#include <omnetpp.h>
#include "RealLowestIDPkt_m.h"

#include "ClusterManager.h"
#include "ClusterPkt_m.h"



typedef struct { int gw ; int head; } RouteEntry;

class Neighbor{
private:
    int node;
    double lastseen;
    std::string neighlist;
public:
    Neighbor(int n, double l, std::string s): node(n),lastseen(l),neighlist(s) {};
public:
    void setlastseen(double l){
        lastseen = l;
    };
    void setneighlist(std::string s){
        neighlist = s;
    };
    static bool cmp_neigh(const Neighbor a, const Neighbor b){
        return (a.node < b.node);
    }
    static bool same(const Neighbor a, const Neighbor b){
        return (a.node == b.node );
    }
    bool operator==(const Neighbor &other){
        return (node == other.node);
    }
    bool operator!=( const Neighbor &other) {
        return !(*this == other);
    }
    int getNode(){
        return node;
    }
    double getLastseen(){
        return lastseen;
    }

};

typedef std::vector<Neighbor> NeighList;
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
    HEADSELECT      = 8,
    RUNNING         = 9,
};
/**
 * @brief A module to generate a certain rate of traffic.
 */
class RealLowestID : public ClusterManager
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

		/** @brief Schedules sending Neighborhood Inquire*/
		SEND_NEIGH_INQ,

		/** @brief Schedules init Process */
		SEND_PROCESS,

		/** @brief Reset to initial state */
		DO_RESET,

		/** @brief Responde ao pedido de ingresso */
		REPLY_JOIN,

		/** @brief Polling */
		POLLING,

		SEND_MYID,

		SEND_HEAD_INIT,

		//Inter Clusters
        /** @brief Cluster Init */
        CLUSTER_FORMATION_PACKET,

		/** @brief Sub classing layers shoudl begin their own kinds at this value.*/
		LAST_CLUSTER_MESSAGE_KIND,
	};

protected:


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

	/** @brief SHortcut to blackboards Packet category.*/
	int catPacket;

	/** @brief SHortcut to blackboards Packet category.*/
	int catClusterNode;

	/** @brief Tracks the number of packets dropped so far.*/
	long nbPacketDropped;

	/** @brief Timer message to schedule next packet send.*/
	cMessage *delayTimer;

	cMessage *publishTimer;


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

    NeighList listenList;

    int head_candidate;


	/** @brief percentage of childs lost accepted */
	double childLostPercentage;

    /* @brief Neighbors list */
    RouteList neighbors;

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
	virtual ~RealLowestID();
	/** @brief Omnet++ Initialisation.*/
	virtual void initialize(int stage);

	/** @brief Called at the end of the simulation to record statistics.*/
	virtual void finish();

protected:

	/** @brief Handle self messages such as timer... */
	virtual void handleSelfMsg(cMessage *msg);


	/** @brief clusterInit */
	virtual void handleClusterMessage(RealLowestIDPkt*);

	/** @brief Handle event RESET */
	virtual void handleReset(cMessage *msg);

	/**/
	virtual void sendMyID();

	/** @brief Handle event  Polling */
	//virtual void ahandlePolling(cMessage *msg);

	/** @brief Trata mensagens vindas da netlayer */
	virtual void handleNetlayerMsg(cMessage*);


	int isHeadValid(int TotalChilds, int ActiveChilds);

	//Metodos de selecao de candidato
	void addCandidate(int, std::string );

	void removeCandidate(int);

    void removeCandidate(Neighbor);

	void  flushCandidates();

	int  getElected();

	std::vector<Neighbor>::iterator findCandidate(int);
    std::vector<Neighbor>::iterator findCandidate(Neighbor);





};

#endif
