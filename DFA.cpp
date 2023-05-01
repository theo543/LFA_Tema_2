#include "DFA.h"
#include <stdexcept>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iostream>

constexpr int NONE = -1;
constexpr auto ALL_NONE = []() -> std::array<int, ALPHABET.len> {
    std::array<int, ALPHABET.len> val{};
    for(int x = 0;x<ALPHABET.len;x++)
        val[x] = NONE;
    return val;
}();

DFA::DFA() : FA(), transitions(1, ALL_NONE) {}

void DFA::addTransition(Transition t) {
    assertInBounds(t);
    if(transitions[t.from][t.sym_to_int()] != NONE) throw std::invalid_argument("Transition already exists");
    transitions[t.from][t.sym_to_int()] = t.to;
}

void DFA::overwriteTransition(Transition t) {
    assertInBounds(t);
    transitions[t.from][t.sym_to_int()] = t.to;
}

void DFA::removeTransition(Transition t) {
    assertInBounds(t);
    if(transitions[t.from][t.sym_to_int()] == NONE) throw std::invalid_argument("Transition not found");
    transitions[t.from][t.sym_to_int()] = NONE;
}

bool DFA::tryAccept(const std::string &s) {
    int current_state = start_state;
    for(char sym : s) {
        assertInBounds(sym);
        current_state = transitions[current_state][sym_to_int(sym)];
        if(current_state == NONE) return false;
    }
    return final_states[current_state];
}

void DFA::resize(int size) {
    if(size <= 0) throw std::invalid_argument("Need at least one state");
    if(size > transitions.size()) {
        int prev_size = transitions.size();
        final_states.resize(size);
        transitions.resize(size);
        for(int x = prev_size;x<transitions.size();x++) {
            final_states[x] = false; ///TODO needed?
            transitions[x] = ALL_NONE;
        }
    } else {
        final_states.resize(size);
        transitions.resize(size);
        for(auto &t : transitions) {
            for(int x = 0;x<ALPHABET.len;x++) {
                if(t[x] >= transitions.size()) t[x] = NONE;
            }
        }
    }
}

struct pair_hash {
    template<class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &p) const {
        return std::hash<T1>()(p.first) ^ std::hash<T2>()(p.second);
    }
};

int first_exit(int state, const std::vector<std::array<int, ALPHABET.len>> &transitions, const std::vector<int> &current_part) {
    for(int x = 0;x<ALPHABET.len;x++) {
        if(current_part[transitions[state][x]] != current_part[state]) return current_part[transitions[state][x]];
    }
    return current_part[state];
}

DFA DFA::minimize() {
    std::vector<int> current_part(transitions.size()), next_part(transitions.size());
    for(int x = 0;x<current_part.size();x++)
        current_part[x] = final_states[x] ? 1 : 0;
    while(true) {
        std::unordered_map<std::pair<int, int>, int, pair_hash> assignment;
        std::unordered_map<int, int> directions;
        int nextid = 0;
        for(int x = 0;x<current_part.size();x++) {
            int leaves_into = first_exit(x, transitions, current_part);
            std::pair<int, int> dir{current_part[x], leaves_into};
            if(assignment.contains(dir))
                next_part[x] = assignment[dir];
            else {
                directions[dir.first]++;
                assignment[dir] = nextid++;
                next_part[x] = assignment[dir];
            }
        }
        bool split = std::any_of(directions.begin(), directions.end(), [](const auto &p) { return p.second > 1; });
        std::swap(current_part, next_part);
        if(!split) break;
    }
    DFA result;
    result.resize(*std::max_element(current_part.begin(), current_part.end()) + 1);
    //TODO need to treeshake dead & unreachable states
    for(int x = 0;x<current_part.size();x++) {
        for(int sym = 0;sym<ALPHABET.len;sym++) {
            result.overwriteTransition({current_part[x], int_to_sym(sym), current_part[transitions[x][sym]]});
        }
        if(final_states[x]) result.setFinalState(current_part[x], true);
    }
    return result;
}

void DFA::print() {
    std::cout << "DFA:\n";
    for(int x = 0;x<transitions.size();x++) {
        std::cout << x << ": ";
        for(int sym = 0;sym<ALPHABET.len;sym++) {
            if(transitions[x][sym] != NONE) {
                std::cout << int_to_sym(sym) << "->" << transitions[x][sym] << " ";
            } else {
                std::cout << int_to_sym(sym) << "->" << "X ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "Final states: ";
    for(int x = 0;x<final_states.size();x++) {
        if(final_states[x]) std::cout << x << " ";
    }
    std::cout << "\n";
}
