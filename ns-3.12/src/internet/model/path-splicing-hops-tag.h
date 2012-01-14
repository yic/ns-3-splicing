/* Path splicing hops tag */

#ifndef PATH_SPLICING_HOPS_TAG_H
#define PATH_SPLICING_HOPS_TAG_H

#include "ns3/tag.h"

namespace ns3 {

class Node;
class Packet;

#define PATH_MAX_N_NODES 19
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

private:
    std::list<int> m_nodeIndices;
};

class PathSplicingReverseHopsTag: public PathSplicingHopsTag
{
public:
    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;
};

} // namespace ns3

#endif /* PATH_SPLICING_HOPS_TAG_H */
