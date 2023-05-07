#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <vector>
#include <filesystem>
#include "DFA.h"
#include "NFA.h"
#include "utils.h"
#include "color.hpp"
#if LIBFSM
#include "libfsmWrapper.h"
#endif
bool bruteforce_strings(const std::string &prev, int size, int maxsize, const std::function<bool(const std::string &)> &callback) {
    if(size >= maxsize) {
        return callback(prev);
    }
    for(char c = ALPHABET.start;c<=ALPHABET.end;c++) {
        if(bruteforce_strings(prev+c, size+1, maxsize, callback)) return true;
    }
    return false;
}
int main() {
    color("blue", "Enable debug output? (1/0): ", true);
    {
        int debug;
        std::cin >> debug;
        setDebugOutputEnabled(debug);
        if(debug) {
            color("blue", "Enable verbose debug output? (1/0): ", true);
            std::cin >> debug;
            setVerboseOutputEnabled(debug);
        }
    }
    struct check {
        std::string name;
        Acceptor *fa;
    };
    std::vector<check> checks;
    NFA nfa;
    DFA unminimized;
    std::string path;
    std::cin.ignore(); // Ignore newline from debug input
    while(true) {
        color("blue", "Enter path to NFA file (empty for menu): ", true);
        getline(std::cin, path);
        if(path.empty()) {
            const std::filesystem::path def_f = std::filesystem::current_path() / "tests";
            std::vector<std::filesystem::path> files;
            files.insert(files.end(), std::filesystem::directory_iterator(def_f), std::filesystem::directory_iterator());
            std::sort(files.begin(), files.end());
            for(int nr = 0;nr<files.size();nr++) {
                std::cout << nr << " - " << files[nr].filename() << std::endl;
            }
            int choice;
            while(true) {
                color("blue", "Enter choice: ", true);
                std::cin >> choice;
                if(choice >= 0 && choice < files.size()) {
                    path = files[choice].string();
                    break;
                } else color("red", "Invalid choice.", true);
            }
        }
        if (!std::filesystem::exists(path) || std::filesystem::is_directory(path)) {
            color("red", "File not found.", true);
        } else break;
    }
    nfa = NFA::deserialize(path);
    color("blue", "NFA created with " + std::to_string(nfa.getSize()) + " states", true);
    unminimized = nfa.determinize();
    color("blue", "DFA created with " + std::to_string(unminimized.getSize()) + " states", true);
    checks.emplace_back("Deserialized NFA", &nfa);
    std::unique_ptr<DFA> shaken = std::make_unique<DFA>();
    DFA minimized = unminimized.minimize(shaken.get());
    color("blue", "Shaken DFA created with " + std::to_string(shaken->getSize()) + " states", true);
    color("blue", "Minimized DFA created with " + std::to_string(minimized.getSize()) + " states", true);
    if(minimized.getSize() == shaken->getSize())
        color("yellow", "Shaken DFA was already minimal (make sure RNG is adding lots of sink transitions to get non-minimal DFA)", true);
    minimized.print(verbose());
#if LIBFSM
    std::cout << "Creating libfsmWrapper (external library)..." << std::endl;
    libfsmWrapper fsm(nfa);
    if(fsm.getSize() != 0) {
        color("blue", "libfsmWrapper created with " + std::to_string(fsm.getSize()) + " states", true);
        if (fsm.getSize() == minimized.getSize())
            color("green", "libfsm agrees that the minimized DFA is minimal", true);
        else color("red", "libfsm disagrees that the minimized DFA is minimal", true);
        checks.emplace_back("libfsm (external library)", &fsm);
    } else {
        color("yellow", "libfsm's DFA is empty, not using for bug checking because libfsm doesn't like empty languages (causes EINVAL)", true);
    }
#endif //LIBFSM
    std::cout<< "Type \"exit\" to exit\n";
    std::cout << "Valid string example from final DFA:" << minimized.get_valid_string() << std::endl;
    std::cout<< "Valid string from unminimized DFA:" << unminimized.get_valid_string() << std::endl;
    std::string input;
    checks.emplace_back("Unminimized DFA", &unminimized);
    checks.emplace_back("Shaken DFA", shaken.get());
    checks.emplace_back("Minimized DFA", &minimized);
    while(true) {
        color("blue", "Input string (_ = lambda) (BF = search for bugs) : ", true);
        std::cin >> input;
        if (input == "exit") break;
        if (input == "_") input = "";
        if (input == "BF") {
            long long bfcounter = 0, yescounter = 0, maxfinds;
            std::function<bool(const std::string&)> checkequal = [&checks, &bfcounter, &yescounter, &maxfinds](const std::string &s) -> bool {
                bool result = checks[0].fa->tryAccept(s);
                for (auto &check_pair : checks) {
                    auto *check = check_pair.fa;
                    if (check->tryAccept(s) != result) {
                        color("red", "Bug found!", true);
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
            color("blue", "Enter max string length: ", true);
            int maxsize;
            std::cin >> maxsize;
            color("blue", "Cancel after how many finds: ", true);
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
            auto *check = check_pair.fa;
            bool thisresult = check->tryAccept(input);
            std::cout << check_pair.name << ": " << (thisresult ? "Yes" : "No") << std::endl;
            bugged |= thisresult != result;
        }
        if(bugged)
            color("red", "Bug found!", true);
        else color("green", "All match!", true);
    }
    return 0;
}
