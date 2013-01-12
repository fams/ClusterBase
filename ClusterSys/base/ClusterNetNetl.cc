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

#include "ClusterNetNetl.h"

#include <cassert>
#include "NetwPkt_m.h"
#include "ClusterPkt_m.h"
#include "NetwControlInfo.h"
#include "BaseMacLayer.h"
#include "AddressingInterface.h"
#include "SimpleAddress.h"
#include "FindModule.h"
#include "ArpInterface.h"
#include "NetwToMacControlInfo.h"

Define_Module(ClusterNetNetl);

void ClusterNetNetl::initialize(int stage)
{
    BaseNetwLayer::initialize(stage);
    if(stage==0){
        ClusterManIn = findGate("ClusterManIn");
        ClusterManOut = findGate("ClusterManOut");
    }
    // TODO - Generated method body
}


/**
 * All messages have to get a sequence number and the ttl filed has to
 * be specified. Afterwards the messages can be handed to the mac
 * layer. The mac address is set to -1 (broadcast address) because the
 * message is flooded (i.e. has to be send to all neighbors)
 *
 * In the case of plain flooding the message sequence number and
 * source address has also be stored in the bcMsgs list, so that this
 * message will not be rebroadcasted, if a copy will be flooded back
 * from the neigbouring nodes.
 *
 * If the maximum number of entries is reached the first (oldest) entry
 * is deleted.
 **/
void ClusterNetNetl::handleLowerMsg(cMessage* msg)
{

    NetwPkt *m = static_cast<NetwPkt *>(msg);
    coreEV << " handling packet from " << m->getSrcAddr() << std::endl;
    //
    ClusterPkt *Cluster_m = static_cast<ClusterPkt*>(m->decapsulate());
    //delete m;
    if(Cluster_m->getKind() < LAST_BASE_CLUSTER_MESSAGE_KIND){
        cMessage *msg = Cluster_m->decapsulate();
        setUpControlInfo(msg, m->getSrcAddr());
        sendUp(msg);
    }
    sendClusterManager(Cluster_m);
    delete m;
  //  ClusterPkt *m = Cluster_m->decapsulate();

    // delete the netw packet

}

/**
 * Redefine this function if you want to process messages from upper
 * layers before they are send to lower layers.
 *
 * For the BaseNetwLayer we just use the destAddr of the network
 * message as a nextHop
 *
 * To forward the message to lower layers after processing it please
 * use @ref sendDown. It will take care of anything needed
 **/
void ClusterNetNetl::handleUpperMsg(cMessage* msg)
{
    assert(dynamic_cast<cPacket*>(msg));
    sendDown(encapsMsg(static_cast<cPacket*>(clusterEncaps(static_cast<cPacket*>(msg)))));
}
/**
 * Adiciona encapsulamento do cluster quando a mensagem vem da camada superiror
 * FIXME: Quando estiver funcionando adiconar controle do cluster
 */
ClusterPkt* ClusterNetNetl::clusterEncaps(cPacket *appPkt){
    LAddress::L3Type netwAddr;

    EV << "Add cluster header" <<endl;
    ClusterPkt *clPkt = new ClusterPkt(appPkt->getName(), CLUSTER_DATA_PACKET);
    clPkt->setMsgtype(CLUSTER_DATA_PACKET);

    //Getting App CInfo
    cObject* cInfo = appPkt->removeControlInfo();

    clPkt->setDestAddr(netwAddr);
    clPkt->setControlInfo( cInfo );


    clPkt->encapsulate(appPkt);

    return clPkt;
}

/**
 * Decapsulates the packet from the received Network packet
 **/
cMessage* ClusterNetNetl::decapsMsg(NetwPkt *msg)
{

    ClusterPkt *Cluster_m = static_cast<ClusterPkt*>(msg->decapsulate());
    cMessage *m = Cluster_m->decapsulate();
  //  ClusterPkt *m = Cluster_m->decapsulate();
    setUpControlInfo(m, msg->getSrcAddr());
    // delete the netw packet
    delete msg;
    return m;
}

void ClusterNetNetl::handleClusterMsg(cMessage *msg)
{
    assert(dynamic_cast<cPacket*>(msg));
    sendDown(encapsMsg(static_cast<cPacket*>(msg)));
}

void ClusterNetNetl::sendClusterManager(ClusterPkt *msg)
{
    ClusterManOut = findGate("ClusterManOut");
    send(msg,ClusterManOut);
}

void ClusterNetNetl::handleMessage(cMessage* msg)
{
    if (msg->isSelfMessage()){
        handleSelfMsg(msg);
    } else if(msg->getArrivalGateId()==upperLayerIn) {
        recordPacket(PassedMessage::INCOMING,PassedMessage::UPPER_DATA,msg);
        handleUpperMsg(msg);
    } else if(msg->getArrivalGateId()==upperControlIn) {
        recordPacket(PassedMessage::INCOMING,PassedMessage::UPPER_CONTROL,msg);
        handleUpperControl(msg);
    } else if(msg->getArrivalGateId()==lowerControlIn){
        recordPacket(PassedMessage::INCOMING,PassedMessage::LOWER_CONTROL,msg);
        handleLowerControl(msg);
    } else if(msg->getArrivalGateId()==lowerLayerIn) {
        recordPacket(PassedMessage::INCOMING,PassedMessage::LOWER_DATA,msg);
        handleLowerMsg(msg);
    } else if(msg->getArrivalGateId() == ClusterManIn){
        recordPacket(PassedMessage::INCOMING,PassedMessage::UPPER_CONTROL,msg);
        handleClusterMsg(msg);
    }
    else if(msg->getArrivalGateId()==-1) {
        /* Classes extending this class may not use all the gates, f.e.
         * BaseApplLayer has no upper gates. In this case all upper gate-
         * handles are initialized to -1. When getArrivalGateId() equals -1,
         * it would be wrong to forward the message to one of these gates,
         * as they actually don't exist, so raise an error instead.
         */
        opp_error("No self message and no gateID?? Check configuration.");
    } else {
        /* msg->getArrivalGateId() should be valid, but it isn't recognized
         * here. This could signal the case that this class is extended
         * with extra gates, but handleMessage() isn't overridden to
         * check for the new gate(s).
         */
        opp_error("Unknown gateID?? Check configuration or override handleMessage().");
    }
}


