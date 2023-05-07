#ifndef LFA_TEMA_2_ACCEPTOR_H
#define LFA_TEMA_2_ACCEPTOR_H
#include <string>
class Acceptor {
public:
    virtual bool tryAccept(const std::string &word) = 0;
    virtual ~Acceptor() = default;
};

#endif //LFA_TEMA_2_ACCEPTOR_H
