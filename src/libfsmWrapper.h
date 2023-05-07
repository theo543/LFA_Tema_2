#if LIBFSM
#ifndef LFA_TEMA_2_LIBFSMWRAPPER_H
#define LFA_TEMA_2_LIBFSMWRAPPER_H

#include "Acceptor.h"
#include "NFA.h"

extern "C" struct fsm;
extern "C" struct fsm_dfavm;

class libfsmWrapper : public Acceptor {
    fsm *fsm;
    fsm_dfavm *vm;
public:
    bool tryAccept(const std::string &word) override;
    libfsmWrapper(const NFA &nfa);
    unsigned int getSize() const;
    virtual ~libfsmWrapper();
};


#endif //LFA_TEMA_2_LIBFSMWRAPPER_H
#endif //LIBFSM