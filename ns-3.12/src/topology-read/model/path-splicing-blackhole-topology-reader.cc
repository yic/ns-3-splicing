#include <fstream>
#include <vector>
#include <utility>
#include <algorithm>
#include <sstream>

#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/random-variable.h"

#include "ns3/log.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/path-splicing-app-helper.h"
#include "ns3/application-container.h"
#include "path-splicing-blackhole-topology-reader.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("PathSplicingBlackholeTopologyReader");

TypeId PathSplicingBlackholeTopologyReader::GetTypeId(void)
{
  static TypeId tid = TypeId("ns3::PathSplicingBlackholeTopologyReader")
    .SetParent<Object>()
  ;
  return tid;
}

PathSplicingBlackholeTopologyReader::PathSplicingBlackholeTopologyReader(std::string latencyFileName, uint32_t linkBandwidth,
        uint32_t victimId, uint32_t attackerId)
    : m_victimId(victimId)
    , m_attackerId(attackerId)
{
    /* read topology */
    std::ifstream fs;
    fs.open(latencyFileName.c_str());
    std::string line;

    if (!fs.is_open ()) {
        NS_LOG_WARN("Cannot open file " << latencyFileName);
        return;
    }

    int from, to;
    double latency;
    int count;

    while (!fs.eof()) {
        line.clear();
        getline(fs, line);

        if (line.length() > 0) {
            count = sscanf(line.c_str(), "%d %d %lf", &from, &to, &latency);
            NS_ASSERT(count == 3);

            m_nodes.push_back(from);
            m_nodes.push_back(to);
            m_links.push_back(std::make_pair(std::make_pair(from, to), latency));
        }
    }

    fs.close();

    std::sort(m_nodes.begin(), m_nodes.end());
    m_nodes.erase(std::unique(m_nodes.begin(), m_nodes.end()), m_nodes.end());
    NS_LOG_INFO("Number of nodes: " << m_nodes.size() << ", Number of links: " << m_links.size());

    /* initiate containers */
    uint32_t node_num = m_nodes.size();

    NS_ASSERT(m_victimId != m_attackerId);
    NS_ASSERT(m_victimId < node_num);
    NS_ASSERT(m_attackerId < node_num);

    m_routers.Create(node_num);
    m_hosts.Create(node_num);

    m_r_h_ndc = new NetDeviceContainer[node_num];

    m_r_r_ndc = new NetDeviceContainer *[node_num];
    for (uint32_t i = 0; i < node_num; i ++)
        m_r_r_ndc[i] = new NetDeviceContainer[node_num];

    m_r_h_ic = new Ipv4InterfaceContainer[node_num];

    m_r_r_ic = new Ipv4InterfaceContainer *[node_num];
    for (uint32_t i = 0; i < node_num; i ++)
        m_r_r_ic[i] = new Ipv4InterfaceContainer[node_num];

    /* create links */
    PointToPointHelper p2pHelper;
    std::stringstream ssDelay, ssBandwidth;

    ssBandwidth.str("");
    ssBandwidth << linkBandwidth << "Mbps";

    //router-to-router links
    std::list<std::pair<std::pair<int, int>, double> >::iterator link_it;

    m_nodeDegree = new uint32_t[node_num];
    for (uint32_t i = 0; i < node_num; i ++)
        m_nodeDegree[i] = 0;

    for (link_it = m_links.begin(); link_it != m_links.end(); link_it ++)
    {
        from = (*link_it).first.first;
        to = (*link_it).first.second;
        m_nodeDegree[from] ++;
        m_nodeDegree[to] ++;
        latency = (*link_it).second;
        ssDelay.str("");
        ssDelay << latency << "ms";

        NodeContainer nc = NodeContainer(m_routers.Get(from), m_routers.Get(to));
        p2pHelper.SetDeviceAttribute("DataRate", StringValue(ssBandwidth.str()));
        p2pHelper.SetChannelAttribute("Delay", StringValue(ssDelay.str()));
        m_r_r_ndc[from][to] = p2pHelper.Install(nc);
    }

//  if (m_nodeDegree[m_victimId] == 1 || m_nodeDegree[m_attackerId] == 1)
//      exit(0);

    //router-to-host links
    for (uint32_t i = 0; i < node_num; i ++)
    {
        NodeContainer nc = NodeContainer(m_routers.Get(i), m_hosts.Get(i));
        p2pHelper.SetDeviceAttribute("DataRate", StringValue(ssBandwidth.str()));
        p2pHelper.SetChannelAttribute("Delay", StringValue("0.1ms"));
        m_r_h_ndc[i] = p2pHelper.Install(nc);
    }

    //attacker-to-victim link
    NodeContainer nc = NodeContainer(m_routers.Get(attackerId), m_hosts.Get(victimId));
    p2pHelper.SetDeviceAttribute("DataRate", StringValue(ssBandwidth.str()));
    p2pHelper.SetChannelAttribute("Delay", StringValue("0.1ms"));
    m_a_v_ndc = p2pHelper.Install(nc);
}

