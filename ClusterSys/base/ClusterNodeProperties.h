/*
 * NodeState.h
 *
 *  Created on: 20/09/2012
 *      Author: fernando
 */

#ifndef CLUSTERNODEPROPERTIES2_H_
#define CLUSTERNODEPROPERTIES2_H_

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
/*! Classe para envio de estatisticas para o WorldStatistics*/
class ClusterStatisticsPacket : public cObject{
    NodeRole currentRole;
    NodeRole lastRole;
    simtime_t lifetime;
public:
    /*! Classe para envio de estatisticas para o WorldStatistics*/

    ClusterStatisticsPacket(NodeRole cr = UNDEFINED_NODE, NodeRole lr = UNDEFINED_NODE, simtime_t lt = 0) :
        cObject(), currentRole(cr), lastRole(lr), lifetime(lt){

    };
    /*! Return current Role */
    NodeRole getcurrentRole() const{
        return currentRole;
    }
    /*! Return old Role */
    NodeRole getlastRole() const{
        return lastRole;
    }

    simtime_t getLifeTime() const {
        return lifetime;

    }

};


#endif /* NODESTATE_H_ */
