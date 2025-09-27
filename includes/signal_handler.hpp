#ifndef SIGNAL_HANDLER_HPP
#define SIGNAL_HANDLER_HPP

#include <csignal> // sig_atomic_t

extern volatile sig_atomic_t globalSignal; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

void setup_signal_handlers();

#endif