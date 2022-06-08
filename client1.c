#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>

int sock;
struct sockaddr_in server;
char message[256], server_reply[256];

void handle_data(){
    while (1){
        printf("Enter message: ");
        memset(message,0,256);
        fgets(message,256,stdin);
        if(send(sock,message,strlen(message),0)<0){
            puts("Send failed");
            exit(-1);
        }
        if(recv(sock,server_reply,256,0)<0){
            puts("recv failed");
            break;
        }
        printf("Server Reply: %s\n",server_reply);
        server_reply[0] = '\0';
    }
}

void initializeClient(char *serv_addr, int serv_port){
    sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock==-1) fprintf(stderr, "Couldn't create socket\n");
    fprintf(stderr, "Client Socket created\n");
    server.sin_addr.s_addr = inet_addr(serv_addr);
    server.sin_family = AF_INET;
    server.sin_port = htons(serv_port);
    if(connect(sock,(struct sockaddr *)&server,sizeof(server))<0){
        fprintf(stderr, "connect failed. Error\n");
        exit(-1);
    }
}

int main(int argc, char **argv){
    if (argc!=3){
        fprintf(stderr, "<Client> need ip and port!\n");
        exit(-1);
    }
    char *serv_addr = argv[1];
    int serv_port = atoi(argv[2]);

    initializeClient(serv_addr, serv_port);
    fprintf(stderr, "Client Connect successfully\n");

    handle_data();

    close(sock);
    return 0;
}
