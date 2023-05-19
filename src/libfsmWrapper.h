#if LIBFSM
#ifndef LFA_TEMA_2_LIBFSMWRAPPER_H
#define LFA_TEMA_2_LIBFSMWRAPPER_H

#include "Acceptor.h"
#include "NFA.h"

extern "C" struct fsm;
extern "C" struct fsm_dfavm;

class libfsmWrapper : public Acceptor {
    fsm *fsm_ptr;
    fsm_dfavm *vm_ptr;
public:
    bool tryAccept(const std::string &word) override;
    explicit libfsmWrapper(const NFA &nfa);
    [[nodiscard]] unsigned int getSize() const;
    ~libfsmWrapper() override;
};


#endif //LFA_TEMA_2_LIBFSMWRAPPER_H
#endif //LIBFSM