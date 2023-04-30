#include "NFA.h"
#include <stdexcept>
#include <algorithm>
#include <map>
#include <set>
#include <queue>

NFA::NFA() : FA() {}

void NFA::resize(int size) {
    if(size <= 0) throw std::invalid_argument("Need at least one state");
    if(size > transitions.size()) {
        int old_size = transitions.size();
        final_states.resize(size);
        transitions.resize(size);
        for(int x = old_size;x<transitions.size();x++) {
            final_states[x] = false; ///TODO: needed?
        }
    }
}

bool NFA::tryAccept(const std::string &s) {
    std::unordered_set<int> s1 = {start_state}, s2 = {};
    std::unordered_set<int> *current  = &s1, *after = &s2;
    for(auto iter = s.begin();iter != s.end() && !current->empty();iter++) {
        char c = *iter;
        assertInBounds(c);
        for(int state : *current) {
            auto &ins = transitions[state][sym_to_int(c)];
            after->insert(ins.begin(), ins.end());
        }
        swap(current, after);
        after->clear();
    }
    return std::any_of(current->begin(), current->end(), [this](int state) {return final_states[state];});
}

void NFA::addTransition(Transition t) {
    assertInBounds(t);
    transitions[t.from][t.sym_to_int()].insert(t.to);
}

void NFA::removeTransition(Transition t) {
    assertInBounds(t);
    auto &set_ = transitions[t.from][t.sym_to_int()];
    auto iter = set_.find(t.to);
    if(iter == set_.end()) throw std::invalid_argument("Transition not found");
    set_.erase(iter);
}

DFA NFA::determinize() {
    typedef std::vector<int> superstate;
    std::map<superstate, int> state_map;
    std::queue<std::map<superstate, int>::iterator> bfs;
    bfs.push(state_map.emplace(superstate{start_state}, 0).first);
    int nextid = 1;
    DFA dfa;
    while(!bfs.empty()) {
        for(int sym = 0;sym<ALPHABET.len;sym++) {
            std::set<int> to_;
            for(int state : bfs.front()->first) {
                auto ins = transitions[state][sym];
                to_.insert(ins.begin(), ins.end());
            }
            superstate to;
            to.insert(to.end(), to_.begin(), to_.end());
            std::sort(to.begin(), to.end());
            auto res = state_map.emplace(to, nextid);
            if(res.second) {
                nextid++;
                dfa.resize(nextid);
                bfs.push(res.first);
            }
            dfa.addTransition(Transition{bfs.front()->second, int_to_sym(sym), res.first->second});
        }
        bfs.pop();
    }
    for(auto &state : state_map) {
        bool fin = std::any_of(state.first.begin(), state.first.end(), [this](int state) {return final_states[state];});
        dfa.setFinalState(state.second, fin);
    }
    return dfa;
}
