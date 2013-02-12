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

cplusplus {{
#include "ClusterPkt_m.h"
}}
packet ClusterPkt;

//Tipos de Mensagem De formacao de cluster
enum ClusterLowestIdMessageTypes 
{ 
    	CLUSTER_INFO		= 15009;
    	CLUSTER_INIT 		= 15010;
    	CLUSTER_JOIN 		= 15020;
    	CLUSTER_ACCEPTED 	= 15030;
    	CLUSTER_NEIGH_INQ 	= 15040;
    	CLUSTER_NEIGH_RESP 	= 15050;
    	CLUSTER_REJOIN		= 15060;
}; 
    
packet RealLowestIDPkt extends ClusterPkt {
	int msgtype enum(ClusterLowestIdMessageTypes);
	int headId;
	int originId;
	string listeningNodes;
	
};