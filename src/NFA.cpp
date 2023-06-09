#include "NFA.h"
#include "utils.h"
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

void NFA::resize(std::size_t size) {
    size = (size == 0) ? 1 : size;
    if(size > transitions.size()) {
        std::size_t old_size = transitions.size();
        final_states.resize(size);
        transitions.resize(size);
        for(std::size_t x = old_size;x<transitions.size();x++) {
            final_states[x] = false;
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

void NFA::foreachTransition(const std::function<void(Transition)>& f) const {
    for(int from = 0;from<transitions.size();from++) {
        for(int sym = 0;sym<ALPHABET.len;sym++) {
            for(int to : transitions[from][sym]) {
                f(Transition{from, int_to_sym(sym), to});
            }
        }
    }
}

void NFA::foreachState(const std::function<void(int, bool)>& f) const {
    for(int state = 0;state<transitions.size();state++) {
        f(state, final_states[state]);
    }
}

DFA NFA::determinize() {
    typedef std::vector<int> superstate;
    std::map<superstate, int> state_map;
    std::queue<std::map<superstate, int>::iterator> bfs;
    bfs.push(state_map.emplace(superstate{start_state}, 0).first);
    verbose() << "Assigned 0 to { " << start_state << " }\n";
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
                std::string output = "Assigned " + std::to_string(nextid) + " to { ";
                for (int x: to) output += std::to_string(x) + ' ';
                output += "}\n";
                verbose() << output;
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
        if(!(ALPHABET.start == start && ALPHABET.end == end && ALPHABET.len == len)) {
            throw std::invalid_argument("Alphabet mismatch");
        }
    }
    std::size_t size;
    in >> size;
    nfa.resize(size);
    for(int i = 0; i < size; i++) {
        std::string chars;
        in >> chars;
        if(chars == EMPTY_STATE_MARKER) {
            verbose() << "State " + std::to_string(i) + " is empty\n";
            continue;
        }
        verbose() << "Adding state " + std::to_string(i) + " with chars " + chars + "\n";
        for(char symchar : chars) {
            nfa.assertInBounds(symchar);
            int sym = sym_to_int(symchar);
            char c;
            in >> c;
            if(c != symchar) {
                throw std::invalid_argument("Incorrect or out-of-order symbol");
            }
            int count;
            in >> count;
            for(int j = 0; in && j < count; j++) {
                int to;
                in >> to;
                nfa.addTransition(Transition{i, int_to_sym(sym), to});
            }
            if(count != nfa.transitions[i][sym].size()) {
                throw std::invalid_argument("Count is incorrect");
            }
        }
    }
    for(int i = 0; in && i < size; i++) {
        bool final;
        in >> final;
        nfa.setFinalState(i, final);
        verbose() << "State " + std::to_string(i) + " is " + (final ? "final" : "not final") + "\n";
    }
    int start;
    in >> start;
    logger()<<"Start state is "<<start<<"\n";
    nfa.setStartState(start);
    return nfa;
}

NFA NFA::deserialize(const std::string &filename) {
    std::ifstream in(filename);
    return deserialize(in);
}
