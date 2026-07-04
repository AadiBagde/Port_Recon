#include "scanner/socket_utils.h"

#ifndef _WIN32
#include <fcntl.h>
#include <errno.h>
#endif

bool initialize_network(void)
{
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        return false;
    }
#endif
    return true;
}

void cleanup_network(void)
{
#ifdef _WIN32
    WSACleanup();
#endif
}

SOCKET create_tcp_socket(void)
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    return sock;
}

bool build_socket_address(struct sockaddr_in *address, const char *ip, uint16_t port)
{
    if (address == NULL || ip == NULL)
    {
        return false;
    }

    memset(address, 0, sizeof(*address));
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
    address->sin_addr.s_addr = inet_addr(ip);

    if (address->sin_addr.s_addr == INADDR_NONE)
    {
        return false;
    }

    return true;
}

bool connect_to_target(SOCKET socket_fd, const char *ip, uint16_t port)
{
    struct sockaddr_in address;

    if (socket_fd == INVALID_SOCKET)
    {
        return false;
    }

    if (!build_socket_address(&address, ip, port))
    {
        return false;
    }

    if (connect(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        return false;
    }

    return true;
}

/**
 * @brief Helper function to set a socket to non-blocking mode.
 */
static void set_socket_nonblocking(SOCKET sock)
{
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);
#else
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);
#endif
}

/**
 * @brief Helper function to restore a socket to blocking mode.
 */
static void set_socket_blocking(SOCKET sock)
{
#ifdef _WIN32
    u_long mode = 0;
    ioctlsocket(sock, FIONBIO, &mode);
#else
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags & ~O_NONBLOCK);
#endif
}

bool connect_with_timeout(SOCKET socket_fd, const char *ip, uint16_t port, int timeout_ms)
{
    struct sockaddr_in address;
    fd_set write_set;
    struct timeval tv;
    bool is_connected = false;

    if (socket_fd == INVALID_SOCKET || ip == NULL || timeout_ms <= 0)
    {
        return false;
    }

    if (!build_socket_address(&address, ip, port))
    {
        return false;
    }

    /* Change socket to non-blocking to prevent indefinite hang on filtered ports */
    set_socket_nonblocking(socket_fd);

    if (connect(socket_fd, (struct sockaddr *)&address, sizeof(address)) == 0)
    {
        /* Connection succeeded immediately */
        set_socket_blocking(socket_fd);
        return true;
    }

    /* Check if the connection is actually pending */
#ifdef _WIN32
    if (WSAGetLastError() != WSAEWOULDBLOCK)
#else
    if (errno != EINPROGRESS)
#endif
    {
        /* Connection failed immediately for some other reason (e.g. refused) */
        set_socket_blocking(socket_fd);
        return false;
    }

    FD_ZERO(&write_set);
    FD_SET(socket_fd, &write_set);

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    /* Use select() to wait boundedly for the socket to become writable */
    int select_result = select((int)(socket_fd + 1), NULL, &write_set, NULL, &tv);

    if (select_result > 0)
    {
        /* The socket is writable; we must check SO_ERROR to confirm connection success */
        int so_error = 0;
#ifdef _WIN32
        int len = sizeof(so_error);
#else
        socklen_t len = sizeof(so_error);
#endif

        if (getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, (char *)&so_error, &len) == 0)
        {
            if (so_error == 0)
            {
                is_connected = true;
            }
        }
    }

    /* Defensive: always restore the socket mode so callers get standard behavior */
    set_socket_blocking(socket_fd);

    return is_connected;
}
