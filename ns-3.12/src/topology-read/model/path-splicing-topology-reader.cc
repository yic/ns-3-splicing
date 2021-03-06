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
#include "path-splicing-topology-reader.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("PathSplicingTopologyReader");

TypeId PathSplicingTopologyReader::GetTypeId(void)
{
  static TypeId tid = TypeId("ns3::PathSplicingTopologyReader")
    .SetParent<Object>()
  ;
  return tid;
}

PathSplicingTopologyReader::PathSplicingTopologyReader(std::string latencyFileName, uint32_t linkBandwidth)
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

    for (link_it = m_links.begin(); link_it != m_links.end(); link_it ++)
    {
        from = (*link_it).first.first;
        to = (*link_it).first.second;
        latency = (*link_it).second;
        ssDelay.str("");
        ssDelay << latency << "ms";

        NodeContainer nc = NodeContainer(m_routers.Get(from), m_routers.Get(to));
        p2pHelper.SetDeviceAttribute("DataRate", StringValue(ssBandwidth.str()));
        p2pHelper.SetChannelAttribute("Delay", StringValue(ssDelay.str()));
        m_r_r_ndc[from][to] = p2pHelper.Install(nc);
    }

    //router-to-host links
    for (uint32_t i = 0; i < node_num; i ++)
    {
        NodeContainer nc = NodeContainer(m_routers.Get(i), m_hosts.Get(i));
        p2pHelper.SetDeviceAttribute("DataRate", StringValue("10Gbps"));
        p2pHelper.SetChannelAttribute("Delay", StringValue("0.1ns"));
        m_r_h_ndc[i] = p2pHelper.Install(nc);
    }
}

PathSplicingTopologyReader::~PathSplicingTopologyReader()
{
    delete [] m_r_h_ndc;

    for (uint32_t i = 0; i < m_nodes.size(); i ++)
        delete [] m_r_r_ndc[i];
    delete [] m_r_r_ndc;

    delete [] m_r_h_ic;

    for (uint32_t i = 0; i < m_nodes.size(); i ++)
        delete [] m_r_r_ic[i];
    delete [] m_r_r_ic;
}

void PathSplicingTopologyReader::LoadPathSplicing(std::string weightFilePrefix, int nSlices)
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

    m_splicingHelper.PopulateAllRoutingTables();
}

void PathSplicingTopologyReader::LoadServers(double startTime, double stopTime, uint32_t portNumber, uint32_t packetSize)
{
    PathSplicingAppServerHelper serverHelper(portNumber);

    for (uint32_t i = 0; i < m_hosts.GetN(); i ++) {
        ApplicationContainer ac = serverHelper.Install(m_hosts.Get(i));
        serverHelper.SetAttribute("PacketSize", UintegerValue(packetSize));
        ac.Start(Seconds(startTime));
        ac.Stop(Seconds(stopTime));
    }
}

void PathSplicingTopologyReader::LoadClients(uint32_t maxSlices, uint32_t maxCount, uint32_t maxRetx,
        double packetInterval, double startTime, double stopTime, uint32_t portNumber, uint32_t packetSize)
{
    for (uint32_t i = 0; i < m_hosts.GetN(); i ++) {
        Ptr<Ipv4> ipv4 = m_hosts.Get(i)->GetObject<Ipv4>();
        PathSplicingAppClientHelper clientHelper(ipv4->GetAddress(1, 0).GetLocal(), portNumber);

        clientHelper.SetAttribute("MaxSlices", UintegerValue(maxSlices));
        clientHelper.SetAttribute("MaxPackets", UintegerValue(maxCount));
        clientHelper.SetAttribute("MaxRetx", UintegerValue(maxRetx));
        clientHelper.SetAttribute("Interval", TimeValue(Seconds(packetInterval)));
        clientHelper.SetAttribute("PacketSize", UintegerValue(packetSize));

        for (uint32_t j = 0; j < m_hosts.GetN(); j ++) {
            if (i != j) {
                ApplicationContainer ac = clientHelper.Install(m_hosts.Get(j));
                ac.Start(Seconds(startTime));
                ac.Stop(Seconds(stopTime));
            }
        }
    }
}

std::vector<std::string> &PathSplicingTopologyReader::split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;

    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }

    return elems;
}

std::vector<std::string> PathSplicingTopologyReader::split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

void PathSplicingTopologyReader::LoadFailures(std::string failedLinksStr, double time)
{
    std::vector<std::string> links = split(failedLinksStr, ';');

    for (std::vector<std::string>::iterator link_it = links.begin(); link_it != links.end(); link_it ++) {
        std::vector<std::string> ends = split(*link_it, '-');

        if (ends.size() != 2) {
            NS_LOG_UNCOND("wrong link format: " << *link_it);
            continue;
        }

        int end0 = atoi(ends[0].c_str());
        int end1 = atoi(ends[1].c_str());

        if (end0 > end1) {
            int tmp = end0;
            end0 = end1;
            end1 = tmp;
        }

        NS_LOG_UNCOND("Fail link " << end0 << "-" << end1);
        //schedule link failures
        NS_ASSERT(m_r_r_ic[end0][end1].GetN() == 2);
        Simulator::Schedule(Seconds(time), &Ipv4::SetDown, m_r_r_ic[end0][end1].Get(0).first, m_r_r_ic[end0][end1].Get(0).second);
        Simulator::Schedule(Seconds(time), &Ipv4::SetDown, m_r_r_ic[end0][end1].Get(1).first, m_r_r_ic[end0][end1].Get(1).second);
    }
}

void PathSplicingTopologyReader::GenerateRandomFailures(double probability, double time)
{
    UniformVariable rand;
    int from, to;
    std::list<std::pair<std::pair<int, int>, double> >::iterator link_it;

    for (link_it = m_links.begin(); link_it != m_links.end(); link_it ++) {
        if (rand.GetValue() < probability) {
            from = (*link_it).first.first;
            to = (*link_it).first.second;
            NS_LOG_UNCOND("Fail link " << from << "-" << to);

            NS_ASSERT(m_r_r_ic[from][to].GetN() == 2);
            Simulator::Schedule(Seconds(time), &Ipv4::SetDown, m_r_r_ic[from][to].Get(0).first, m_r_r_ic[from][to].Get(0).second);
            Simulator::Schedule(Seconds(time), &Ipv4::SetDown, m_r_r_ic[from][to].Get(1).first, m_r_r_ic[from][to].Get(1).second);
        }
    }
}


void PathSplicingTopologyReader::ScheduleRoutingRecomputation(double time)
{
    Simulator::Schedule(Seconds(time), &Ipv4PathSplicingRoutingHelper::PopulateAllRoutingTables, &m_splicingHelper);
}

} /* namespace ns3 */
