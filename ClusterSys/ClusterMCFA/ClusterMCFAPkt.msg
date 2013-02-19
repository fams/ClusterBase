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

enum MCFAMessageTypes 
{ 
    	MCFA_ASFREQ 	= 15010;
    	MCFA_ASFREP		= 15020;
    	MCFA_RERM 		= 15030;
    	MCFA_JREQ 		= 15040;
    	MCFA_JREP 		= 15050;
    	MCFA_CHSEL	 	= 15060;
    	MCFA_LREQ 		= 15070;
    	MCFA_MOBINFO	= 15080;
    	MCFA_CLINFO		= 15090;
    	MCFA_REMOVE		= 15100;
}; 
    
packet ClusterMCFAPkt extends ClusterPkt {
	int msgtype enum(MCFAMessageTypes);
	int headId;
	int originId;
	double ERM;
	double Speed;
	double Direction;
	int Question;
	string childinfo;
};