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
#include "NetwPkt_m.h"
#include "ArpInterface.h"
#include "NetwToMacControlInfo.h"

Define_Module(ClusterNetNetl);
/*
void ClusterNetNetl::initialize()
{
    // TODO - Generated method body
}
*/

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
    sendUp(decapsMsg(m));
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
    sendDown(encapsMsg(static_cast<cPacket*>(msg)));
}



NetwPkt* ClusterNetNetl::encapsMsg(cPacket *appPkt) {
    LAddress::L2Type macAddr;
    LAddress::L3Type netwAddr;

    EV <<"in encaps...\n";
    ClusterPkt *clPkt = new ClusterPkt(appPkt->getName(), appPkt->getKind());

    NetwPkt *pkt = new NetwPkt(clPkt->getName(), clPkt->getKind());
    pkt->setBitLength(headerLength);

    cObject* cInfo = appPkt->removeControlInfo();

    if(cInfo == NULL){
    EV << "warning: Application layer did not specifiy a destination L3 address\n"
       << "\tusing broadcast address instead\n";
    netwAddr = LAddress::L3BROADCAST;
    } else {
    EV <<"CInfo removed, netw addr="<< NetwControlInfo::getAddressFromControlInfo( cInfo ) << std::endl;
        netwAddr = NetwControlInfo::getAddressFromControlInfo( cInfo );
    delete cInfo;
    }

    pkt->setSrcAddr(myNetwAddr);
    pkt->setDestAddr(netwAddr);
    EV << " netw "<< myNetwAddr << " sending packet" <<std::endl;
    if(LAddress::isL3Broadcast( netwAddr )) {
        EV << "sendDown: nHop=L3BROADCAST -> message has to be broadcasted"
           << " -> set destMac=L2BROADCAST\n";
        macAddr = LAddress::L2BROADCAST;
    }
    else{
        EV <<"sendDown: get the MAC address\n";
        macAddr = arp->getMacAddr(netwAddr);
    }

    setDownControlInfo(pkt, macAddr);
    clPkt->encapsulate(appPkt);
    //encapsulate the application packet
    pkt->encapsulate(clPkt);
    EV <<" pkt encapsulated\n";
    return pkt;
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
