#include "Motd.hpp"

#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "consts.hpp"

#include <ctime>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <sstream>

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

/**
 * @brief open and read the motd.conf file
 *        replace the var by her data
 *        send the message to the client
 *
 * @param server
 * @param client
 */
void Motd::execute(Server& server, Client& client)
{
    std::string   line, newline;
    std::ifstream inputFile;

    const char*   filename = "motd.conf";
    ReplyHandler& rh       = ReplyHandler::get_instance(&server);

    inputFile.open(filename);
    if (inputFile.is_open()) {
        std::string nick = client.get_nickname();
        rh.process_response(client, RPL_MOTDSTART, nick);
        while (getline(inputFile, line)) {
            newline = _str_replace(line, "$(servername)", server.get_name());
            newline = _str_replace(newline, "$(nick)", nick);
            newline = _str_replace(newline, "$(date)", _get_current_time());
            newline.append("\r\n");
            rh.process_response(client, RPL_MOTD, nick + " :- " + newline.c_str());
            newline.clear();
        }
        rh.process_response(client, RPL_ENDOFMOTD, nick);
    }
    inputFile.close();
}

/**
 * @brief find a word in a line and replace by an other
 *
 * @param str the raw line
 * @param find  the word to replace
 * @param replace the new word
 * @return a new string with the new word
 */
std::string Motd::_str_replace(const std::string& str, const std::string& find, const std::string& replace)
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

/**
 * @brief calculate and format the current date
 *
 * @return date like AAAA-MM-DD
 */
std::string Motd::_get_current_time()
{
    std::time_t        now   = std::time(0);
    struct std::tm*    tmPtr = std::localtime(&now);
    std::ostringstream oss;
    oss << (tmPtr->tm_year + EPOCH_TIME_START) << "-" << std::setfill('0') << std::setw(2) << (tmPtr->tm_mon + 1) << "-"
        << std::setfill('0') << std::setw(2) << tmPtr->tm_mday;
    return oss.str();
}
