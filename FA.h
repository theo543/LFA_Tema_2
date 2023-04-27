#ifndef LFA_TEMA_2_FA_H
#define LFA_TEMA_2_FA_H

#include <vector>
#include <string>
#include <unordered_map>

struct Transition {
    int from;
    int to;
    char symbol;
};

struct Edge {
    int to;
    char symbol;
};

class FA {
protected:
    int start_state = 0;
    std::vector<std::unordered_map<char, std::vector<int>>> transitions;
    std::vector<bool> final_states;
    FA() = default;
public:
    void resize(int size);
    void setStartState(int state);
    void setFinalState(int state, bool is_final);
    virtual void addTransition(Transition t);
    virtual bool tryAccept(const std::string &s) = 0;
    ~FA() = default;
};


#endif //LFA_TEMA_2_FA_H
