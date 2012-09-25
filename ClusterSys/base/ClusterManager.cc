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

#include "ClusterManager.h"
#include "ClusterNodeProperties.h"
#include "Packet.h"
#include <assert.h>

const simsignalwrap_t ClusterManager::catClusterNodeStatsSignal = simsignalwrap_t(CLUSTER_SIGNAL_STATISTICS);

Define_Module(ClusterManager);


void ClusterManager::initialize()
{
    recClusterLifeTime.setName("Cluster Lifetime");
}

void ClusterManager::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()){
        handleSelfMsg(msg);
    } else if(msg->getArrivalGateId()==ClusterManIn) {
        handleNetlayerMsg(msg);
    }
}





ClusterManager::~ClusterManager()
{
}




void ClusterManager::addChild(LAddress::L3Type childAddress)
{
}



void ClusterManager::removeChild(LAddress::L3Type childAddress)
{
}



std::vector<NodeEntry> ClusterManager::getChildList()
{
    return ChildList;
}



void ClusterManager::reset()
{
}



NodePhase ClusterManager::getCurrentPhase()
{
    return currentPhase;
}

NodeRole ClusterManager::getCurrentRole()
{
    return currentRole;
}

void ClusterManager::changeNodeIcon(NodeRole role)
{
    //Pega o Node Pai e troca a imagem
    cDisplayString& dispStr = getParentModule()->getParentModule()->getDisplayString();
    switch(role){
    case HEAD_NODE:
        dispStr.setTagArg("i2",0,"status/green");
    break;
    case CHILD_NODE:
        dispStr.setTagArg("i2",0,"status/yellow");
    break;
    case UNDEFINED_NODE:
        dispStr.setTagArg("i2",0,"status/gray");
    break;
    }

}

void ClusterManager::handleSelfMsg(cMessage *msg)
{
}

void ClusterManager::handleNetlayerMsg(cMessage *msg)
{
}

void ClusterManager::setCurrentPhase(NodePhase newPhase)
{

    currentPhase = newPhase;
}

void ClusterManager::setCurrentRole(NodeRole newRole)
{
    //Publica Mudanca de Role
    ClusterStatisticsPacket c(newRole,currentRole);
    emit(catClusterNodeStatsSignal, &c);

    //Se deixou de ser head, notificar
    if(newRole != currentRole){
        if(currentRole == HEAD_NODE){
            simtime_t lifetime;
            lifetime = simTime() = lastRoleChange;
            recClusterLifeTime.record(lifetime);
        }
        lastRoleChange = simTime();
    }
    currentRole = newRole;
}

void ClusterManager::start()
{
}


