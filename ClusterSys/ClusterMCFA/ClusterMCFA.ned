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

package clustersys.ClusterMCFA;
import clustersys.base.ClusterManager;



import org.mixim.base.modules.IBaseApplLayer;


simple ClusterMCFA extends ClusterManager
{
    parameters:
         //@signal[changeType](type="int");
        //@signal[rxMsg](type="int");
        //@statistic[nodeType](title="NodeType"; source="changeType"; record=vector,stats; interpolationmode=none);
        double packetTime @unit(s); // the time a packet needs to be transmitted
        int headerLength @unit(bit);
        double packetsPerPacketTime; // the packets to generate per packet time (regulates traffic)
        //
        int headJoinTime = default(200); //the time node will stay on HeadJoin state
        //@brief Tempo procurando Vizinhos
        int neighInqTime = default(500); //the time node will stay on HeadJoin state
        //@brief Intervalo entre pings
        int pollingTime = default(100);
        //@brief Timeout para RESET
        int resetTime = default(999);
        //Tentativas de retransmissao
        int retransmissionTime = default(10);
        int joinAttempts = default(2);
        double childLostPercentage = default(0.1);
        
        int 	asfreqTime = default(50); //Seconds waiting for ASFREQ
        int 	rermTimeout = default(5); //Espera pelo RERM
        double  ChooseProb = default(0.9);
        double 	rewardP = default (0.1);
        double 	penaltyP = default(0.1);
        double	gpsInterval = default(1.0);
        double  MergeMobility = default(0.1);
        double  MergeTime = default(5);
        bool    MergeEnable = default(false);
 //       int		timeout = default(20);
        
        @class(ClusterMCFA);
        
        
    gates:
}