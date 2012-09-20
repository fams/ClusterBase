//
// Generated file, do not edit! Created by opp_msgc 4.2 from ClusterMCFA.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "ClusterMCFA_m.h"

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
    cEnum *e = cEnum::find("MCFAMessageTypes");
    if (!e) enums.getInstance()->add(e = new cEnum("MCFAMessageTypes"));
    e->insert(MCFA_ASFREQ, "MCFA_ASFREQ");
    e->insert(MCFA_ASFREP, "MCFA_ASFREP");
    e->insert(MCFA_RERM, "MCFA_RERM");
    e->insert(MCFA_JREQ, "MCFA_JREQ");
    e->insert(MCFA_JREP, "MCFA_JREP");
    e->insert(MCFA_CHSEL, "MCFA_CHSEL");
    e->insert(MCFA_LREQ, "MCFA_LREQ");
    e->insert(MCFA_MOBINFO, "MCFA_MOBINFO");
);

Register_Class(ClusterMCFA);

ClusterMCFA::ClusterMCFA(const char *name, int kind) : cPacket(name,kind)
{
    this->msgtype_var = 0;
    this->headId_var = 0;
    this->originId_var = 0;
    this->ERM_var = 0;
    this->Speed_var = 0;
    this->Direction_var = 0;
}

ClusterMCFA::ClusterMCFA(const ClusterMCFA& other) : cPacket(other)
{
    copy(other);
}

ClusterMCFA::~ClusterMCFA()
{
}

ClusterMCFA& ClusterMCFA::operator=(const ClusterMCFA& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void ClusterMCFA::copy(const ClusterMCFA& other)
{
    this->msgtype_var = other.msgtype_var;
    this->headId_var = other.headId_var;
    this->originId_var = other.originId_var;
    this->ERM_var = other.ERM_var;
    this->Speed_var = other.Speed_var;
    this->Direction_var = other.Direction_var;
}

void ClusterMCFA::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->msgtype_var);
    doPacking(b,this->headId_var);
    doPacking(b,this->originId_var);
    doPacking(b,this->ERM_var);
    doPacking(b,this->Speed_var);
    doPacking(b,this->Direction_var);
}

void ClusterMCFA::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->msgtype_var);
    doUnpacking(b,this->headId_var);
    doUnpacking(b,this->originId_var);
    doUnpacking(b,this->ERM_var);
    doUnpacking(b,this->Speed_var);
    doUnpacking(b,this->Direction_var);
}

int ClusterMCFA::getMsgtype() const
{
    return msgtype_var;
}

void ClusterMCFA::setMsgtype(int msgtype)
{
    this->msgtype_var = msgtype;
}

int ClusterMCFA::getHeadId() const
{
    return headId_var;
}

void ClusterMCFA::setHeadId(int headId)
{
    this->headId_var = headId;
}

int ClusterMCFA::getOriginId() const
{
    return originId_var;
}

void ClusterMCFA::setOriginId(int originId)
{
    this->originId_var = originId;
}

double ClusterMCFA::getERM() const
{
    return ERM_var;
}

void ClusterMCFA::setERM(double ERM)
{
    this->ERM_var = ERM;
}

double ClusterMCFA::getSpeed() const
{
    return Speed_var;
}

void ClusterMCFA::setSpeed(double Speed)
{
    this->Speed_var = Speed;
}

double ClusterMCFA::getDirection() const
{
    return Direction_var;
}

void ClusterMCFA::setDirection(double Direction)
{
    this->Direction_var = Direction;
}

class ClusterMCFADescriptor : public cClassDescriptor
{
  public:
    ClusterMCFADescriptor();
    virtual ~ClusterMCFADescriptor();

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

Register_ClassDescriptor(ClusterMCFADescriptor);

ClusterMCFADescriptor::ClusterMCFADescriptor() : cClassDescriptor("ClusterMCFA", "cPacket")
{
}

ClusterMCFADescriptor::~ClusterMCFADescriptor()
{
}

bool ClusterMCFADescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<ClusterMCFA *>(obj)!=NULL;
}

const char *ClusterMCFADescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int ClusterMCFADescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 6+basedesc->getFieldCount(object) : 6;
}

unsigned int ClusterMCFADescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<6) ? fieldTypeFlags[field] : 0;
}

const char *ClusterMCFADescriptor::getFieldName(void *object, int field) const
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
        "ERM",
        "Speed",
        "Direction",
    };
    return (field>=0 && field<6) ? fieldNames[field] : NULL;
}

int ClusterMCFADescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='m' && strcmp(fieldName, "msgtype")==0) return base+0;
    if (fieldName[0]=='h' && strcmp(fieldName, "headId")==0) return base+1;
    if (fieldName[0]=='o' && strcmp(fieldName, "originId")==0) return base+2;
    if (fieldName[0]=='E' && strcmp(fieldName, "ERM")==0) return base+3;
    if (fieldName[0]=='S' && strcmp(fieldName, "Speed")==0) return base+4;
    if (fieldName[0]=='D' && strcmp(fieldName, "Direction")==0) return base+5;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *ClusterMCFADescriptor::getFieldTypeString(void *object, int field) const
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
        "double",
        "double",
        "double",
    };
    return (field>=0 && field<6) ? fieldTypeStrings[field] : NULL;
}

const char *ClusterMCFADescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "MCFAMessageTypes";
            return NULL;
        default: return NULL;
    }
}

int ClusterMCFADescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    ClusterMCFA *pp = (ClusterMCFA *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ClusterMCFADescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    ClusterMCFA *pp = (ClusterMCFA *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getMsgtype());
        case 1: return long2string(pp->getHeadId());
        case 2: return long2string(pp->getOriginId());
        case 3: return double2string(pp->getERM());
        case 4: return double2string(pp->getSpeed());
        case 5: return double2string(pp->getDirection());
        default: return "";
    }
}

bool ClusterMCFADescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    ClusterMCFA *pp = (ClusterMCFA *)object; (void)pp;
    switch (field) {
        case 0: pp->setMsgtype(string2long(value)); return true;
        case 1: pp->setHeadId(string2long(value)); return true;
        case 2: pp->setOriginId(string2long(value)); return true;
        case 3: pp->setERM(string2double(value)); return true;
        case 4: pp->setSpeed(string2double(value)); return true;
        case 5: pp->setDirection(string2double(value)); return true;
        default: return false;
    }
}

const char *ClusterMCFADescriptor::getFieldStructName(void *object, int field) const
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
        NULL,
        NULL,
    };
    return (field>=0 && field<6) ? fieldStructNames[field] : NULL;
}

void *ClusterMCFADescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    ClusterMCFA *pp = (ClusterMCFA *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


