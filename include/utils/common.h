#ifndef COMMON_H
#define COMMON_H
/* Standard Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/*  Networking (Windows) */
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

/* Project Constants */
#define MAX_IP_LENGTH  16 
#define MAX_SERVICE_NAME 32
#define MAX_BANNER_LENGTH  256
#define MAX_FILENAME       256

#endif
