#ifndef PATH_SPLICING_BLACKHOLE_TOPOLOGY_READER_HELPER_H
#define PATH_SPLICING_BLACKHOLE_TOPOLOGY_READER_HELPER_H

#include "ns3/path-splicing-blackhole-topology-reader.h"

namespace ns3 {

class PathSplicingBlackholeTopologyReaderHelper
{
public:
    PathSplicingBlackholeTopologyReaderHelper();
    Ptr<PathSplicingBlackholeTopologyReader> GetTopologyReader(std::string latencyFileName, uint32_t linkBandwidth,
            uint32_t victimId, uint32_t attackerId);

private:
    Ptr<PathSplicingBlackholeTopologyReader> m_reader;
};

} // namespace ns3

#endif /* PATH_SPLICING_BLACKHOLE_TOPOLOGY_READER_HELPER_H */
