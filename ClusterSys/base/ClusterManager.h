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

#ifndef __CLUSTERSYS_CLUSTERMANAGER_H_
#define __CLUSTERSYS_CLUSTERMANAGER_H_

#include <omnetpp.h>
#include <list>

#include "MiXiMDefs.h"
#include "BaseNetwLayer.h"
#include "SimpleAddress.h"
#include "ClusterPkt_m.h"
#include "NetwControlInfo.h"
#include "ClusterNodeProperties.h"
/**
 * TODO - Generated class
 */
typedef struct {
    LAddress::L3Type NodeAddr;
    double lastSeen;
} NodeEntry;

//class ClusterManager :public BaseModule{

class ClusterManager: public BaseModule {

public:
    //@brief SignalID para Estatisticas dos nodes
    static const simsignalwrap_t catClusterNodeStatsSignal;
    simsignal_t rxMessageSignal;
    simsignal_t txMessageSignal;
    enum ClusterMessageTypes {
        CLUSTER_DATA_PACKET = 13001,
        CLUSTER_CONTROL_BLOCK,
        CLUSTER_BASE_PING,
        CLUSTER_BASE_POLL,
        LAST_BASE_CLUSTER_MESSAGE_KIND
    };
protected:
    cOutVector recClusterLifeTime;
    /* @brief Canais vindos da net */
    int NetNetlIn;
    int NetNetlOut;

    /** @brief Application ID */
    int myAddress;

    /** @brief Header lenght */
    int headerLength;

    /** @brief intervalo de polling */
    double pollingTime;

    /** @brief Timer Message to pooling nodes */
    cMessage *pollingTimer;

    /** @brief time to reset entire thing*/
    double resetTime;

    /** @brief Reset message to schedule Reset */
    cMessage *resetTimer;



private:
    /** Statistics */
    //simsignal_t changeTypeSignal;
    /*@brief Child List, aponta para os filhotes */
    std::map<LAddress::L3Type, NodeEntry> ChildList;

    /* @brief Papel Atual */
    NodeRole currentRole;

    /*@brief Fase atual do processo de cluster */
    NodePhase currentPhase;

    /*@brief ultimo papel, usado para calcular mudancas e tempo de vida */
    NodeRole lastRole;

    /* Momento da ultima mudanca de role */
    simtime_t lastRoleChange; //

    /* @brief timeout para o node */
    int nodeTimeout;

    /** @brief HeadId */
    int headAddress;


    /* @brief ultima vez que o head foi visto */
    double headLastSeen;



public:
    ClusterManager() :
            recClusterLifeTime("Cluster Lifetime"), headerLength(0), NetNetlIn(
                    -1), NetNetlOut(-1), currentRole(UNDEFINED_NODE), lastRole(
                    UNDEFINED_NODE) {
    }

    ;
    virtual ~ClusterManager();
    /* @brief Adiciona node na lista de childs */
    void addChild(LAddress::L3Type);

    void removeChild(LAddress::L3Type);

    //std::map<LAddress::L3Type,NodeEntry>::iterator getChild(LAddress::L3Type);

    NodePhase getCurrentPhase();

    NodeRole getCurrentRole();

    /** @brief Called every time a message arrives*/
    virtual void handleMessage(cMessage *msg);
    int getNodeTimeout() const;
    void setNodeTimeout(int nodeTimeout);
    int getHeadAddress() const;
    LAddress::L3Type getAddress();
    void setHeadAddress(int headAddress);

private:
    /** @brief Reset and restart */
    virtual void reset();
    virtual void start();

    //@brief Force Icon of Node
    virtual void changeNodeIcon(NodeRole role);

protected:
    virtual void sendNetLayer(cMessage*);
    virtual void initialize(int stage);
    virtual void handleSelfMsg(cMessage *msg);
    virtual void handleNetlayerMsg(cMessage *msg);
    virtual void handlePolling(cMessage*);
    virtual void HeadPolling(cMessage *msg);
    virtual void ChildPolling(cMessage *msg);
    virtual void UndefinedPolling(cMessage *msg);
    virtual int isHeadValid(int,int){return true;};
    virtual void BroadPing();
    virtual void NodePing(LAddress::L3Type);
    virtual void Pong(LAddress::L3Type);
    virtual void updateSeen(LAddress::L3Type);
    virtual void updateSeen();
    void updateSeenChild(LAddress::L3Type);



    /** @brief preenche o pacote */
    virtual void setPktValues(ClusterPkt *, int , int  , int  );
    /** @brief Send a broadcast message to lower layer. */
    virtual void sendBroadcast(ClusterPkt*);
    /** @brief Send a Message to other node */
    virtual void sendDirectMessage(ApplPkt*, LAddress::L3Type destAddr);
    /* Trata o ping dos hosts */
    virtual void handlePingMsg(ClusterPkt*);
    std::map<LAddress::L3Type, NodeEntry> getChildList();
    void setCurrentRole(NodeRole newRole);
    void setCurrentPhase(NodePhase newPhase);
    cDisplayString& getNodeDisplayString();
    void setTTString(char*);
    virtual void nodeGarbageCollector();

};

#endif
