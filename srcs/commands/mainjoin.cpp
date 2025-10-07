#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>
// #include <stddef.h>

#define NC   "\e[0m"
#define RED  "\e[31m"
#define BLUE "\e[34m"
#define CYAN "\e[36m"
bool is_matching_pattern(const std::string& pattern, const std::string& str)
{
    size_t p = 0, s = 0, star = std::string::npos, match = 0;

    while (s < str.size()) {
        // std::cout << " w-p:" << p << " s:" << s << " star:" << star << " match:" << match << std::endl;
        if (p < pattern.size() && (pattern[p] == '?' || pattern[p] == str[s])) {
            // match direct ou ?
            // if (pattern[p] == '?')
            //     match++;
            p++;
            s++;
        } else if (p < pattern.size() && pattern[p] == '*') {
            // on note la position du *
            star = p++;
            // if (p - 1 > 0 && pattern[p - 1] != '*')
            match = s;
        } else if (star != std::string::npos) {
            // pas de match direct : on revient au dernier char apres *
            p = star + 1;
            s = ++match;
        } else {
            // std::cout << "\t-p:" << p << " s:" << s << " star:" << star << " match:" << match << std::endl;
            return false;
        }
    }
    while (p < pattern.size() && pattern[p] == '*')
        p++;
    // if (p < pattern.size() && star != std::string::npos && match) {
    //     std::cout << "\t*p:" << p << " s:" << s << " star:" << star << " match:" << match << std::endl;
    //     return false;
    // }
    // std::cout << "\t+p:" << p << " s:" << s << " star:" << star << " match:" << match << std::endl;
    // return true;
    return p == pattern.size();
}

