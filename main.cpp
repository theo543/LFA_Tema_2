#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include "DFA.h"
#include "NFA.h"
bool bruteforce_strings(const std::string &prev, int size, int maxsize, const std::function<bool(const std::string &)> &callback) {
    if(size > maxsize) {
        return callback(prev);
    }
    for(char c = ALPHABET.start;c<=ALPHABET.end;c++) {
        if(bruteforce_strings(prev+c, size+1, maxsize, callback)) return true;
    }
    return false;
}
int main() {
    std::cout<<"Enable debug output? (1/0): ";
    {
        int debug;
        std::cin >> debug;
        setDebugOutputEnabled(debug);
        std::ios::sync_with_stdio(false);
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
        std::cout << "Input string (_ = lambda) (BF = search for bugs) : ";
        std::cin >> input;
        if (input == "exit") break;
        if (input == "_") input = "";
        if (input == "BF") {
            long long bfcounter = 0, yescounter = 0, maxfinds;
            std::function<bool(const std::string&)> checkequal = [&checks, &bfcounter, &yescounter, &maxfinds](const std::string &s) -> bool {
                bool result = checks[0]->tryAccept(s);
                for (FA *check: checks) {
                    if (check->tryAccept(s) != result) {
                        std::cout << "Bug found!\n";
                        std::cout << "Cause string: " << s << "\n";
                        maxfinds--;
                        std::cout<<"Searches left: " << maxfinds << "\n";
                        if(maxfinds <= 0) return true;
                    }
                }
                bfcounter++;
                if(result) yescounter++;
                if(bfcounter % 100000 == 0) std::cout << "Checked " << bfcounter << " strings, " << yescounter << " of which were accepted" << std::endl;
                return false;
            };
            std::cout << "Enter max string length: ";
            int maxsize;
            std::cin >> maxsize;
            std::cout << "Cancel after how many finds: ";
            std::cin >> maxfinds;
            for(int length = 1; length <= maxsize; length++) {
                std::cout << "Checking strings of length " << length << std::endl;
                if (bruteforce_strings("", 0, length, checkequal))
                    break;
            }
            continue;
        }
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
