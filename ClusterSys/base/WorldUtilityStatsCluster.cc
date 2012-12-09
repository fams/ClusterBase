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

#include "WorldUtilityStatsCluster.h"
#include "Packet.h"
#include "BaseLayer.h"
#include "FindModule.h"
#include "ClusterManager.h"

Define_Module(WorldUtilityStatsCluster);

void WorldUtilityStatsCluster::initialize(int stage)
{
	BaseWorldUtility::initialize(stage);
	if(stage == 0) {
		recordVectors = par("recordVectors");
		bitrate = par("bitrate");

		bitsSent = 0;
		bitsReceived = 0;

		//register for global stats to collect
		FindModule<>::findNetwork(this)->subscribe(BaseLayer::catPacketSignal, this);
		FindModule<>::findNetwork(this)->subscribe(ClusterManager::catClusterNodeStatsSignal,this);

		sent.setName("Bits generated");
		rcvd.setName("Bits received");
	}
}


void WorldUtilityStatsCluster::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
	Enter_Method_Silent();
	if(signalID == BaseLayer::catPacketSignal)
	{
		const Packet* p = static_cast<const Packet*>(obj);
		double nbBitsSent = p->getNbBitsSent();
		double nbBitsRcvd = p->getNbBitsReceived();
		bitsSent += nbBitsSent;
		bitsReceived += nbBitsRcvd;

		if(recordVectors) {
			sent.record(bitsSent);
			rcvd.record(bitsReceived);
		}
	}else if (signalID == ClusterManager::catClusterNodeStatsSignal){
	    LAddress::L3Type addr = static_cast<ClusterManager*>(source)->getAddress();

	    const ClusterStatisticsPacket* p = static_cast<const ClusterStatisticsPacket*>(obj);
	    NodeStatusList[addr] = p->getcurrentRole();
	    if (p->getcurrentRole() != p->getlastRole()){
	        if(p->getcurrentRole() == HEAD_NODE){
	            clusterLifeTime.record( p->getLifeTime());
	        }
	    }
        if(recordVectors) {
            recCluster();
        }
	}
}
void WorldUtilityStatsCluster::recCluster(){
    //Global status about cluster Heads
   int numHead=0;
   int numUndef=0;
   int numChild=0;
    for(std::map<LAddress::L3Type,NodeRole>::iterator it = NodeStatusList.begin(); it != NodeStatusList.end(); it++ ){
        switch(it->second){
        case HEAD_NODE:
            numHead++;
            break;
        case CHILD_NODE:
            numChild++;
            break;
        case UNDEFINED_NODE:
            numUndef++;
            break;
        }
    }
    nH.record(numHead);
    nU.record(numUndef);
    nC.record(numChild);
}

void WorldUtilityStatsCluster::finish()
{
	recordScalar("GlobalTrafficGenerated", bitsSent, "bit");
	recordScalar("GlobalTrafficReceived", bitsReceived, "bit");

	if (bitrate) {
		recordScalar("Traffic", bitsSent / bitrate / simTime());
	}
	else {
		recordScalar("Traffic", bitsSent / simTime());
	}
	recCluster();
	double hosts = NodeStatusList.size();
	if(!par("bcTraffic"))
		hosts = 2;
	if (bitrate && hosts > 1) {
		recordScalar("Usage", bitsReceived / bitrate / simTime() / (hosts-1));
	}
	else {
		recordScalar("Usage", bitsReceived / simTime());
	}
}
