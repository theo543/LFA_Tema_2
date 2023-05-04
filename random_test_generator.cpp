#include "NFA.h"
#include <iostream>
#include <random>
std::mt19937 rng(std::random_device{}());
int main() {
    std::cout << "Generating random NFA\n";
    int total_states, total_transitions;
    std::cout << "Enter total states: ";
    std::cin >> total_states;
    std::cout << "Enter total transitions: ";
    std::cin >> total_transitions;
    NFA nfa;
    nfa.resize(total_states);
    std::uniform_int_distribution<int> states(0, 99);
    std::uniform_int_distribution<int> symbols(0, ALPHABET.len - 1);
    for(int i = 0; i < total_transitions; i++) {
        int from = states(rng);
        int to = states(rng);
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
