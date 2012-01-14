/* Path splicing hops tag */

#ifndef PATH_SPLICING_HOPS_TAG_H
#define PATH_SPLICING_HOPS_TAG_H

#include "ns3/tag.h"
#include "ns3/packet-tag-list.h"

namespace ns3 {

class Node;
class Packet;

#define PATH_MAX_N_NODES (PACKET_TAG_MAX_SIZE - 1)
#define PATH_MAX_NODE_INDEX 256

class PathSplicingHopsTag : public Tag
{
public:
    PathSplicingHopsTag();
    PathSplicingHopsTag(const PathSplicingHopsTag &o);
    PathSplicingHopsTag &operator = (const PathSplicingHopsTag &o);

    bool operator == (PathSplicingHopsTag const &o) const;

    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;

    virtual uint32_t GetSerializedSize(void) const;
    virtual void Serialize(TagBuffer i) const;
    virtual void Deserialize(TagBuffer i);
    virtual void Print(std::ostream &os) const;

    void AddNewNode(int nodeId);
    std::string ToString();

private:
    std::list<int> m_nodeIndices;

    friend class PathSplicingReverseHopsTag;
};

class PathSplicingReverseHopsTag: public PathSplicingHopsTag
{
public:
    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;
    PathSplicingReverseHopsTag &operator = (const PathSplicingHopsTag &o);
};

} // namespace ns3

#endif /* PATH_SPLICING_HOPS_TAG_H */
