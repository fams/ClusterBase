/*
 * NodeState.h
 *
 *  Created on: 20/09/2012
 *      Author: fernando
 */

#ifndef CLUSTERNODEPROPERTIES_H_
#define CLUSTERNODEPROPERTIES_H_

#include <sstream>
#include <omnetpp.h>
#include "MiXiMDefs.h"

#define CLUSTER_SIGNAL_STATISTICS "clustersys.nodestatistics"

enum NodeRole {
    HEAD_NODE = 2,
    CHILD_NODE = 1,
    UNDEFINED_NODE= 0,
};
enum NodePhase {
    FORMATION = 0,
    TRANSMISSION = 1,
};

class ClusterStatisticsPacket : public cObject{
    NodeRole currentRole;
    NodeRole lastRole;
public:
    ClusterStatisticsPacket(NodeRole cr = UNDEFINED_NODE, NodeRole lr = UNDEFINED_NODE) :
        cObject(), currentRole(cr), lastRole(lr){

    };
};


#endif /* NODESTATE_H_ */
