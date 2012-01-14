#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "path-splicing-app-client.h"
#include "path-splicing-serial-tag.h"
#include "ns3/path-splicing-hops-tag.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("PathSplicingAppClientApplication");
NS_OBJECT_ENSURE_REGISTERED(PathSplicingAppClient);

TypeId PathSplicingAppClient::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::PathSplicingAppClient")
        .SetParent<Application>()
        .AddConstructor<PathSplicingAppClient>()
        .AddAttribute("RemoteAddress",
                "The destination Ipv4Address of the outbound packets",
                Ipv4AddressValue(),
                MakeIpv4AddressAccessor(&PathSplicingAppClient::m_peerAddress),
                MakeIpv4AddressChecker())
        .AddAttribute("RemotePort",
                "The destination port of the outbound packets",
                UintegerValue(0),
                MakeUintegerAccessor(&PathSplicingAppClient::m_peerPort),
                MakeUintegerChecker<uint16_t> ())
        .AddAttribute("PacketSize",
                "Size of echo data in outbound packets",
                UintegerValue(100),
                MakeUintegerAccessor(&PathSplicingAppClient::m_size),
                MakeUintegerChecker<uint32_t>())
        .AddAttribute ("MaxPackets",
                "The maximum number of packets the application will send",
                UintegerValue(100),
                MakeUintegerAccessor(&PathSplicingAppClient::m_count),
                MakeUintegerChecker<uint32_t>())
        .AddAttribute ("Interval",
                "The time to wait between packets",
                TimeValue(Seconds(1.0)),
                MakeTimeAccessor(&PathSplicingAppClient::m_interval),
                MakeTimeChecker())
        .AddAttribute ("MaxSlices",
                "The maximum number of slices for path splicing",
                UintegerValue(1),
                MakeUintegerAccessor(&PathSplicingAppClient::m_maxSlices),
                MakeUintegerChecker<uint32_t>())
        .AddAttribute ("MaxRetx",
                "The maximum number of retry for each request",
                UintegerValue(5),
                MakeUintegerAccessor(&PathSplicingAppClient::m_maxRetx),
                MakeUintegerChecker<uint32_t>())
        ;
    return tid;
}

PathSplicingAppClient::PathSplicingAppClient()
    : m_socket(0)
    , m_size(0)
    , m_sent(0)
    , m_srtt(Seconds(0))
    , m_rttvar(Seconds(0))
    , m_rto(Seconds(1.0))
    , m_pathTag()
    , m_pendingRequests()
{
    NS_LOG_FUNCTION_NOARGS();
}

PathSplicingAppClient::~PathSplicingAppClient()
{
    NS_LOG_FUNCTION_NOARGS();
}

void PathSplicingAppClient::SetRemote(Ipv4Address ip, uint16_t port)
{
    m_peerAddress = ip;
    m_peerPort = port;
}

void PathSplicingAppClient::DoDispose()
{
    NS_LOG_FUNCTION_NOARGS();
    Application::DoDispose();
}

void PathSplicingAppClient::StartApplication()
{
    NS_LOG_FUNCTION_NOARGS();

    if (m_socket == 0)
    {
        TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
        m_socket = Socket::CreateSocket(GetNode(), tid);
        m_socket->Bind();
        m_socket->Connect(InetSocketAddress(m_peerAddress, m_peerPort));
    }

    m_socket->SetRecvCallback(MakeCallback(&PathSplicingAppClient::HandleRead, this));

    ScheduleTransmit(Seconds(0.));
}

void PathSplicingAppClient::StopApplication()
{
    NS_LOG_FUNCTION_NOARGS();

    NS_LOG_FUNCTION_NOARGS ();

    if (m_socket != 0)
    {
        m_socket->Close();
        m_socket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket> >());
        m_socket = 0;
    }

    Simulator::Cancel(m_sendEvent);
}

void PathSplicingAppClient::ScheduleTransmit(Time dt)
{
    NS_LOG_FUNCTION_NOARGS();
    m_sendEvent = Simulator::Schedule(dt, &PathSplicingAppClient::Send, this);
}

void PathSplicingAppClient::Send()
{
    NS_LOG_FUNCTION_NOARGS ();
    NS_ASSERT(m_sendEvent.IsExpired());

    //construct packet
    Ptr<Packet> p = Create<Packet>(m_size);

    //add packet tag
    PathSplicingSerialTag serialTag;
    serialTag.SetSerialNumber(m_sent);
    p->AddPacketTag(serialTag);

    p->AddPacketTag(m_pathTag);
    p->AddPacketTag(m_reversePathTag);

    PathSplicingHopsTag hopsTag;
    p->AddPacketTag(hopsTag);

    //send packet
    m_socket->Send(p);
//    NS_LOG_INFO("Node " << GetNode()->GetId() << " send request #" << m_sent << " to " << m_peerAddress);

    //schedule timeout
    EventId timeoutEvent = Simulator::Schedule(m_rto, &PathSplicingAppClient::Timeout, this, m_sent);
    PathSplicingAppPrtEntry *entry = new PathSplicingAppPrtEntry(m_sent, Simulator::Now(), timeoutEvent, m_pathTag, m_reversePathTag);
    m_pendingRequests.push_back(entry);

    m_sent ++;

    //schedule next packet
    if (m_sent < m_count)
        ScheduleTransmit(m_interval);
}

