#include "scanner/services.h"

typedef struct
{
    uint16_t port;
    const char *name;
} ServiceEntry;

static const ServiceEntry SERVICE_TABLE[] = {
    { 21,   "FTP" },
    { 22,   "SSH" },
    { 23,   "TELNET" },
    { 25,   "SMTP" },
    { 53,   "DNS" },
    { 80,   "HTTP" },
    { 110,  "POP3" },
    { 143,  "IMAP" },
    { 443,  "HTTPS" },
    { 445,  "SMB" },
    { 3306, "MYSQL" },
    { 3389, "RDP" },
    { 5432, "POSTGRESQL" },
    { 8080, "HTTP-ALT" }
};

static const size_t SERVICE_TABLE_SIZE = sizeof(SERVICE_TABLE) / sizeof(SERVICE_TABLE[0]);

const char *lookup_service_name(uint16_t port)
{
    for (size_t i = 0; i < SERVICE_TABLE_SIZE; ++i)
    {
        if (SERVICE_TABLE[i].port == port)
        {
            return SERVICE_TABLE[i].name;
        }
    }
    
    return SERVICE_UNKNOWN;
}
