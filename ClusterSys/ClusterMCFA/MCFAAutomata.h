/*
 * MCFAAutomata.h
 *
 *  Created on: 02/05/2012
 *      Author: fernando
 */

#ifndef MCFAAUTOMATA_H_
#define MCFAAUTOMATA_H_
#include <vector>
#include <map>
#include <algorithm>
#include <omnetpp.h>

enum ClusterNodeStates{
    UNDEFINED       = 0,
    INITIALIZING    = 1,
    HEAD_JOIN       = 2,
    HEAD_NEIGH      = 3,
    HEAD_MESSAGE    = 4,
    CHILD_JOIN      = 5,
    CHILD_MESSAGE   = 6,
    ACTSETFORM      = 7,
    HEADSELECT      = 8,
    RUNNING         = 9,
};


typedef struct MobInfo {double speed; double direction;} MobInfo;
typedef struct ActionSetData {double Probability; double ERMt; MobInfo *Mobility; simtime_t lastseen;};

class MCFAAutomata {
	//Action Set
	std::vector<int> ActionSet;
	std::map<int, ActionSetData> ActionSetProperties;

	double rewardP;
	double penaltyP;
	double MyID;
	int epoch;

public:
	MCFAAutomata(double R, double P );

	virtual ~MCFAAutomata();

	//* @brief Adiciona entrada ao AC
	double addAction(int node, MobInfo *mi, MobInfo* myMob);

	void removeAction(int node);

	//* @brief init Probability */
	double initProb();

	//* @brief Limpa Action Set
	void clearAS();

/* inutil	void updateERM(int node, MobInfo *mi, MobInfo* myMob); */

	/** @brief return a random neighborh*/
	int randNeigh();
	int getNeigh(int node);
 /** New Epoch */
	void newEpoch(int node,MobInfo* myMob,  MobInfo* mi);

	/** Retorna o ERMt do proprio node */
	double getERMt();

	/** @brief retorna o ERMt de um node especifico
	 *
	 */
	double getERMtj(int node);

	/** @brief Retorna a media movel T */
	double getT();

	/** @brief calcula a mobilidade relativa entre dois nodes */
	double RM(MobInfo* a, MobInfo* b);

	double getProbability(int node);

	void penalize(int Action);

	void reward(int Action);

	/* @brief retorna o grau do node */
	int getDegree();

	std::vector<int> garbageCollector(simtime_t);

    void setMyID(double MyID);


};



#endif /* MCFAAUTOMATA_H_ */
