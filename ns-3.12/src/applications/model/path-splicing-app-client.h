#ifndef PATH_SPLICING_CLIENT_H
#define PATH_SPLICING_CLIENT_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/traced-callback.h"
#include "path-splicing-app-prt-entry.h"
#include "ns3/path-splicing-path-tag.h"
#include "ns3/random-variable.h"

namespace ns3 {

class Socket;
class Packet;

class PathSplicingAppClient : public Application 
{

#define PATH_SPLICING_RTO_ALPHA 0.125
#define PATH_SPLICING_RTO_BETA 0.25
#define PATH_SPLICING_RTO_K 4

public:
    static TypeId GetTypeId(void);

    PathSplicingAppClient();
    virtual ~PathSplicingAppClient();

    void SetRemote(Ipv4Address ip, uint16_t port);

protected:
    virtual void DoDispose(void);

private:
    virtual void StartApplication(void);
    virtual void StopApplication(void);

    void ScheduleTransmit(Time dt);
    void Send();
    void HandleRead(Ptr<Socket> socket);
    void Timeout(uint32_t serialNumber);

    Ipv4Address m_peerAddress;
    uint16_t m_peerPort;
    Ptr<Socket> m_socket;

    uint32_t m_size;
    uint32_t m_count;
    uint32_t m_sent;
    uint32_t m_maxSlices;
    uint32_t m_maxRetx;

    Time m_interval;
    Time m_srtt;
    Time m_rttvar;
    Time m_rto;

    EventId m_sendEvent;
    UniformVariable m_randPath;
    UniformVariable m_randReversePath;

    PathSplicingPathTag m_pathTag;
    PathSplicingReversePathTag m_reversePathTag;

    std::list<PathSplicingAppPrtEntry *> m_pendingRequests;
};

} // namespace ns3

#endif /* PATH_SPLICING_CLIENT_H */
