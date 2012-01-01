/* Path splicing info tag */

#include <stdint.h>
#include "path-splicing-serial-tag.h"

namespace ns3 {

PathSplicingSerialTag::PathSplicingSerialTag()
    : m_serialNumber(0)
{
}

TypeId PathSplicingSerialTag::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::PathSplicingSerialTag")
        .SetParent<Tag>()
        .AddConstructor<PathSplicingSerialTag>()
        ;
    return tid;
}

TypeId PathSplicingSerialTag::GetInstanceTypeId(void) const
{
    return GetTypeId();
}

uint32_t PathSplicingSerialTag::GetSerializedSize(void) const
{
    return 4;
}

void PathSplicingSerialTag::Serialize(TagBuffer i) const
{
    i.WriteU32(m_serialNumber);
}

void PathSplicingSerialTag::Deserialize(TagBuffer i)
{
    m_serialNumber = i.ReadU32();
}

void PathSplicingSerialTag::Print(std::ostream &os) const
{
    os << "Path Splicing App Serial Number: " << m_serialNumber << std::endl;
}

void PathSplicingSerialTag::SetSerialNumber(uint32_t serialNumber)
{
    m_serialNumber = serialNumber;
}

uint32_t PathSplicingSerialTag::GetSerialNumber()
{
    return m_serialNumber;
}

} // namespace ns3

