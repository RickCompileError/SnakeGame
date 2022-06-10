#include "snakeio.h"

int recv_package(int fd, Package *package){
    char buf[BUF_SIZE];
    int nbytes;
    if ((nbytes = recv(fd, buf, sizeof(BUF_SIZE), 0)) <= 0) return nbytes;
    else {
        package->kind = buf[0];
        switch (package->kind){
            case NEW_CONNECT:
                package->gi.y = (((int16_t)buf[1])<<8) | (int16_t)buf[2];
                package->gi.x = (((int16_t)buf[3])<<8) | (int16_t)buf[4];
                memcpy(package->gi.map, buf+5, BOARD_COLS);
                break;
            case SNAKE:
                package->gi.y = (((int16_t)buf[1])<<8) | (int16_t)buf[2];
                package->gi.x = (((int16_t)buf[3])<<8) | (int16_t)buf[4];
                package->gi.dir = buf[5];
                break;
            case NEW_DIR:
                package->gi.dir = buf[1];
                package->gi.uid = buf[2];
                break;
            case NEW_APPLE:
                package->gi.y = (((int16_t)buf[1])<<8) | (int16_t)buf[2];
                package->gi.x = (((int16_t)buf[3])<<8) | (int16_t)buf[4];
                break;
            case EAT_APPLE:
            case USER_DIE:
                package->gi.uid = buf[1];
                break;
            default:
                break;
        }
        return nbytes;
    }
}

int send_package(int fd, Package *package){
    char buf[BUF_SIZE];
    buf[0] = package->kind;
    switch(package->kind){
        case NEW_CONNECT:
            buf[1] = package->gi.y>>8;
            buf[2] = package->gi.y;
            buf[3] = package->gi.x>>8;
            buf[4] = package->gi.x;
            memcpy(buf+5, package->gi.map, BOARD_COLS);
            break;
        case SNAKE:
            buf[1] = package->gi.y>>8;
            buf[2] = package->gi.y;
            buf[3] = package->gi.x>>8;
            buf[4] = package->gi.x;
            buf[5] = package->gi.dir;
            buf[6] = package->gi.uid;
            break;
        case NEW_DIR:
            buf[1] = package->gi.dir;
            buf[2] = package->gi.uid;
            break;
        case NEW_APPLE:
            buf[1] = package->gi.y>>8;
            buf[2] = package->gi.y;
            buf[3] = package->gi.x>>8;
            buf[4] = package->gi.x;
            break;
        case EAT_APPLE:
        case USER_DIE:
            buf[1] = package->gi.uid;
            break;
    }
    return send(fd, buf, BUF_SIZE, 0);
}
