#include "signal_handler.hpp"

volatile sig_atomic_t globalSignal = 0; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

static void handleSignal(int signal)
{
    globalSignal = signal;
}

void setupSignalHandlers()
{
    struct sigaction sa = {};
    sa.sa_handler = handleSignal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);
}