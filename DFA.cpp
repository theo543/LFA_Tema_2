#include "DFA.h"
#include <stdexcept>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

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

/// \param states Vector of states
/// \return true if they all have the same transitions
bool DFA::check_identical(std::vector<int> states) {
    if(states.empty()) return true;
    for(int sym = 0;sym<ALPHABET.len;sym++) {
        int val = transitions[states[0]][sym];
        if(std::any_of(states.begin(), states.end(), [this, val, sym](int s){
            return val == transitions[s][sym];
        })) return false;
    }
    return true;
}


/// \param part Partitions to compact ids of
/// \return Number of partitions found
int compact_ids(std::vector<int> &part) {
    std::unordered_map<int, int> to_compacted;
    int id = -1;
    for(int x : part) {
        if(to_compacted.find(x) == to_compacted.end()) {
            id++;
            to_compacted[x] = id;
        }
    }
    return id + 1;
}

DFA DFA::minimize() {
    std::vector<int> part(final_states.begin(), final_states.end());
    std::unordered_map<int, int> leaves_into; //TODO this should be a pair of where it leaves from and where it leaves into
    {
        int last_id = 1;
        bool changed = true;
        while (changed) {
            changed = false;
            for (int x = 0; x < final_states.size(); x++) {
                bool leaves = false;
                for (int sym = 0; sym < ALPHABET.len; sym++) {
                    if (part[transitions[x][sym]] != part[x]) {
                        leaves = true;
                        break;
                    }
                }
                if (leaves) {
                    if (leaves_into.find(part[x]) == leaves_into.end()) {
                        last_id++;
                        leaves_into[part[x]] = last_id;
                    }
                    part[x] = leaves_into[part[x]];
                    changed = true;
                }
            }
        }
    }
    int part_nr = compact_ids(part);
    std::vector<bool> added(part_nr, false);
    DFA result;
    result.resize(part_nr);

    throw "TODO";
}
