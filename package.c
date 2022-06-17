#include "snakeio.h"

int recv_package(int fd, Package *package){
    char buf[BUF_SIZE];
    int nbytes;
    memset(buf,0,sizeof(buf));
    if ((nbytes = recv(fd, buf, BUF_SIZE, 0)) <= 0) return nbytes;
    else {
        fprintf(stderr, "[Package] Received %d bytes Data: ", nbytes);
        for (int i=0;i<BUF_SIZE;i++) fprintf(stderr,"%d",buf[i]);
        fprintf(stderr,"\n");
        package->type = buf[0];
        switch (package->type){
            case SET_ID:
                package->gi.uid = buf[1];
                fprintf(stderr,"[Package] Received set ID: %d\n", package->gi.uid);
                break;
            case SET_MAP:
                package->gi.y = buf[1];
                package->gi.x = buf[2];
                memcpy(package->gi.map, buf+3, BOARD_COLS);
                fprintf(stderr,"[Package] Received Map: %s\n",package->gi.map);
                break;
            case SET_SNAKE:
            case NEW_SNAKE:
                package->gi.y = buf[1];
                package->gi.x = buf[2];
                package->gi.dir = buf[3];
                package->gi.uid = buf[4];
                fprintf(stderr,"[Package] Received snake: y:%d, x:%d, dir: %d, uid: %d\n", package->gi.y, package->gi.x, package->gi.dir, package->gi.uid);
                break;
            case NEW_DIR:
                package->gi.dir = buf[1];
                package->gi.uid = buf[2];
                fprintf(stderr,"[Package] Received new dir: dir: %d, uid: %d\n", package->gi.dir, package->gi.uid);
                break;
            case NEW_APPLE:
                package->gi.y = buf[1];
                package->gi.x = buf[2];
                fprintf(stderr,"[Package] Received new apple: y: %d, x: %d\n", package->gi.y, package->gi.x);
                break;
            case EAT_APPLE:
            case USER_DIE:
                package->gi.uid = buf[1];
                fprintf(stderr,"[Package] Received eat apple or user die uid: %d\n", package->gi.uid);
                break;
            default:
                break;
        }
        return nbytes;
    }
}

int send_package(int fd, Package *package){
    char buf[BUF_SIZE];
    memset(buf, 0, sizeof(buf));
    buf[0] = package->type;
    switch(package->type){
        case SET_ID:
            buf[1] = package->gi.uid;
            break;
        case SET_MAP:
            buf[1] = package->gi.y;
            buf[2] = package->gi.x;
            memcpy(buf+3, package->gi.map, BOARD_COLS);
            fprintf(stderr,"[Package] Send Map: %s\n",buf+5);
            break;
        case SET_SNAKE:
        case NEW_SNAKE:
            buf[1] = package->gi.y;
            buf[2] = package->gi.x;
            buf[3] = package->gi.dir;
            buf[4] = package->gi.uid;
            break;
        case NEW_DIR:
            buf[1] = package->gi.dir;
            buf[2] = package->gi.uid;
            break;
        case NEW_APPLE:
            buf[1] = package->gi.y;
            buf[2] = package->gi.x;
            break;
        case EAT_APPLE:
        case USER_DIE:
            buf[1] = package->gi.uid;
            break;
    }
    fprintf(stderr, "[Package] Send data: "); 
    for (int i=0;i<BUF_SIZE;i++) fprintf(stderr,"%d",buf[i]);
    fprintf(stderr, "\n");
    return send(fd, buf, BUF_SIZE, 0);
}

void sendDirection(int fd, int8_t uid, Direction dir){
    Package package;
    memset(&package, 0, sizeof(package));
    package.type = NEW_DIR;
    package.gi.uid = uid;
    package.gi.dir = dir;
    if (send_package(fd, &package) < 0){
        fprintf(stderr, "Send direction error\n");
        exit(-1);
    }
}

void sendEatApple(int fd, int8_t uid){
    Package package;
    memset(&package, 0, sizeof(package));
    package.type = EAT_APPLE;
    package.gi.uid = uid;
    if (send_package(fd, &package) < 0){
        fprintf(stderr, "Send eat apple error\n");
        exit(-1);
    }
}

void sendDie(int fd, int8_t uid){
    Package package;
    memset(&package, 0, sizeof(package));
    package.type = USER_DIE;
    package.gi.uid = uid;
    if (send_package(fd, &package) < 0){
        fprintf(stderr, "send user die error\n");
        exit(-1);
    }
}
