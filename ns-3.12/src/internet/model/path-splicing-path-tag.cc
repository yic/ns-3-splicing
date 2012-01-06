/* Path splicing info tag */

#include <stdint.h>
#include "ns3/ipv4-address.h"
#include "path-splicing-path-tag.h"

namespace ns3 {

PathSplicingPathTag::PathSplicingPathTag()
    : m_sliceIndices()
{
}

PathSplicingPathTag::PathSplicingPathTag(const PathSplicingPathTag &o)
    : m_sliceIndices(o.m_sliceIndices)
{
}

PathSplicingPathTag &PathSplicingPathTag::operator = (const PathSplicingPathTag &o)
{
    if (this == &o)
        return *this;

    m_sliceIndices = o.m_sliceIndices;
    return *this;
}

bool PathSplicingPathTag::operator == (PathSplicingPathTag const &o) const
{
    return (m_sliceIndices == o.m_sliceIndices);
}

TypeId PathSplicingPathTag::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::PathSplicingPathTag")
        .SetParent<Tag>()
        .AddConstructor<PathSplicingPathTag>()
        ;
    return tid;
}

TypeId PathSplicingPathTag::GetInstanceTypeId(void) const
{
    return GetTypeId();
}

uint32_t PathSplicingPathTag::GetSerializedSize(void) const
{
    //the first byte stores number of slice indices
    //each of the following bytes stores two slice indices
    return 1 + m_sliceIndices.size() / 2 + m_sliceIndices.size() % 2;
}

void PathSplicingPathTag::Serialize(TagBuffer i) const
{
    NS_ASSERT(m_sliceIndices.size() <= PATH_SPLICING_MAX_N_HOPS);
    i.WriteU8(m_sliceIndices.size());

    bool highOrder = true;
    uint8_t number, sliceIndex;
    number = 0;

    for (std::list<int>::const_iterator it = m_sliceIndices.begin(); it != m_sliceIndices.end(); it ++)
    {
        sliceIndex = *it;
        NS_ASSERT(sliceIndex < PATH_SPLICING_MAX_SLICE_INDEX);

        if (highOrder)
            number = sliceIndex << 4;
        else
        {
            number = number | sliceIndex;
            i.WriteU8(number);
        }

        highOrder = !highOrder;
    }

    if (!highOrder)
        i.WriteU8(number);
}

void PathSplicingPathTag::Deserialize(TagBuffer i)
{
    m_sliceIndices.clear();
    int nHops = i.ReadU8();
    uint8_t number;

    for (int k = 0; k < nHops / 2; k ++)
    {
        number = i.ReadU8();
        m_sliceIndices.push_back(number >> 4);
        m_sliceIndices.push_back(number & 0xF);
    }

    if (nHops % 2)
    {
        number = i.ReadU8();
        m_sliceIndices.push_back(number >> 4);
    }

    NS_ASSERT((int)m_sliceIndices.size() == nHops);
}

void PathSplicingPathTag::Print(std::ostream &os) const
{
    os << "Ipv4 Path Splicing Info: " << std::endl;
    os << "  No. of hops: " << m_sliceIndices.size() << std::endl;
    os << "  Tags:";

    for (std::list<int>::const_iterator it = m_sliceIndices.begin(); it != m_sliceIndices.end(); it ++)
        os << " " << *it;

    os << std::endl;
}

int PathSplicingPathTag::GetNHops()
{
    return m_sliceIndices.size();
}

void PathSplicingPathTag::AddNewHop(int slice)
{
    m_sliceIndices.push_front(slice);
}

int PathSplicingPathTag::GetCurrentHop()
{
    return m_sliceIndices.back();
}

void PathSplicingPathTag::RemoveCurrentHop()
{
    m_sliceIndices.pop_back();
}

void PathSplicingPathTag::RandomizeTags(UniformVariable &rand, uint32_t max)
{
    std::list<int> newSliceIndices;
    int toss;
    int oldSize = m_sliceIndices.size();

    for (int i = 0; i < PATH_SPLICING_MAX_N_HOPS; i ++)
    {
        toss = rand.GetInteger(0, 1);

        if (toss == 0)
        {
            //do not change the current hop
            if (i < oldSize)
            {
                newSliceIndices.push_front(m_sliceIndices.back());
                m_sliceIndices.pop_back();
            }
            else
                newSliceIndices.push_front(0);
        }
        else
        {
            NS_ASSERT(toss == 1);

            //randomly pick the current hop
            newSliceIndices.push_front(rand.GetInteger(0, max - 1));

            if (i < oldSize)
                m_sliceIndices.pop_back();
        }
    }

    m_sliceIndices = newSliceIndices;
}

TypeId PathSplicingReversePathTag::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::PathSplicingReversePathTag")
        .SetParent<PathSplicingPathTag>()
        .AddConstructor<PathSplicingReversePathTag>()
        ;
    return tid;
}

TypeId PathSplicingReversePathTag::GetInstanceTypeId(void) const
{
    return GetTypeId();
}

} // namespace ns3

