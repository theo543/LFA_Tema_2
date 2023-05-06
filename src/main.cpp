#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <vector>
#include <filesystem>
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
    struct check {
        std::string name;
        FA *fa;
    };
    std::vector<check> checks;
    ///TODO nonminimal_iterations.txt takes a different amount of iterations to minimize every time, why isn't it deterministic?
    ///TODO add an external library for extra checking (this https://github.com/katef/libfsm looks useful if it has minimization)
    NFA nfa;
    DFA unminimized;
    std::string path;
    while(true) {
        std::cout << "Enter path to NFA file (empty for menu): ";
        getline(std::cin, path);
        if(path.empty()) {
            const std::filesystem::path def_f = std::filesystem::current_path() / "tests";
            std::vector<std::filesystem::path> files;
            files.insert(files.end(), std::filesystem::directory_iterator(def_f), std::filesystem::directory_iterator());
            std::sort(files.begin(), files.end());
            for(int nr = 1;nr<files.size();nr++) {
                std::cout << nr << " - " << files[nr].filename() << std::endl;
            }
            int choice;
            while(true) {
                std::cout << "Enter choice: ";
                std::cin >> choice;
                if(choice >= 0 && choice < files.size()) {
                    path = files[choice].string();
                    break;
                } else std::cout << "Invalid choice." << std::endl;
            }
        }
        if (!std::filesystem::exists(path)) {
            std::cout << "File not found." << std::endl;
        } else break;
    }
    nfa = NFA::deserialize(path);
    std::cout << "NFA created with " << nfa.getSize() << " states" << std::endl;
    unminimized = nfa.determinize();
    std::cout << "DFA created with " << unminimized.getSize() << " states" << std::endl;
    checks.emplace_back("Deserialized NFA", &nfa);
    unminimized.print(logger());
    DFA shaken = unminimized.treeshake(); // just to make sure this works even on its own
    std::cout << "(Only) Shaken DFA has " << shaken.getSize() << " states" << std::endl;
    DFA minimized = unminimized.minimize();
    std::cout << "Minimized DFA created with " << minimized.getSize() << " states" << std::endl;
    if(minimized.getSize() == shaken.getSize())
        std::cout << "Shaken DFA was already minimal (make sure RNG is adding lots of sink transitions to get non-minimal DFA)" << std::endl;
    minimized.print(logger());
    std::cout<< "Type \"exit\" to exit\n";
    std::cout << "Valid string example from final DFA:" << minimized.get_valid_string() << std::endl;
    std::cout<< "Valid string from unminimized DFA:" << unminimized.get_valid_string() << std::endl;
    std::string input;
    checks.emplace_back("Unminimized DFA", &unminimized);
    checks.emplace_back("Minimized DFA", &minimized);
    checks.emplace_back("Shaken DFA", &shaken);
    while(true) {
        std::cout << "Input string (_ = lambda) (BF = search for bugs) : ";
        std::cin >> input;
        if (input == "exit") break;
        if (input == "_") input = "";
        if (input == "BF") {
            long long bfcounter = 0, yescounter = 0, maxfinds;
            std::function<bool(const std::string&)> checkequal = [&checks, &bfcounter, &yescounter, &maxfinds](const std::string &s) -> bool {
                bool result = checks[0].fa->tryAccept(s);
                for (auto &check_pair : checks) {
                    FA *check = check_pair.fa;
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
                if(bfcounter % 10000000 == 0) std::cout << "Checked " << bfcounter << " strings, " << yescounter << " of which were accepted" << std::endl;
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
        bool result = checks[0].fa->tryAccept(input);
        bool bugged = false;
        for (auto &check_pair : checks) {
            FA *check = check_pair.fa;
            bool thisresult = check->tryAccept(input);
            std::cout << check_pair.name << ": " << (thisresult ? "Yes" : "No") << std::endl;
            bugged |= thisresult != result;
        }
        std::cout << (bugged ? "Bug found!" : "All match!") << std::endl;
    }
    return 0;
}
