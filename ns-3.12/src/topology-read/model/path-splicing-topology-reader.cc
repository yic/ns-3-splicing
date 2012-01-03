#include <fstream>
#include <vector>
#include <utility>
#include <algorithm>
#include <sstream>

#include "ns3/string.h"
#include "ns3/uinteger.h"

#include "ns3/log.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/ipv4-path-splicing-routing-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/path-splicing-app-helper.h"
#include "ns3/application-container.h"
#include "path-splicing-topology-reader.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("PathSplicingPathSplicingTopologyReader");

TypeId PathSplicingTopologyReader::GetTypeId (void)
{
  static TypeId tid = TypeId("ns3::PathSplicingTopologyReader")
    .SetParent<Object>()
  ;
  return tid;
}

PathSplicingTopologyReader::PathSplicingTopologyReader()
{
}

void PathSplicingTopologyReader::Load(std::string latencyFileName, std::string weightFilePrefix, int nSlices,
        NodeContainer &routers, NodeContainer &hosts, NetDeviceContainer **r_h_ndc,
        NetDeviceContainer ***r_r_ndc, Ipv4InterfaceContainer **r_h_ic, Ipv4InterfaceContainer ***r_r_ic)
{
    /* read topology */
    std::vector<int> nodes;
    std::list<std::pair<std::pair<int, int>, double> > links;

    std::ifstream fs;
    fs.open(latencyFileName.c_str());
    std::string line;

    if (!fs.is_open ()) {
        NS_LOG_WARN("Cannot open file " << latencyFileName);
        return;
    }

    int from, to;
    double latency;

    while (!fs.eof()) {
        line.clear();
        getline(fs, line);

        if (line.length() > 0) {
            int count = sscanf(line.c_str(), "%d %d %lf", &from, &to, &latency);
            NS_ASSERT(count == 3);

            nodes.push_back(from);
            nodes.push_back(to);
            links.push_back(std::make_pair(std::make_pair(from, to), latency));
        }
    }

    fs.close();

    std::sort(nodes.begin(), nodes.end());
    nodes.erase(std::unique(nodes.begin(), nodes.end()), nodes.end());
    NS_LOG_INFO("Number of nodes: " << nodes.size() << ", Number of links: " << links.size());

    /* initiate containers */
    int node_num = nodes.size();

    routers.Create(node_num);
    hosts.Create(node_num);

    *r_h_ndc = new NetDeviceContainer[node_num];

    *r_r_ndc = new NetDeviceContainer *[node_num];
    for (int i = 0; i < node_num; i ++)
        (*r_r_ndc)[i] = new NetDeviceContainer[node_num];

    *r_h_ic = new Ipv4InterfaceContainer[node_num];

    *r_r_ic = new Ipv4InterfaceContainer *[node_num];
    for (int i = 0; i < node_num; i ++)
        (*r_r_ic)[i] = new Ipv4InterfaceContainer[node_num];

    /* create links */
    PointToPointHelper p2pHelper;
    std::stringstream ss;

    //router-to-router links
    std::list<std::pair<std::pair<int, int>, double> >::iterator link_it;
    for (link_it = links.begin(); link_it != links.end(); link_it ++)
    {
        from = (*link_it).first.first;
        to = (*link_it).first.second;
        latency = (*link_it).second;
        ss.str("");
        ss << latency << "ms";

        NodeContainer nc = NodeContainer(routers.Get(from), routers.Get(to));
        p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
        p2pHelper.SetChannelAttribute("Delay", StringValue(ss.str()));
        NetDeviceContainer ndc = p2pHelper.Install(nc);
        (*r_r_ndc)[from][to] = ndc;
    }

    //router-to-host links
    std::vector<int>::iterator node_it;
    for (node_it = nodes.begin(); node_it != nodes.end(); node_it ++)
    {
        from = *node_it;
        NodeContainer nc = NodeContainer(routers.Get(from), hosts.Get(from));
        p2pHelper.SetDeviceAttribute("DataRate", StringValue("1000Mbps"));
        p2pHelper.SetChannelAttribute("Delay", StringValue("0.1ms"));
        NetDeviceContainer ndc = p2pHelper.Install(nc);
        (*r_h_ndc)[from] = ndc;
    }

    /* install Internet stack */
    Ipv4PathSplicingRoutingHelper splicingHelper;
    splicingHelper.SetNSlices(nSlices);
    InternetStackHelper internetHelper;
    internetHelper.SetRoutingHelper(splicingHelper);
    internetHelper.Install(routers);
    internetHelper.Install(hosts);

    /* assign IP addresses */
    Ipv4AddressHelper ipv4Helper;

    int first, second, third;
    //router-to-router links
    for (link_it = links.begin(); link_it != links.end(); link_it ++)
    {
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
        NS_ASSERT((*r_r_ndc)[from][to].GetN() == 2);
        Ipv4InterfaceContainer ic = ipv4Helper.Assign((*r_r_ndc)[from][to]);
        (*r_r_ic)[from][to] = ic;
    }

    //router-to-host links
    for (node_it = nodes.begin(); node_it != nodes.end(); node_it ++)
    {
        from = *node_it;

        first = 192;
        second = 168;
        third = from;

        while (third > 255) {
            second += 1;
            third -= 256;
        }

        ss.str("");
        ss << first << "." << second << "." << third << ".0";

        ipv4Helper.SetBase(ss.str().c_str(), "255.255.255.0");
        NS_ASSERT((*r_h_ndc)[from].GetN() == 2);
        Ipv4InterfaceContainer ic = ipv4Helper.Assign((*r_h_ndc)[from]);
        (*r_h_ic)[from] = ic;
    }

    /* configure link weights */
    int weight;

    //router-to-router links
    //default slice
    ss.str("");
    ss << weightFilePrefix << ".orig";

    fs.open(ss.str().c_str());

    if (!fs.is_open ()) {
        NS_LOG_WARN("Cannot open file " << ss.str());
        return;
    }

    while (!fs.eof()) {
        line.clear();
        getline(fs, line);

        if (line.length() > 0) {
            int count = sscanf(line.c_str(), "%d %d %d", &from, &to, &weight);
            NS_ASSERT(count == 3);
            NS_ASSERT((*r_r_ic)[from][to].GetN() == 2);
            splicingHelper.SetMetric(0, routers.Get(from)->GetId(), (*r_r_ic)[from][to].Get(0).second, weight);
            splicingHelper.SetMetric(0, routers.Get(to)->GetId(), (*r_r_ic)[from][to].Get(1).second, weight);
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
                int count = sscanf(line.c_str(), "%d %d %d", &from, &to, &weight);
                NS_ASSERT(count == 3);
                NS_ASSERT((*r_r_ic)[from][to].GetN() == 2);
                splicingHelper.SetMetric(i, routers.Get(from)->GetId(), (*r_r_ic)[from][to].Get(0).second, weight);
                splicingHelper.SetMetric(i, routers.Get(to)->GetId(), (*r_r_ic)[from][to].Get(1).second, weight);
            }
        }

        fs.close();
    }

    //router-to-host links
    for (node_it = nodes.begin(); node_it != nodes.end(); node_it ++) {
        from = *node_it;
        NS_ASSERT((*r_h_ic)[from].GetN() == 2);

        for (int i = 0; i < nSlices; i ++) {
            splicingHelper.SetMetric(i, routers.Get(from)->GetId(), (*r_h_ic)[from].Get(0).second, 1);
            splicingHelper.SetMetric(i, hosts.Get(from)->GetId(), (*r_h_ic)[from].Get(1).second, 1);
        }
    }

    splicingHelper.PopulateAllRoutingTables();
}

