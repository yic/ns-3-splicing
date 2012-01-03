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
    std::string latencyFileName("data/sprint/latency.orig");
    std::string weightFilePrefix("data/sprint/weight");

    CommandLine cmd;
    cmd.AddValue("FailedLinks", "Links to fail, e.g., use --FailedLinks=A-B;C-D to fail links A-B and C-D", failedLinksStr);
    cmd.Parse (argc, argv);

    PathSplicingTopologyReaderHelper readerHelper;
    Ptr<PathSplicingTopologyReader> reader = readerHelper.GetTopologyReader(latencyFileName);
    reader->LoadPathSplicing(weightFilePrefix, 5);
    reader->LoadServers();
    reader->LoadClients(5, 2, 5, 10);

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

    /* run */
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
