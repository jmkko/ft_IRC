#include "signal_handler.hpp"

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
volatile sig_atomic_t globalSignal = 0;

static void handle_signal(int signal) { globalSignal = signal; }

void setup_signal_handlers()
{
	struct sigaction sa = {};
	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
	sa.sa_handler = handle_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGABRT, &sa, NULL);
}
