#include "Motd.hpp"

#include "LogManager.hpp"
#include "ReplyHandler.hpp"
#include "consts.hpp"
#include "reply_codes.hpp"

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <linux/limits.h>
#include <ostream>
#include <sstream>
#include <unistd.h>

/************************************************************
 *		🥚 CONSTRUCTORS & DESTRUCTOR						*
 ************************************************************/

Motd::Motd(const std::string& params) : _params(params) {}
Motd::~Motd() {}

/*************************************************************
 *		🛠️ FUNCTIONS											*
 *************************************************************/

void Motd::execute(Server& server, Client& client)
{
    std::string   line, newline;
    std::ifstream inputFile;

    const char* filename = MOTD_FILE;
#ifdef TEST
    filename = MOTD_FILE_FOR_TEST;
#endif

    ReplyHandler& rh = ReplyHandler::get_instance(&server);
    inputFile.open(filename);
    if (inputFile.is_open()) {
        std::string nick = client.get_nickname();
        rh.process_response(client, RPL_MOTDSTART, "", NULL, "- " + server.get_name() + " message of the day -");
        while (getline(inputFile, line)) {
            newline = _str_replace(line, "$(servername)", server.get_name());
            newline = _str_replace(newline, "$(nick)", nick);
            newline = _str_replace(newline, "$(date)", _get_current_time());
            rh.process_response(client, RPL_MOTD, "", NULL, newline);
            newline.clear();
        }
        rh.process_response(client, RPL_ENDOFMOTD, "");
    } else {
        LOG_w_CMD(strerror(errno));
        rh.process_response(client, ERR_NOMOTD);
    }
    inputFile.close();
}

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

std::string Motd::_get_current_time()
{
    std::time_t        now   = std::time(0);
    struct std::tm*    tmPtr = std::localtime(&now);
    std::ostringstream oss;
    oss << (tmPtr->tm_year + EPOCH_TIME_START) << "-" << std::setfill('0') << std::setw(2) << (tmPtr->tm_mon + 1) << "-"
        << std::setfill('0') << std::setw(2) << tmPtr->tm_mday;
    return oss.str();
}
