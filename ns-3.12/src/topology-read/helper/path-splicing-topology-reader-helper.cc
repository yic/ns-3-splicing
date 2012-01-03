#include "ns3/path-splicing-topology-reader-helper.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("PathSplicingTopologyReaderHelper");

PathSplicingTopologyReaderHelper::PathSplicingTopologyReaderHelper()
{
    m_reader = 0;
}

Ptr<PathSplicingTopologyReader> PathSplicingTopologyReaderHelper::GetTopologyReader(std::string latencyFileName)
{
    m_reader = CreateObject<PathSplicingTopologyReader>(latencyFileName);
    return m_reader;
}

} // namespace ns3
