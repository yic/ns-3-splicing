/* Path splicing routing helper */

#include <vector>
#include "ns3/log.h"
#include "ns3/ptr.h"
#include "ns3/names.h"
#include "ns3/node.h"
#include "ns3/ipv4.h"
#include "ns3/ipv4-route.h"
#include "ns3/assert.h"
#include "ns3/node-list.h"
#include "ns3/global-route-manager.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-routing-protocol.h"
#include "ns3/ipv4-global-routing.h"
#include "ipv4-path-splicing-routing-helper.h"
#include "ipv4-global-routing-helper.h"

NS_LOG_COMPONENT_DEFINE("Ipv4PathSplicingRoutingHelper");

namespace ns3 {

Ipv4PathSplicingRoutingHelper::Ipv4PathSplicingRoutingHelper()
    : n_slices(1)
{
    m_nodeMetrics = new NodeMetrics[n_slices];
}

Ipv4PathSplicingRoutingHelper::Ipv4PathSplicingRoutingHelper(const Ipv4PathSplicingRoutingHelper &o)
{
    n_slices = o.n_slices;
    m_nodeMetrics = new NodeMetrics[n_slices];
}

Ipv4PathSplicingRoutingHelper::~Ipv4PathSplicingRoutingHelper()
{
    delete [] m_nodeMetrics;
}

Ipv4PathSplicingRoutingHelper* Ipv4PathSplicingRoutingHelper::Copy(void) const
{
    return new Ipv4PathSplicingRoutingHelper(*this);
}

Ptr<Ipv4RoutingProtocol> Ipv4PathSplicingRoutingHelper::Create(Ptr<Node> node) const
{
    Ptr<Ipv4PathSplicingRouting> pathSplicing = CreateObject<Ipv4PathSplicingRouting>(n_slices);

    Ipv4GlobalRoutingHelper helper;
    Ptr<Ipv4GlobalRouting> globalRouting = DynamicCast<Ipv4GlobalRouting>(helper.Create(node));
    NS_ASSERT(globalRouting);
    pathSplicing->SetActualGlobalRouting(globalRouting);

    return pathSplicing;
}

void Ipv4PathSplicingRoutingHelper::SetNSlices(int nSlices)
{
    n_slices = nSlices;
    delete [] m_nodeMetrics;
    m_nodeMetrics = new NodeMetrics[n_slices];
}

Ptr<Ipv4PathSplicingRouting> Ipv4PathSplicingRoutingHelper::GetPathSplicingRouting(Ptr<Ipv4> ipv4)
{
    NS_LOG_FUNCTION(this);

    Ptr<Ipv4RoutingProtocol> ipv4rp = ipv4->GetRoutingProtocol();
    NS_ASSERT_MSG(ipv4rp, "No routing protocol associated with Ipv4");

    if (DynamicCast<Ipv4PathSplicingRouting>(ipv4rp))
        return DynamicCast<Ipv4PathSplicingRouting>(ipv4rp);

    return 0;
}

void Ipv4PathSplicingRoutingHelper::PopulateAllRoutingTables(void)
{
    NS_LOG_FUNCTION(this);

    for (int i = 0; i < n_slices; i ++)
    {
        for (NodeList::Iterator node = NodeList::Begin(); node != NodeList::End(); node ++)
        {
            Ptr<Ipv4> ipv4 = (*node)->GetObject<Ipv4>();
            Ptr<Ipv4PathSplicingRouting> pathSplicing = GetPathSplicingRouting(ipv4);
            pathSplicing->ClearGlobalRoutingTable();

            //set link metrics according to configuration

            for (uint32_t iface = 1; iface < ipv4->GetNInterfaces(); iface ++)
            {
                NS_ASSERT(m_nodeMetrics[i][(*node)->GetId()][iface]);
                ipv4->SetMetric(iface, m_nodeMetrics[i][(*node)->GetId()][iface]);
            }
        }

        //compute routing tables
        GlobalRouteManager::ClearLSDB();
        GlobalRouteManager::BuildGlobalRoutingDatabase();
        GlobalRouteManager::InitializeRoutes();

        //copy global routing tables to slice routing tables
        for (NodeList::Iterator node = NodeList::Begin(); node != NodeList::End(); node ++)
        {
            Ptr<Ipv4> ipv4 = (*node)->GetObject<Ipv4>();
            Ptr<Ipv4PathSplicingRouting> pathSplicing = GetPathSplicingRouting(ipv4);
            pathSplicing->CopyGlobalRoutingTableToSlice(i);
        }
    }
}

void Ipv4PathSplicingRoutingHelper::SetMetric(int slice, uint32_t nodeId, uint32_t interface, uint16_t metric)
{
    NS_ASSERT(slice < n_slices);
    NS_ASSERT(nodeId < NodeList::GetNNodes());

    if (m_nodeMetrics[slice].size() < NodeList::GetNNodes())
        m_nodeMetrics[slice].resize(NodeList::GetNNodes());

    Ptr<Ipv4> ipv4 = NodeList::GetNode(nodeId)->GetObject<Ipv4>();
    NS_ASSERT(ipv4);
    NS_ASSERT(interface < ipv4->GetNInterfaces());

    if (m_nodeMetrics[slice][nodeId].size() < ipv4->GetNInterfaces())
        m_nodeMetrics[slice][nodeId].resize(ipv4->GetNInterfaces());

    m_nodeMetrics[slice][nodeId][interface] = metric;
}

} // namespace ns3
