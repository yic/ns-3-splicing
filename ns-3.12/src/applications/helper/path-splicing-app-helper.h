#ifndef PATH_SPLICING_APP_HELPER_H
#define PATH_SPLICING_APP_HELPER_H

#include <stdint.h>
#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-address.h"

namespace ns3 {

class PathSplicingAppServerHelper
{
public:
    PathSplicingAppServerHelper(uint16_t port);

    void SetAttribute(std::string name, const AttributeValue &value);
    ApplicationContainer Install(Ptr<Node> node) const;
    ApplicationContainer Install(std::string nodeName) const;
    ApplicationContainer Install(NodeContainer c) const;

private:
    Ptr<Application> InstallPriv(Ptr<Node> node) const;

    ObjectFactory m_factory;
};

class PathSplicingAppClientHelper
{
public:
    PathSplicingAppClientHelper(Ipv4Address ip, uint16_t port);

    void SetAttribute(std::string name, const AttributeValue &value);
    ApplicationContainer Install(Ptr<Node> node) const;
    ApplicationContainer Install(std::string nodeName) const;
    ApplicationContainer Install(NodeContainer c) const;

private:
    Ptr<Application> InstallPriv(Ptr<Node> node) const;
    ObjectFactory m_factory;
};

} // namespace ns3

#endif /* PATH_SPLICING_APP_HELPER_H */
