/* Path splicing serial tag */

#ifndef PATH_SPLICING_SERIAL_TAG_H
#define PATH_SPLICING_SERIAL_TAG_H

#include "ns3/tag.h"

namespace ns3 {

class PathSplicingSerialTag : public Tag
{
public:
    PathSplicingSerialTag();

    static TypeId GetTypeId(void);
    virtual TypeId GetInstanceTypeId(void) const;

    virtual uint32_t GetSerializedSize(void) const;
    virtual void Serialize(TagBuffer i) const;
    virtual void Deserialize(TagBuffer i);
    virtual void Print(std::ostream &os) const;

    void SetSerialNumber(uint32_t serialNumber);
    uint32_t GetSerialNumber();
private:
    uint32_t m_serialNumber;
};

} // namespace ns3

#endif /* PATH_SPLICING_SERIAL_TAG_H */
