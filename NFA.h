#ifndef LFA_TEMA_2_NFA_H
#define LFA_TEMA_2_NFA_H

#include "FA.h"
#include "DFA.h"
#include <unordered_map>
#include <unordered_set>

class NFA : public FA {
    std::vector<std::array<std::unordered_set<int>, ALPHABET.len>> transitions;
public:
    NFA();

    void resize(int size) override;
    bool tryAccept(const std::string &s) override;
    void addTransition(Transition t) override;
    void removeTransition(Transition t) override;
    DFA determinize();
    void serialize(std::ostream &out) const;
    void serialize(const std::string &filename) const;
    NFA static deserialize(std::istream &in);
    NFA static deserialize(const std::string &filename);
};

#endif //LFA_TEMA_2_NFA_H
