#include <fstream>
#include <vector>
#include <utility>
#include <algorithm>
#include <sstream>

#include "ns3/string.h"

#include "ns3/log.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/ipv4-path-splicing-routing-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
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

void PathSplicingTopologyReader::LoadTopology(std::string fileName, int nSlices,
        NodeContainer &routers, NodeContainer &hosts, NetDeviceContainer **r_h_ndc,
        NetDeviceContainer ***r_r_ndc, Ipv4InterfaceContainer **r_h_ic, Ipv4InterfaceContainer ***r_r_ic)
{
    /* read topology */
    std::vector<int> nodes;
    std::list<std::pair<std::pair<int, int>, double> > links;

    std::ifstream fs;
    fs.open(fileName.c_str());
    std::string line;

    if (!fs.is_open ()) {
        NS_LOG_WARN("Cannot open file " << fileName);
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
}

void PathSplicingTopologyReader::LoadWeights(std::string filePrefix, int nSlices,
        NodeContainer &routers, NodeContainer &hosts, NetDeviceContainer **r_h_ndc,
        NetDeviceContainer ***r_r_ndc, Ipv4InterfaceContainer **r_h_ic, Ipv4InterfaceContainer ***r_r_ic)
{

}

} /* namespace ns3 */
