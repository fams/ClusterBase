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

public:
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
	
};

#endif
