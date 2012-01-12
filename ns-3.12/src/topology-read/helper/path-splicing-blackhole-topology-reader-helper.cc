#include "ns3/path-splicing-blackhole-topology-reader-helper.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("PathSplicingBlackholeTopologyReaderHelper");

PathSplicingBlackholeTopologyReaderHelper::PathSplicingBlackholeTopologyReaderHelper()
{
    m_reader = 0;
}

Ptr<PathSplicingBlackholeTopologyReader> PathSplicingBlackholeTopologyReaderHelper::GetTopologyReader(std::string latencyFileName, uint32_t linkBandwidth,
        uint32_t victimId, uint32_t attackerId)
{
    m_reader = CreateObject<PathSplicingBlackholeTopologyReader>(latencyFileName, linkBandwidth, victimId, attackerId);
    return m_reader;
}

} // namespace ns3
