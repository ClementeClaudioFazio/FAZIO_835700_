#include "protocol.h"

int main(int argc, char *argv[]) {
    setbuf(stdout, NULL); /* Output immediately */

    #if defined _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("Error at WSAStartup\n");
        return -1;
    }
    #endif

    int port = PROTOPORT;
    char *ip_addr = "127.0.0.1"; /* Default IP */

    if (argc > 1) ip_addr = argv[1];
    if (argc > 2) port = atoi(argv[2]);

    /* 1. Create Socket */
    int c_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (c_socket < 0) {
        errorhandler("socket creation failed.\n");
        clearwinsock();
        return -1;
    }

    /* 2. Construct Server Address */
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr(ip_addr);
    sad.sin_port = htons(port);

    /* 3. Connect to Server */
    if (connect(c_socket, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
        errorhandler("Failed to connect.\n");
        closesocket(c_socket);
        clearwinsock();
        return -1;
    }

    char input_buffer[BUFFER_SIZE];
    char recv_buffer[BUFFER_SIZE];

    printf("Connected to server.\n");
    printf("Types: n=numeric, a=alpha, m=mixed, s=secure\n");
    printf("Enter request (e.g., 'n 8') or 'q' to quit.\n");

    /* Main Client Loop */
    while (1) {
        printf("\n> ");

        /* 4. Read User Input */
        if (fgets(input_buffer, BUFFER_SIZE, stdin) == NULL) break;

        /* Remove newline */
        input_buffer[strcspn(input_buffer, "\n")] = 0;

        /* Check for 'q' to quit */
        if (input_buffer[0] == 'q') {
            printf("Closing connection.\n");
            break;
        }

        /* 5. Send Request to Server */
        send(c_socket, input_buffer, strlen(input_buffer), 0);

        /* 6. Receive Password */
        memset(recv_buffer, 0, BUFFER_SIZE);
        int bytes_rcvd = recv(c_socket, recv_buffer, BUFFER_SIZE - 1, 0);

        if (bytes_rcvd <= 0) {
            printf("Server closed connection.\n");
            break;
        }

        recv_buffer[bytes_rcvd] = '\0';
        printf("Generated Password: %s\n", recv_buffer);
    }

    closesocket(c_socket);
    clearwinsock();

    #if defined _WIN32
    system("pause");
    #endif

    return 0;
}