int main(int ac, char** av)
{
    (void)ac;
    (void)av;
    // std::string              params = "#chan1,#chan2,#chan3 key1,key2";
    // std::istringstream       iss(params);
    // std::vector<std::string> channels;
    // std::vector<std::string> keys;
    // std::vector<std::string> cLst;
    // std::string              channelsList;
    // std::string              keysList;
    // std::string              value;
    //
    // iss >> channelsList;
    // iss >> keysList;
    // if (iss >> std::ws && !iss.eof())
    //     std::cout << "to many params\n";
    // if (channelsList[channelsList.size()-1] == ',')
    //     std::cout << "empty last chan" << std::endl;
    // std::cout << "first token : " << channelsList << std::endl;
    // std::istringstream issChannels(channelsList);
    // while (std::getline(issChannels, value, ',')) {
    //     if (value[0] == '#' || value[0] == '&')
    //         channels.push_back(value);
    //     else
    //      std::cout << "Bad Channel Mask\n";
    // }
    // std::cout << "second token : " << keysList << std::endl;
    // std::istringstream issKeys(keysList);
    // while (std::getline(issKeys, value, ',')) {
    //     keys.push_back(value);
    // }
    //
    // std::vector<std::string>::iterator it = channels.begin();
    // size_t                             rank = 0;
    // for (; it < channels.end(); it++) {
    //     std::cout << "channels : " << *it << std::endl;
    //     if (rank < keys.size())
    //         cLst.push_back(*it + " "+ keys[rank]);
    //     else
    //         cLst.push_back(*it);
    //     rank++;
    // }
    // std::vector<std::string>::iterator ite = cLst.begin();
    // std::string chan;
    // std::string psswd;
    // while (ite != cLst.end())
    // {
    //     std::istringstream isschan(*ite);
    //     std::cout << *ite << std::endl;
    //     isschan >> chan;
    //     isschan >> psswd;
    //     if (psswd.empty())
    //         std::cout << "no key channel\n";
    //     chan.clear(); psswd.clear();
    //     ite++;
    // }
    // std::set<>
    //
    // std::for_each(v.begin(), v.end(), [](int &n) { n++; });
    // std::string pattern = "abc*edf**fh*cd";
    // std::string str = "abcedfuuuuuufhgcd";

    // std::string pattern(av[1]);
    // std::string str(av[2]);
    // std::cout << "pattern:\t" << pattern << std::endl;
    // std::cout << "str:\t" << str << std::endl;
    // std::string        line;
    // std::istringstream iss(pattern);
    // std::string        searchStr;
    // bool               check = true;
    // bool               wBegin = false;
    // bool               wEnd = true;
    // int                nbElem = 0;
    // while (getline(iss, line, '*')) {
    //     std::cout << nbElem << '=' << line << '=' << std::endl;
    //     if (line.empty() && !nbElem) {
    //         // std::cout << "wildcard at beginning" << std::endl;
    //         wBegin = true;
    //         nbElem++;
    //         continue;
    //     }
    //     if (iss.eof()) {
    //         std::cout << "last no wildcard" << std::endl;
    //         wEnd = false;
    //     }
    //     if (!nbElem && !wBegin) {
    //         searchStr = str.substr(0, line.size());
    //         if (searchStr != line) {
    //             std::cout << "--- false match line begin" << std::endl;
    //             check = false;
    //         }
    //         str = str.substr(line.size());
    //         // std::cout << "newstr: " << str << std::endl;
    //     } else if (!wEnd) {
    //         searchStr = str.substr(str.size() - line.size(), line.size());
    //         if (searchStr != line) {
    //             std::cout << "---false match line end" << std::endl;
    //             check = false;
    //         }
    //     } else if (nbElem && str.find(line) != std::string::npos && str.find(line)) {
    //         std::cout << "---match middle end pos:" << str.find(line) << std::endl;
    //         str = str.substr(str.find(line) + line.size());
    //         // std::cout << "newstr: " << str << std::endl;
    //     } else if (!line.empty()) {
    //         check = false;
    //         std::cout << "---false divers" << std::endl;
    //     }
    //     std::cout << "newstr: [" << str << "]" << std::endl;
    //     std::cout << "*******" << std::endl;
    //
    //     nbElem++;
    // }
    // if (check && !(wEnd && str.empty()))
    //     std::cout << "!!!!! CHECK OK !!!!!\n";
    // else
    //     std::cout << " CHECK KO \n";
    // std::cout << "* begin:" << wBegin << std::endl;
    // std::cout << "* end:" << wEnd << std::endl;
    // std::cout << "str empty:" << str.empty() << std::endl;
    // std::cout << "end * check:" << (str.empty() && wEnd) << std::endl;
    std::string pattern = "*abc*";
    std::string str     = "abc";
    bool        check   = is_matching_pattern(pattern, str);
    std::cout << "Is valid pattern [" << pattern << "] for:[" << str << "]->" << (check ? "ok" : "false") << std::endl;
    pattern = "*abc";
    str     = "abc";
    check   = is_matching_pattern(pattern, str);
    std::cout << "Is valid pattern [" << pattern << "] for:[" << str << "]->" << (check ? "ok" : "false") << std::endl;
    pattern = "abc*";
    str     = "abc";
    check   = is_matching_pattern(pattern, str);
    std::cout << "Is valid pattern [" << pattern << "] for:[" << str << "]->" << (check ? "ok" : "false") << std::endl;
    pattern = "*abc*";
    str     = "abc";
    check   = is_matching_pattern(pattern, str);
    std::cout << "Is valid pattern [" << pattern << "] for:[" << str << "]->" << (check ? "ok" : "false") << std::endl;
    pattern = "jto?2";
    str     = "jto12";
    check   = is_matching_pattern(pattern, str);
    std::cout << "Is valid pattern [" << pattern << "] for:[" << str << "]->" << (check ? "ok" : "false") << std::endl;
    pattern = "jto?";
    str     = "jto12";
    check   = is_matching_pattern(pattern, str);
    std::cout << "Is valid pattern [" << pattern << "] for:[" << str << "]->" << (check ? "ok" : "false") << std::endl;
    pattern = "jto?";
    str     = "jto1";
    check   = is_matching_pattern(pattern, str);
    std::cout << "Is valid pattern [" << pattern << "] for:[" << str << "]->" << (check ? "ok" : "false") << std::endl;
    pattern = "mar*";
    str     = "john";
    check   = is_matching_pattern(pattern, str);
    std::cout << "Is valid pattern [" << pattern << "] for:[" << str << "]->" << (check ? "ok" : "false") << std::endl;
    pattern = "abc*u";
    str     = "abcdeu";
    check   = is_matching_pattern(pattern, str);
    std::cout << "Is valid pattern [" << pattern << "] for:[" << str << "]->" << (check ? "ok" : "false") << std::endl;
    pattern = "*abc*u";
    str     = "sabcdeu";
    check   = is_matching_pattern(pattern, str);
    std::cout << "Is valid pattern [" << pattern << "] for:[" << str << "]->" << (check ? "ok" : "false") << std::endl;
    pattern = "**abc*u";
    str     = "sabcdeu";
    check   = is_matching_pattern(pattern, str);
    std::cout << "Is valid pattern [" << pattern << "] for:[" << str << "]->" << (check ? "ok" : "false") << std::endl;
    pattern = "**abc***u*t";
    str     = "sabcdeu-t";
    check   = is_matching_pattern(pattern, str);
    std::cout << "Is valid pattern [" << pattern << "] for:[" << str << "]->" << (check ? "ok" : "false") << std::endl;
    pattern = "**abc***u*t?";
    str     = "sabcdeu-t1";
    check   = is_matching_pattern(pattern, str);
    std::cout << "Is valid pattern [" << pattern << "] for:[" << str << "]->" << (check ? "ok" : "false") << std::endl;
    pattern = "**abc***u*t?p";
    str     = "sabcdeu-t1p";
    check   = is_matching_pattern(pattern, str);
    std::cout << "Is valid pattern [" << pattern << "] for:[" << str << "]->" << (check ? "ok" : "false") << std::endl;
    pattern = "**abc***u*t?p*";
    str     = "sabcdeu-t1pg";
    check   = is_matching_pattern(pattern, str);
    std::cout << "Is valid pattern [" << pattern << "] for:[" << str << "]->" << (check ? "ok" : "false") << std::endl;
    pattern = "**abc***u*t?p?*";
    str     = "sabcdeu-t1pg";
    check   = is_matching_pattern(pattern, str);
    std::cout << RED "Is valid pattern [" << pattern << "] for:[" << str << "]->" << (check ? "ok" : "false") << std::endl;
    pattern = "*hello";
    str     = "sabcdeu";
    check   = is_matching_pattern(pattern, str);
    std::cout << NC "Is valid pattern [" << pattern << "] for:[" << str << "]->" << (check ? "ok" : "false") << std::endl;
    pattern = "*";
    str     = "sabcdeu-t1pghtototutu";
    check   = is_matching_pattern(pattern, str);
    std::cout << "Is valid pattern [" << pattern << "] for:[" << str << "]->" << (check ? "ok" : "false") << std::endl;
}
