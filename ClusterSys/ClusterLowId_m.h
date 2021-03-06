//
// Generated file, do not edit! Created by opp_msgc 4.2 from ClusterLowId.msg.
//

#ifndef _CLUSTERLOWID_M_H_
#define _CLUSTERLOWID_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0402
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif



/**
 * Enum generated from <tt>ClusterLowId.msg</tt> by opp_msgc.
 * <pre>
 * enum ClusterLowIdTypes 
 * { 
 *     	CLUSTER_INIT 		= 11;
 *     	CLUSTER_JOIN 		= 21;
 *     	CLUSTER_ACCEPTED 	= 31;
 *     	CLUSTER_NEIGH_INQ 	= 41;
 *     	CLUSTER_NEIGH_RESP 	= 51;
 *     	CLUSTER_PING 		= 61;
 *     	CLUSTER_PONG 		= 71;
 *     	CLUSTER_REJOIN		= 81;
 * };
 * </pre>
 */
enum ClusterLowIdTypes {
    CLUSTER_INIT = 11,
    CLUSTER_JOIN = 21,
    CLUSTER_ACCEPTED = 31,
    CLUSTER_NEIGH_INQ = 41,
    CLUSTER_NEIGH_RESP = 51,
    CLUSTER_PING = 61,
    CLUSTER_PONG = 71,
    CLUSTER_REJOIN = 81
};

/**
 * Class generated from <tt>ClusterLowId.msg</tt> by opp_msgc.
 * <pre>
 * packet ClusterLowIdPkt {
 * 	int msgtype enum(ClusterLowIdTypes);
 * 	int headId;
 * 	int originId;
 * };
 * </pre>
 */
class ClusterLowIdPkt : public ::cPacket
{
  protected:
    int msgtype_var;
    int headId_var;
    int originId_var;

  private:
    void copy(const ClusterLowIdPkt& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ClusterLowIdPkt&);

  public:
    ClusterLowIdPkt(const char *name=NULL, int kind=0);
    ClusterLowIdPkt(const ClusterLowIdPkt& other);
    virtual ~ClusterLowIdPkt();
    ClusterLowIdPkt& operator=(const ClusterLowIdPkt& other);
    virtual ClusterLowIdPkt *dup() const {return new ClusterLowIdPkt(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getMsgtype() const;
    virtual void setMsgtype(int msgtype);
    virtual int getHeadId() const;
    virtual void setHeadId(int headId);
    virtual int getOriginId() const;
    virtual void setOriginId(int originId);
};

inline void doPacking(cCommBuffer *b, ClusterLowIdPkt& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, ClusterLowIdPkt& obj) {obj.parsimUnpack(b);}


#endif // _CLUSTERLOWID_M_H_
