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
#include "ClusterNodeProperties.h"
/**
 * TODO - Generated class
 */
typedef struct { LAddress::L3Type NodeAddr; int lastSeen; } NodeEntry;

//class ClusterManager :public BaseModule{

class ClusterManager : public BaseModule
{

public:
    //@brief SignalID para Estatisticas dos nodes
    const static simsignalwrap_t catClusterNodeStatsSignal;

protected:
    cOutVector recClusterLifeTime;
    int ClusterManIn;
    int ClusterManOut;

private:
    std::vector<NodeEntry> ChildList;
    NodeRole  currentRole; //Papel Atual do Node
    NodePhase currentPhase; //Fase atual do processo de cluster
    NodeRole  lastRole;   //ultimo papel, usado para calcular mudancas e tempo de vida
    simtime_t lastRoleChange; //Tempo da ultima mudanca de role
    int nodeTimeout;

    /** @brief Application ID */
    int myAddress;

    /** @brief HeadId */
    int headAddress;

public:
    ClusterManager():
            recClusterLifeTime(),
            ClusterManIn(-1),
            ClusterManOut(-1),
            lastRole(UNDEFINED_NODE),
            currentRole(UNDEFINED_NODE)
    {};
    virtual ~ClusterManager();
    void    addChild(LAddress::L3Type);
    void    removeChild(LAddress::L3Type);
    NodePhase getCurrentPhase();
    NodeRole getCurrentRole();

    /** @brief Called every time a message arrives*/
    virtual void handleMessage(cMessage *msg);

private:
    virtual void reset();
    virtual void start();

    //@brief Force Icon of Node
    virtual void changeNodeIcon(NodeRole role);


protected:
    virtual void initialize();
    virtual void handleSelfMsg(cMessage *msg);
    virtual void handleNetlayerMsg(cMessage *msg);
    std::vector<NodeEntry> getChildList();
    void setCurrentRole(NodeRole newRole);
    void setCurrentPhase(NodePhase newPhase);
};



#endif
