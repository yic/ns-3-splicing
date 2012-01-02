#include <fstream>
#include <vector>
#include <utility>
#include <algorithm>
#include <sstream>

#include "ns3/string.h"

#include "ns3/log.h"
#include "ns3/point-to-point-helper.h"
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

void PathSplicingTopologyReader::Load(std::string fileName, int nSlices,
        NodeContainer &routers, NodeContainer &hosts, NetDeviceContainer ***r_h_ndc,
        NetDeviceContainer ****r_r_ndc, Ipv4InterfaceContainer ***r_h_ic, Ipv4InterfaceContainer ****r_r_ic)
{
    std::vector<int> nodes;
    std::list<std::pair<std::pair<int, int>, double> > links;

    //read topology
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

    //initiate arrays
    int node_num = nodes.size();

    routers.Create(node_num);
    hosts.Create(node_num);

    *r_h_ndc = new NetDeviceContainer *[node_num];
    for (int i = 0; i < node_num; i ++)
        (*r_h_ndc)[i] = NULL;

    *r_r_ndc = new NetDeviceContainer **[node_num];
    for (int i = 0; i < node_num; i ++) {
        (*r_r_ndc)[i] = new NetDeviceContainer *[node_num];

        for (int j = 0; j < node_num; j ++)
            (*r_r_ndc)[i][j] = NULL;
    }

    *r_h_ic = new Ipv4InterfaceContainer *[node_num];
    for (int i = 0; i < node_num; i ++)
        (*r_h_ic)[i] = NULL;

    *r_r_ic = new Ipv4InterfaceContainer **[node_num];
    for (int i = 0; i < node_num; i ++) {
        (*r_r_ic)[i] = new Ipv4InterfaceContainer *[node_num];

        for (int j = 0; j < node_num; j ++)
            (*r_r_ic)[i][j] = NULL;
    }

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
        (*r_r_ndc)[from][to] = &ndc;
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
        (*r_h_ndc)[from] = &ndc;
    }

    //install Internet stack

}

} /* namespace ns3 */
