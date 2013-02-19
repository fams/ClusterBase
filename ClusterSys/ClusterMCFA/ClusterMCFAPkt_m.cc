//
// Generated file, do not edit! Created by opp_msgc 4.2 from ClusterMCFA/ClusterMCFAPkt.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "ClusterMCFAPkt_m.h"

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
    e->insert(MCFA_CLINFO, "MCFA_CLINFO");
    e->insert(MCFA_REMOVE, "MCFA_REMOVE");
);

Register_Class(ClusterMCFAPkt);

ClusterMCFAPkt::ClusterMCFAPkt(const char *name, int kind) : ClusterPkt(name,kind)
{
    this->msgtype_var = 0;
    this->headId_var = 0;
    this->originId_var = 0;
    this->ERM_var = 0;
    this->Speed_var = 0;
    this->Direction_var = 0;
    this->Question_var = 0;
    this->childinfo_var = 0;
}

ClusterMCFAPkt::ClusterMCFAPkt(const ClusterMCFAPkt& other) : ClusterPkt(other)
{
    copy(other);
}

ClusterMCFAPkt::~ClusterMCFAPkt()
{
}

ClusterMCFAPkt& ClusterMCFAPkt::operator=(const ClusterMCFAPkt& other)
{
    if (this==&other) return *this;
    ClusterPkt::operator=(other);
    copy(other);
    return *this;
}

void ClusterMCFAPkt::copy(const ClusterMCFAPkt& other)
{
    this->msgtype_var = other.msgtype_var;
    this->headId_var = other.headId_var;
    this->originId_var = other.originId_var;
    this->ERM_var = other.ERM_var;
    this->Speed_var = other.Speed_var;
    this->Direction_var = other.Direction_var;
    this->Question_var = other.Question_var;
    this->childinfo_var = other.childinfo_var;
}

void ClusterMCFAPkt::parsimPack(cCommBuffer *b)
{
    ClusterPkt::parsimPack(b);
    doPacking(b,this->msgtype_var);
    doPacking(b,this->headId_var);
    doPacking(b,this->originId_var);
    doPacking(b,this->ERM_var);
    doPacking(b,this->Speed_var);
    doPacking(b,this->Direction_var);
    doPacking(b,this->Question_var);
    doPacking(b,this->childinfo_var);
}

void ClusterMCFAPkt::parsimUnpack(cCommBuffer *b)
{
    ClusterPkt::parsimUnpack(b);
    doUnpacking(b,this->msgtype_var);
    doUnpacking(b,this->headId_var);
    doUnpacking(b,this->originId_var);
    doUnpacking(b,this->ERM_var);
    doUnpacking(b,this->Speed_var);
    doUnpacking(b,this->Direction_var);
    doUnpacking(b,this->Question_var);
    doUnpacking(b,this->childinfo_var);
}

int ClusterMCFAPkt::getMsgtype() const
{
    return msgtype_var;
}

void ClusterMCFAPkt::setMsgtype(int msgtype)
{
    this->msgtype_var = msgtype;
}

int ClusterMCFAPkt::getHeadId() const
{
    return headId_var;
}

void ClusterMCFAPkt::setHeadId(int headId)
{
    this->headId_var = headId;
}

int ClusterMCFAPkt::getOriginId() const
{
    return originId_var;
}

void ClusterMCFAPkt::setOriginId(int originId)
{
    this->originId_var = originId;
}

double ClusterMCFAPkt::getERM() const
{
    return ERM_var;
}

void ClusterMCFAPkt::setERM(double ERM)
{
    this->ERM_var = ERM;
}

double ClusterMCFAPkt::getSpeed() const
{
    return Speed_var;
}

void ClusterMCFAPkt::setSpeed(double Speed)
{
    this->Speed_var = Speed;
}

double ClusterMCFAPkt::getDirection() const
{
    return Direction_var;
}

void ClusterMCFAPkt::setDirection(double Direction)
{
    this->Direction_var = Direction;
}

int ClusterMCFAPkt::getQuestion() const
{
    return Question_var;
}

