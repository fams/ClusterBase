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

package clustersys.RealLowestID;
import clustersys.base.ClusterManager;

//
// TODO auto-generated type
//



simple RealLowestID extends ClusterManager
{
    parameters:
        
        
        //@signal[changeType](type="int");
        //@signal[rxMsg](type="int");
        //@statistic[nodeType](title="NodeType"; source="changeType"; record=vector,stats; interpolationmode=none);
        //@statistic[rxMessageSignal](title="Rx Msg"; source="rxMsg"; record=vector,stats; interpolationmode=none);
        //@statistic[txMessageSignal](title="Rx Msg"; source="rxMsg"; record=vector,stats; interpolationmode=none);
        
        //Necessario pelo base, nao sei se vou usar 
       // double packetTime @unit(s); // the time a packet needs to be transmitted
        //double packetsPerPacketTime = default(0); // the packets to generate per packet time (regulates traffic)
        //bool debug; // debug switch
        
        //Caracteristicas do LowestID
        //@brief Tempo de eleição
        int electionTime = default(10);
        //@brief Tempo procurando Hosts
        int headJoinTime = default(200); //the time node will stay on HeadJoin state
        //@brief Tempo procurando Vizinhos
        int neighInqTime = default(500); //the time node will stay on HeadJoin state
        //@brief Intervalo entre pings
        int pollingTime = default(100);
        //Tentativas de retransmissao
        int retransmissionTime = default(2);
        //@brief Timeout para RESET
        int resetTime = default(999);
        
        int publishTime = default(3);
        //@brief tamanho do cabecalho
        int headerLength @unit(bit);
        //@percentagem de filhotes perdidos antes de declarar o Cluster Morto
        double childLostPercentage = default(0.5);
        @class(RealLowestID);
    gates:

}
