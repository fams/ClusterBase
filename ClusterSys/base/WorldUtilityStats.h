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

#ifndef __WORLDUTILITYSTATS_H__
#define __WORLDUTILITYSTATS_H__

#include <omnetpp.h>
#include "BaseWorldUtility.h"
#include "ClusterNodeProperties.h"

/**
 * @brief Collects global statistics (like channel usage).
 *
 * @ingroup exampleIEEE802154Narrow
 */
class WorldUtilityStats : public BaseWorldUtility,
						  public cListener
{
protected:
	/** @brief Stores the number of bits sent */
	double bitsSent;
	/** @brief Stores the number of bits received */
	double bitsReceived;

	cOutVector sent;
	cOutVector rcvd;

	bool recordVectors;

	double bitrate;


	int catPacket;

	/** @brief BBItem category for Cluster Nodes*/
	NodeRole nodeRole;

	/** @brief Store the number of patckets received*/
	double packetReceived;

	cOutVector rcvdC;

	cOutVector clusterLifeTime;

	 //Global status about cluster Heads
	int numHead;
	int numUndef;
	int numChild;

	//Vetores Globais de Role de Nos
	cOutVector nH,nU,nC;


protected:
    virtual void initialize(int stage);

public:
	WorldUtilityStats()
		: BaseWorldUtility()
		, cListener()
		, bitsSent(0)
		, bitsReceived(0)
		, sent()
		, rcvd()
		, recordVectors(false)
		, bitrate(0)
	    , numHead(0)
	    , numUndef(0)
	    , numChild(0)
	{}
	virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);

	virtual void finish();
};

#endif
