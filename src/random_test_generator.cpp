#include "NFA.h"
#include <iostream>
#include <random>
std::mt19937 rng(std::random_device{}());
int main() {
    std::cout << "Generating random NFA\n";
    int total_states, total_transitions, sink_states;
    float sink_chance;
    std::cout << "Enter nr. of non-sink states: ";
    std::cin >> total_states;
    if(total_states <= 0) {
        total_states = 1;
        std::cout << "Using minimum value of 1\n";
    }
    std::cout << "Enter nr. of sink states: ";
    std::cin >> sink_states;
    std::cout << "Enter nr. of random transitions: ";
    std::cin >> total_transitions;
    std::cout << "Enter percentage of transitions redirected to sink state: ";
    std::cin >> sink_chance;
    NFA nfa;
    nfa.resize(total_states + sink_states);
    std::uniform_int_distribution<int> states(0, total_states - 1);
    std::uniform_int_distribution<int> sinks(total_states, total_states + sink_states - 1);
    std::uniform_int_distribution<int> symbols(0, ALPHABET.len - 1);
    std::bernoulli_distribution sink(sink_chance);
    for(int i = 0; i < total_transitions; i++) {
        int from = states(rng);
        int to = states(rng);
        if(sink(rng)) to = sinks(rng);
        char sym = int_to_sym(symbols(rng));
        nfa.addTransition({from, sym, to});
    }
    float final_chance;
    std::cout << "Enter final state chance: ";
    std::cin >> final_chance;
    std::bernoulli_distribution finals(final_chance);
    for(int i = 0; i < total_states; i++) {
        nfa.setFinalState(i, finals(rng));
    }
    nfa.setStartState(states(rng));
    std::cout << "Path: ";
    std::string path;
    std::cin >> path;
    nfa.serialize(path);
    return 0;
}