void PathSplicingAppClient::Timeout(uint32_t serialNumber)
{
    PathSplicingAppPrtEntry *entry = 0;
    std::list<PathSplicingAppPrtEntry *>::iterator it;

    for (it = m_pendingRequests.begin(); it != m_pendingRequests.end(); it ++)
    {
        if ((*it)->GetSerialNumber() == serialNumber)
        {
            entry = *it;
            break;
        }
    }
    NS_ASSERT(entry);

    NS_ASSERT(entry->GetTimeoutEvent().IsExpired());

    if (entry->GetRetx() < m_maxRetx)
    {
        PathSplicingPathTag oldPathTag = entry->GetPathSplicingPathTag();
        PathSplicingReversePathTag oldReversePathTag = entry->GetPathSplicingReversePathTag();

        if (m_pathTag == oldPathTag)
            m_pathTag.RandomizeTags(m_randPath, m_maxSlices);

        if (m_reversePathTag == oldReversePathTag)
            m_reversePathTag.RandomizeTags(m_randReversePath, m_maxSlices);

        Ptr<Packet> p = Create<Packet>(m_size);

        PathSplicingSerialTag serialTag;
        serialTag.SetSerialNumber(serialNumber);
        p->AddPacketTag(serialTag);

        p->AddPacketTag(m_reversePathTag);
        p->AddPacketTag(m_pathTag);

        m_socket->Send(p);
        entry->IncrementRetx();
//        NS_LOG_INFO("Node " << GetNode()->GetId() << " retx request #" << serialNumber << "(" << entry->GetRetx() << ") to " << m_peerAddress);

        EventId timeoutEvent = Simulator::Schedule(m_rto, &PathSplicingAppClient::Timeout, this, serialNumber);

        entry->SetTimeoutEvent(timeoutEvent);
        entry->SetSendTime(Simulator::Now());
        entry->SetPathSplicingPathTag(m_pathTag);
        entry->SetPathSplicingReversePathTag(m_reversePathTag);
    }
    else
    {
        m_pendingRequests.erase(it);
        NS_LOG_INFO("Node " << GetNode()->GetId() << " give up request #" << serialNumber << " to " << m_peerAddress);
    }
}

void PathSplicingAppClient::HandleRead(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this << socket);
    Ptr<Packet> packet;
    Address from;

    while (packet = socket->RecvFrom(from))
    {
        if (InetSocketAddress::IsMatchingType(from))
        {
            PathSplicingSerialTag serialTag;
            NS_ASSERT(packet->PeekPacketTag(serialTag));
            uint32_t serialNumber = serialTag.GetSerialNumber();

            PathSplicingAppPrtEntry *entry = 0;
            std::list<PathSplicingAppPrtEntry *>::iterator it;

            for (it = m_pendingRequests.begin(); it != m_pendingRequests.end(); it ++)
            {
                if ((*it)->GetSerialNumber() == serialNumber)
                {
                    entry = *it;
                    break;
                }
            }

            if (!entry) {
                NS_LOG_INFO("Node " << GetNode()->GetId() << " received unexpected reply #" << serialNumber << " from " << m_peerAddress);
                continue;
            }

            //cancel the timeout event
            Simulator::Cancel(entry->GetTimeoutEvent());

            PathSplicingReverseHopsTag reverseHopsTag;
            packet->PeekPacketTag(reverseHopsTag);
            PathSplicingHopsTag hopsTag;
            packet->PeekPacketTag(hopsTag);

            //update srtt, rttvar, m_rto
            Time rtt = Simulator::Now() - entry->GetSendTime();
            NS_LOG_INFO("Node " << GetNode()->GetId() << " received reply #" << serialNumber << " from " << m_peerAddress <<
                    " after " << rtt << " (retx=" << entry->GetRetx() << ") " << "path: " << GetNode()->GetId() << "-" <<
                    reverseHopsTag.ToString() << "-" << hopsTag.ToString());

            if (entry->GetRetx() > 0)
            {
                if (m_rto < rtt)
                    m_rto = rtt;

//                NS_LOG_INFO("- rto = " << m_rto);
            }
            else
            {
                //update srtt and rttvar (RFC 2988)
                if (m_srtt.GetNanoSeconds() == 0)
                {
                    //first RTT measurement
                    NS_ASSERT(m_rttvar.GetNanoSeconds() == 0);
                    m_rttvar = NanoSeconds(rtt.GetNanoSeconds() / 2);
                    m_srtt = rtt;
//                    NS_LOG_INFO("- srtt = "<< m_srtt << ", rttvar = " << m_rttvar);
                }
                else
                {
                    m_rttvar = NanoSeconds((1 - PATH_SPLICING_RTO_BETA) * m_rttvar.GetNanoSeconds() +
                            PATH_SPLICING_RTO_BETA * abs(m_srtt.GetNanoSeconds() - rtt.GetNanoSeconds()));
                    m_srtt = NanoSeconds((1 - PATH_SPLICING_RTO_ALPHA) * m_srtt.GetNanoSeconds() +
                            PATH_SPLICING_RTO_ALPHA * rtt.GetNanoSeconds());
//                    NS_LOG_INFO("srtt = "<< m_srtt << ", rttvar = " << m_rttvar);
                }

                m_rto = NanoSeconds(m_srtt.GetNanoSeconds() + PATH_SPLICING_RTO_K * m_rttvar.GetNanoSeconds());

//                NS_LOG_INFO("rto = " << m_rto);
            }

            //remove PIT entry
            m_pendingRequests.erase(it);
        }
    }
}

} // Namespace ns3
