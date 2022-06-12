#include "snakeio.h"

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

/******* game relative *******/
int user_to_fd[MAX_USER];
int fd_to_user[MAX_FD];
int user_num;
Game *game;

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
    for (i=0;i<MAX_USER;i++) user_to_fd[i] = -1;
    for (i=0;i<MAX_FD;i++) fd_to_user[i] = -1;
    user_num = 0;
    game = initGame(SERVER);
}

void broadcast(Package *package){
    for(i = 0; i <= fdmax; i++) {
        if (FD_ISSET(i, &master_fds) && (i != listener)) {
            if (send_package(i, package) < 0) {
                fprintf(stderr, "broadcast error\n");
            }
        }
    }
}

void handleNewSnake(int newfd){
    serverAddSnake(game, fd_to_user[newfd]);
    Package package;
    memset(&package, 0, sizeof(package));
    package.kind = SET_MAP;
    for (i=0;i<BOARD_ROWS;i++){
        getStr(game->board, i, 0, package.gi.map);
        package.gi.y = i;
        package.gi.x = 0;
        if (send_package(newfd, &package) < 0) {
            fprintf(stderr, "send map error\n");
        }
    }
    memset(&package, 0, sizeof(package));
    package.kind = NEW_SNAKE;
    for (i=0;i<MAX_USER;i++){
        if (user_to_fd[i]>=0){
            Coordinate coor = tail(game->snakes[i]);
            package.gi.y = gety(coor);
            package.gi.x = getx(coor);
            package.gi.dir = getDirection(game->snakes[i]);
            package.gi.uid = i;
            if (user_to_fd[i]==newfd){
                broadcast(&package);
            } else if (send_package(newfd, &package) < 0){
                fprintf(stderr, "send snake error\n");
            }
        }
    }
    memset(&package, 0, sizeof(package));
    package.kind = SET_ID;
    package.gi.uid = fd_to_user[newfd];
    if (send_package(newfd, &package) < 0){
        fprintf(stderr, "send send id error\n");
    }
}

void closeConnection(int sender_fd_num){
    if (nbytes == 0) {
        fprintf(stderr, "selectserver: socket %d hung up\n", sender_fd_num);
    } else {
        fprintf(stderr, "recv error");
    }
    removeSnake(game, fd_to_user[sender_fd_num]);
    Package package;
    memset(&package, 0, sizeof(package));
    package.kind = USER_DIE;
    package.gi.uid = fd_to_user[sender_fd_num];
    broadcast(&package);
    user_to_fd[fd_to_user[sender_fd_num]] = -1;
    fd_to_user[sender_fd_num] = -1;
    user_num--;
    close(sender_fd_num); 
    FD_CLR(sender_fd_num, &master_fds); 
}

void handleConnection(){
    addrlen = sizeof(remoteaddr);
    newfd = accept(listener,(struct sockaddr *)&remoteaddr,&addrlen);
    if (newfd == -1) fprintf(stderr, "accept error\n");
    user_num++;
    if (user_num > MAX_USER) {
        closeConnection(newfd);   
    } else {
        FD_SET(newfd, &master_fds); 
        if (newfd > fdmax) fdmax = newfd; 
        fprintf(stderr, "selectserver: new connection from %s on socket %d\n", inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET_ADDRSTRLEN), newfd);
        for (i=0;i<MAX_USER;i++){
            if (user_to_fd[i] == -1){
                user_to_fd[i] = newfd;
                fd_to_user[newfd] = i;
            }
        }
        handleNewSnake(newfd);
    }
}

void handlePackage(Package package){
    switch(package.kind){
        case NEW_DIR:
            setUserDir(game, package.gi.uid, package.gi.dir);
            broadcast(&package);
            break;
        case EAT_APPLE:
            addAt(game->board, nextHead(game->snakes[package.gi.uid]));
            addPiece(game->snakes[package.gi.uid], nextHead(game->snakes[package.gi.uid]));
            broadcast(&package);
            deleteApple(game);
            createApple(game);
            memset(&package, 0, sizeof(package));
            package.kind = NEW_APPLE;
            package.gi.y = getAppleY(game->apple);
            package.gi.x = getAppleX(game->apple);
            broadcast(&package);
            break;
        case USER_DIE:
            removeSnake(game, package.gi.uid);
            fd_to_user[user_to_fd[package.gi.uid]] = -1;
            user_to_fd[package.gi.uid] = -1;
            broadcast(&package);
            break;
        default:
            break;
    }
}

void handleData(int sender_fd_num){
    Package package;
    memset(&package,0,sizeof(package));
    if ((nbytes = recv_package(sender_fd_num, &package)) <= 0)
        closeConnection(sender_fd_num);
    else
        handlePackage(package);
}

static void *selectLoop(){
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
    return NULL;
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

    pthread_t t;
    pthread_create(&t, NULL, selectLoop, NULL);
    startGame(game); 
    pthread_join(t, NULL);

    return 0;
}
