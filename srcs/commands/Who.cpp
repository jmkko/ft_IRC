#include "Who.hpp"

#include <string>

Who::Who() {}
Who::Who(const std::string& clientsLst) : _clientsLst(clientsLst) {}
Who::~Who() {}

Who::Who(const Who& mother) : ICommand() { (void)mother; }

Who& Who::operator=(const Who& other)
{
    (void)other;
    return *this;
}

void Who::execute(Server& s, Client& c)
{
    (void)s;
    (void)c;
}

ReplyCode Who::check_args(Server& server, Client& client, std::string& params)
{
    std::string        mask;
    std::string        op;
    std::istringstream iss(params);
    (void)server;
    (void)client;

    iss >> mask;
    iss >> op;

    return (RPL_SUCCESS);
}

bool Who::_is_valid_pattern(const std::string& pattern, const std::string& str) const
{
    std::string        line;
    std::istringstream iss(pattern);
    std::string        searchStr;
    std::string        workingStr = str; // Create a local copy to modify
    bool               check = true;
    bool               wBegin = false;
    bool               wEnd = true;
    int                nbElem = 0;
    while (getline(iss, line, '*')) {
        if (line.empty() && !nbElem) {
            wBegin = true;
            nbElem++;
            continue;
        }
        if (iss.eof()) {
            wEnd = false;
        }
        if (!nbElem && !wBegin) {
            searchStr = workingStr.substr(0, line.size());
            if (searchStr != line) {
                check = false;
            }
            workingStr = workingStr.substr(line.size());
        } else if (!wEnd) {
            searchStr = workingStr.substr(workingStr.size() - line.size(), line.size());
            if (searchStr != line) {
                check = false;
            }
        } else if (nbElem && workingStr.find(line) != std::string::npos && workingStr.find(line)) {
            workingStr = workingStr.substr(workingStr.find(line) + line.size());
        } else if (!line.empty()) {
            check = false;
        }
        nbElem++;
    }
    return (check && !(wEnd && workingStr.empty()));
}
