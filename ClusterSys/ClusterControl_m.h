//
// Generated file, do not edit! Created by opp_msgc 4.2 from ClusterControl.msg.
//

#ifndef _CLUSTERCONTROL_M_H_
#define _CLUSTERCONTROL_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0402
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif



/**
 * Class generated from <tt>ClusterControl.msg</tt> by opp_msgc.
 * <pre>
 * message ClusterControl {
 *     int someField;
 *     string anotherField;
 *     double arrayField1[];
 *     double arrayField2[10];
 * }
 * </pre>
 */
class ClusterControl : public ::cMessage
{
  protected:
    int someField_var;
    opp_string anotherField_var;
    double *arrayField1_var; // array ptr
    unsigned int arrayField1_arraysize;
    double arrayField2_var[10];

  private:
    void copy(const ClusterControl& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ClusterControl&);

  public:
    ClusterControl(const char *name=NULL, int kind=0);
    ClusterControl(const ClusterControl& other);
    virtual ~ClusterControl();
    ClusterControl& operator=(const ClusterControl& other);
    virtual ClusterControl *dup() const {return new ClusterControl(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getSomeField() const;
    virtual void setSomeField(int someField);
    virtual const char * getAnotherField() const;
    virtual void setAnotherField(const char * anotherField);
    virtual void setArrayField1ArraySize(unsigned int size);
    virtual unsigned int getArrayField1ArraySize() const;
    virtual double getArrayField1(unsigned int k) const;
    virtual void setArrayField1(unsigned int k, double arrayField1);
    virtual unsigned int getArrayField2ArraySize() const;
    virtual double getArrayField2(unsigned int k) const;
    virtual void setArrayField2(unsigned int k, double arrayField2);
};

inline void doPacking(cCommBuffer *b, ClusterControl& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, ClusterControl& obj) {obj.parsimUnpack(b);}


#endif // _CLUSTERCONTROL_M_H_
