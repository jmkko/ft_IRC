#include <string>
#include <vector>

#include <iostream>
#include <sstream>
// #include <stddef.h>

int main()
{

    std::string              params = "#chan1,#chan2,#chan3 key1,key2";
    std::istringstream       iss(params);
    std::vector<std::string> channels;
    std::vector<std::string> keys;
    std::vector<std::string> cLst;
    std::string              channelsList;
    std::string              keysList;
    std::string              value;

    iss >> channelsList;
    iss >> keysList;
    if (iss >> std::ws && !iss.eof())
        std::cout << "to many params\n";
    if (channelsList[channelsList.size()-1] == ',')
        std::cout << "empty last chan" << std::endl;
    std::cout << "first token : " << channelsList << std::endl;
    std::istringstream issChannels(channelsList);
    while (std::getline(issChannels, value, ',')) {
        if (value[0] == '#' || value[0] == '&')
            channels.push_back(value);
        else
         std::cout << "Bad Channel Mask\n";
    }
    std::cout << "second token : " << keysList << std::endl;
    std::istringstream issKeys(keysList);
    while (std::getline(issKeys, value, ',')) {
        keys.push_back(value);
    }

    std::vector<std::string>::iterator it = channels.begin();
    size_t                             rank = 0;
    for (; it < channels.end(); it++) {
        std::cout << "channels : " << *it << std::endl;
        if (rank < keys.size())
            cLst.push_back(*it + " "+ keys[rank]);
        else
            cLst.push_back(*it);
        rank++;
    }
    std::vector<std::string>::iterator ite = cLst.begin();
    std::string chan;
    std::string psswd;
    while (ite != cLst.end())
    {
        std::istringstream isschan(*ite);
        std::cout << *ite << std::endl;
        isschan >> chan;
        isschan >> psswd;
        if (psswd.empty())
            std::cout << "no key channel\n";
        chan.clear(); psswd.clear();
        ite++;
    }
    std::set<>

    std::for_each(v.begin(), v.end(), [](int &n) { n++; });
}
