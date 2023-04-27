#ifndef LFA_TEMA_2_DFA_H
#define LFA_TEMA_2_DFA_H

#include <unordered_set>
#include "FA.h"

class DFA : public FA {
protected:
    std::vector<std::unordered_set<char>> symbols;
public:
    DFA();
    bool tryAccept(const std::string &s) override;
    void addTransition(Transition t) override;
    DFA minimize();
};


#endif //LFA_TEMA_2_DFA_H
