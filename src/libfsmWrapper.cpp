#if LIBFSM
#include <stdexcept>
#include <iostream>
#include "libfsmWrapper.h"
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
    return fsm_vm_match_buffer(vm, word.c_str(), word.size());
}

libfsmWrapper::libfsmWrapper(const NFA &nfa) {
    std::cout << "Creating libfsm DFA from our NFA\n";
    fsm = fsm_new(nullptr);
    fsm_addstate_bulk(fsm, nfa.getSize());
    fsm_setstart(fsm, nfa.getStartState());
    nfa.foreachTransition([this](Transition t) {
        fsm_addedge_literal(fsm, t.from, t.to, t.symbol);
    });
    nfa.foreachState([this](int state, bool is_final) {
        fsm_setend(fsm, state, is_final);
    });
    std::cout << "Determinising libfsm DFA\n";
    throw_if_error(fsm_determinise(fsm), 0);
    std::cout << "Minimising libfsm DFA\n";
    throw_if_error(fsm_minimise(fsm), 0);
    logger() << "Minimized libfsm DFA has " << fsm_countstates(fsm) << " states\n";
    std::cout << "Compiling libfsm DFA for quick matching\n";
    vm = fsm_vm_compile(fsm);
}

unsigned int libfsmWrapper::getSize() const {
    return fsm_countstates(fsm);
}

libfsmWrapper::~libfsmWrapper() {
    fsm_free(fsm);
    fsm_vm_free(vm);
}
#endif //LIBFSM
