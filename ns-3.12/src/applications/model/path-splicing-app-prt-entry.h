#ifndef PATH_SPLICING_APP_PRT_ENTRY_H
#define PATH_SPLICING_APP_PRT_ENTRY_H

#include "ns3/event-id.h"

#include "ns3/application.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/traced-callback.h"

#include "ns3/path-splicing-path-tag.h"

namespace ns3 {

class PathSplicingAppPrtEntry {

public:
    PathSplicingAppPrtEntry(uint32_t serialNumber, Time sendTime, EventId timeoutEvent,
            PathSplicingPathTag pathTag, PathSplicingReversePathTag reversePathTag);
    ~PathSplicingAppPrtEntry();

    uint32_t GetSerialNumber();

    void SetSendTime(Time sendTime);
    Time GetSendTime();

    void SetTimeoutEvent(EventId timeoutEvent);
    EventId GetTimeoutEvent();

    void SetPathSplicingPathTag(PathSplicingPathTag tag);
    PathSplicingPathTag GetPathSplicingPathTag();

    void SetPathSplicingReversePathTag(PathSplicingReversePathTag tag);
    PathSplicingReversePathTag GetPathSplicingReversePathTag();

    void IncrementRetx();
    uint32_t GetRetx();

private:
    uint32_t m_serialNumber;
    Time m_sendTime;
    EventId m_timeoutEvent;
    PathSplicingPathTag m_pathTag;
    PathSplicingReversePathTag m_reversePathTag;
    uint32_t m_retx;

};

} // namespace ns3

#endif /* PATH_SPLICING_APP_PRT_ENTRY_H */
