#include "DFA.h"
#include <algorithm>
#include <set>
#include <stdexcept>

void DFA::addTransition(Transition t) {
    if(t.from < 0 || t.from >= transitions.size()) {
        throw std::runtime_error("Invalid DFA - transition from out of bounds state");
    }
    if(t.to < 0 || t.to >= transitions.size()) {
        throw std::runtime_error("Invalid DFA - transition to out of bounds state");
    }
    if(symbols[t.from].find(t.symbol) != symbols[t.from].end()) {
        throw std::runtime_error("Invalid DFA - duplicate transition symbol");
    }
    symbols[t.from].insert(t.symbol);
    transitions[t.from].push_back({t.to, t.symbol});
}

bool DFA::tryAccept(const std::string &s) {
    int current_state = start_state;
    auto iter = s.begin();
    while(iter != s.end()) {
        char sym = *iter;
        auto nxt = std::find_if(transitions[current_state].begin(), transitions[current_state].end(), [sym](Edge e) {
            return e.symbol == sym;
        });
        if(nxt == transitions[current_state].end())
            return false;
        current_state = nxt->to;
    }
    return final_states[current_state];
}
