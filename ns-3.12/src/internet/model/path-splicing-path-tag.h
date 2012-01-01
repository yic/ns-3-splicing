/* Path splicing path tag */

#ifndef PATH_SPLICING_PATH_TAG_H
#define PATH_SPLICING_PATH_TAG_H

#include "ns3/tag.h"
#include "ns3/random-variable.h"

namespace ns3 {

class Node;
class Packet;

#define PATH_SPLICING_MAX_N_HOPS 38
#define PATH_SPLICING_MAX_SLICE_INDEX 16

class PathSplicingPathTag : public Tag
{
public:
    PathSplicingPathTag();
    PathSplicingPathTag(const PathSplicingPathTag &o);
    PathSplicingPathTag &operator = (const PathSplicingPathTag &o);

    bool operator == (PathSplicingPathTag const &o) const;

    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;

    virtual uint32_t GetSerializedSize(void) const;
    virtual void Serialize(TagBuffer i) const;
    virtual void Deserialize(TagBuffer i);
    virtual void Print(std::ostream &os) const;

    int GetNHops();
    void AddNewHop(int slice);
    int GetCurrentHop();
    void RemoveCurrentHop();
    void RandomizeTags(UniformVariable &rand, uint32_t max);

private:
    std::list<int> m_sliceIndices;

};

class PathSplicingReversePathTag: public PathSplicingPathTag
{
public:
    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;
};

} // namespace ns3

#endif /* PATH_SPLICING_PATH_TAG_H */
