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

#ifndef __CLUSTERSYS_CLUSTERNETNETL_H_
#define __CLUSTERSYS_CLUSTERNETNETL_H_

#include <omnetpp.h>
#include "BaseNetwLayer.h"
#include "SimpleAddress.h"
#include "ClusterPkt_m.h"

/**
 * TODO - Generated class
 */
class MIXIM_API ClusterNetNetl : public BaseNetwLayer
{
protected:
    int ClusterManOut;
protected:
    //virtual void initialize();
 //   virtual NetwPkt* encapsMsg(cPacket*);
    /** @brief decapsulate higher layer message from NetwPkt */
    virtual cMessage* decapsMsg(NetwPkt*);

    /** @brief Handle messages from upper layer */
    virtual void handleUpperMsg(cMessage *);

    /** @brief Handle messages from lower layer */
    virtual void handleLowerMsg(cMessage *);

    /** @brief Add cluster Encaps */
    ClusterPkt* clusterEncaps(cPacket*);

    void sendClusterManager(ClusterPkt*);
};

#endif
