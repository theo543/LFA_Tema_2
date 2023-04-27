#include <stdexcept>
#include "FA.h"

void FA::resize(int size) {
    if(size < 0) {
        throw std::runtime_error("Size must be positive");
    }
    if(start_state >= size) {
        throw std::runtime_error("Start state would become out of bounds");
    }
    transitions.resize(size);
    final_states.resize(size);
}

void FA::setFinalState(int state, bool is_final) {
    if(state < 0 || state >= final_states.size()) {
        throw std::runtime_error("State out of bounds");
    }
    final_states[state] = is_final;
}

void FA::setStartState(int state) {
    if(state < 0 || state >= transitions.size()) {
        throw std::runtime_error("State out of bounds");
    }
    start_state = state;
}
