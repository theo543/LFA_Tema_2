#ifndef LFA_TEMA_2_UTILS_H
#define LFA_TEMA_2_UTILS_H

#include <iosfwd>

void setVerboseOutputEnabled(bool);
void setDebugOutputEnabled(bool);
std::ostream &logger();
std::ostream &verbose();
#endif //LFA_TEMA_2_UTILS_H
