#include "snakeio.h"

/******* socket relative *******/
int sock;
struct sockaddr_in server;

/******* game relative *******/
Game *game;
int id;

/******* message relative *******/
int nbytes;

void handlePackage(Package package){
    fprintf(stderr,"[Client] Start handle package kind %d\n", package.kind);
    pthread_mutex_lock(&lock);
    switch(package.kind){
        case SET_ID:
            id = package.gi.uid;
            game->snake = game->snakes[id];
            game->id = id;
            showGameInfo(game);
            break;
        case SET_MAP:
            setStr(game->board, package.gi.y, package.gi.x, package.gi.map);
            break;
        case NEW_SNAKE:
            clientAddSnake(game, package.gi);
            break;
        case NEW_DIR:
            setUserDir(game, package.gi.uid, package.gi.dir);
            break;
        case EAT_APPLE:
            if (game->id!=package.gi.uid){
                addAt(game->board, nextHead(game->snakes[package.gi.uid]));
                addPiece(game->snakes[package.gi.uid], nextHead(game->snakes[package.gi.uid]));
            }
            break;
        case NEW_APPLE:
            deleteApple(game);
            addApple(game, initCoordinate(package.gi.y, package.gi.x, 'A'));
            redraw(game);
            break;
        case USER_DIE:
            if (game->id!=package.gi.uid){
                removeSnake(game, package.gi.uid);
                redraw(game);
            }
            break;
        default:
            break;
    }
    pthread_mutex_unlock(&lock);
    fprintf(stderr,"[Client] Successfully handle package kind %d\n", package.kind);
}

void closeConnection(){
    if (nbytes == 0)
        fprintf(stderr, "[Client] Receive error\n");
    else
        fprintf(stderr, "[Client] Receive error %d\n", errno);
    close(sock);
    game->game_over = true;
    fprintf(stderr, "[Client] Close Connection\n");
}

static void *handle_data(){
    while (!game->game_over){
        Package package;
        memset(&package,0,sizeof(package));
        if ((nbytes = recv_package(sock, &package)) <= 0)
            closeConnection();
        else
            handlePackage(package);
    }
    fprintf(stderr, "[Client] Stop handle data\n");
    return NULL;
}

void initializeClient(char *serv_addr, int serv_port){
    fprintf(stderr, "[Client] Start Connecting to server\n");
    sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock==-1) fprintf(stderr, "[Client] Couldn't create socket\n");
    fprintf(stderr, "[Client] Client Socket created\n");
    server.sin_addr.s_addr = inet_addr(serv_addr);
    server.sin_family = AF_INET;
    server.sin_port = htons(serv_port);
    if(connect(sock,(struct sockaddr *)&server,sizeof(server))<0){
        fprintf(stderr, "[Client] Connect failed. Error\n");
        exit(-1);
    }
    game = initGame(CLIENT);
    game->fd = sock;
    id = -1;
    fprintf(stderr, "[Client] Client Connect successfully\n");
}

int main(int argc, char **argv){
    if (argc!=3){
        fprintf(stderr, "[Client] need ip and port!\n");
        exit(-1);
    }
    char *serv_addr = argv[1];
    int serv_port = atoi(argv[2]);

    // initialize mutex lock
    pthread_mutex_init(&lock, NULL);

    initializeClient(serv_addr, serv_port);

    pthread_t t;
    pthread_create(&t, NULL, handle_data, NULL);
    startGame(game);
    pthread_join(t, NULL);

    close(sock);
    return 0;
}
