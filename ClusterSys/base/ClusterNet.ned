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

package clustersys.base;
import ned.IdealChannel;
import org.mixim.base.connectionManager.IConnectionManager;
import org.mixim.base.modules.IBaseNetwLayer;
import clustersys.LowestID.ClusterLowestID;

module ClusterNet like IBaseNetwLayer
{
    parameters:
        string ClusterManagerType = default("ClusterManager");
        @display("i=block/fork;bgl=2");

        //Submodule
        bool stats = default(false);
        int headerLength @unit("bit");
        int timeout = default(5); //NodeTimeout

    gates:
        input upperLayerIn; // from application layer
        output upperLayerOut; // to application layer
        input upperControlIn; // control from application layer
        output upperControlOut; // control to application layer
        input lowerLayerIn; // from NIC
        output lowerLayerOut; // to NIC
        input lowerControlIn; // control from NIC
        output lowerControlOut; // control to NIC
    //connections:
    //    upperLayerIn ;

    submodules:
        ClusterManager: <ClusterManagerType> like IClusterManager {
            @display("p=186,86");
            timeout = timeout;

        }
        clusterNetNetl: ClusterNetNetl {
            @display("p=101,86");
            headerLength = headerLength;
            stats = stats;
        }
    connections:
        upperLayerIn --> clusterNetNetl.upperLayerIn;
        upperLayerOut <-- clusterNetNetl.upperLayerOut;

        upperControlIn --> clusterNetNetl.upperControlIn;
        upperControlOut <-- clusterNetNetl.upperControlOut;

        lowerLayerIn --> clusterNetNetl.lowerLayerIn;
        lowerLayerOut <-- clusterNetNetl.lowerLayerOut;

        lowerControlIn --> clusterNetNetl.lowerControlIn;
        lowerControlOut <-- clusterNetNetl.lowerControlOut;

        clusterNetNetl.ClusterManOut --> ClusterManager.NetNetlIn;
        ClusterManager.NetNetlOut --> clusterNetNetl.ClusterManIn;
}
