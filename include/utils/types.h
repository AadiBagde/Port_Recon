#ifndef TYPES_H
#define TYPES_H

#include "common.h"

typedef struct
{
    char target_ip[MAX_IP_LENGTH];

    int start_port;
    int end_port;

    /* Maximum time (in milliseconds) to wait for a TCP handshake (e.g. 50 - 5000 ms) */
    int timeout_ms;


    int thread_count;

    bool verbose;

    char output_file[MAX_FILENAME];

} ScanConfig;

typedef struct
{
    int port;

    bool is_open;

    char service[MAX_SERVICE_NAME];

    char banner[MAX_BANNER_LENGTH];

    int confidence;

} PortResult;

typedef struct
{
    int total_ports;

    int scanned_ports;

    int open_ports;

    int closed_ports;

    double elapsed_time;

} ScanStats;

#endif