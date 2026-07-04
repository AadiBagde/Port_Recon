#include "banner/banner.h"
#include "scanner/scanner.h"
#include "scanner/services.h"
#include "scanner/socket_utils.h"

#define DEFAULT_SCAN_TIMEOUT_MS 500

PortResult scan_port(const ScanConfig *config, uint16_t port)
{
    PortResult result;
    memset(&result, 0, sizeof(result));

    result.port = port;
    result.is_open = false;
    strncpy(result.service, "unknown", MAX_SERVICE_NAME - 1);
    result.service[MAX_SERVICE_NAME - 1] = '\0';
    result.banner[0] = '\0';
    result.confidence = 0;

    if (config == NULL)
    {
        return result;
    }

    SOCKET sock = create_tcp_socket();
    if (sock == INVALID_SOCKET)
    {
        return result;
    }

    int timeout = config->timeout_ms > 0 ? config->timeout_ms : DEFAULT_SCAN_TIMEOUT_MS;

    if (connect_with_timeout(sock, config->target_ip, port, timeout))
    {
        const char *service_name = lookup_service_name(port);
        result.is_open = true;
        strncpy(result.service, service_name, MAX_SERVICE_NAME - 1);
        result.service[MAX_SERVICE_NAME - 1] = '\0';
        grab_banner(sock, result.banner, sizeof(result.banner), timeout);
    }

#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif

    return result;
}

PortResult *scan_range(const ScanConfig *config, ScanStats *stats)
{
    if (config == NULL || stats == NULL)
    {
        return NULL;
    }

    if (config->start_port < 1 || config->end_port > 65535 || config->start_port > config->end_port)
    {
        return NULL;
    }

    int total_ports = config->end_port - config->start_port + 1;
    PortResult *results = (PortResult *)malloc(total_ports * sizeof(PortResult));
    if (results == NULL)
    {
        return NULL;
    }

    memset(stats, 0, sizeof(*stats));
    stats->total_ports = total_ports;

    for (int i = 0; i < total_ports; i++)
    {
        uint16_t current_port = config->start_port + i;
        results[i] = scan_port(config, current_port);

        stats->scanned_ports++;
        if (results[i].is_open)
        {
            stats->open_ports++;
        }
        else
        {
            stats->closed_ports++;
        }
    }

    /* Elapsed time handling would go here, currently 0.0 */
    stats->elapsed_time = 0.0;

    return results;
}
