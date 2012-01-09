#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/address-utils.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "path-splicing-app-server.h"
#include "ns3/path-splicing-path-tag.h"
#include "ns3/path-splicing-serial-tag.h"
//#include "ns3/node-list.h"
//#include "ns3/ipv4.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("PathSplicingAppServerApplication");
NS_OBJECT_ENSURE_REGISTERED(PathSplicingAppServer);

TypeId PathSplicingAppServer::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::PathSplicingAppServer")
        .SetParent<Application>()
        .AddConstructor<PathSplicingAppServer>()
        .AddAttribute("Port",
                "Port on which we listen for incoming packets.",
                UintegerValue(9),
                MakeUintegerAccessor(&PathSplicingAppServer::m_port),
                MakeUintegerChecker<uint16_t>())
        .AddAttribute("PacketSize",
                "Size of data packet.",
                UintegerValue(1024),
                MakeUintegerAccessor(&PathSplicingAppServer::m_size),
                MakeUintegerChecker<uint32_t>())
        ;
    return tid;
}

PathSplicingAppServer::PathSplicingAppServer()
{
    NS_LOG_FUNCTION_NOARGS();
}

PathSplicingAppServer::~PathSplicingAppServer()
{
    NS_LOG_FUNCTION_NOARGS();
    m_socket = 0;
}

void PathSplicingAppServer::DoDispose(void)
{
    NS_LOG_FUNCTION_NOARGS();
    Application::DoDispose();
}

void PathSplicingAppServer::StartApplication(void)
{
    NS_LOG_FUNCTION_NOARGS();

    if (m_socket == 0)
    {
        TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
        m_socket = Socket::CreateSocket(GetNode(), tid);
        InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), m_port);
        m_socket->Bind(local);
    }

    m_socket->SetRecvCallback(MakeCallback(&PathSplicingAppServer::HandleRead, this));
}

void PathSplicingAppServer::StopApplication()
{
    NS_LOG_FUNCTION_NOARGS();

    if (m_socket != 0)
    {
        m_socket->Close();
        m_socket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket> >());
    }
}

//int GetNodeIdFromAddress(Address addr) {
//    NodeList::Iterator listEnd = NodeList::End();
//
//    for (NodeList::Iterator i = NodeList::Begin(); i != listEnd; i++) {
//        Ptr<Node> node = *i;
//        Ptr<Ipv4> ipv4Local = node->GetObject<Ipv4>();
//
//        int32_t iface = ipv4Local->GetInterfaceForAddress(InetSocketAddress::ConvertFrom(addr).GetIpv4());
//
//        if (iface > 0)
//            return node->GetId();
//    }
//
//    return -1;
//}

void PathSplicingAppServer::HandleRead(Ptr<Socket> socket)
{
    Ptr<Packet> packet;
    Address from;

    while (packet = socket->RecvFrom(from))
    {
        if (InetSocketAddress::IsMatchingType(from))
        {
            PathSplicingSerialTag serialTag;
            packet->PeekPacketTag(serialTag);

//            NS_LOG_INFO("Node " << GetNode()->GetId() << " receive request #" << serialTag.GetSerialNumber() << " from " <<
//                    InetSocketAddress::ConvertFrom(from).GetIpv4());

            PathSplicingReversePathTag reversePathTag;
            packet->PeekPacketTag(reversePathTag);

            Ptr<Packet> p = Create<Packet>(m_size);

            p->AddPacketTag(serialTag);

            PathSplicingPathTag pathTag;
            pathTag = reversePathTag;
            p->AddPacketTag(pathTag);

            PathSplicingReversePathTag emptyReversePathTag;
            p->AddPacketTag(emptyReversePathTag);

            socket->SendTo(p, 0, from);
        }
    }
}

} // Namespace ns3
