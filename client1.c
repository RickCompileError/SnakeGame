#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
int main(int argc,char *argv[]){
    int sock;
    struct sockaddr_in server;
    char message[256], server_reply[256];
    sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock==-1){
        printf("Could not create socket");
    }
    puts("Socket created");
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(9034);
    if(connect(sock,(struct sockaddr *)&server,sizeof(server))<0){
        perror("connect failed. Error");
        return 1;
    }
    puts("Connected\n");
    for(;;){
        printf("Enter message: ");
        memset(message,0,256);
        fgets(message,256,stdin);
        if(send(sock,message,strlen(message),0)<0){
            puts("Send failed");
            return 1;
        }
        if(recv(sock,server_reply,256,0)<0){
            puts("recv failed");
            break;
        }
        printf("Server Reply: %s\n",server_reply);
        server_reply[0] = '\0';
    }
    close(sock);
    return 0;
}
