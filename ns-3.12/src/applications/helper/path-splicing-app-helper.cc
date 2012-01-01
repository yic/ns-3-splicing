#include "path-splicing-app-helper.h"
#include "ns3/path-splicing-app-server.h"
#include "ns3/path-splicing-app-client.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"

namespace ns3 {

PathSplicingAppServerHelper::PathSplicingAppServerHelper(uint16_t port)
{
    m_factory.SetTypeId(PathSplicingAppServer::GetTypeId());
    SetAttribute("Port", UintegerValue(port));
}

void PathSplicingAppServerHelper::SetAttribute(std::string name, const AttributeValue &value)
{
    m_factory.Set(name, value);
}

ApplicationContainer PathSplicingAppServerHelper::Install(Ptr<Node> node) const
{
    return ApplicationContainer(InstallPriv(node));
}

ApplicationContainer PathSplicingAppServerHelper::Install(std::string nodeName) const
{
    Ptr<Node> node = Names::Find<Node>(nodeName);
    return ApplicationContainer(InstallPriv(node));
}

ApplicationContainer PathSplicingAppServerHelper::Install(NodeContainer c) const
{
    ApplicationContainer apps;

    for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i)
        apps.Add(InstallPriv(*i));

    return apps;
}

Ptr<Application> PathSplicingAppServerHelper::InstallPriv(Ptr<Node> node) const
{
    Ptr<Application> app = m_factory.Create<PathSplicingAppServer>();
    node->AddApplication(app);

    return app;
}

PathSplicingAppClientHelper::PathSplicingAppClientHelper(Ipv4Address address, uint16_t port)
{
    m_factory.SetTypeId(PathSplicingAppClient::GetTypeId());
    SetAttribute("RemoteAddress", Ipv4AddressValue(address));
    SetAttribute("RemotePort", UintegerValue(port));
}

void PathSplicingAppClientHelper::SetAttribute(std::string name, const AttributeValue &value)
{
    m_factory.Set(name, value);
}

ApplicationContainer PathSplicingAppClientHelper::Install(Ptr<Node> node) const
{
    return ApplicationContainer(InstallPriv(node));
}

ApplicationContainer PathSplicingAppClientHelper::Install(std::string nodeName) const
{
    Ptr<Node> node = Names::Find<Node>(nodeName);
    return ApplicationContainer(InstallPriv(node));
}

ApplicationContainer PathSplicingAppClientHelper::Install(NodeContainer c) const
{
    ApplicationContainer apps;

    for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i)
        apps.Add(InstallPriv(*i));

    return apps;
}

Ptr<Application> PathSplicingAppClientHelper::InstallPriv(Ptr<Node> node) const
{
    Ptr<Application> app = m_factory.Create<PathSplicingAppClient>();
    node->AddApplication(app);

    return app;
}

} // namespace ns3
