//
// Generated file, do not edit! Created by opp_msgc 4.2 from ClusterLowId.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "ClusterLowId_m.h"

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
    cEnum *e = cEnum::find("ClusterLowIdTypes");
    if (!e) enums.getInstance()->add(e = new cEnum("ClusterLowIdTypes"));
    e->insert(CLUSTER_INIT, "CLUSTER_INIT");
    e->insert(CLUSTER_JOIN, "CLUSTER_JOIN");
    e->insert(CLUSTER_ACCEPTED, "CLUSTER_ACCEPTED");
    e->insert(CLUSTER_NEIGH_INQ, "CLUSTER_NEIGH_INQ");
    e->insert(CLUSTER_NEIGH_RESP, "CLUSTER_NEIGH_RESP");
    e->insert(CLUSTER_PING, "CLUSTER_PING");
    e->insert(CLUSTER_PONG, "CLUSTER_PONG");
    e->insert(CLUSTER_REJOIN, "CLUSTER_REJOIN");
);

Register_Class(ClusterLowIdPkt);

ClusterLowIdPkt::ClusterLowIdPkt(const char *name, int kind) : cPacket(name,kind)
{
    this->msgtype_var = 0;
    this->headId_var = 0;
    this->originId_var = 0;
}

ClusterLowIdPkt::ClusterLowIdPkt(const ClusterLowIdPkt& other) : cPacket(other)
{
    copy(other);
}

ClusterLowIdPkt::~ClusterLowIdPkt()
{
}

ClusterLowIdPkt& ClusterLowIdPkt::operator=(const ClusterLowIdPkt& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void ClusterLowIdPkt::copy(const ClusterLowIdPkt& other)
{
    this->msgtype_var = other.msgtype_var;
    this->headId_var = other.headId_var;
    this->originId_var = other.originId_var;
}

void ClusterLowIdPkt::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->msgtype_var);
    doPacking(b,this->headId_var);
    doPacking(b,this->originId_var);
}

void ClusterLowIdPkt::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->msgtype_var);
    doUnpacking(b,this->headId_var);
    doUnpacking(b,this->originId_var);
}

int ClusterLowIdPkt::getMsgtype() const
{
    return msgtype_var;
}

void ClusterLowIdPkt::setMsgtype(int msgtype)
{
    this->msgtype_var = msgtype;
}

int ClusterLowIdPkt::getHeadId() const
{
    return headId_var;
}

void ClusterLowIdPkt::setHeadId(int headId)
{
    this->headId_var = headId;
}

int ClusterLowIdPkt::getOriginId() const
{
    return originId_var;
}

void ClusterLowIdPkt::setOriginId(int originId)
{
    this->originId_var = originId;
}

class ClusterLowIdPktDescriptor : public cClassDescriptor
{
  public:
    ClusterLowIdPktDescriptor();
    virtual ~ClusterLowIdPktDescriptor();

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

Register_ClassDescriptor(ClusterLowIdPktDescriptor);

ClusterLowIdPktDescriptor::ClusterLowIdPktDescriptor() : cClassDescriptor("ClusterLowIdPkt", "cPacket")
{
}

ClusterLowIdPktDescriptor::~ClusterLowIdPktDescriptor()
{
}

bool ClusterLowIdPktDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<ClusterLowIdPkt *>(obj)!=NULL;
}

const char *ClusterLowIdPktDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int ClusterLowIdPktDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount(object) : 3;
}

unsigned int ClusterLowIdPktDescriptor::getFieldTypeFlags(void *object, int field) const
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
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *ClusterLowIdPktDescriptor::getFieldName(void *object, int field) const
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
    };
    return (field>=0 && field<3) ? fieldNames[field] : NULL;
}

int ClusterLowIdPktDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='m' && strcmp(fieldName, "msgtype")==0) return base+0;
    if (fieldName[0]=='h' && strcmp(fieldName, "headId")==0) return base+1;
    if (fieldName[0]=='o' && strcmp(fieldName, "originId")==0) return base+2;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *ClusterLowIdPktDescriptor::getFieldTypeString(void *object, int field) const
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
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : NULL;
}

const char *ClusterLowIdPktDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "ClusterLowIdTypes";
            return NULL;
        default: return NULL;
    }
}

int ClusterLowIdPktDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    ClusterLowIdPkt *pp = (ClusterLowIdPkt *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ClusterLowIdPktDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    ClusterLowIdPkt *pp = (ClusterLowIdPkt *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getMsgtype());
        case 1: return long2string(pp->getHeadId());
        case 2: return long2string(pp->getOriginId());
        default: return "";
    }
}

bool ClusterLowIdPktDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    ClusterLowIdPkt *pp = (ClusterLowIdPkt *)object; (void)pp;
    switch (field) {
        case 0: pp->setMsgtype(string2long(value)); return true;
        case 1: pp->setHeadId(string2long(value)); return true;
        case 2: pp->setOriginId(string2long(value)); return true;
        default: return false;
    }
}

const char *ClusterLowIdPktDescriptor::getFieldStructName(void *object, int field) const
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
    };
    return (field>=0 && field<3) ? fieldStructNames[field] : NULL;
}

void *ClusterLowIdPktDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    ClusterLowIdPkt *pp = (ClusterLowIdPkt *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


