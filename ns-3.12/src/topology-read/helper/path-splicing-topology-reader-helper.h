#ifndef PATH_SPLICING_TOPOLOGY_READER_HELPER_H
#define PATH_SPLICING_TOPOLOGY_READER_HELPER_H

#include "ns3/path-splicing-topology-reader.h"

namespace ns3 {

class PathSplicingTopologyReaderHelper
{
public:
    PathSplicingTopologyReaderHelper();
    Ptr<PathSplicingTopologyReader> GetTopologyReader();

private:
    Ptr<PathSplicingTopologyReader> m_reader;
};

} // namespace ns3

#endif /* PATH_SPLICING_TOPOLOGY_READER_HELPER_H */
