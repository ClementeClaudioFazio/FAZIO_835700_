#include "protocol.h"

/* Function Prototypes */
void generate_numeric(int length, char *buffer);
void generate_alpha(int length, char *buffer);
void generate_mixed(int length, char *buffer);
void generate_secure(int length, char *buffer);

int main(int argc, char *argv[]) {
    setbuf(stdout, NULL); /* Disable buffering for instant console output */

    /* 1. Initialize Winsock for Windows */
    #if defined _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        errorhandler("Error at WSAStartup\n");
        return -1;
    }
    #endif

    int port = PROTOPORT;
    if (argc > 1) {
        port = atoi(argv[1]); /* Use port from command line if provided */
    }

    /* 2. Create Socket */
    int my_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (my_socket < 0) {
        errorhandler("socket creation failed.\n");
        clearwinsock();
        return -1;
    }

    /* 3. Construct Server Address */
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr("127.0.0.1"); /* Localhost */
    sad.sin_port = htons(port);

    /* 4. Bind Socket */
    if (bind(my_socket, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
        errorhandler("bind() failed.\n");
        closesocket(my_socket);
        clearwinsock();
        return -1;
    }

    /* 5. Listen */
    if (listen(my_socket, QLEN) < 0) {
        errorhandler("listen() failed.\n");
        closesocket(my_socket);
        clearwinsock();
        return -1;
    }

    /* Initialize random seed */
    srand(time(NULL));

    struct sockaddr_in cad; /* Client address */
    int client_socket;      /* Socket descriptor for client */
    int client_len;         /* Address size (int for Windows) */

    printf("Server listening on port %d. Waiting for connections...\n", port);

    /* Main server loop */
    while (1) {
        client_len = sizeof(cad);

        /* 6. Accept Connection */
        client_socket = accept(my_socket, (struct sockaddr *)&cad, &client_len);
        if (client_socket < 0) {
            errorhandler("accept() failed.\n");
            continue; /* Try next connection */
        }

        /* Print New Connection Info */
        printf("New connection from %s:%d\n", inet_ntoa(cad.sin_addr), ntohs(cad.sin_port));

        /* Handle Client Requests */
        char buffer[BUFFER_SIZE];
        char password[BUFFER_SIZE];

        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            /* Receive data */
            int bytes_rcvd = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);

            if (bytes_rcvd <= 0) {
                printf("Connection closed by client.\n");
                closesocket(client_socket);
                break;
            }

            buffer[bytes_rcvd] = '\0';

            /* Parse Request (e.g., "n 8") */
            char type;
            int len;
            if (sscanf(buffer, " %c %d", &type, &len) == 2) {

                /* Validate length */
                if (len < MIN_PW_LEN) len = MIN_PW_LEN;
                if (len > MAX_PW_LEN) len = MAX_PW_LEN;

                memset(password, 0, BUFFER_SIZE);

                /* Generate Password based on type */
                switch (type) {
                    case 'n': generate_numeric(len, password); break;
                    case 'a': generate_alpha(len, password); break;
                    case 'm': generate_mixed(len, password); break;
                    case 's': generate_secure(len, password); break;
                    default: strcpy(password, "Error: Invalid Type");
                }

                /* Send Password back */
                send(client_socket, password, strlen(password), 0);
            } else {
                char *error_msg = "Error: Invalid Format";
                send(client_socket, error_msg, strlen(error_msg), 0);
            }
        }
    }
}

/* Implementation of generation functions */

void generate_numeric(int length, char *buffer) {
    for (int i = 0; i < length; i++) {
        buffer[i] = '0' + (rand() % 10);
    }
    buffer[length] = '\0';
}

void generate_alpha(int length, char *buffer) {
    for (int i = 0; i < length; i++) {
        buffer[i] = 'a' + (rand() % 26);
    }
    buffer[length] = '\0';
}

void generate_mixed(int length, char *buffer) {
    char *chars = "abcdefghijklmnopqrstuvwxyz0123456789";
    for (int i = 0; i < length; i++) {
        buffer[i] = chars[rand() % 36];
    }
    buffer[length] = '\0';
}

void generate_secure(int length, char *buffer) {
    char *chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
    int set_len = strlen(chars);
    for (int i = 0; i < length; i++) {
        buffer[i] = chars[rand() % set_len];
    }
    buffer[length] = '\0';
}


