/* Path splicing routing */

#include <iomanip>
#include "ns3/log.h"
#include "ns3/names.h"
#include "ns3/packet.h"
#include "ns3/node.h"
#include "ns3/simulator.h"
#include "ns3/ipv4-route.h"
#include "ns3/output-stream-wrapper.h"
#include "ipv4-path-splicing-routing.h"
#include "ipv4-routing-table-entry.h"
#include "path-splicing-path-tag.h"
#include "path-splicing-hops-tag.h"

NS_LOG_COMPONENT_DEFINE("Ipv4PathSplicingRouting");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(Ipv4PathSplicingRouting);

TypeId Ipv4PathSplicingRouting::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Ipv4PathSplicingRouting")
        .SetParent<Ipv4RoutingProtocol>()
        .AddConstructor<Ipv4PathSplicingRouting>()
        ;
    return tid;
}

Ipv4PathSplicingRouting::Ipv4PathSplicingRouting()
{
    Ipv4PathSplicingRouting(1);
}

Ipv4PathSplicingRouting::Ipv4PathSplicingRouting(int nSlices)
    : m_ipv4(0)
    , n_slices(nSlices)
    , m_routing(0)
{
    NS_LOG_FUNCTION(this);

    m_globalRoutings = new Ptr<Ipv4GlobalRouting>[n_slices];

    for (int i = 0; i < n_slices; i ++)
        m_globalRoutings[i] = CreateObject<Ipv4GlobalRouting>();
}

void Ipv4PathSplicingRouting::SetActualGlobalRouting(Ptr<Ipv4GlobalRouting> globalRouting)
{
    m_routing = globalRouting;
}

Ipv4PathSplicingRouting::~Ipv4PathSplicingRouting()
{
    NS_LOG_FUNCTION(this);
}

int Ipv4PathSplicingRouting::GetSliceFromPacket(Ptr<Packet> p)
{
    PathSplicingPathTag pathTag;
    p->RemovePacketTag(pathTag);

    int slice = 0;

    NS_LOG_INFO("Number of hops in tag: " << pathTag.GetNHops());

    if (pathTag.GetNHops() > 0)
    {
        slice = pathTag.GetCurrentHop();
        pathTag.RemoveCurrentHop();
    }

    p->AddPacketTag(pathTag);

    return slice;
}

Ptr<Ipv4Route> Ipv4PathSplicingRouting::RouteOutput(Ptr<Packet> p, const Ipv4Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr)
{
    NS_LOG_FUNCTION(this << header.GetDestination () << " " << header.GetSource () << " " << oif);
    Ptr<Ipv4Route> route;

    int slice = GetSliceFromPacket(p);
    NS_ASSERT(slice < n_slices);
    NS_LOG_INFO("Use slice " << slice);

    route = m_globalRoutings[slice]->RouteOutput(p, header, oif, sockerr);
    if (route)
    {
        NS_LOG_LOGIC("Found route " << route);
        sockerr = Socket::ERROR_NOTERROR;
        return route;
    }

    NS_LOG_LOGIC("Route not found");
    sockerr = Socket::ERROR_NOROUTETOHOST;
    return 0;
}

bool Ipv4PathSplicingRouting::RouteInput(Ptr<const Packet> p, const Ipv4Header &header, Ptr<const NetDevice> idev,
        UnicastForwardCallback ucb, MulticastForwardCallback mcb, LocalDeliverCallback lcb, ErrorCallback ecb)
{
    NS_LOG_FUNCTION(this << p << header << header.GetSource() << header.GetDestination() << idev);

    Ptr<Packet> packetCopy = p->Copy();
    int slice = GetSliceFromPacket(packetCopy);
    NS_ASSERT(slice < n_slices);

    PathSplicingHopsTag hopsTag;
    packetCopy->RemovePacketTag(hopsTag);
    hopsTag.AddNewNode(m_ipv4->GetObject<Node>()->GetId());
    packetCopy->AddPacketTag(hopsTag);

    return m_globalRoutings[slice]->RouteInput(packetCopy, header, idev, ucb, mcb, lcb, ecb);
}

void Ipv4PathSplicingRouting::DoDispose(void)
{
    NS_LOG_FUNCTION(this);
    delete [] m_globalRoutings;
    m_ipv4 = 0;
}

void Ipv4PathSplicingRouting::NotifyInterfaceUp(uint32_t interface)
{
    NS_LOG_FUNCTION(this << interface);

    for (int i = 0; i < n_slices; i ++)
        m_globalRoutings[i]->NotifyInterfaceUp(interface);
}

void Ipv4PathSplicingRouting::NotifyInterfaceDown(uint32_t interface)
{
    NS_LOG_FUNCTION(this << interface);

    for (int i = 0; i < n_slices; i ++)
        m_globalRoutings[i]->NotifyInterfaceDown(interface);
}

void Ipv4PathSplicingRouting::NotifyAddAddress(uint32_t interface, Ipv4InterfaceAddress address)
{
    NS_LOG_FUNCTION(this << interface << " " << address.GetLocal());

    for (int i = 0; i < n_slices; i ++)
        m_globalRoutings[i]->NotifyAddAddress(interface, address);
}

void Ipv4PathSplicingRouting::NotifyRemoveAddress(uint32_t interface, Ipv4InterfaceAddress address)
{
    NS_LOG_FUNCTION(this << interface << " " << address.GetLocal());

    for (int i = 0; i < n_slices; i ++)
        m_globalRoutings[i]->NotifyRemoveAddress(interface, address);
}

void Ipv4PathSplicingRouting::SetIpv4(Ptr<Ipv4> ipv4)
{
    NS_LOG_FUNCTION(this << ipv4);
    NS_ASSERT(m_ipv4 == 0 && ipv4 != 0);
    m_ipv4 = ipv4;

    NS_ASSERT(m_routing != 0);
    m_routing->SetIpv4(ipv4);

    for (int i = 0; i < n_slices; i ++)
        m_globalRoutings[i]->SetIpv4(ipv4);
}

void Ipv4PathSplicingRouting::PrintRoutingTable(Ptr<OutputStreamWrapper> stream) const
{
    *stream->GetStream() << "Node: " << m_ipv4->GetObject<Node>()->GetId()
        << " Time: " << Simulator::Now().GetSeconds() << "s"
        << " Ipv4PathSplicingRouting table" << std::endl;

    for (int i = 0; i < n_slices; i ++)
    {
        *stream->GetStream() << " -Slice: " << i << std::endl;
        m_globalRoutings[i]->PrintRoutingTable(stream);
    }

    *stream->GetStream() << std::endl;
}

void Ipv4PathSplicingRouting::ClearGlobalRoutingTable()
{
    m_routing->ClearRoutingTables();
}

void Ipv4PathSplicingRouting::CopyGlobalRoutingTableToSlice(int slice)
{
    NS_LOG_FUNCTION(this << slice);

    NS_ASSERT(slice < n_slices);
    m_globalRoutings[slice]->CopyRoutingTablesFrom(PeekPointer(m_routing));
}

} // namespace ns3
