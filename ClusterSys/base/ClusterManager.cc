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
#include <assert.h>
#include "FindModule.h"
#include <BaseNetwLayer.h>
#include <AddressingInterface.h>

const simsignalwrap_t ClusterManager::catClusterNodeStatsSignal = simsignalwrap_t(CLUSTER_SIGNAL_STATISTICS);

Define_Module(ClusterManager);


void ClusterManager::initialize(int stage)
{
    BaseModule::initialize(stage);
    if(stage == 0){
        NetNetlIn = findGate("NetNetlIn");
        NetNetlOut = findGate("NetNetlOut");

        /** Init Node */
        setCurrentRole(UNDEFINED_NODE);

        /** Init Node State */
        setCurrentPhase(FORMATION);

    }else if (stage == 1) {

        cModule *netw = FindModule<BaseNetwLayer*>::findSubModule(findHost());
        if(!netw) {
            netw = findHost()->getSubmodule("netw");
            if(!netw) {
                opp_error("Could not find network layer module. This means "
                          "either no network layer module is present or the "
                          "used network layer module does not subclass from "
                          "BaseNetworkLayer.");
            }
        }
        AddressingInterface* addrScheme = FindModule<AddressingInterface*>
                                                    ::findSubModule(findHost());
        if(addrScheme) {
            myAddress = addrScheme->myNetwAddr(netw);
        } else {
            myAddress = netw->getId();
        }
    }
}

void ClusterManager::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()){
        handleSelfMsg(msg);
    } else if(msg->getArrivalGateId()==NetNetlIn) {
        handleNetlayerMsg(msg);
    }
}



ClusterManager::~ClusterManager()
{
}


void ClusterManager::updateSeen(LAddress::L3Type childAddress)
{
    ChildList[childAddress].lastSeen = simTime().dbl();
}


void ClusterManager::addChild(LAddress::L3Type childAddress)
{
    ChildList[childAddress].lastSeen = simTime().dbl();
}



void ClusterManager::removeChild(LAddress::L3Type childAddress)
{
    std::map<LAddress::L3Type,NodeEntry>::iterator it;
    it = ChildList.find(childAddress);
    ChildList.erase(it);
}



std::map<LAddress::L3Type,NodeEntry> ClusterManager::getChildList()
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
    //cDisplayString& dispStr = getParentModule()->getParentModule()->getDisplayString();
    cDisplayString& dispStr = getNodeDisplayString();
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

int ClusterManager::getNodeTimeout() const
{
    return nodeTimeout;
}

void ClusterManager::setNodeTimeout(int nodeTimeout)
{
    this->nodeTimeout = nodeTimeout;
}

int ClusterManager::getHeadAddress() const
{
    return headAddress;
}

void ClusterManager::setHeadAddress(int headAddress)
{
    this->headAddress = headAddress;
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
    changeNodeIcon(newRole);
}

void ClusterManager::start()
{
}

void ClusterManager::sendNetLayer(cMessage *msg)
{
    NetNetlOut = findGate("NetNetlOut");
    send(msg,NetNetlOut);
}

cDisplayString & ClusterManager::getNodeDisplayString()
{
    return findHost()->getDisplayString();
    //return getParentModule()->getParentModule()->getDisplayString();
}


void ClusterManager::setTTString(char *tt)
{
    cDisplayString& dispStr = getNodeDisplayString();
    dispStr.setTagArg("tt", 0, tt);
}


