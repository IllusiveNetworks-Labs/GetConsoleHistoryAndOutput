#include "StringUtil.h"
#include <regex>

using namespace std;

string trimStr(const string& str) {    
    regex str_regex(R"(^(.*?)\s*$)");
    smatch str_match;
    if (regex_match(str, str_match, str_regex) && str_match.size() > 1) {
        return str_match[1];
    }
    return "";
}