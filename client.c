/*
 * very simple client function
 * if you want to saw your friend message you have to give r or R
 */

/*
 * Need to create sperate port for recive message(feature)
 */


#include<stdio.h> 
#include<string.h>   
#include<sys/socket.h>    
#include<arpa/inet.h> 
#include <fcntl.h> 

#define SERVERIP "172.17.3.5"

int main(int argc , char *argv[])
{
	int sock, last_fd;
	struct sockaddr_in server;
	char message[1000] , server_reply[2000];


	sock = socket(AF_INET , SOCK_STREAM , 0);
	//last_fd = sock;
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");

	server.sin_addr.s_addr = inet_addr(SERVERIP); //server address
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );


	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	puts("Connected\n");
	int n;

	if (read(sock , server_reply , 2000) == -1) {
            perror("read fail");
            return 1;
        }

	printf("%s\n", server_reply);
	//fcntl(last_fd, F_SETFL, O_NONBLOCK);
	fcntl(sock, F_SETFL, O_NONBLOCK);

	while(1)
	{
		printf("Enter message : ");
		fgets(message, 50, stdin);

		if(!((strcmp(message, "r\n") == 0) || (strcmp(message, "R\n") == 0)))        
			if( write(sock , message , strlen(message)) < 0){
				puts("Send failed");
				return 1;
			}
		if((strcmp(message, "r\n") == 0) || (strcmp(message, "R\n") == 0)) {
			memset(server_reply, 0, sizeof(server_reply));
			while(1){      
				if( (n = read(sock , server_reply , 2000)) < 0)
				{
					puts("recv failed");
					break;
				}
				break;
			}
			printf("%s\n", server_reply);
		}
		memset(message, 0, sizeof(message));
	}

	close(sock);
	return 0;
}
