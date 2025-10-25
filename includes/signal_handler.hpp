/**
 * @file signal_handler.hpp
 * @brief utility methods for signal handling
 * @version 0.1
 * @date 2025-10-24
 *
 * @copyright Copyright (c) 2025
 */
#ifndef SIGNAL_HANDLER_HPP
#define SIGNAL_HANDLER_HPP

#include <csignal> // sig_atomic_t

/**
 * @brief init globalSignal to 0
 * this variable allow to check signals to stop safely the server
 */
extern volatile sig_atomic_t globalSignal; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

/**
 * @brief init SIGINT, SIGTERM and SIGABRT to NULL
 */
void setup_signal_handlers();

#endif
