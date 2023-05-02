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
    unminimized.print();
    DFA dfa = unminimized.minimize();
    std::cout << "Minimized DFA created with " << dfa.getSize() << " states" << std::endl;
    dfa.print();
    std::cout<< "Type \"exit\" to exit\n";
    std::string input;
    while(true) {
        std::cout<<"Check str: ";
        std::cin >> input;
        if(input == "exit") break;
        bool d = dfa.tryAccept(input), n = unminimized.tryAccept(input);
        if(d == n) std::cout << (n ? "Yes" : "No") << std::endl;
        else std::cerr << "Bug found!" << std::endl;
    }
    return 0;
}
