#include "signal_handler.hpp"

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
volatile sig_atomic_t globalSignal = 0;

static void handleSignal(int signal)
{
    globalSignal = signal;
}

void setupSignalHandlers()
{
    struct sigaction sa = {};
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
    sa.sa_handler = handleSignal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);
    sigaction(SIGABRT, &sa, nullptr);
}