PathSplicingBlackholeTopologyReader::~PathSplicingBlackholeTopologyReader()
{
    delete [] m_r_h_ndc;

    for (uint32_t i = 0; i < m_nodes.size(); i ++)
        delete [] m_r_r_ndc[i];
    delete [] m_r_r_ndc;

    delete [] m_r_h_ic;

    for (uint32_t i = 0; i < m_nodes.size(); i ++)
        delete [] m_r_r_ic[i];
    delete [] m_r_r_ic;

    delete [] m_nodeDegree;
}

void PathSplicingBlackholeTopologyReader::LoadPathSplicing(std::string weightFilePrefix, int nSlices)
{
    /* install Internet stack */
    m_splicingHelper.SetNSlices(nSlices);
    InternetStackHelper internetHelper;
    internetHelper.SetRoutingHelper(m_splicingHelper);
    internetHelper.Install(m_routers);
    internetHelper.Install(m_hosts);

    /* assign IP addresses */
    Ipv4AddressHelper ipv4Helper;
    std::stringstream ss;
    int first, second, third, from, to;

    //router-to-router links
    std::list<std::pair<std::pair<int, int>, double> >::iterator link_it;

    for (link_it = m_links.begin(); link_it != m_links.end(); link_it ++) {
        from = (*link_it).first.first;
        to = (*link_it).first.second;

        first = 10;
        second = from;
        third = to;

        if (second > 255) {
            first += 1;
            second -= 256;
        }

        if (third > 255) {
            first += 2;
            third -= 256;
        }

        ss.str("");
        ss << first << "." << second << "." << third << ".0";

        ipv4Helper.SetBase(ss.str().c_str(), "255.255.255.0");
        NS_ASSERT(m_r_r_ndc[from][to].GetN() == 2);
        m_r_r_ic[from][to] = ipv4Helper.Assign(m_r_r_ndc[from][to]);
    }

    //router-to-host links
    for (uint32_t i = 0; i < m_nodes.size(); i ++)
    {
        first = 192;
        second = 168;
        third = i;

        while (third > 255) {
            second += 1;
            third -= 256;
        }

        ss.str("");
        ss << first << "." << second << "." << third << ".0";

        ipv4Helper.SetBase(ss.str().c_str(), "255.255.255.0");
        NS_ASSERT(m_r_h_ndc[i].GetN() == 2);
        m_r_h_ic[i] = ipv4Helper.Assign(m_r_h_ndc[i]);
    }

    //attacker-to-victim link
    ipv4Helper.SetBase("172.16.0.0", "255.255.255.0");
    m_a_v_ic = ipv4Helper.Assign(m_a_v_ndc);

    /* configure link weights */
    int weight;

    //router-to-router links
    //default slice
    ss.str("");
    ss << weightFilePrefix << ".orig";

    std::ifstream fs;
    std::string line;

    fs.open(ss.str().c_str());

    if (!fs.is_open ()) {
        NS_LOG_WARN("Cannot open file " << ss.str());
        return;
    }

    int count;

    while (!fs.eof()) {
        line.clear();
        getline(fs, line);

        if (line.length() > 0) {
            count = sscanf(line.c_str(), "%d %d %d", &from, &to, &weight);
            NS_ASSERT(count == 3);
            NS_ASSERT(m_r_r_ic[from][to].GetN() == 2);
            m_splicingHelper.SetMetric(0, m_routers.Get(from)->GetId(), m_r_r_ic[from][to].Get(0).second, weight);
            m_splicingHelper.SetMetric(0, m_routers.Get(to)->GetId(), m_r_r_ic[from][to].Get(1).second, weight);
        }
    }

    fs.close();

    //other slices
    for (int i = 1; i < nSlices; i ++) {
        ss.str("");
        ss << weightFilePrefix << "." << i;

        fs.open(ss.str().c_str());

        if (!fs.is_open ()) {
            NS_LOG_WARN("Cannot open file " << ss.str());
            return;
        }

        while (!fs.eof()) {
            line.clear();
            getline(fs, line);

            if (line.length() > 0) {
                count = sscanf(line.c_str(), "%d %d %d", &from, &to, &weight);
                NS_ASSERT(count == 3);
                NS_ASSERT(m_r_r_ic[from][to].GetN() == 2);
                m_splicingHelper.SetMetric(i, m_routers.Get(from)->GetId(), m_r_r_ic[from][to].Get(0).second, weight);
                m_splicingHelper.SetMetric(i, m_routers.Get(to)->GetId(), m_r_r_ic[from][to].Get(1).second, weight);
            }
        }

        fs.close();
    }

    //router-to-host links
    for (uint32_t i = 0; i < m_nodes.size(); i ++) {
        NS_ASSERT(m_r_h_ic[i].GetN() == 2);

        for (int j = 0; j < nSlices; j ++) {
            m_splicingHelper.SetMetric(j, m_routers.Get(i)->GetId(), m_r_h_ic[i].Get(0).second, 1);
            m_splicingHelper.SetMetric(j, m_hosts.Get(i)->GetId(), m_r_h_ic[i].Get(1).second, 1);
        }
    }

    //attacker-to-victim link
    for (int j = 0; j < nSlices; j ++) {
        m_splicingHelper.SetMetric(j, m_routers.Get(m_attackerId)->GetId(), m_a_v_ic.Get(0).second, 1);
        m_splicingHelper.SetMetric(j, m_hosts.Get(m_victimId)->GetId(), m_a_v_ic.Get(1).second, 1);
    }

    m_a_v_ic.Get(0).first->SetDown(m_a_v_ic.Get(0).second);
    m_a_v_ic.Get(1).first->SetDown(m_a_v_ic.Get(1).second);

    m_splicingHelper.PopulateAllRoutingTables();
}

