#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include "DFA.h"
#include "NFA.h"
int main() {
    std::cout<<"Enable debug output? (1/0): ";
    {
        int debug;
        std::cin >> debug;
        setDebugOutputEnabled(debug);
    }
    std::cout << "Enter path to NFA file: ";
    std::string path;
    std::cin >> path;
    NFA nfa = NFA::deserialize(path);
    std::cout << "NFA created with " << nfa.getSize() << " states" << std::endl;
    DFA unminimized = nfa.determinize();
    std::cout << "DFA created with " << unminimized.getSize() << " states" << std::endl;
    unminimized.print(logger());
    DFA shaken = unminimized.treeshake(); // just to make sure this works even on its own
    DFA dfa = unminimized.minimize();
    std::cout << "Minimized DFA created with " << dfa.getSize() << " states" << std::endl;
    dfa.print(logger());
    std::cout<< "Type \"exit\" to exit\n";
    std::cout<< "Valid string example from final DFA:" << dfa.get_valid_string() << std::endl;
    std::cout<< "Valid string from unminimized DFA:" << unminimized.get_valid_string() << std::endl;
    std::string input;
    FA *checks[] = {&nfa, &unminimized, &shaken, &dfa};
    while(true) {
        std::cout << "Check str: ";
        std::cin >> input;
        if (input == "exit") break;
        bool result = checks[0]->tryAccept(input);
        for (FA *check: checks) {
            if (check->tryAccept(input) != result)
                goto debug_output;
        }
        std::cout << (result ? "Yes" : "No") << std::endl;
        continue;
        debug_output:
        std::cout<<"Bug found!\n";
        std::cout<< "NFA result: " << (checks[0]->tryAccept(input) ? "Yes" : "No") << std::endl;
        std::cout<< "Unminimized DFA result: " << (checks[1]->tryAccept(input) ? "Yes" : "No") << std::endl;
        std::cout<< "Treeshaken DFA result: " << (checks[2]->tryAccept(input) ? "Yes" : "No") << std::endl;
        std::cout<< "Minimized DFA result: " << (checks[3]->tryAccept(input) ? "Yes" : "No") << std::endl;
    }
    return 0;
}