void PathSplicingTopologyReader::LoadServers(NodeContainer &hosts, double startTime, double stopTime, uint32_t portNumber)
{
    PathSplicingAppServerHelper serverHelper(portNumber);

    for (uint32_t i = 0; i < hosts.GetN(); i ++) {
        ApplicationContainer ac = serverHelper.Install(hosts.Get(i));
        ac.Start(Seconds(startTime));
        ac.Stop(Seconds(stopTime));
    }
}

void PathSplicingTopologyReader::LoadClients(NodeContainer &hosts, uint32_t maxSlices, uint32_t maxCount, uint32_t maxRetx, double packetInterval,
        double startTime, double stopTime, uint32_t packetSize, uint32_t portNumber)
{
    for (uint32_t i = 0; i < hosts.GetN(); i ++) {
        Ptr<Ipv4> ipv4 = hosts.Get(i)->GetObject<Ipv4>();
        PathSplicingAppClientHelper clientHelper(ipv4->GetAddress(1, 0).GetLocal(), portNumber);

        clientHelper.SetAttribute("MaxSlices", UintegerValue(maxSlices));
        clientHelper.SetAttribute("MaxPackets", UintegerValue(maxCount));
        clientHelper.SetAttribute("MaxRetx", UintegerValue(maxRetx));
        clientHelper.SetAttribute("Interval", TimeValue(Seconds(packetInterval)));
        clientHelper.SetAttribute("PacketSize", UintegerValue(packetSize));

        for (uint32_t j = 0; j < hosts.GetN(); j ++) {
            if (i != j) {
                ApplicationContainer ac = clientHelper.Install(hosts.Get(j));
                ac.Start(Seconds(startTime));
                ac.Stop(Seconds(stopTime));
            }
        }
    }
}

} /* namespace ns3 */
