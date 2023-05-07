#include "utils.h"
#include <iostream>

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
