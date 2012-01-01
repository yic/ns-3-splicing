#include "path-splicing-app-prt-entry.h"
#include "ns3/assert.h"

namespace ns3 {

PathSplicingAppPrtEntry::PathSplicingAppPrtEntry(uint32_t serialNumber, Time sendTime, EventId timeoutEvent,
        PathSplicingPathTag pathTag, PathSplicingReversePathTag reversePathTag)
    : m_serialNumber(serialNumber)
    , m_sendTime(sendTime)
    , m_timeoutEvent(timeoutEvent)
    , m_pathTag(pathTag)
    , m_reversePathTag(reversePathTag)
    , m_retx(0)
{

}

PathSplicingAppPrtEntry::~PathSplicingAppPrtEntry()
{

}

uint32_t PathSplicingAppPrtEntry::GetSerialNumber()
{
    return m_serialNumber;
}

void PathSplicingAppPrtEntry::SetSendTime(Time sendTime)
{
    m_sendTime = sendTime;
}

Time PathSplicingAppPrtEntry::GetSendTime()
{
    return m_sendTime;
}

void PathSplicingAppPrtEntry::SetTimeoutEvent(EventId timeoutEvent)
{
    m_timeoutEvent = timeoutEvent;
}

EventId PathSplicingAppPrtEntry::GetTimeoutEvent()
{
    return m_timeoutEvent;
}

void PathSplicingAppPrtEntry::SetPathSplicingPathTag(PathSplicingPathTag tag)
{
    m_pathTag = tag;
}

PathSplicingPathTag PathSplicingAppPrtEntry::GetPathSplicingPathTag()
{
    return m_pathTag;
}

void PathSplicingAppPrtEntry::SetPathSplicingReversePathTag(PathSplicingReversePathTag tag)
{
    m_reversePathTag = tag;
}

PathSplicingReversePathTag PathSplicingAppPrtEntry::GetPathSplicingReversePathTag()
{
    return m_reversePathTag;
}

void PathSplicingAppPrtEntry::IncrementRetx()
{
    m_retx++;
}

uint32_t PathSplicingAppPrtEntry::GetRetx()
{
    return m_retx;
}

} // namespace ns3
