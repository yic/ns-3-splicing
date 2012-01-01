/* Path splicing routing */

#ifndef IPV4_PATH_SPLICING_ROUTING_H
#define IPV4_PATH_SPLICING_ROUTING_H

#include <list>
#include <utility>
#include <stdint.h>
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/socket.h"
#include "ns3/ptr.h"
#include "ns3/ipv4.h"
#include "ns3/ipv4-routing-protocol.h"
#include "ns3/ipv4-global-routing.h"

namespace ns3 {

class Packet;
class NetDevice;
class Ipv4Interface;
class Ipv4Address;
class Ipv4Header;
class Ipv4RoutingTableEntry;
class Node;

class Ipv4PathSplicingRouting : public Ipv4RoutingProtocol
{
public:
    static TypeId GetTypeId(void);

    Ipv4PathSplicingRouting();
    Ipv4PathSplicingRouting(int nSlices);
    virtual ~Ipv4PathSplicingRouting();

    virtual Ptr<Ipv4Route> RouteOutput(Ptr<Packet> p, const Ipv4Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr);

    virtual bool RouteInput (Ptr<const Packet> p, const Ipv4Header &header, Ptr<const NetDevice> idev,
            UnicastForwardCallback ucb, MulticastForwardCallback mcb, LocalDeliverCallback lcb, ErrorCallback ecb);

    virtual void NotifyInterfaceUp(uint32_t interface);
    virtual void NotifyInterfaceDown(uint32_t interface);
    virtual void NotifyAddAddress(uint32_t interface, Ipv4InterfaceAddress address);
    virtual void NotifyRemoveAddress(uint32_t interface, Ipv4InterfaceAddress address);
    virtual void SetIpv4(Ptr<Ipv4> ipv4);
    virtual void PrintRoutingTable(Ptr<OutputStreamWrapper> stream) const;

    void SetActualGlobalRouting(Ptr<Ipv4GlobalRouting> globalRouting);
    void ClearGlobalRoutingTable();
    void CopyGlobalRoutingTableToSlice(int slice);

protected:
    virtual void DoDispose(void);

private:
    int GetSliceFromPacket(Ptr<Packet> p);

    Ptr<Ipv4> m_ipv4;
    //number of slices for path splicing routing
    int n_slices;
    //the one that actually computes routing tables
    Ptr<Ipv4GlobalRouting> m_routing;
    //array of Ipv4GlobalRouting, one for each slice
    Ptr<Ipv4GlobalRouting> *m_globalRoutings;
};

} // Namespace ns3

#endif /* IPV4_PATH_SPLICING_ROUTING_H */
