#include <vector>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include "utils.h"
#include "color.hpp"

void Config::applyOutputConf() const {
    setDebugOutputEnabled(logger);
    setVerboseOutputEnabled(verbose);
}

Config::Config(std::istream &in) {
    color("blue", "Enable debug output? (1/0): ", true);
    in >> logger;
    if(logger) {
        color("blue", "Enable verbose debug output? (1/0): ", true);
        in >> verbose;
    }
    in.ignore(); // Ignore newline from debug input
    while(true) {
        color("blue", "Enter path to NFA file (empty for menu): ", true);
        getline(in, path);
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
                in >> choice;
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
    color("blue", "Enable brute-force bug search? (1/0):\n");
    in >> BF;
    if(BF) {
        color("blue", "Enter max string length: ", true);
        in >> BF_len;
        color("blue", "Cancel after how many finds: ", true);
        in >> BF_count;
    }
}

Config::Config(int argc, char **argv) {
    if(argc < 2) throw std::invalid_argument("no arguments found");
    path = argv[1];
    for(int x = 2;x<argc;x++) {
        std::string arg{argv[x]};
        if(arg == "-v") logger = verbose = true;
        else if(arg == "-l") logger = true;
        else if(arg == "-BF") {
            if((argc - x - 1) < 2) {
                color("red", "Ignoring incomplete BF flag (need max len and max fails until stop)", true);
            } else {
                try {
                    BF_len = std::stoll(argv[x + 1]);
                    BF_count = std::stoll(argv[x + 2]);
                    BF = true;
                    x += 2;
                } catch(...) {
                    color("red", "Failed to parse BF config values", true);
                    BF = false;
                }
            }
        } else {
            color("red", "Unknown argument: " + arg, true);
        }
    }
}

static bool dbg_value = false;
static bool verbose_value = false;

void setDebugOutputEnabled(bool value) {
    dbg_value = value;
}

void setVerboseOutputEnabled(bool value) {
    verbose_value = value;
}

static std::ostream null_stream(nullptr);

std::ostream &logger() {
    if(dbg_value) return std::cout;
    else return null_stream;
}

std::ostream &verbose() {
    if(verbose_value) return std::cout;
    else return null_stream;
}
