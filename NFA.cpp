#include "NFA.h"
#include <stdexcept>
#include <algorithm>
#include <map>
#include <set>
#include <queue>
#include <iostream>
#include <fstream>
#include <cassert>

constexpr auto EMPTY_STATE_MARKER = "_";

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
            assert(std::is_sorted(to.begin(), to.end()));
            auto res = state_map.emplace(to, nextid);
            if(res.second) {
                if(getDebugOutputEnabled()) {
                    std::cout << "Assigned " << nextid << " to ";
                    for (int x: to) std::cout << x << ' ';
                    std::cout << '\n';
                }
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


// FORMAT:
// alphabet \n
// size \n
// transitions:
//    for each state:
//        chars with transitions \n
//        for each char with transitions:
//            char number of transitions \n
//            transitions \n
// final states \n
// start state

void NFA::serialize(std::ostream &out) const {
    out << ALPHABET.start << ' ' << ALPHABET.end << ' ' << ALPHABET.len << '\n';
    out << transitions.size() << '\n';
    for(const auto & state : transitions) {
        std::string chars;
        for (int sym = 0; sym < ALPHABET.len; sym++) {
            if (!state[sym].empty()) {
                chars += int_to_sym(sym);
            }
        }
        if(chars.empty()) {
            out << EMPTY_STATE_MARKER << '\n';
            continue;
        }
        out << chars << '\n';
        for(int sym = 0;sym<ALPHABET.len;sym++) {
            if(state[sym].empty()) continue;
            out << int_to_sym(sym) << ' ' << state[sym].size() << ' ';
            for(int to : state[sym]) {
                out << to << ' ';
            }
            out << '\n';
        }
    }
    for(bool final : final_states) {
        out << final << ' ';
    }
    out << '\n';
    out << start_state << '\n';
}

void NFA::serialize(const std::string &filename) const {
    std::ofstream out(filename);
    serialize(out);
}

NFA NFA::deserialize(std::istream &in) {
    NFA nfa;
    {
        char start, end;
        int len;
        in >> start >> end >> len;
        assert(ALPHABET.start == start && ALPHABET.end == end && ALPHABET.len == len);
    }
    int size;
    in >> size;
    nfa.resize(size);
    for(int i = 0; i < size; i++) {
        std::string chars;
        in >> chars;
        if(chars == EMPTY_STATE_MARKER) {
            if(getDebugOutputEnabled()) std::cout<<"State "<<i<<" is empty\n";
            continue;
        }
        if(getDebugOutputEnabled()) std::cout<<"Adding state "<<i<<" with chars "<<chars<<"\n";
        for(char symchar : chars) {
            int sym = sym_to_int(symchar);
            char c;
            in >> c;
            assert(c == symchar);
            int count;
            in >> count;
            for(int j = 0; j < count; j++) {
                int to;
                in >> to;
                nfa.addTransition(Transition{i, int_to_sym(sym), to});
            }
            assert(count == nfa.transitions[i][sym].size());
        }
    }
    for(int i = 0; i < size; i++) {
        bool final;
        in >> final;
        nfa.setFinalState(i, final);
        if(getDebugOutputEnabled()) std::cout<<"State "<<i<<" is "<<(final ? "final" : "not final")<<"\n";
    }
    int start;
    in >> start;
    if(getDebugOutputEnabled()) std::cout<<"Start state is "<<start<<"\n";
    nfa.setStartState(start);
    return nfa;
}

NFA NFA::deserialize(const std::string &filename) {
    std::ifstream in(filename);
    return deserialize(in);
}
