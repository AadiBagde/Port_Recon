#include "banner/banner.h"

#define MIN_BANNER_TIMEOUT_MS 1
#define RECEIVE_FLAGS 0

static void sanitize_banner(char *banner)
{
    if (banner == NULL)
    {
        return;
    }

    for (size_t i = 0; banner[i] != '\0'; ++i)
    {
        unsigned char character = (unsigned char)banner[i];

        if (character == '\r' || character == '\n' || character == '\t')
        {
            banner[i] = ' ';
        }
        else if (character < 32 || character > 126)
        {
            banner[i] = '.';
        }
    }
}

static bool wait_for_readable_socket(SOCKET socket_fd, int timeout_ms)
{
    fd_set read_set;
    struct timeval timeout;
    int select_result;

    FD_ZERO(&read_set);
    FD_SET(socket_fd, &read_set);

    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    select_result = select((int)(socket_fd + 1), &read_set, NULL, NULL, &timeout);
    return select_result > 0 && FD_ISSET(socket_fd, &read_set);
}

bool grab_banner(SOCKET socket_fd, char *output, size_t output_size, int timeout_ms)
{
    int bytes_received;

    if (socket_fd == INVALID_SOCKET || output == NULL || output_size == 0)
    {
        return false;
    }

    output[0] = '\0';

    if (timeout_ms < MIN_BANNER_TIMEOUT_MS)
    {
        return false;
    }

    if (!wait_for_readable_socket(socket_fd, timeout_ms))
    {
        return false;
    }

    bytes_received = recv(socket_fd, output, (int)(output_size - 1), RECEIVE_FLAGS);
    if (bytes_received <= 0)
    {
        output[0] = '\0';
        return false;
    }

    output[bytes_received] = '\0';
    sanitize_banner(output);

    return output[0] != '\0';
}
