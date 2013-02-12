//
// Generated file, do not edit! Created by opp_msgc 4.2 from RealLowestID/RealLowestIDPkt.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "RealLowestIDPkt_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




EXECUTE_ON_STARTUP(
    cEnum *e = cEnum::find("ClusterLowestIdMessageTypes");
    if (!e) enums.getInstance()->add(e = new cEnum("ClusterLowestIdMessageTypes"));
    e->insert(CLUSTER_INFO, "CLUSTER_INFO");
    e->insert(CLUSTER_INIT, "CLUSTER_INIT");
    e->insert(CLUSTER_JOIN, "CLUSTER_JOIN");
    e->insert(CLUSTER_ACCEPTED, "CLUSTER_ACCEPTED");
    e->insert(CLUSTER_NEIGH_INQ, "CLUSTER_NEIGH_INQ");
    e->insert(CLUSTER_NEIGH_RESP, "CLUSTER_NEIGH_RESP");
    e->insert(CLUSTER_REJOIN, "CLUSTER_REJOIN");
);

Register_Class(RealLowestIDPkt);

RealLowestIDPkt::RealLowestIDPkt(const char *name, int kind) : ClusterPkt(name,kind)
{
    this->msgtype_var = 0;
    this->headId_var = 0;
    this->originId_var = 0;
    this->listeningNodes_var = 0;
}

RealLowestIDPkt::RealLowestIDPkt(const RealLowestIDPkt& other) : ClusterPkt(other)
{
    copy(other);
}

RealLowestIDPkt::~RealLowestIDPkt()
{
}

RealLowestIDPkt& RealLowestIDPkt::operator=(const RealLowestIDPkt& other)
{
    if (this==&other) return *this;
    ClusterPkt::operator=(other);
    copy(other);
    return *this;
}

void RealLowestIDPkt::copy(const RealLowestIDPkt& other)
{
    this->msgtype_var = other.msgtype_var;
    this->headId_var = other.headId_var;
    this->originId_var = other.originId_var;
    this->listeningNodes_var = other.listeningNodes_var;
}

void RealLowestIDPkt::parsimPack(cCommBuffer *b)
{
    ClusterPkt::parsimPack(b);
    doPacking(b,this->msgtype_var);
    doPacking(b,this->headId_var);
    doPacking(b,this->originId_var);
    doPacking(b,this->listeningNodes_var);
}

void RealLowestIDPkt::parsimUnpack(cCommBuffer *b)
{
    ClusterPkt::parsimUnpack(b);
    doUnpacking(b,this->msgtype_var);
    doUnpacking(b,this->headId_var);
    doUnpacking(b,this->originId_var);
    doUnpacking(b,this->listeningNodes_var);
}

int RealLowestIDPkt::getMsgtype() const
{
    return msgtype_var;
}

void RealLowestIDPkt::setMsgtype(int msgtype)
{
    this->msgtype_var = msgtype;
}

int RealLowestIDPkt::getHeadId() const
{
    return headId_var;
}

void RealLowestIDPkt::setHeadId(int headId)
{
    this->headId_var = headId;
}

int RealLowestIDPkt::getOriginId() const
{
    return originId_var;
}

void RealLowestIDPkt::setOriginId(int originId)
{
    this->originId_var = originId;
}

const char * RealLowestIDPkt::getListeningNodes() const
{
    return listeningNodes_var.c_str();
}

void RealLowestIDPkt::setListeningNodes(const char * listeningNodes)
{
    this->listeningNodes_var = listeningNodes;
}

class RealLowestIDPktDescriptor : public cClassDescriptor
{
  public:
    RealLowestIDPktDescriptor();
    virtual ~RealLowestIDPktDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(RealLowestIDPktDescriptor);

RealLowestIDPktDescriptor::RealLowestIDPktDescriptor() : cClassDescriptor("RealLowestIDPkt", "ClusterPkt")
{
}

RealLowestIDPktDescriptor::~RealLowestIDPktDescriptor()
{
}

bool RealLowestIDPktDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<RealLowestIDPkt *>(obj)!=NULL;
}

const char *RealLowestIDPktDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int RealLowestIDPktDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
}

unsigned int RealLowestIDPktDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *RealLowestIDPktDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "msgtype",
        "headId",
        "originId",
        "listeningNodes",
    };
    return (field>=0 && field<4) ? fieldNames[field] : NULL;
}

int RealLowestIDPktDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='m' && strcmp(fieldName, "msgtype")==0) return base+0;
    if (fieldName[0]=='h' && strcmp(fieldName, "headId")==0) return base+1;
    if (fieldName[0]=='o' && strcmp(fieldName, "originId")==0) return base+2;
    if (fieldName[0]=='l' && strcmp(fieldName, "listeningNodes")==0) return base+3;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *RealLowestIDPktDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
        "string",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : NULL;
}

const char *RealLowestIDPktDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "ClusterLowestIdMessageTypes";
            return NULL;
        default: return NULL;
    }
}

int RealLowestIDPktDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    RealLowestIDPkt *pp = (RealLowestIDPkt *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string RealLowestIDPktDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    RealLowestIDPkt *pp = (RealLowestIDPkt *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getMsgtype());
        case 1: return long2string(pp->getHeadId());
        case 2: return long2string(pp->getOriginId());
        case 3: return oppstring2string(pp->getListeningNodes());
        default: return "";
    }
}

bool RealLowestIDPktDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    RealLowestIDPkt *pp = (RealLowestIDPkt *)object; (void)pp;
    switch (field) {
        case 0: pp->setMsgtype(string2long(value)); return true;
        case 1: pp->setHeadId(string2long(value)); return true;
        case 2: pp->setOriginId(string2long(value)); return true;
        case 3: pp->setListeningNodes((value)); return true;
        default: return false;
    }
}

const char *RealLowestIDPktDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<4) ? fieldStructNames[field] : NULL;
}

void *RealLowestIDPktDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    RealLowestIDPkt *pp = (RealLowestIDPkt *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


