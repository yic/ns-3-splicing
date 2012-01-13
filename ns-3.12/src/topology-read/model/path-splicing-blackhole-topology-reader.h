#ifndef PATH_SPLICING_BLACKHOLE_TOPOLOGY_READER_H
#define PATH_SPLICING_BLACKHOLE_TOPOLOGY_READER_H

#include "ns3/object.h"
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/ipv4-path-splicing-routing-helper.h"

namespace ns3 {

class PathSplicingBlackholeTopologyReader : public Object
{
public:
    static TypeId GetTypeId(void);

    PathSplicingBlackholeTopologyReader(std::string latencyFileName, uint32_t linkBandwidth,
            uint32_t victimId, uint32_t attackerId);
    virtual ~PathSplicingBlackholeTopologyReader();

    void LoadPathSplicing(std::string weightFilePrefix, int nSlices);
    void LoadServers(double startTime, double stopTime, uint32_t portNumber, uint32_t packetSize);
    void LoadClients(uint32_t maxSlices, uint32_t maxCount, uint32_t maxRetx, double packetInterval,
            double startTime, double stopTime, uint32_t portNumber, uint32_t packetSize);

    void StartHijackingAt(uint32_t time);
private:
    NodeContainer m_routers;
    NodeContainer m_hosts;

    std::vector<int> m_nodes;
    std::list<std::pair<std::pair<int, int>, double> > m_links;

    NetDeviceContainer *m_r_h_ndc;
    NetDeviceContainer **m_r_r_ndc;
    NetDeviceContainer m_a_v_ndc;
    Ipv4InterfaceContainer *m_r_h_ic;
    Ipv4InterfaceContainer **m_r_r_ic;
    Ipv4InterfaceContainer m_a_v_ic;

    uint32_t *m_nodeDegree;

    uint32_t m_victimId;
    uint32_t m_attackerId;
    Ipv4PathSplicingRoutingHelper m_splicingHelper;
};

}; // end namespace ns3

#endif /* PATH_SPLICING_BLACKHOLE_TOPOLOGY_READER_H */
