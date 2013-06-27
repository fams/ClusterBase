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
#include <assert.h>
#include "FindModule.h"
#include "ClusterPkt_m.h"
#include <BaseNetwLayer.h>
#include <Packet.h>
#include <AddressingInterface.h>
#include "ClusterNodeProperties.h"


const simsignalwrap_t ClusterManager::catClusterNodeStatsSignal = simsignalwrap_t(CLUSTER_SIGNAL_STATISTICS);

Define_Module(ClusterManager);


void ClusterManager::initialize(int stage)
{
    BaseModule::initialize(stage);
    if(stage == 0){
        NetNetlIn = findGate("NetNetlIn");
        NetNetlOut = findGate("NetNetlOut");
        //Polling
        nodeTimeout = par("timeout");
        pollingTimer = new cMessage("polling-timer", CLUSTER_BASE_POLL);
        pollingTime = par("pollingTime");

    }else if (stage == 1) {

        cModule *netw = FindModule<BaseNetwLayer*>::findSubModule(findHost());
        if(!netw) {
            netw = findHost()->getSubmodule("netw");
            if(!netw) {
                opp_error("Could not find network layer module. This means "
                          "either no network layer module is present or the "
                          "used network layer module does not subclass from "
                          "BaseNetworkLayer.");
            }
        }
        AddressingInterface* addrScheme = FindModule<AddressingInterface*>
                                                    ::findSubModule(findHost());
        if(addrScheme) {
            myAddress = addrScheme->myNetwAddr(netw);
        } else {
            myAddress = netw->getId();
        }
        /** Init Node */
        setCurrentRole(UNDEFINED_NODE);

        /** Init Node State */
        setCurrentPhase(FORMATION);
    }
}

void ClusterManager::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()){
        handleSelfMsg(msg);
    } else if(msg->getArrivalGateId()==NetNetlIn) {
        handleNetlayerMsg(msg);
    }
}



ClusterManager::~ClusterManager()
{
}

void ClusterManager::updateSeenChild(LAddress::L3Type childAddress){
    std::map<LAddress::L3Type, NodeEntry>::iterator it;
    it = ChildList.find(childAddress);
    if (it == ChildList.end()){
        return;
    }else {
        ChildList[childAddress].lastSeen = simTime().dbl();
    }
}

void ClusterManager::updateSeen(LAddress::L3Type node)
{
    updateSeenChild(node);
    //ChildList[childAddress].lastSeen = simTime().dbl();
}


void ClusterManager::addChild(LAddress::L3Type childAddress)
{
    debugEV << "Adicionando Child " << childAddress << endl;
    ChildList[childAddress].lastSeen = simTime().dbl();
    debugEV << "Agora tenho: " << ChildList.size() << endl;
}



void ClusterManager::removeChild(LAddress::L3Type childAddress)
{
    std::map<LAddress::L3Type,NodeEntry>::iterator it;
    it = ChildList.find(childAddress);
    if(it != ChildList.end())
        ChildList.erase(it);
}

void ClusterManager::clearChilds(){
    ChildList.clear();
}


std::map<LAddress::L3Type,NodeEntry> ClusterManager::getChildList()
{
    return ChildList;
}



void ClusterManager::reset()
{
}



NodePhase ClusterManager::getCurrentPhase()
{
    return currentPhase;
}

NodeRole ClusterManager::getCurrentRole()
{
    return currentRole;
}

void ClusterManager::changeNodeIcon(NodeRole role)
{
    //Pega o Node Pai e troca a imagem
    //cDisplayString& dispStr = getParentModule()->getParentModule()->getDisplayString();
    if (!ev.isGUI())
            return;
    cDisplayString& dispStr = getNodeDisplayString();
    switch(role){
    case HEAD_NODE:
        dispStr.setTagArg("i2",0,"status/green");
    break;
    case CHILD_NODE:
        dispStr.setTagArg("i2",0,"status/yellow");
    break;
    case UNDEFINED_NODE:
        dispStr.setTagArg("i2",0,"status/gray");
    break;
    }

}

void ClusterManager::handleSelfMsg(cMessage *msg)
{
}
void ClusterManager::updateSeen(){
    setHeadLastSeen(simTime().dbl());
}

void ClusterManager::Pong(LAddress::L3Type node){
    ClusterPkt *pkt = new ClusterPkt("DIRECT: CHILD PONG",
            CLUSTER_BASE_PING);
    setPktValues(pkt, CLUSTER_PONG, getHeadAddress(), myAddress);
    sendDirectMessage(pkt, node);
}
void ClusterManager::handlePingMsg(ClusterPkt *msg) {
    int msgType = msg->getMsgtype();
    switch (getCurrentRole()) {
    case CHILD_NODE: {
        if (msg->getHeadId() != getHeadAddress())
            return;
        if (msgType == CLUSTER_PING) {
            updateSeen();
            debugEV << "Recebi um PING! de " << msg->getSrcAddr()
                    << " Meu headAddress eh: " << getHeadAddress() << endl;
            Pong(msg->getSrcAddr());
        }
    }
        break;
    case HEAD_NODE: {
        if (msgType == CLUSTER_PONG) {
            debugEV << "Recebi um PONG!! de " << msg->getSrcAddr()
                    << " Meu headAddress eh: " << getHeadAddress() << endl;
            /*Verifica se recebeu um pong de um filhote */
            std::map<LAddress::L3Type, NodeEntry>::iterator it;
            it = ChildList.find(msg->getSrcAddr());
            if (it == ChildList.end())
                return;
            updateSeen(msg->getSrcAddr());
        }
    }

        break;

    }
}

