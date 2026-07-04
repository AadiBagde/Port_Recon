#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include "utils/types.h"

/**
 * @brief Prints PortRecon command-line usage information.
 *
 * @param program_name Name of the executable to display in examples.
 */
void print_usage(const char *program_name);

/**
 * @brief Parses command-line arguments into a scan configuration.
 *
 * Supported flags are -i <ip>, -p <start-end>, -t <ms>, -v, and -h.
 * On validation failure, a concise error is printed to stderr and false is
 * returned. The -h path prints help and exits successfully.
 *
 * @param argc Argument count from main().
 * @param argv Argument vector from main().
 * @param config Already allocated ScanConfig to populate.
 * @return true when arguments are valid, false otherwise.
 */
bool parse_arguments(int argc, char *argv[], ScanConfig *config);

#endif
