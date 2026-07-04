#include "cli/parser.h"

#define DEFAULT_TIMEOUT_MS 500
#define MIN_PORT 1
#define MAX_PORT_NUMBER 65535
#define BASE_10 10

static bool is_valid_ipv4_address(const char *ip)
{
    if (ip == NULL || ip[0] == '\0')
    {
        return false;
    }

    return inet_addr(ip) != INADDR_NONE;
}

static bool parse_positive_int(const char *value, int *parsed)
{
    char *end = NULL;
    long number;

    if (value == NULL || value[0] == '\0' || parsed == NULL)
    {
        return false;
    }

    number = strtol(value, &end, BASE_10);
    if (*end != '\0' || number <= 0 || number > INT32_MAX)
    {
        return false;
    }

    *parsed = (int)number;
    return true;
}

static bool parse_port_range(const char *value, int *start_port, int *end_port)
{
    const char *separator;
    char start_buffer[16];
    char end_buffer[16];
    size_t start_length;
    size_t end_length;

    if (value == NULL || start_port == NULL || end_port == NULL)
    {
        return false;
    }

    separator = strchr(value, '-');
    if (separator == NULL || strchr(separator + 1, '-') != NULL)
    {
        return false;
    }

    start_length = (size_t)(separator - value);
    end_length = strlen(separator + 1);
    if (start_length == 0 || end_length == 0 ||
        start_length >= sizeof(start_buffer) || end_length >= sizeof(end_buffer))
    {
        return false;
    }

    memcpy(start_buffer, value, start_length);
    start_buffer[start_length] = '\0';
    memcpy(end_buffer, separator + 1, end_length + 1);

    if (!parse_positive_int(start_buffer, start_port) ||
        !parse_positive_int(end_buffer, end_port))
    {
        return false;
    }

    return *start_port >= MIN_PORT &&
           *end_port <= MAX_PORT_NUMBER &&
           *start_port <= *end_port;
}

static bool has_option_value(int index, int argc, char *argv[])
{
    return index + 1 < argc && argv[index + 1] != NULL && argv[index + 1][0] != '-';
}

void print_usage(const char *program_name)
{
    const char *name = program_name != NULL ? program_name : "portrecon";

    printf("Usage: %s -i <ip> -p <start-end> [options]\n", name);
    printf("\n");
    printf("Options:\n");
    printf("  -i <ip>           Target IPv4 address (required)\n");
    printf("  -p <start-end>    Port range, e.g. 1-1000 (required)\n");
    printf("  -t <ms>           Timeout per port in milliseconds (default: %d)\n", DEFAULT_TIMEOUT_MS);
    printf("  -v                Enable verbose output\n");
    printf("  -h                Show this help text\n");
}

bool parse_arguments(int argc, char *argv[], ScanConfig *config)
{
    bool has_ip = false;
    bool has_port_range = false;

    if (config == NULL || argv == NULL)
    {
        fprintf(stderr, "Error: invalid parser input\n");
        return false;
    }

    memset(config, 0, sizeof(*config));
    config->timeout_ms = DEFAULT_TIMEOUT_MS;

    if (argc == 1)
    {
        return false;
    }

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-h") == 0)
        {
            print_usage(argv[0]);
            exit(0);
        }
        else if (strcmp(argv[i], "-v") == 0)
        {
            config->verbose = true;
        }
        else if (strcmp(argv[i], "-i") == 0)
        {
            if (!has_option_value(i, argc, argv))
            {
                fprintf(stderr, "Error: missing value for -i\n");
                return false;
            }

            if (!is_valid_ipv4_address(argv[i + 1]))
            {
                fprintf(stderr, "Error: invalid IPv4 address '%s'\n", argv[i + 1]);
                return false;
            }

            strncpy(config->target_ip, argv[i + 1], MAX_IP_LENGTH - 1);
            config->target_ip[MAX_IP_LENGTH - 1] = '\0';
            has_ip = true;
            ++i;
        }
        else if (strcmp(argv[i], "-p") == 0)
        {
            if (!has_option_value(i, argc, argv))
            {
                fprintf(stderr, "Error: missing value for -p\n");
                return false;
            }

            if (!parse_port_range(argv[i + 1], &config->start_port, &config->end_port))
            {
                fprintf(stderr, "Error: invalid port range '%s'\n", argv[i + 1]);
                return false;
            }

            has_port_range = true;
            ++i;
        }
        else if (strcmp(argv[i], "-t") == 0)
        {
            if (!has_option_value(i, argc, argv))
            {
                fprintf(stderr, "Error: missing value for -t\n");
                return false;
            }

            if (!parse_positive_int(argv[i + 1], &config->timeout_ms))
            {
                fprintf(stderr, "Error: invalid timeout '%s'\n", argv[i + 1]);
                return false;
            }

            ++i;
        }
        else
        {
            fprintf(stderr, "Error: unknown option '%s'\n", argv[i]);
            return false;
        }
    }

    if (!has_ip)
    {
        fprintf(stderr, "Error: missing required -i <ip>\n");
        return false;
    }

    if (!has_port_range)
    {
        fprintf(stderr, "Error: missing required -p <start-end>\n");
        return false;
    }

    return true;
}