void ClusterManager::handleNetlayerMsg(cMessage *msg)
{
    //Contabilizando Pacotes Recebidos
    emit(rxMessageSignal,1);

    switch (msg->getKind()) {
    case CLUSTER_BASE_PING:{
        ClusterPkt *m;
        m = static_cast<ClusterPkt *>(msg);

        //Contabilizando pacotes recebidos
        //emit(rxMessageSignal, 1);
        handlePingMsg(m);
        /*delete msg;
        msg = 0;*/
    }break;
    case CLUSTER_BASE_LEAVE:{
        ClusterPkt *m;
        m = static_cast<ClusterPkt *>(msg);
        handleLeave(m);
    }
    break;
    }
}

int ClusterManager::getNodeTimeout() const
{
    return nodeTimeout;
}

void ClusterManager::setNodeTimeout(int nodeTimeout)
{
    this->nodeTimeout = nodeTimeout;
}

int ClusterManager::getHeadAddress() const
{
    return headAddress;
}
LAddress::L3Type ClusterManager::getAddress(){
    return myAddress;
}

void ClusterManager::setHeadAddress(int headAddress)
{
    this->headAddress = headAddress;
}

void ClusterManager::setCurrentPhase(NodePhase newPhase)
{

    currentPhase = newPhase;
}

void ClusterManager::setCurrentRole(NodeRole newRole)
{
    //Publica Mudanca de Role
    //ClusterStatisticsPacket c(newRole,currentRole);
    //emit(catClusterNodeStatsSignal, &c);
    //double lt=0;
    simtime_t lifetime = 0;
    //Se deixou de ser head, notificar
    if(newRole != currentRole){
        if(currentRole == HEAD_NODE){
            //simtime_t lifetime;
            lifetime = simTime() - lastRoleChange;
            debugEV << "Cluster Lifetime: " << lifetime.dbl() <<endl;
            recClusterLifeTime.record(lifetime);
        }
        lastRoleChange = simTime();
    }
    ClusterStatisticsPacket c(newRole,currentRole,lifetime);
    emit(catClusterNodeStatsSignal, &c);
    currentRole = newRole;
    changeNodeIcon(newRole);
}

void ClusterManager::start()
{
}

void ClusterManager::sendNetLayer(cMessage *msg)
{
    NetNetlOut = findGate("NetNetlOut");
    send(msg,NetNetlOut);
}

cDisplayString & ClusterManager::getNodeDisplayString()
{
    return findHost()->getDisplayString();
    //return getParentModule()->getParentModule()->getDisplayString();
}

void ClusterManager::setTTString(char *tt)
{
    if (!ev.isGUI())
            return;
    cDisplayString& dispStr = getNodeDisplayString();
    dispStr.setTagArg("tt", 0, tt);
}

void ClusterManager::HeadPolling(cMessage *msg){
    debugEV << "Handle head Polling" << endl;
    debugEV << "Iniciando Garbage collector com : " << ChildList.size() << endl;
    nodeGarbageCollector();
    debugEV << "Finalizando Garbage collector com : " << ChildList.size() << endl;
    int ActiveChilds, TotalChilds;
    debugEV << "Lista de afiliatdos tem: " << ChildList.size() << endl;
    ActiveChilds = TotalChilds = ChildList.size();
    BroadPing();
    //debugEV << ": "<< TotalChilds << "\t ActiveChilds: " << ActiveChilds << endl;
    if(isHeadValid(TotalChilds,ActiveChilds)){
        cancelEvent(resetTimer);
        int rndTime = (dblrand() * myAddress);
        scheduleAt( simTime() + nodeTimeout + rndTime , resetTimer);
    }else{
        debugEV << "Tenho menos afiliados que o minimo, agendando um RESET" <<endl;
        SendLeave();
        cancelEvent(resetTimer);
        scheduleAt( simTime() + 0.1, resetTimer);
    }
}

void ClusterManager::ChildPolling(cMessage *msg){
    debugEV << "Handle child Polling" << endl;
    if ((headLastSeen + nodeTimeout )  < simTime().dbl()){
        debugEV << "Estourou o timer, vou resetar" << endl;
        cancelEvent(resetTimer);
        scheduleAt( simTime() + 1, resetTimer);
    }else{
        debugEV << "Vi meu Head em: " << headLastSeen << " timeout em: " << (headLastSeen + nodeTimeout) << endl;
        cancelEvent(resetTimer);
        scheduleAt( simTime() + headLastSeen + nodeTimeout  , resetTimer);
    }
}
void ClusterManager::UndefinedPolling(cMessage *msg){
}
double ClusterManager::getTimeoutAt()
{
    return headLastSeen + nodeTimeout;
}

