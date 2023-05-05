#include "DFA.h"
#include <stdexcept>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <queue>
#include <random>

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

struct tuple_hash {
    template<class T1, class T2, class T3>
    std::size_t operator()(const std::tuple<T1, T2, T3> &p) const {
        return std::hash<T1>()(get<0>(p)) ^ std::hash<T2>()(get<1>(p)) ^ std::hash<T3>()(get<2>(p));
    }
};

static std::pair<int, int> first_exit(int state, const std::vector<std::array<int, ALPHABET.len>> &transitions, const std::vector<int> &current_part) {
    for(int x = 0;x<ALPHABET.len;x++) {
        int to = transitions[state][x];
        if(to != NONE && current_part[to] != current_part[state]) return {current_part[to], x};
    }
    return {current_part[state], -1};
}

DFA DFA::treeshake() {
    std::vector<bool> reachable(final_states.size(), false);
    std::vector<std::vector<int>> edges_to(final_states.size());
    std::queue<int> bfs;
    bfs.push(start_state);
    while (!bfs.empty()) {
        int state = bfs.front();
        bfs.pop();
        if (reachable[state]) {
            continue;
        }
        reachable[state] = true;
        for (int x = 0; x < ALPHABET.len; x++) {
            if (transitions[state][x] != NONE) {
                bfs.push(transitions[state][x]);
                edges_to[transitions[state][x]].push_back(state);
            }
        }
    }
    std::vector<bool> live(final_states.size(), false);
    bfs = {};
    for (int x = 0; x < final_states.size(); x++) {
        if (final_states[x])
            bfs.push(x);
    }
    while (!bfs.empty()) {
        int state = bfs.front();
        bfs.pop();
        if (live[state])
            continue;
        live[state] = true;
        for (int x : edges_to[state]) {
            bfs.push(x);
        }
    }
    std::vector<bool> kept(final_states.size());
    int newsize = 0;
    for (int x = 0; x < final_states.size(); x++) {
        kept[x] = reachable[x] && live[x];
        newsize += kept[x];
    }
    DFA dfa;
    if(newsize != 0) {
        dfa.resize(newsize);
        std::vector<int> newids(final_states.size(), NONE);
        for (int x = 0, skipped = 0; x < final_states.size(); x++) {
            if (kept[x]) {
                newids[x] = x - skipped;
            } else skipped++;
        }
        for (int x = 0; x < final_states.size(); x++) {
            if (!kept[x]) continue;
            dfa.setFinalState(newids[x], final_states[x]);
            for (int y = 0; y < ALPHABET.len; y++) {
                if (transitions[x][y] != NONE && kept[transitions[x][y]]) {
                    dfa.addTransition({newids[x], int_to_sym(y), newids[transitions[x][y]]});
                }
            }
        }
    }
    return dfa;
}

static void print_partition(const std::vector<int> &partition, int nr) {
    logger()<<nr<<"th partition: ";
    for(int x : partition) {
        logger() << x << " ";
    }
    logger() << std::endl;
}

DFA DFA::minimize() {
    DFA dfa = treeshake();
    logger() << "Temporary treeshaken DFA:" << std::endl;
    dfa.print(logger());
    logger() << "Minimizing..." << std::endl;
    std::vector<int> current_part(dfa.transitions.size()), next_part(dfa.transitions.size());
    for(int x = 0;x<current_part.size();x++)
        current_part[x] = dfa.final_states[x] ? 1 : 0;
    print_partition(current_part, 0);
    int nr = 0;
    while(true) {
        std::unordered_map<std::tuple<int, int, int>, int, tuple_hash> assignment;
        std::unordered_map<int, int> directions;
        int nextid = 0;
        for(int x = 0;x<current_part.size();x++) {
            std::pair<int, int> leaves_into = first_exit(x, dfa.transitions, current_part);
            std::tuple<int, int, int> dir{current_part[x], leaves_into.first, leaves_into.second};
            if(assignment.contains(dir))
                next_part[x] = assignment[dir];
            else {
                directions[get<0>(dir)]++;
                assignment[dir] = nextid++;
                next_part[x] = assignment[dir];
            }
        }
        bool split = std::any_of(directions.begin(), directions.end(), [](const auto &p) { return p.second > 1; });
        std::swap(current_part, next_part);
        print_partition(current_part, ++nr);
        if (!split) break;
    }
    DFA result;
    logger() << "Writing partitions to DFA..." << std::endl;
    result.resize(*std::max_element(current_part.begin(), current_part.end()) + 1);
    result.setStartState(current_part[dfa.start_state]);
//    logger() << "Start state: " << current_part[dfa.start_state] << " from state " << dfa.start_state << std::endl;
    for(int x = 0;x<current_part.size();x++) {
        for(int sym = 0;sym<ALPHABET.len;sym++) {
            if(dfa.transitions[x][sym] != NONE) {
                result.overwriteTransition({current_part[x], int_to_sym(sym), current_part[dfa.transitions[x][sym]]});
//                logger() << "Transition " << current_part[x] << " " << int_to_sym(sym) << " " << current_part[dfa.transitions[x][sym]] << std::endl;
//                logger() << "From state " << x << " to state " << dfa.transitions[x][sym] << std::endl;
            }
        }
        if(dfa.final_states[x]) result.setFinalState(current_part[x], true);
    }
    return result;
}

void DFA::print(std::ostream &out) {
    out << "DFA:\n";
    for(int x = 0;x<transitions.size();x++) {
        out << x << ": ";
        for(int sym = 0;sym<ALPHABET.len;sym++) {
            if(transitions[x][sym] != NONE) {
                out << int_to_sym(sym) << "->" << transitions[x][sym] << " ";
            } else {
                out << int_to_sym(sym) << "->" << "X ";
            }
        }
        out << "\n";
    }
    out << "Start state: " << start_state << "\n";
    out << "Final states: ";
    for(int x = 0;x<final_states.size();x++) {
        if(final_states[x]) out << x << " ";
    }
    out << "\n";
}

std::string DFA::get_valid_string() {
    struct node {
        char sym = 0;
        int state = 0;
        int prev = 0;
    };
    std::vector<node> bfs;
    bfs.emplace_back(0, start_state);
    std::string ret;
    std::vector<bool> visited(final_states.size(), false);
    for(int x = 0;x < bfs.size();x++) {
        auto elem = bfs[x];
        if(final_states[elem.state] && x != 0) {
            ret = "";
            while(elem.sym != 0) {
                ret += elem.sym;
                elem = bfs[elem.prev];
            }
            std::reverse(ret.begin(), ret.end());
            break;
        }
        for(int sym = 0;sym<ALPHABET.len;sym++) {
            if(transitions[elem.state][sym] != NONE && !visited[transitions[elem.state][sym]]) {
                bfs.emplace_back(int_to_sym(sym), transitions[elem.state][sym], x);
                visited[transitions[elem.state][sym]] = true;
            }
        }
    }
    if(ret.empty()) {
        if(final_states[start_state]) ret = " Lambda.";
        else ret = " Language is empty.";
    }
    return ret;
}
