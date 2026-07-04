#include "utils/common.h"
#include "utils/types.h"
#include "cli/parser.h"
#include "scanner/socket_utils.h"
#include "scanner/scanner.h"

#define EXIT_SETUP_FAILURE 1

static void print_scan_header(const ScanConfig *config)
{
    printf("Starting scan\n");
    printf("Target  : %s\n", config->target_ip);
    printf("Ports   : %d-%d\n", config->start_port, config->end_port);
    printf("Timeout : %d ms\n\n", config->timeout_ms);
}

static void print_result_row(const PortResult *result)
{
    printf("%-8d %-8s %-14s %-s\n",
           result->port,
           result->is_open ? "OPEN" : "CLOSED",
           result->service,
           result->banner[0] != '\0' ? result->banner : "-");
}

static void print_results(const ScanConfig *config, const PortResult *results, const ScanStats *stats)
{
    printf("%-8s %-8s %-14s %-s\n", "PORT", "STATUS", "SERVICE", "BANNER");

    for (int i = 0; i < stats->total_ports; ++i)
    {
        if (config->verbose)
        {
            printf("scanning port %d... timeout=%dms\n", results[i].port, config->timeout_ms);
        }

        print_result_row(&results[i]);
    }

    printf("\n");
}

static void print_summary(const ScanStats *stats)
{
    printf("Open Ports   : %d\n", stats->open_ports);
    printf("Closed Ports : %d\n", stats->closed_ports);
    printf("Total Ports  : %d\n", stats->total_ports);
}

int main(int argc, char *argv[])
{
    ScanConfig config;
    ScanStats stats;
    PortResult *results;

    if (argc == 1)
    {
        fprintf(stderr, "Usage hint: %s -i <ip> -p <start-end> [-t <ms>] [-v]\n", argv[0]);
        return EXIT_SETUP_FAILURE;
    }

    if (!parse_arguments(argc, argv, &config))
    {
        fprintf(stderr, "Usage hint: %s -h\n", argv[0]);
        return EXIT_SETUP_FAILURE;
    }

    if (!initialize_network())
    {
        fprintf(stderr, "Error: failed to initialize network subsystem\n");
        return EXIT_SETUP_FAILURE;
    }

    print_scan_header(&config);

    results = scan_range(&config, &stats);
    if (results == NULL)
    {
        fprintf(stderr, "Error: scan failed\n");
        cleanup_network();
        return EXIT_SETUP_FAILURE;
    }

    print_results(&config, results, &stats);
    print_summary(&stats);

    free(results);

    cleanup_network();
    return 0;
}