void PathSplicingBlackholeTopologyReader::LoadServers(double startTime, double stopTime, uint32_t portNumber, uint32_t packetSize)
{
    PathSplicingAppServerHelper serverHelper(portNumber);

    ApplicationContainer ac = serverHelper.Install(m_hosts.Get(m_victimId));
    serverHelper.SetAttribute("PacketSize", UintegerValue(packetSize));
    ac.Start(Seconds(startTime));
    ac.Stop(Seconds(stopTime));
}

void PathSplicingBlackholeTopologyReader::LoadClients(uint32_t maxSlices, uint32_t maxCount, uint32_t maxRetx,
        double packetInterval, double startTime, double stopTime, uint32_t portNumber, uint32_t packetSize)
{
    Ptr<Ipv4> ipv4 = m_hosts.Get(m_victimId)->GetObject<Ipv4>();
    PathSplicingAppClientHelper clientHelper(ipv4->GetAddress(1, 0).GetLocal(), portNumber);

    clientHelper.SetAttribute("MaxSlices", UintegerValue(maxSlices));
    clientHelper.SetAttribute("MaxPackets", UintegerValue(maxCount));
    clientHelper.SetAttribute("MaxRetx", UintegerValue(maxRetx));
    clientHelper.SetAttribute("Interval", TimeValue(Seconds(packetInterval)));
    clientHelper.SetAttribute("PacketSize", UintegerValue(packetSize));

    for (uint32_t i = 0; i < m_hosts.GetN(); i ++) {
//      if (i != m_victimId && i != m_attackerId && m_nodeDegree[i] > 1) {
        if (i != m_victimId && i != m_attackerId) {
            ApplicationContainer ac = clientHelper.Install(m_hosts.Get(i));
            ac.Start(Seconds(startTime));
            ac.Stop(Seconds(stopTime));
        }
    }
}

void PathSplicingBlackholeTopologyReader::StartHijackingAt(uint32_t time)
{
    Simulator::Schedule(Seconds(time - 0.1), &Ipv4::SetUp, m_a_v_ic.Get(0).first, m_a_v_ic.Get(0).second);
    Simulator::Schedule(Seconds(time - 0.1), &Ipv4::SetUp, m_a_v_ic.Get(1).first, m_a_v_ic.Get(1).second);

    Simulator::Schedule(Seconds(time), &Ipv4PathSplicingRoutingHelper::PopulateAllRoutingTables, &m_splicingHelper);

    Simulator::Schedule(Seconds(time + 0.1), &Ipv4::SetDown, m_a_v_ic.Get(0).first, m_a_v_ic.Get(0).second);
    Simulator::Schedule(Seconds(time + 0.1), &Ipv4::SetDown, m_a_v_ic.Get(1).first, m_a_v_ic.Get(1).second);
}

} /* namespace ns3 */
