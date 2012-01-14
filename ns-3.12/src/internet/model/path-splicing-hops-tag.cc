/* Path splicing hops tag */

#include <stdint.h>
#include "ns3/ipv4-address.h"
#include "path-splicing-hops-tag.h"

namespace ns3 {

PathSplicingHopsTag::PathSplicingHopsTag()
    : m_nodeIndices()
{
}

PathSplicingHopsTag::PathSplicingHopsTag(const PathSplicingHopsTag &o)
    : m_nodeIndices(o.m_nodeIndices)
{
}

PathSplicingHopsTag &PathSplicingHopsTag::operator = (const PathSplicingHopsTag &o)
{
    if (this == &o)
        return *this;

    m_nodeIndices = o.m_nodeIndices;
    return *this;
}

bool PathSplicingHopsTag::operator == (PathSplicingHopsTag const &o) const
{
    return (m_nodeIndices == o.m_nodeIndices);
}

TypeId PathSplicingHopsTag::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::PathSplicingHopsTag")
        .SetParent<Tag>()
        .AddConstructor<PathSplicingHopsTag>()
        ;
    return tid;
}

TypeId PathSplicingHopsTag::GetInstanceTypeId(void) const
{
    return GetTypeId();
}

uint32_t PathSplicingHopsTag::GetSerializedSize(void) const
{
    //the first byte stores number of slice indices
    //each of the following bytes stores one node index
    return 1 + m_nodeIndices.size();
}

void PathSplicingHopsTag::Serialize(TagBuffer i) const
{
    NS_ASSERT(m_nodeIndices.size() <= PATH_MAX_N_NODES);
    i.WriteU8(m_nodeIndices.size());

    uint8_t nodeIndex;

    for (std::list<int>::const_iterator it = m_nodeIndices.begin(); it != m_nodeIndices.end(); it ++)
    {
        nodeIndex = *it;
        NS_ASSERT(nodeIndex < PATH_MAX_NODE_INDEX);

        i.WriteU8(nodeIndex);
    }
}

void PathSplicingHopsTag::Deserialize(TagBuffer i)
{
    m_nodeIndices.clear();
    int nHops = i.ReadU8();
    uint8_t number;

    for (int k = 0; k < nHops; k ++)
    {
        number = i.ReadU8();
        m_nodeIndices.push_back(number);
    }

    NS_ASSERT((int)m_nodeIndices.size() == nHops);
}

void PathSplicingHopsTag::Print(std::ostream &os) const
{
    os << "Ipv4 Path Splicing Hops: " << std::endl;
    os << "  No. of nodes: " << m_nodeIndices.size() << std::endl;
    os << "  Nodes:";

    for (std::list<int>::const_iterator it = m_nodeIndices.begin(); it != m_nodeIndices.end(); it ++)
        os << " " << *it;

    os << std::endl;
}

std::string PathSplicingHopsTag::ToString()
{
    std::stringstream ss;

    if (m_nodeIndices.size() > 0) {
        std::list<int>::const_iterator it = m_nodeIndices.begin();
        ss << *it;
        it ++;

        for (; it != m_nodeIndices.end(); it ++)
            ss << "-" << *it;
    }

    return ss.str();
}

void PathSplicingHopsTag::AddNewNode(int nodeId)
{
    NS_ASSERT(m_nodeIndices.size() < PATH_MAX_N_NODES);
    m_nodeIndices.push_back(nodeId);
}

TypeId PathSplicingReverseHopsTag::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::PathSplicingReverseHopsTag")
        .SetParent<PathSplicingHopsTag>()
        .AddConstructor<PathSplicingReverseHopsTag>()
        ;
    return tid;
}

TypeId PathSplicingReverseHopsTag::GetInstanceTypeId(void) const
{
    return GetTypeId();
}

PathSplicingReverseHopsTag &PathSplicingReverseHopsTag::operator = (const PathSplicingHopsTag &o)
{
    m_nodeIndices = o.m_nodeIndices;
    return *this;
}

} // namespace ns3

