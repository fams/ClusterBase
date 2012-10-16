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
#include "BaseApplLayer.h"
#include "NetwControlInfo.h"
#include <omnetpp.h>
#include "ClusterLowestIdPkt_m.h"

#include "ClusterManager.h"
#include "ClusterPkt_m.h"


typedef struct { int gw ; int head; } RouteEntry;

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
class ClusterLowestID : public ClusterManager
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
		SEND_HEAD_INIT = LAST_BASE_CLUSTER_MESSAGE_KIND,

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

	/** @brief SHortcut to blackboards Packet category.*/
	int catPacket;

	/** @brief SHortcut to blackboards Packet category.*/
	int catClusterNode;

	/** @brief Tracks the number of packets dropped so far.*/
	long nbPacketDropped;

	/** @brief Timer message to schedule next packet send.*/
	cMessage *delayTimer;


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

    /** @brief ClusterState */
    ClusterNodeStates clusterNodeState;


public:
	virtual ~ClusterLowestID();
	/** @brief Omnet++ Initialisation.*/
	virtual void initialize(int stage);

	/** @brief Called at the end of the simulation to record statistics.*/
	virtual void finish();

protected:

	/** @brief Handle self messages such as timer... */
	virtual void handleSelfMsg(cMessage *msg);


	/** @brief clusterInit */
	virtual void handleClusterMessage(ClusterLowestIdPkt*);

	/** @brief Handle event RESET */
	virtual void handleReset(cMessage *msg);

	/** @brief Handle event  Polling */
	//virtual void ahandlePolling(cMessage *msg);

	/** @brief Trata mensagens vindas da netlayer */
	virtual void handleNetlayerMsg(cMessage*);

	/** @brief preenche o pacote */
	virtual void setPktValues(ClusterPkt *, int , int  , int  );

	int isHeadValid(int TotalChilds, int ActiveChilds);

};

#endif
