#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/******* file descriptor *******/
fd_set master_fds;
fd_set read_fds;
int fdmax;
int listener; 
int newfd; 

/******* address relative *******/
char *port;
struct addrinfo hints, *ai, *p;
struct sockaddr_storage remoteaddr; 
socklen_t addrlen;
char remoteIP[INET6_ADDRSTRLEN];
int yes=1;

/******* message relative *******/
char buf[256]; 
int nbytes;
int i, rv;

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void initializeServer(){
    FD_ZERO(&master_fds);
    FD_ZERO(&read_fds);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    /* getaddrinfo() converts domain name or IP
     * into a dynamically allocated linked list
     * of struct addrinfo structures.
     * return 0 for success.
     */
    if ((rv = getaddrinfo(NULL, port, &hints, &ai)) != 0){
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
    }
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) continue;
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }
        break;
    }
    // check if socket and addr binding success
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }
    freeaddrinfo(ai); 
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }
    FD_SET(listener, &master_fds);
    fdmax = listener; 
}

void handleConnection(){
    addrlen = sizeof(remoteaddr);
    newfd = accept(listener,(struct sockaddr *)&remoteaddr,&addrlen);
    if (newfd == -1) fprintf(stderr, "accept error\n");
    else {
        FD_SET(newfd, &master_fds); 
        if (newfd > fdmax) fdmax = newfd; 
        fprintf(stderr, "selectserver: new connection from %s on socket %d\n",
            inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET_ADDRSTRLEN), newfd);
    }
}

void broadcast(int sender_fd_num){
    for(i = 0; i <= fdmax; i++) {
        if (FD_ISSET(i, &master_fds) && (i != listener) && (i != sender_fd_num)) {
            if (send(i, buf, nbytes, 0) == -1) {
                fprintf(stderr, "send error\n");
            }
        }
    }
}

void closeConnection(int sender_fd_num){
    if (nbytes == 0) {
        fprintf(stderr, "selectserver: socket %d hung up\n", sender_fd_num);
    } else {
        fprintf(stderr, "recv error");
    }
    close(sender_fd_num); 
    FD_CLR(sender_fd_num, &master_fds); 
}

void handleData(int sender_fd_num){
    if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0)
        closeConnection(sender_fd_num);
    else
        broadcast(sender_fd_num);
}

void selectLoop(){
    while (1) {
        read_fds = master_fds;
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) { // block
            fprintf(stderr, "error: select()\n");
            exit(4);
        }
        for (i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { 
                if (i == listener) handleConnection();
                else handleData(i);
            }
        }
    } 
}

int main(int argc, char **argv)
{
    // get port number
    if (argc<2){
        fprintf(stderr, "<Server> need port insert!\n");
        exit(-1);
    }
    port = argv[1];

    initializeServer();

    selectLoop();

    return 0;
}