double ClusterManager::getHeadLastSeen() const
{
    return headLastSeen;
}

void ClusterManager::setHeadLastSeen(double headLastSeen)
{
    this->headLastSeen = headLastSeen;
}

void ClusterManager::createReset(int signal){
    resetTimer      = new cMessage("reset-timer", signal);
}

void ClusterManager::nodeGarbageCollector(){
    int ActiveChilds, TotalChilds;
    ActiveChilds = TotalChilds = ChildList.size();
    if(ActiveChilds<1)
        return;

    std::map<LAddress::L3Type, NodeEntry> ::iterator it = ChildList.begin();
    while(it != ChildList.end()) {
        if (((it->second).lastSeen + nodeTimeout) < simTime().dbl() ){
            ActiveChilds--;
            std::map<LAddress::L3Type, NodeEntry> ::iterator erase = it;
            it++;
            ChildList.erase(erase);
        }else{
            it++;
        }
    }
}
void ClusterManager::NodePing(LAddress::L3Type node){

}
void ClusterManager::BroadPing(){
    ClusterPkt *pkt = new ClusterPkt("DIRECT: HEAD POLLING", CLUSTER_BASE_PING);
    setPktValues(pkt,CLUSTER_PING, getHeadAddress(), myAddress);
    sendBroadcast(pkt);
}
void ClusterManager::SendLeave(){
    ClusterPkt *pkt = new ClusterPkt("DIRECT: HEAD FORCE LEAVE", CLUSTER_BASE_LEAVE);
    setPktValues(pkt,CLUSTER_FORCE_LEAVE, myAddress, myAddress);
    sendBroadcast(pkt);
}
void ClusterManager::NotifyLeave(){
    ClusterPkt *pkt = new ClusterPkt("DIRECT: CHILD NOTIFY LEAVE",
            CLUSTER_BASE_LEAVE);
    setPktValues(pkt, CLUSTER_NOTIFY_LEAVE, getHeadAddress(), myAddress);
    sendDirectMessage(pkt, getHeadAddress());
}
void ClusterManager::handlePolling(cMessage *msg){
    debugEV << "Received polling event!!!!" << endl;
        switch(getCurrentRole()){
        case CHILD_NODE:{
            ChildPolling(msg);
        }
        break;
        case HEAD_NODE:{
            HeadPolling(msg);
        }
        break;
        case UNDEFINED_NODE:
            UndefinedPolling(msg);
            break;
        default:
            debugEV << "Type Not Handled" << getCurrentRole() << endl;
            break;
        }
        //switch(nodeType){

}
void ClusterManager::handleLeave(ClusterPkt *msg){
    int msgType = msg->getMsgtype();
    switch(msgType){
    case CLUSTER_FORCE_LEAVE:{
        if ((getCurrentRole() != HEAD_NODE) && (msg->getHeadId() == getHeadAddress())){
            debugEV << "Recebi uma desconexao forcada, agendando um RESET" <<endl;
            cancelEvent(resetTimer);
            scheduleAt( simTime() + 0.1, resetTimer);
        }
    }
    break;
    case CLUSTER_NOTIFY_LEAVE:{
        if (getCurrentRole() == HEAD_NODE){
            removeChild(msg->getSrcAddr());
        }
    }
    break;
    }
}
void ClusterManager::sendBroadcast(ClusterPkt* pkt)
{
    pkt->setDestAddr(-1);
    // we use the host modules getIndex() as a appl address
    pkt->setSrcAddr( myAddress );
    pkt->setBitLength(headerLength);

    // set the control info to tell the network layer the layer 3
    // address;

    pkt->setControlInfo( new NetwControlInfo(LAddress::L3BROADCAST) );

    debugEV << "Sending broadcast packet!!\n";
    //Contabilizando pacotes enviados
    emit(txMessageSignal,1);
    sendNetLayer( pkt );
}
void ClusterManager::sendDirectMessage(ApplPkt* pkt, LAddress::L3Type destAddr){
    if(destAddr == myAddress){
        return;
    }
    pkt->setDestAddr(destAddr);
    // we use the host modules getIndex() as a appl address
    pkt->setSrcAddr( myAddress );

    // set the control info to tell the network layer the layer 3
    // address;
    pkt->setControlInfo( new NetwControlInfo(pkt->getDestAddr()) );

    debugEV << "Mensage direta" <<endl;
    debugEV << "Enviando Mensagem direta para " << destAddr << "!!" << endl;
    //Contabilizando pacotes enviados
    emit(txMessageSignal,1);
    //Packet p(packetLength, 1, 0)
    //emit(BaseMacLayer::catPacketSignal,&p);
    sendNetLayer( pkt );
    debugEV << "Enviei agora" <<endl;
}


void ClusterManager::setPktValues(ClusterPkt *pkt, int msgType = 0, int h = 0 , int origAddr = 0 ){
    pkt->setMsgtype(msgType);
    pkt->setHeadId(h);
    pkt->setOriginId(origAddr);
}
