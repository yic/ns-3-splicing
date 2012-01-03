#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/topology-read-module.h"

NS_LOG_COMPONENT_DEFINE("SprintPathSplicing");

using namespace ns3;

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
    reader->LoadFailures(failedLinksStr);

    /* run */
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
