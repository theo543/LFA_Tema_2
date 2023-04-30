#include <stdexcept>
#include "FA.h"

FA::FA() : start_state(0), final_states(1) {}

void FA::assertInBounds(int state) const {
    if(state < 0 || state >= final_states.size()) throw std::invalid_argument("State out of bounds");
}

void FA::assertInBounds(char symbol) const {
    if(symbol < ALPHABET.start || symbol > ALPHABET.end) throw std::invalid_argument("Symbol out of bounds");
}

void FA::assertInBounds(Transition t) const {
    assertInBounds(t.from);
    assertInBounds(t.symbol);
    assertInBounds(t.to);
}

void FA::setFinalState(int state, bool is_final) {
    assertInBounds(state);
    final_states[state] = is_final;
}

void FA::setStartState(int state) {
    assertInBounds(state);
    start_state = state;
}
