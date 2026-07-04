#ifndef SERVICES_H
#define SERVICES_H

#include "utils/common.h"

#define SERVICE_UNKNOWN "unknown"

/**
 * @brief Looks up a service name by its well-known port number.
 * 
 * @param port The port number to look up.
 * @return The service name string if found, otherwise SERVICE_UNKNOWN.
 */
const char *lookup_service_name(uint16_t port);

#endif
