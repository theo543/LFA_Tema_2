#ifndef LFA_TEMA_2_NFA_H
#define LFA_TEMA_2_NFA_H

#include "FA.h"
#include "DFA.h"

class NFA : public FA {
protected:
public:
    NFA();
    bool tryAccept(const std::string &s) override;
    void addTransition(Transition t) override;
    DFA determinize();
};

#endif //LFA_TEMA_2_NFA_H
