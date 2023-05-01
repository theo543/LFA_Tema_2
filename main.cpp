#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <cassert>
#include "DFA.h"
#include "NFA.h"
int main() {
    //TODO: more complex tests
    NFA nfa;
    std::string match;
    std::cout << "Match str containing: ";
    std::cin>>match;
    int msize = static_cast<int>(match.size());
    nfa.resize(msize + 1);
    for(char c = ALPHABET.start; c <= ALPHABET.end; c++) {
        nfa.addTransition({0, c, 0});
        nfa.addTransition({msize, c, msize});
    }
    for(int x = 0;x<msize;x++)
        nfa.addTransition({x, match[x], x + 1});
    nfa.setFinalState(msize, true);
    int unreachable_cycles;
    std::cout << "Unreachable cycles: ";
    std::cin >> unreachable_cycles;
    for(int i = 0;i<unreachable_cycles;i++) {
        int prev = nfa.getSize() - 1;
        nfa.resize(nfa.getSize() + 100);
        for(int x = prev;x<nfa.getSize();x++) {
            for(char c = ALPHABET.start; c <= ALPHABET.end; c++) {
                nfa.addTransition({x, c, x});
            }
        }
    }
    int dead_states;
    std::cout << "Dead states: ";
    std::cin >> dead_states;
    nfa.resize(nfa.getSize() + dead_states);
    std::mt19937 rng(std::random_device{}());
    for(int x = 0;x<nfa.getSize();x++) {
        for(int y = 0;y<dead_states;y++) {
            for(char c = ALPHABET.start; c <= ALPHABET.end; c++) {
                if(rng() % 10) nfa.addTransition({x, c, nfa.getSize() - 1 - y});
            }
        }
    }
    std::cout << "NFA created with " << nfa.getSize() << " states" << std::endl;
    DFA unminimized = nfa.determinize();
    std::cout << "DFA created with " << unminimized.getSize() << " states" << std::endl;
    unminimized.print();
    DFA dfa = unminimized.minimize();
    std::cout << "Minimized DFA created with " << dfa.getSize() << " states" << std::endl;
    dfa.print();
    assert(dfa.getSize() == match.size() + 1);
    std::cout<< "Type \"exit\" to exit\n";
    std::string input;
    while(true) {
        std::cout<<"Check str: ";
        std::cin >> input;
        if(input == "exit") break;
        bool d = dfa.tryAccept(input), n = dfa.tryAccept(input);
        if(d == n) std::cout << (n ? "Yes" : "No") << std::endl;
        else std::cerr << "Bug found!" << std::endl;
    }
    return 0;
}
