#include "NFA.h"
#include <algorithm>
#include <numeric>
#include <limits>
#include <set>
#include <map>
#include <unordered_set>
#include <queue>
#include <stdexcept>
#include <cassert>

NFA::NFA() : FA() {}

void NFA::addTransition(Transition t) {
    if(t.from < 0 || t.from >= transitions.size()) {
        throw std::runtime_error("Invalid NFA - transition from out of bounds state");
    }
    if(t.to < 0 || t.to >= transitions.size()) {
        throw std::runtime_error("Invalid NFA - transition to out of bounds state");
    }
    transitions[t.from][t.symbol].push_back(t.to);
}

bool NFA::tryAccept(const std::string &s) {
    std::unordered_set<int> s1 = {start_state}, s2 = {};
    std::unordered_set<int> *current  = &s1, *after = &s2;
    auto iter = s.begin();
    while(iter != s.end() && !current->empty()) {
        char c = *iter;
        for(int state : *current) {
            for(auto &e : transitions[state][c]) {
                after->insert(e);
            }
        }
        swap(current, after);
        after->clear();
    }
    return std::any_of(current->begin(), current->end(), [this](int state) {return final_states[state];});
}

DFA NFA::determinize() {
    typedef std::set<int> superstate;
    std::map<superstate, int> state_map;
    std::queue<superstate> bfs;
    bfs.push({start_state});
    state_map[{start_state}] = 0;
    DFA dfa;
    while(!bfs.empty()) {
        std::unordered_map<char, superstate> results;
        for(auto state : bfs.front()) {
            for(auto &e : transitions[state]) {
                results[e.first].insert(e.second.begin(), e.second.end());
            }
        }
        for(auto &result : results) {
            if(state_map.find(result.second) == state_map.end()) {
                bfs.push(result.second);
                assert(state_map.size() < std::numeric_limits<int>::max());
                state_map[result.second] = state_map.size();
                dfa.resize(state_map.size());
            }
            dfa.addTransition({state_map[bfs.front()], state_map[result.second], result.first});
            bfs.pop();
        }
    }
    for(auto &state : state_map) {
        if(std::any_of(state.first.begin(), state.first.end(), [this](int state) {return final_states[state];})) {
            dfa.setFinalState(state.second, true);
        } else {
            dfa.setFinalState(state.second, false);
        }
    }
    return dfa;
}
