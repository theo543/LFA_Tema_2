#ifndef LFA_TEMA_2_UTILS_H
#define LFA_TEMA_2_UTILS_H

#include <iosfwd>
#include <string>

struct Config {
    std::string path;
    bool logger = false;
    bool verbose = false;
    bool BF = false;
    long long BF_len = 0;
    long long BF_count = 1;
    void applyOutputConf() const;
    /// From stdin
    explicit Config(std::istream &in);
    /// From args
    Config(int argc, char**argv);
};

void setVerboseOutputEnabled(bool);
void setDebugOutputEnabled(bool);
std::ostream &logger();
std::ostream &verbose();
#endif //LFA_TEMA_2_UTILS_H
