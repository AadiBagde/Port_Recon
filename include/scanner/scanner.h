#ifndef SCANNER_H
#define SCANNER_H

#include "utils/common.h"
#include "utils/types.h"

/**
 * @brief Scans a single port and returns the result.
 * 
 * @param config Pointer to the scan configuration.
 * @param port The target port number.
 * @return A populated PortResult structure.
 */
PortResult scan_port(const ScanConfig *config, uint16_t port);

/**
 * @brief Scans a range of ports sequentially and updates scan statistics.
 * 
 * @param config Pointer to the scan configuration.
 * @param stats Pointer to the statistics structure to update.
 * @return Dynamically allocated array of PortResult structures. 
 *         The caller is responsible for freeing this memory.
 *         Returns NULL on failure.
 */
PortResult *scan_range(const ScanConfig *config, ScanStats *stats);

#endif
