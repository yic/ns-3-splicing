#include "ns3/core-module.h"

NS_LOG_COMPONENT_DEFINE("SprintPathSplicing");

using namespace ns3;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;

    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }

    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}


int main (int argc, char *argv[])
{
    std::string failedLinksStr = "";

    CommandLine cmd;
    cmd.AddValue("FailedLinks", "Links to fail, e.g., use --FailedLinks=A-B;C-D to fail links A-B and C-D", failedLinksStr);
    cmd.Parse (argc, argv);

    //fail links
    std::vector<std::string> links = split(failedLinksStr, ';');

    for (std::vector<std::string>::iterator link_it = links.begin(); link_it != links.end(); link_it ++) {
        std::vector<std::string> ends = split(*link_it, '-');

        if (ends.size() != 2) {
            NS_LOG_UNCOND("wrong link format: " << *link_it);
            continue;
        }

        NS_LOG_UNCOND(atoi(ends[0].c_str()) << " " << atoi(ends[1].c_str()));
        //to-do schedule link failures
    }

    return 0;
}