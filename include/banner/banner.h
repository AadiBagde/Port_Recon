#ifndef BANNER_H
#define BANNER_H

#include "scanner/socket_utils.h"
#include "utils/common.h"

/**
 * @brief Attempts to read a service banner from an already-connected socket.
 *
 * The function waits up to timeout_ms for readable data, stores a printable
 * sanitized banner in output, and always null-terminates output when possible.
 *
 * @param socket_fd Connected TCP socket.
 * @param output Destination buffer for the banner.
 * @param output_size Size of the destination buffer in bytes.
 * @param timeout_ms Maximum receive wait time in milliseconds.
 * @return true if banner data was captured, false otherwise.
 */
bool grab_banner(SOCKET socket_fd, char *output, size_t output_size, int timeout_ms);

#endif
