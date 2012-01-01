#ifndef PATH_SPLICING_APP_SERVER_H
#define PATH_SPLICING_APP_SERVER_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"

namespace ns3 {

class Socket;
class Packet;

class PathSplicingAppServer : public Application 
{
public:
    static TypeId GetTypeId (void);

    PathSplicingAppServer ();
    virtual ~PathSplicingAppServer ();

protected:
    virtual void DoDispose (void);

private:
    virtual void StartApplication (void);
    virtual void StopApplication (void);

    void HandleRead(Ptr<Socket> socket);

    uint16_t m_port;
    uint32_t m_size;
    Ptr<Socket> m_socket;
};

} // namespace ns3

#endif /* PATH_SPLICING_APP_SERVER_H */

