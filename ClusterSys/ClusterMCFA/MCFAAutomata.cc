/*
 * MCFAAutomata.cpp
 *
 *  Created on: 02/05/2012
 *      Author: fernando
 */

#include "MCFAAutomata.h"
#include <stdlib.h>
#include <math.h>


MCFAAutomata::MCFAAutomata(double R, double P) {
	// TODO Auto-generated constructor stub
	 /* initialize random seed: */
	  srand ( time(NULL) );

	  //Parametros de reforco/punicao
	  rewardP = R;
	  penaltyP = P;
	  epoch = 1;
}

MCFAAutomata::~MCFAAutomata() {
	// TODO Auto-generated destructor stub
	clearAS();
}

void MCFAAutomata::clearAS(){
	for( std::map<int, ActionSetData>::iterator it = ActionSetProperties.begin(); it != ActionSetProperties.end(); it++){
		ActionSetData asd = (*it).second;
		delete asd.Mobility;
	}
	ActionSetProperties.clear();
	ActionSet.clear();
}

double MCFAAutomata::addAction(int node, MobInfo *mi, MobInfo* myMob){
	//FIXME Vc esta criando um leak de memoria
	ActionSet.push_back(node);
	ActionSetProperties[node].Mobility = mi;
	//ActionSetProperties[node].stage = 1;
	double ERMt = RM(myMob, mi);
	ActionSetProperties[node].ERMt = ERMt;
	ActionSetProperties[node].lastseen = simTime();
	return ERMt;
}



double MCFAAutomata::initProb(){
	double p = (double)1/ActionSet.size();
	for( std::map<int, ActionSetData>::iterator it = ActionSetProperties.begin(); it != ActionSetProperties.end(); it++){
		(*it).second.Probability = p;
	}
	return p ;
}
//Atualiza o MOBINFO a cada EPOCH
/*
void MCFAAutomata::updateERM(int node, MobInfo *mi, MobInfo* myMob){
	ActionSetProperties[node].Mobility = mi;
	ActionSetProperties[node].stage++;
	double ERMt = RM(myMob, mi);
	ActionSetProperties[node].ERM = ERMt;
}
*/

double MCFAAutomata::getProbability(int node){
	return ActionSetProperties[node].Probability;
}

void MCFAAutomata::newEpoch(int node, MobInfo* myMob, MobInfo *mi)
{
	int i=0;
	double RMt;
	ActionSetProperties[node].Mobility = mi;
	ActionSetProperties[node].lastseen = simTime();
	for(i=0;i<ActionSet.size();i++){
		RMt = RM(myMob, ActionSetProperties[node].Mobility );

		ActionSetProperties[node].ERMt = ((ActionSetProperties[node].ERMt * epoch) + RMt)/(epoch + 1);
	}
	epoch++;
}
void MCFAAutomata::updateSeen(int node){
    ActionSetProperties[node].lastseen = simTime();
}
double MCFAAutomata::getT(){

}

//Atualiza o vetor de probabilidades quando Reforca
void MCFAAutomata::reward(int Action){
	int i=0;
	for( i=0; i< ActionSet.size();i++){
		if(ActionSet[i] == Action){
			ActionSetProperties[ActionSet[i]].Probability = ActionSetProperties[ActionSet[i]].Probability + rewardP * (1-ActionSetProperties[ActionSet[i]].Probability);
		}else{
			ActionSetProperties[ActionSet[i]].Probability = (1 - rewardP) * ActionSetProperties[ActionSet[i]].Probability;
		}
	}
}
//Atualiza o vetor de probabilidades quando Pune
void MCFAAutomata::penalize(int Action){
	int i=0;
	for(i=0;i<ActionSet.size();i++){
		if(ActionSet[i] == Action){
			ActionSetProperties[ActionSet[i]].Probability = (1 - penaltyP) * ActionSetProperties[ActionSet[i]].Probability;
		}else{
			ActionSetProperties[ActionSet[i]].Probability = (penaltyP/(ActionSet.size() -1)) + ( 1 - penaltyP) * (ActionSetProperties[ActionSet[i]].Probability);
		}
	}
}


double MCFAAutomata::getERMtj(int node){
	return ActionSetProperties[node].ERMt;
}

double MCFAAutomata::getERMt(){
	double sumERM = 0;
	int i = 0;
	for(std::vector<int>::iterator it = ActionSet.begin(); it!= ActionSet.end(); it++){
		if(*it != MyID){
			sumERM += getERMtj(*it);
			i++;
		}
	}
	if(i==0){
	    return 0;
	}else{
	    return sumERM/i;
	}
}



double MCFAAutomata::RM(MobInfo *a, MobInfo *b){
	//RM Equation
	return sqrt((a->speed * a->speed)+(b->speed * b->speed) - (2 * a->speed * b->speed * cos(a->direction - b->direction)));
}

int MCFAAutomata::randNeigh(){

	  /* generate secret number: */
	  int rn = rand() % ActionSet.size();
	  return ActionSet[rn];
}

int MCFAAutomata::getNeigh(int node){
    return ActionSet[node];
}

int MCFAAutomata::getDegree(){
	return (ActionSet.size() - 1) ;

}
void MCFAAutomata::removeAction(int node){

    //Remove o elemento da lista de ActionSet
    ActionSet.erase(std::remove(ActionSet.begin(), ActionSet.end(), node), ActionSet.end());
    //Guarda a sua chance de escolha
    double nodeP = ActionSetProperties[node].Probability;
    //apaga as propriedades
    delete ActionSetProperties[node].Mobility;
    std::map<int, ActionSetData>::iterator it;
    it = ActionSetProperties.find(node);
    ActionSetProperties.erase(it);

    //distribui a probabilidade de forma proporcional
    int i=0;
    for(i=0;i<ActionSet.size();i++){
        ActionSetProperties[ActionSet[i]].Probability += ActionSetProperties[ActionSet[i]].Probability * (nodeP/(1-nodeP) );
    }
}

std::vector<int> MCFAAutomata::garbageCollector(simtime_t threshold){
    std::vector<int> removidos;
    int i;
    for(i=0;i<ActionSet.size();i++){
        if((ActionSetProperties[ActionSet[i]].lastseen < threshold) && (ActionSet[i] != MyID)){
            removidos.push_back(ActionSet[i]);
            removeAction(ActionSet[i]);
        }
    }
    return removidos;
}

void MCFAAutomata::setMyID(double MyID){
	this->MyID = MyID;
}
