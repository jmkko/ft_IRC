#include "Motd.hpp"

#include <ostream>

Motd::Motd() {}
Motd::~Motd() {}
Motd::Motd(const std::string& params) : _params(params) {}
Motd::Motd(const Motd& other) : _params(other._params) {}
Motd& Motd::operator=(const Motd& other)
{
    if (this != &other)
        _params = other._params;
    return (*this);
}

void Motd::execute(Server& server, Client& client)
{
    std::string        line, newline;
    std::ifstream      inputFile;
    std::ostringstream oss;

    std::string   find     = "$(servername)";
    std::string   replace  = server.get_name();
    const char*   filename = "motd.conf";
    ReplyHandler& rh       = ReplyHandler::get_instance(&server);

    inputFile.open(filename);
    if (inputFile.is_open()) {

        while (getline(inputFile, line)) {
            newline = strReplace(line, "$(servername)", server.get_name());
            newline = strReplace(line, "$(nick)", client.get_nickname());
            newline = strReplace(line, "$(date)", replace);
            oss << newline << "\r\n";
        }
    }
    inputFile.close();
}
else std::cout << RED << "No matching file" << RESET << std::endl;
return 0;
}
}

std::string Motd::_str_replace(std::string str, std::string find, std::string replace)
{
    std::string newstr, trimstr, tmpstr;

    tmpstr                     = str;
    std::string::size_type pos = str.find(find);
    while (pos != std::string::npos) {
        trimstr = tmpstr.substr(pos);
        newstr.append(tmpstr, 0, pos);
        newstr.append(replace);
        tmpstr = trimstr.substr(find.size());
        pos    = tmpstr.find(find);
    }
    newstr.append(tmpstr);
    return newstr;
}
