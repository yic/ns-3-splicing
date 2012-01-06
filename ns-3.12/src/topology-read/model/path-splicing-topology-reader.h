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

    PathSplicingTopologyReader(std::string latencyFileName);
    virtual ~PathSplicingTopologyReader();

    void LoadPathSplicing(std::string weightFilePrefix, int nSlices);
    void LoadServers(double startTime, double stopTime, uint32_t portNumber, uint32_t packetSize);
    void LoadClients(uint32_t maxSlices, uint32_t maxCount, uint32_t maxRetx, double packetInterval,
            double startTime, double stopTime, uint32_t portNumber, uint32_t packetSize);
    void LoadFailures(std::string failedLinksStr, double time);
    void GenerateRandomFailures(double probability, double time);
private:
    NodeContainer m_routers;
    NodeContainer m_hosts;

    std::vector<int> m_nodes;
    std::list<std::pair<std::pair<int, int>, double> > m_links;

    NetDeviceContainer *m_r_h_ndc;
    NetDeviceContainer **m_r_r_ndc;
    Ipv4InterfaceContainer *m_r_h_ic;
    Ipv4InterfaceContainer **m_r_r_ic;

    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
    std::vector<std::string> split(const std::string &s, char delim);
};

}; // end namespace ns3

#endif /* PATH_SPLICING_TOPOLOGY_READER_H */
