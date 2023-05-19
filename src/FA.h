#ifndef LFA_TEMA_2_FA_H
#define LFA_TEMA_2_FA_H

#include <vector>
#include <string>
#include <iosfwd>
#include "Acceptor.h"

constexpr struct {
    char start = 'a';
    char end = 'z';
    int len = end - start + 1;
} ALPHABET;

inline int sym_to_int(char symbol) {
    return symbol - ALPHABET.start;
}

inline char int_to_sym(int symbol) {
    return static_cast<char>(symbol) + ALPHABET.start;
}

struct Transition {
    int from;
    char symbol;
    int to;
    [[nodiscard]] int sym_to_int() const {
        return ::sym_to_int(symbol);
    }
};



class FA : public Acceptor {
protected:
    int start_state = 0;
    std::vector<bool> final_states;
    FA();
    void assertInBounds(int state) const;
    void assertInBounds(char symbol) const;
    void assertInBounds(Transition t) const;
public:
    void setStartState(int state);
    void setFinalState(int state, bool is_final);
    [[nodiscard]] int getSize() const;
    [[nodiscard]] int getStartState() const;
    virtual void resize(std::size_t size) = 0;
    virtual void addTransition(Transition t) = 0;
    virtual void removeTransition(Transition t) = 0;
};


#endif //LFA_TEMA_2_FA_H
