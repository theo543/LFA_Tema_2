#ifndef LFA_TEMA_2_DFA_H
#define LFA_TEMA_2_DFA_H

#include <array>
#include "FA.h"

class DFA : public FA {
    std::vector<std::array<int, ALPHABET.len>> transitions;
public:
    DFA();
    bool tryAccept(const std::string &s) override;

    void resize(int size) override;
    void addTransition(Transition t) override;
    void overwriteTransition(Transition t);
    void removeTransition(Transition t) override;
    DFA minimize();
};


#endif //LFA_TEMA_2_DFA_H
