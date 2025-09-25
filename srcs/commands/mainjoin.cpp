#include <string>
#include <vector>

#include <iostream>
#include <sstream>
// #include <stddef.h>

int main()
{

    std::string              params = "#channel1,&channel2 123,567,333";
    std::istringstream       iss(params);
    std::vector<std::string> channels;
    std::vector<std::string> keys;
    std::string              value;

    iss >> value;
    std::cout << "first token : " << value << std::endl;
    std::istringstream issChannels(value);
    while (std::getline(issChannels, value, ',')) {
        if (value[0] == '#' || value[0] == '&')
            channels.push_back(value);
        else
         std::cout << "Bad Channel Mask\n";
    }
    iss >> value;
    std::cout << "second token : " << value << std::endl;
    std::istringstream issKeys(value);
    while (std::getline(issKeys, value, ',')) {
        keys.push_back(value);
    }
    if (iss>> value)
        std::cout << "to many params\n";

    std::vector<std::string>::iterator it = channels.begin();
    size_t                             rank = 0;
    for (; it < channels.end(); it++) {
        std::cout << "channels : " << *it;
        if (rank < keys.size())
            std::cout << "--is key : " << keys[rank];
        rank++;
        std::cout << std::endl;
    }
}
