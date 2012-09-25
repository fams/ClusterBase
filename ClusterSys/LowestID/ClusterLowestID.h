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
//#include "WorldUtilityStats.h"
#include "NetwControlInfo.h"
//#include "ClusterNode.h"
#include <omnetpp.h>
//#include "ClusterInit_m.h"



typedef struct { int gw ; int head; } RouteEntry;

typedef std::list<int> NodeList;
typedef std::vector<RouteEntry> RouteList;

/**
 * @brief A module to generate a certain rate of traffic.
 */
class ClusterLowestID : public BaseApplLayer
{
private:
	/** Statistics */
	//simsignal_t changeTypeSignal;
	simsignal_t rxMessageSignal;
	simsignal_t txMessageSignal;


public:

	/** @brief The message kinds used by this layer.*/
	enum ClusterApplLayerMessageKinds {
		/** @brief Schedules sending init Cluster.*/
		SEND_HEAD_INIT = LAST_BASE_APPL_MESSAGE_KIND,

				/** @brief Schedules sending Neighborhood Inquire*/
		SEND_NEIGH_INQ,

		/** @brief Schedules init Process */
		SEND_PROCESS,

		/** @brief Data Packet.*/
		CLUSTER_DATA_PACKET,

		/** @brief Cluster Init */
		CLUSTER_INIT_PACKET,
		/** @brief Reset to initial state */
		RESET,

		REPLY_JOIN,

		/** @brief Polling */
		POLL,

		/** @brief Sub classing layers shoudl begin their own kinds at this value.*/
		LAST_CLUSTER_MESSAGE_KIND,
	};

protected:


	/** @brief The time it takes to transmit a packet.
	 * Bit length divided by bitrate.*/
	simtime_t packetTime;


	/** @brief Time to stay in HEAD_JOIN state.*/
	double headJoinTime;

	/** @brief time to reset entire thing*/
	double resetTime;

	/** @brief time to restransmit message */
	double retransmissionTime;

	/** @brief Time to stay in HEAD_NEIGH state..*/
	double neighInqTime;

	double pollingTime;

	/** @brief SHortcut to blackboards Packet category.*/
	int catPacket;

	/** @brief SHortcut to blackboards Packet category.*/
	int catClusterNode;

	/** @brief Tracks the number of packets dropped so far.*/
	long nbPacketDropped;

	/** @brief Timer message to schedule next packet send.*/
	cMessage *delayTimer;

	/** @brief Reset message to schedule Reset */
	cMessage *resetTimer;

	/** @brief Timer Message to pooling nodes */

	cMessage *pollingTimer;

	/** @brief Pointer to world utility module.*/
	BaseWorldUtility* world;

	/** @brief ClusterStage */
	ClusterNodeType nodeType;

	/** @brief ClusterState */
	ClusterNodeStates clusterNodeState;

	/** @brief Application ID */
    int myAppAddr;

    /** @brief HeadId */
    int headAddr;

    /** @brief ChildList */
    NodeList childs;

    NodeList childs_pre;

    /** Maximo de filhotes atingidos */
    int maxChilds;

	/** @brief percentage of childs lost accepted */
	double childLostPercentage;

    /* @brief Neighbors list */
    RouteList neighbors;

    int joinAttempts;

    /** @brief Tentativas de contactar o head */
    int pollAttempt;

    /** @brief verifica se o ping retornou ok */
    int pong;

    int joinAttempts_cur ;

    // @brief display String
	// cDisplayString dispStr ;

    simtime_t clusterLifeTime;


public:
	virtual ~ClusterLowestID();
	/** @brief Omnet++ Initialisation.*/
	virtual void initialize(int stage);

	/** @brief Called at the end of the simulation to record statistics.*/
	virtual void finish();

protected:

	/** @brief Handle self messages such as timer... */
	virtual void handleSelfMsg(cMessage *msg);

	/** @brief Handle messages from lower layer */
	virtual void handleLowerMsg(cMessage *msg);



	/** @brief Send a broadcast message to lower layer. */
	virtual void sendBroadcast(ApplPkt*);

	/** @brief Send a Message to other node */
	virtual void sendDirectMessage(ApplPkt*, int);

	/** @brief clusterInit */
	virtual void handleClusterMessage(ClusterInit* m);

	/** @brief change NodeType */
	virtual void changeNodeType(ClusterNodeType n, int head = 0);

	/** @brief prepare packet */
	virtual void setPktValues(ClusterInit*, int , int , int );

	/** @brief Handle event RESET */
	virtual void handleReset(cMessage *msg);

	/** @brief Handle event  Polling */
	virtual void handlePolling(cMessage *msg);

};

#endif
