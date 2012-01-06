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
    Config::SetDefault("ns3::DropTailQueue::MaxPackets", UintegerValue(1000));

    std::string failedLinksStr("");
    std::string latencyFileName("data/sprint/latency.orig");
    std::string weightFilePrefix("data/sprint/weight");

    uint32_t nSlices = 5;
    uint32_t nPackets = 2;
    uint32_t nRetx = 5;
    double packetInterval = 10.0;

    double appStartTime = 1.0;
    double appStopTime = 100.0;
    uint32_t nAppPort = 9;
    uint32_t nRequestSize = 1024;
    uint32_t nResponseSize = 1024;

    double linkFailTime = 8.0;

    CommandLine cmd;
    cmd.AddValue("FailedLinks", "Links to fail, e.g., use --FailedLinks=A-B;C-D to fail links A-B and C-D", failedLinksStr);
    cmd.AddValue("LatencyFile", "Path to the latency file", latencyFileName);
    cmd.AddValue("WeightFilePrefix", "Prefix of the weight files", weightFilePrefix);
    cmd.AddValue("SliceNumber", "Number of slices to use for path splicing, default is 5", nSlices);
    cmd.AddValue("PacketNumber", "Number of requests to send, default is 2", nPackets);
    cmd.AddValue("RetxNumber", "Number of retx allowed, default is 5", nRetx);
    cmd.AddValue("PacketInterval", "Packet interval between consecutive requests in seconds, default is 10.0", packetInterval);
    cmd.AddValue("AppStartTime", "Application start time in seconds, default is 1.0", appStartTime);
    cmd.AddValue("AppStopTime", "Application stop time in seconds, default is 100.0", appStopTime);
    cmd.AddValue("AppPort", "Port number the application server uses, default is 9", nAppPort);
    cmd.AddValue("RequestSize", "Size of request, default is 1024", nRequestSize);
    cmd.AddValue("ResponseSize", "Size of response, default is 1024", nResponseSize);
    cmd.AddValue("LinkFailTime", "Link fail time in seconds, default is 8.0", linkFailTime);
    cmd.Parse (argc, argv);

    PathSplicingTopologyReaderHelper readerHelper;
    Ptr<PathSplicingTopologyReader> reader = readerHelper.GetTopologyReader(latencyFileName);
    reader->LoadPathSplicing(weightFilePrefix, nSlices);
    reader->LoadServers(appStartTime, appStopTime, nAppPort, nResponseSize);
    reader->LoadClients(nSlices, nPackets, nRetx, packetInterval, appStartTime, appStopTime, nAppPort, nRequestSize);
    reader->LoadFailures(failedLinksStr, linkFailTime);

    /* run */
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
