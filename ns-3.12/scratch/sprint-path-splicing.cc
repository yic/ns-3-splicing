#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/topology-read-module.h"

NS_LOG_COMPONENT_DEFINE("SprintPathSplicing");

using namespace ns3;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;

    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }

    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

int main (int argc, char *argv[])
{
    std::string failedLinksStr("");
    std::string latencyFileName("scratch/sprint/latency.orig");
    std::string weightFilePrefix("scratch/sprint/weight");

    CommandLine cmd;
    cmd.AddValue("FailedLinks", "Links to fail, e.g., use --FailedLinks=A-B;C-D to fail links A-B and C-D", failedLinksStr);
    cmd.Parse (argc, argv);

    NodeContainer routers;
    NodeContainer hosts;
    NetDeviceContainer *r_h_ndc = NULL;
    NetDeviceContainer **r_r_ndc = NULL;
    Ipv4InterfaceContainer *r_h_ic = NULL;
    Ipv4InterfaceContainer **r_r_ic = NULL;

    PathSplicingTopologyReaderHelper readerHelper;
    Ptr<PathSplicingTopologyReader> reader = readerHelper.GetTopologyReader();
    reader->LoadTopology(latencyFileName, 5, routers, hosts, &r_h_ndc, &r_r_ndc, &r_h_ic, &r_r_ic);
    reader->LoadWeights(weightFilePrefix, 5, routers, hosts, &r_h_ndc, &r_r_ndc, &r_h_ic, &r_r_ic);

    //fail links
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

//      NS_LOG_UNCOND("Fail link " << end0 << "-" << end1);
        //schedule link failures
//      NS_ASSERT(r_r_ic[end0][end1].GetN() == 2);
//      Simulator::Schedule(Seconds(8),&Ipv4::SetDown, r_r_ic[end0][end1]->Get(0).first, r_r_ic[end0][end1]->Get(0).second);
//      Simulator::Schedule(Seconds(8),&Ipv4::SetDown, r_r_ic[end0][end1]->Get(1).first, r_r_ic[end0][end1]->Get(1).second);
    }

    //delete arrays
    int node_num = routers.GetN();

    delete [] r_h_ndc;

    for (int i = 0; i < node_num; i ++)
        delete [] r_r_ndc[i];
    delete [] r_r_ndc;

    delete [] r_h_ic;

    for (int i = 0; i < node_num; i ++)
        delete [] r_r_ic[i];
    delete [] r_r_ic;

    return 0;
}
