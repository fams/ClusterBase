#ifndef GPSINFO_H
#define GPSINFO_H

#include <omnetpp.h>
//#include "ImNotifiable.h"
#include "Move.h"

//#define PI 3.141592654



class  GPSInfo : public cObject{

private:
	simtime_t 	lastPosTime;
	Coord 		lastCoor;
	Coord		currCoor;
	double 		currSpeed;
	double		currDirection;
	double      totalDistance;
	simtime_t   firstMove;

public:
	GPSInfo(){
	    totalDistance = 0;
	    currSpeed = 0;
	    firstMove = simTime();
	}
	void updatePos(Coord *newCoor){
		//Calculando Velocidade
		double distance = newCoor->distance(&currCoor);
		currSpeed = distance/(simTime() - lastPosTime);
		if ((newCoor->y == lastCoor.y) && (newCoor->x == lastCoor.x)){
		   currDirection =0;
		}else{
		    currDirection = atan((newCoor->y - lastCoor.y)/(newCoor->x - lastCoor.x));
		}
		lastCoor  = *newCoor; 
		currCoor  = *newCoor;
		lastPosTime = simTime();
		totalDistance += distance;
	}
	void setDistance(double d){
	    totalDistance = 0;
	}
		
	double getSpeed(){
		return currSpeed;
	}
	Coord getCoor(){
		return currCoor;
	}
	double getDirection(){
		return currDirection ;
	}
	double getTotalDistance(){
	    return totalDistance;
	}
	double getTime(){
	    return simTime().dbl();
	}
	double getAvgSpeed(){
//	    debugEV << totalDistance << " / " << " (" << simTime().dbl() << "-"  << firstMove << ")";
	    return totalDistance/(simTime().dbl()-firstMove);
	}

};

#endif
