#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include "DFA.h"
#include "NFA.h"
int main() {
    std::cout << "Enter path to NFA file: ";
    std::string path;
    std::cin >> path;
    NFA nfa = NFA::deserialize(path);
    std::cout << "NFA created with " << nfa.getSize() << " states" << std::endl;
    DFA unminimized = nfa.determinize();
    std::cout << "DFA created with " << unminimized.getSize() << " states" << std::endl;
    if(getDebugOutputEnabled()) unminimized.print();
    DFA shaken = unminimized.treeshake(); // just to make sure this works even on its own
    DFA dfa = unminimized.minimize();
    std::cout << "Minimized DFA created with " << dfa.getSize() << " states" << std::endl;
    if(getDebugOutputEnabled()) dfa.print();
    std::cout<< "Type \"exit\" to exit\n";
    std::string input;
    FA *checks[] = {&nfa, &unminimized, &shaken, &dfa};
    while(true) {
        std::cout << "Check str: ";
        std::cin >> input;
        if (input == "exit") break;
        bool result = checks[0]->tryAccept(input);
        for (FA *check: checks) {
            if (check->tryAccept(input) != result)
                std::cerr << "Bug found!\n";
        }
        std::cout << (result ? "Yes" : "No") << std::endl;
    }
    return 0;
}
