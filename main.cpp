#include <iostream>
#include <string>
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
    std::cout << "NFA created with " << nfa.getSize() << " states" << std::endl;
    DFA unminimized = nfa.determinize();
    std::cout << "DFA created with " << unminimized.getSize() << " states" << std::endl;
    DFA dfa = unminimized.minimize();
    std::cout << "Minimized DFA created with " << dfa.getSize() << " states" << std::endl;
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
