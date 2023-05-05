#ifndef LFA_TEMA_2_FA_H
#define LFA_TEMA_2_FA_H

#include <vector>
#include <string>
#include <iostream>

bool getDebugOutputEnabled();
void setDebugOutputEnabled(bool);
std::ostream &logger();

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



class FA {
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
    int getSize() const;
    virtual void resize(int size) = 0;
    virtual bool tryAccept(const std::string &s) = 0;
    virtual void addTransition(Transition t) = 0;
    virtual void removeTransition(Transition t) = 0;
    virtual ~FA() = default;
};


#endif //LFA_TEMA_2_FA_H
