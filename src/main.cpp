#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <filesystem>
#include <memory>
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
int main(int argc, char** argv) {
    Config config = (argc >= 2) ? Config(argc, argv) : Config(std::cin);
    config.applyOutputConf();
    struct check {
        std::string name;
        Acceptor *fa;
        check(std::string name, Acceptor *fa) : name(std::move(name)), fa(fa) {}
    };
    std::vector<check> checks;
    NFA nfa;
    DFA unminimized;
    try {
        nfa = NFA::deserialize(config.path);
    } catch(std::invalid_argument &e) {
        color("red", "Invalid NFA file: " + std::string(e.what()), true);
        return 1;
    }
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
    checks.emplace_back("Unminimized DFA", &unminimized);
    checks.emplace_back("Shaken DFA", shaken.get());
    checks.emplace_back("Minimized DFA", &minimized);
    std::cout<< "Type \"exit\" to exit\n";
    std::cout << "Valid string example from final DFA:" << minimized.get_valid_string() << std::endl;
    std::cout<< "Valid string from unminimized DFA:" << unminimized.get_valid_string() << std::endl;
    if(config.BF) {
        long long bf_counter = 0, yes_counter = 0, max_finds = config.BF_count;
        std::function<bool(const std::string&)> check_equal = [&checks, &bf_counter, &yes_counter, &max_finds](const std::string &s) -> bool {
            bool result = checks[0].fa->tryAccept(s);
            for (auto &check_pair : checks) {
                auto *check = check_pair.fa;
                if (check->tryAccept(s) != result) {
                    color("red", "Bug found!", true);
                    std::cout << "Cause string: " << s << "\n";
                    max_finds--;
                    std::cout << "Searches left: " << max_finds << "\n";
                    if(max_finds <= 0) return true;
                }
            }
            bf_counter++;
            if(result) yes_counter++;
            if(bf_counter % 10000000 == 0) std::cout << "Checked " << bf_counter << " strings, " << yes_counter << " of which were accepted" << std::endl;
            return false;
        };
        for(int length = 1; length <= config.BF_len; length++) {
            std::cout << "Checking strings of length " << length << std::endl;
            if (bruteforce_strings("", 0, length, check_equal))
                break;
        }
    } else while(true) {
        std::string input;
        color("blue", "Input string (_ = lambda, exit = exit): ", true);
        std::cin >> input;
        if (input == "exit") break;
        if (input == "_") input = "";
        bool bad = false;
        for(char c : input) bad |= !(ALPHABET.start <= c && c <= ALPHABET.end);
        if(bad) {
            color("red", "Invalid input string", true);
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
