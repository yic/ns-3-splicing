#ifndef PATH_SPLICING_TOPOLOGY_READER_H
#define PATH_SPLICING_TOPOLOGY_READER_H

#include "ns3/object.h"
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/ipv4-interface-container.h"

namespace ns3 {

class PathSplicingTopologyReader : public Object
{
public:
    static TypeId GetTypeId(void);
    PathSplicingTopologyReader();
    void Load(std::string latencyFileName, std::string weightFilePrefix, int nSlices,
            NodeContainer &routers, NodeContainer &hosts, NetDeviceContainer **r_h_ndc,
            NetDeviceContainer ***r_r_ndc, Ipv4InterfaceContainer **r_h_ic, Ipv4InterfaceContainer ***r_r_ic);
    void LoadServers(NodeContainer &hosts, double startTime = 1.0, double stopTime = 100.0, uint32_t portNumber = 9);
    void LoadClients(NodeContainer &hosts, uint32_t maxSlices, uint32_t maxCount, uint32_t maxRetx, double packetInterval,
            double startTime = 1.0, double stopTime = 100.0, uint32_t packetSize = 1024, uint32_t portNumber = 9);
private:
};

}; // end namespace ns3

#endif /* PATH_SPLICING_TOPOLOGY_READER_H */