void ClusterMCFAPkt::setQuestion(int Question)
{
    this->Question_var = Question;
}

const char * ClusterMCFAPkt::getChildinfo() const
{
    return childinfo_var.c_str();
}

void ClusterMCFAPkt::setChildinfo(const char * childinfo)
{
    this->childinfo_var = childinfo;
}

class ClusterMCFAPktDescriptor : public cClassDescriptor
{
  public:
    ClusterMCFAPktDescriptor();
    virtual ~ClusterMCFAPktDescriptor();

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

Register_ClassDescriptor(ClusterMCFAPktDescriptor);

ClusterMCFAPktDescriptor::ClusterMCFAPktDescriptor() : cClassDescriptor("ClusterMCFAPkt", "ClusterPkt")
{
}

ClusterMCFAPktDescriptor::~ClusterMCFAPktDescriptor()
{
}

bool ClusterMCFAPktDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<ClusterMCFAPkt *>(obj)!=NULL;
}

const char *ClusterMCFAPktDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int ClusterMCFAPktDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 8+basedesc->getFieldCount(object) : 8;
}

unsigned int ClusterMCFAPktDescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<8) ? fieldTypeFlags[field] : 0;
}

const char *ClusterMCFAPktDescriptor::getFieldName(void *object, int field) const
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
        "Question",
        "childinfo",
    };
    return (field>=0 && field<8) ? fieldNames[field] : NULL;
}

int ClusterMCFAPktDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='m' && strcmp(fieldName, "msgtype")==0) return base+0;
    if (fieldName[0]=='h' && strcmp(fieldName, "headId")==0) return base+1;
    if (fieldName[0]=='o' && strcmp(fieldName, "originId")==0) return base+2;
    if (fieldName[0]=='E' && strcmp(fieldName, "ERM")==0) return base+3;
    if (fieldName[0]=='S' && strcmp(fieldName, "Speed")==0) return base+4;
    if (fieldName[0]=='D' && strcmp(fieldName, "Direction")==0) return base+5;
    if (fieldName[0]=='Q' && strcmp(fieldName, "Question")==0) return base+6;
    if (fieldName[0]=='c' && strcmp(fieldName, "childinfo")==0) return base+7;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *ClusterMCFAPktDescriptor::getFieldTypeString(void *object, int field) const
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
        "int",
        "string",
    };
    return (field>=0 && field<8) ? fieldTypeStrings[field] : NULL;
}

const char *ClusterMCFAPktDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int ClusterMCFAPktDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    ClusterMCFAPkt *pp = (ClusterMCFAPkt *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ClusterMCFAPktDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    ClusterMCFAPkt *pp = (ClusterMCFAPkt *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getMsgtype());
        case 1: return long2string(pp->getHeadId());
        case 2: return long2string(pp->getOriginId());
        case 3: return double2string(pp->getERM());
        case 4: return double2string(pp->getSpeed());
        case 5: return double2string(pp->getDirection());
        case 6: return long2string(pp->getQuestion());
        case 7: return oppstring2string(pp->getChildinfo());
        default: return "";
    }
}

bool ClusterMCFAPktDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    ClusterMCFAPkt *pp = (ClusterMCFAPkt *)object; (void)pp;
    switch (field) {
        case 0: pp->setMsgtype(string2long(value)); return true;
        case 1: pp->setHeadId(string2long(value)); return true;
        case 2: pp->setOriginId(string2long(value)); return true;
        case 3: pp->setERM(string2double(value)); return true;
        case 4: pp->setSpeed(string2double(value)); return true;
        case 5: pp->setDirection(string2double(value)); return true;
        case 6: pp->setQuestion(string2long(value)); return true;
        case 7: pp->setChildinfo((value)); return true;
        default: return false;
    }
}

const char *ClusterMCFAPktDescriptor::getFieldStructName(void *object, int field) const
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
        NULL,
        NULL,
    };
    return (field>=0 && field<8) ? fieldStructNames[field] : NULL;
}

void *ClusterMCFAPktDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    ClusterMCFAPkt *pp = (ClusterMCFAPkt *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


