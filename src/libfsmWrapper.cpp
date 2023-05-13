#if LIBFSM
#include <stdexcept>
#include <iostream>
#include "libfsmWrapper.h"
#include "utils.h"
extern "C" {
#include <fsm/fsm.h>
#include <fsm/vm.h>
#include <errno.h>
}

static void throw_if_error(int result, int error) {
    if(result == error)
        throw std::runtime_error("libfsm error " + std::to_string(errno));
}

bool libfsmWrapper::tryAccept(const std::string &word) {
    if(!vm_ptr) {
        std::cout << "Compiling libfsm DFA for quick matching\n";
        vm_ptr = fsm_vm_compile(fsm_ptr);
    }
    return fsm_vm_match_buffer(vm_ptr, word.c_str(), word.size());
}

libfsmWrapper::libfsmWrapper(const NFA &nfa) : vm_ptr(nullptr) {
    std::cout << "Creating libfsm DFA from our NFA\n";
    fsm_ptr = fsm_new(nullptr);
    fsm_addstate_bulk(fsm_ptr, nfa.getSize());
    fsm_setstart(fsm_ptr, nfa.getStartState());
    nfa.foreachTransition([this](Transition t) {
        fsm_addedge_literal(fsm_ptr, t.from, t.to, t.symbol);
    });
    nfa.foreachState([this](int state, bool is_final) {
        fsm_setend(fsm_ptr, state, is_final);
    });
    logger() << "libfsm NFA has " << fsm_countstates(fsm_ptr) << " states\n";
    logger() << "libfsm NFA starts from " << [this](){fsm_state_t t;fsm_getstart(fsm_ptr,&t);return t;}() << "\n";
    std::cout << "Determinising libfsm NFA\n";
    throw_if_error(fsm_determinise(fsm_ptr), 0);
    logger() << "libfsm DFA has " << fsm_countstates(fsm_ptr) << " states\n";
    std::cout << "Minimising libfsm DFA\n";
    throw_if_error(fsm_minimise(fsm_ptr), 0);
    logger() << "Minimized libfsm DFA has " << fsm_countstates(fsm_ptr) << " states\n";
}

unsigned int libfsmWrapper::getSize() const {
    return fsm_countstates(fsm_ptr);
}

libfsmWrapper::~libfsmWrapper() {
    fsm_free(fsm_ptr);
    if(vm_ptr)
        fsm_vm_free(vm_ptr);
}
#endif //LIBFSM
