#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include "utils/common.h"

/* POSIX compatibility for SOCKET types */
#ifndef _WIN32
typedef int SOCKET;
#define INVALID_SOCKET (-1)
#endif

/**
 * @brief Initializes the network subsystem (Winsock on Windows).
 * 
 * @return true if initialization succeeded, false otherwise.
 */
bool initialize_network(void);

/**
 * @brief Cleans up the network subsystem resources.
 */
void cleanup_network(void);

/**
 * @brief Creates an IPv4 TCP socket.
 * 
 * This function creates a new stream socket for the TCP protocol 
 * in the AF_INET (IPv4) domain.
 * 
 * @return A valid SOCKET descriptor on success, or INVALID_SOCKET on failure.
 */
SOCKET create_tcp_socket(void);

/**
 * @brief Builds a socket address structure from an IP string and port.
 * 
 * @param address Pointer to the sockaddr_in structure to populate.
 * @param ip The IP address as a string.
 * @param port The target port number.
 * @return true if successful, false if the IP address is invalid or arguments are NULL.
 */
bool build_socket_address(struct sockaddr_in *address, const char *ip, uint16_t port);

/**
 * @brief Connects a socket to a specific target IP and port.
 * 
 * @param socket_fd The created socket descriptor.
 * @param ip The IP address as a string.
 * @param port The target port number.
 * @return true if connection succeeds, false otherwise.
 */
bool connect_to_target(SOCKET socket_fd, const char *ip, uint16_t port);

/**
 * @brief Connects a socket to a specific target IP and port with a timeout limit.
 * 
 * Uses select() to boundedly wait for a TCP connection handshake.
 * 
 * @param socket_fd The created socket descriptor.
 * @param ip The target IP address as a string.
 * @param port The target port number.
 * @param timeout_ms The maximum time to wait in milliseconds.
 * @return true if the connection was fully established, false otherwise.
 */
bool connect_with_timeout(SOCKET socket_fd, const char *ip, uint16_t port, int timeout_ms);

#endif