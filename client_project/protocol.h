/*
 * protocol.h
 * Common header file for Client and Server
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

/* Windows specific includes */
#if defined _WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/* Constants defined in requirements */
#define PROTOPORT 60000      /* Default port number */
#define QLEN 5               /* Max client queue size */
#define BUFFER_SIZE 256
#define MIN_PW_LEN 6         /* Minimum password length */
#define MAX_PW_LEN 32        /* Maximum password length */

/* Helper function to clear Winsock on Windows */
void clearwinsock() {
    #if defined _WIN32
    WSACleanup();
    #endif
}

/* Error handling function */
void errorhandler(char *errorMessage) {
    printf("%s", errorMessage);
}

#endif

