/* Path splicing routing helper */

#ifndef IPV4_PATH_SPLICING_ROUTING_HELPER_H
#define IPV4_PATH_SPLICING_ROUTING_HELPER_H

#include "ns3/ipv4.h"
#include "ns3/ipv4-path-splicing-routing.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/node.h"
#include "ns3/net-device.h"
#include "ns3/ipv4-routing-helper.h"
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"

namespace ns3 {

class Ipv4PathSplicingRoutingHelper : public Ipv4RoutingHelper
{
public:
    Ipv4PathSplicingRoutingHelper();
    Ipv4PathSplicingRoutingHelper(const Ipv4PathSplicingRoutingHelper &);
    virtual ~Ipv4PathSplicingRoutingHelper();

    Ipv4PathSplicingRoutingHelper *Copy(void) const;

    virtual Ptr<Ipv4RoutingProtocol> Create(Ptr<Node> node) const;

    void SetNSlices(int nSlices);

    Ptr<Ipv4PathSplicingRouting> GetPathSplicingRouting(Ptr<Ipv4> ipv4);

    void PopulateAllRoutingTables(void);

    void SetMetric(int slice, uint32_t nodeId, uint32_t interface, uint16_t metric);
private:
    //interface->metric
    typedef std::vector<uint16_t> Metrics;
    //node->interface->metric
    typedef std::vector<Metrics> NodeMetrics;
    //array of node metrics, one for each slice
    NodeMetrics *m_nodeMetrics;

    int n_slices;
};

} // namespace ns3

#endif /* IPV4_PATH_SPLICING_ROUTING_HELPER_H */
