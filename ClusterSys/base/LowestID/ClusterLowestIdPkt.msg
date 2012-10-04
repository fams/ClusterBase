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
enum ClusterMessageTypes 
{ 
    	CLUSTER_INIT 		= 60010;
    	CLUSTER_JOIN 		= 60020;
    	CLUSTER_ACCEPTED 	= 60030;
    	CLUSTER_NEIGH_INQ 	= 60040;
    	CLUSTER_NEIGH_RESP 	= 60050;
    	CLUSTER_PING 		= 60060;
    	CLUSTER_PONG 		= 60070;
    	CLUSTER_REJOIN		= 60080;
}; 
    
packet ClusterLowestIdPkt extends ClusterPkt {
	int msgtype enum(ClusterMessageTypes);
	int headId;
	int originId;
};