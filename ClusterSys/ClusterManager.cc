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

#include "ClusterManager.h"

Define_Module(ClusterManager);

void ClusterManager::initialize()
{
    // TODO - Generated method body
}

void ClusterManager::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

ClusterManager::ClusterManager()
{
}



ClusterManager::~ClusterManager()
{
}



void ClusterManager::changeType(int NodeType)
{
}



void ClusterManager::addChild(LAddress::L3Type childAddress)
{
}



void ClusterManager::removeChild(LAddress::L3Type childAddress)
{
}



std::vector<NodeEntry> ClusterManager::getChildList()
{
    return ChildList;
}



void ClusterManager::reset()
{
}



void ClusterManager::start()
{
}


