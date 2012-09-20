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

#ifndef __CLUSTERSYS_CLUSTERMANAGER_H_
#define __CLUSTERSYS_CLUSTERMANAGER_H_

#include <omnetpp.h>
#include <list>


#include "MiXiMDefs.h"
#include "BaseNetwLayer.h"
#include "SimpleAddress.h"
/**
 * TODO - Generated class
 */
typedef struct { LAddress::L3Type NodeAddr; int lastSeen; } NodeEntry;

//class ClusterManager :public BaseModule{

class ClusterManager : public BaseModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    std::vector<NodeEntry> ChildList;
    int NodeType;
public:
    ClusterManager();
    virtual ~ClusterManager();
    void    changeType(int);
    void    addChild(LAddress::L3Type);
    void    removeChild(LAddress::L3Type);
    std::vector<NodeEntry> getChildList();
private:
    virtual void reset();
    virtual void start();
};

#endif
