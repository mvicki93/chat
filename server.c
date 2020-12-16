/*
 * basic chat application for linux env
 * used TCP connection
 */


#include<stdio.h>
#include<string.h>    
#include<stdlib.h>    
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h>    
#include<pthread.h> 

typedef struct detail_client{
	struct detail_client *next;
	char name[10];
	int fd;
}detail;

detail *head, *temp1; 
int fd = -1;
void *connection_handler(void *);
int global_decs_table[10], i = 0; 
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc , char *argv[])
{
	int socket_desc , client_sock , c , *new_sock;
	struct sockaddr_in server , client;


	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");


	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );


	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{

		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");


	listen(socket_desc , 3);


	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);



	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		puts("Connection accepted");

		pthread_t sniffer_thread;
		new_sock = malloc(1);
		*new_sock = client_sock;
		global_decs_table[i++] = client_sock; //feature ref, now i used list for store session info.

                /* Handler thread for handle clien's */     
		if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
		{
			perror("could not create thread");
			return 1;
		}


		puts("Handler assigned");
	}

	if (client_sock < 0)
	{
		perror("accept failed");
		return 1;
	}

	return 0;
}

/*
 * connection_handler
 *
 * Handler function handle all client connection.
 */

void *connection_handler(void *socket_desc)
{

	int sock = *(int*)socket_desc;
	int read_size;

	char *message , client_message[2000];
	int sock1 = global_decs_table[1]; 
	char buffer[10],  buffer1[2000];;

	message = "Greetings! First u must register your name on server usrname size must 6 char.\nsend one usr to another usr u must know ur friend id or name";
	if (write(sock , message , strlen(message)) == -1)
            perror("write faild");

	memset(client_message, 0, sizeof(client_message)); 
	if(!(read_size = read(sock , client_message , 2000)))
            perror("read fail not store your deatails in server");
       
        #if DEBUG
        printf("client message %s fd %d\n", client_message, sock);
        #endif

	client_details(client_message, buffer);
	add_data_to_list(buffer, sock);   
	memset(client_message, 0, sizeof(client_message));
	memset(buffer1, 0, sizeof(client_message));

	while( (read_size = read(sock , client_message , 2000)) > 0 ){
		pthread_mutex_lock(&lock);
		client_details(client_message, buffer);
		printf("send to %s\n", buffer); 
		cmb_fd(buffer1, sock);
		cmb_name(buffer, buffer1, client_message, sock);
#if 0
		if(fd == -1)
			printf("connection failure\n");
		else
			printf("alive online\n");
#endif

#if DEBUG        
                printf("%s final message: fd %d\n", buffer1, fd);
#endif
		if (write(fd , buffer1 , strlen(buffer1)) == -1)
                    perror("server");
		memset(client_message, 0, sizeof(client_message));
		memset(buffer1, 0, sizeof(buffer1));
		pthread_mutex_unlock(&lock);
	}

	if(read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}

	free(socket_desc);

	return 0;
}

/*
 * add_data_to_list 
 * add client info to list (client username and socket)
 */

int add_data_to_list(char *buffr, int fd)
{
	do{
	    if(head == NULL){
	        detail *temp;
		temp = (detail *)malloc(sizeof (*temp));
		if((temp = (detail *)malloc(sizeof (*temp))) == NULL){
		    printf("malloc fail\n");
		    break;
		 }

		 memset(temp, 0, sizeof(*temp));
		 head = temp;
		 strcpy(head -> name, buffr);
		 head -> fd = fd;
		 temp1 = head;

	     }else{
	         detail *temp;
		 if((temp = (detail *)malloc(sizeof (*temp))) == NULL){
		     printf("malloc fail\n");
		     break;
		 }
		 strcpy(temp -> name, buffr);
		 printf("add list %s\n", temp -> name);
		 temp -> fd = fd;
		 temp1 -> next = temp;
		 temp1 = temp1 -> next;
	    }

	}while(0);

	return 0;
}

/*
 * client_details
 *
 * Get the first 6 digit (client name should 6 digit(current design)).
 */

int client_details(char *clnt_message, char *buffer)
{
	char *temp = clnt_message, i = 0;
	while(i < 6){
		*(buffer + i++) = *temp++;	
	}
	*(buffer + i) = '\0';
	strcpy(clnt_message, temp);
	return 0;
}

int display()
{
	printf("name %s fd %d addr %g\n", head -> name, head -> fd, head -> next);
	return 0;
}

/*
 * cmb_name
 * Get destination file descriptor from list
 */
 
int cmb_name(char *buffer, char *buffer1, char *client_message, int sock)
{
	detail *temp;
	temp = head;
	int t = 1;
#if 1
	do{
	    if(temp == NULL){
	        printf("head also null no client connect\n");
		t = 2;
		break;
	    }
	    if(temp -> next == NULL){
	        fd = temp -> fd;
		if(strcmp(temp -> name, buffer) == 0){
	            fd = temp -> fd;
		    ap_nd(buffer1,  client_message);
		    break;
		}

		memset(buffer1, 0, strlen(buffer1));
		strcpy(buffer1, "Network");
		ap_nd(buffer1,  "no one online");
		t = 3;
		break;
	     }

	     while(temp -> next != NULL){
	         if(strcmp(temp -> name, buffer) == 0){
		     fd = temp -> fd;
		     memset(buffer1, 0, strlen(buffer));
		     ap_nd(buffer1,  client_message);
		     goto end;
		 }
		 t++;
		 temp = temp -> next;
	      }
	      if(strcmp(temp -> name, buffer) == 0){
	         fd = temp -> fd; 
		 ap_nd(buffer1,  client_message);
	      } else{
	         fd = sock;
		 memset(buffer1, 0, strlen(buffer));
		 strcpy(buffer1, "Network");
		 ap_nd(buffer1,  "not connect");

	     }
        end:
	     break;
    }while(0);
#endif
	return 0;
}

/*
 * cmb_fd
 * Get username via fd
 */

int cmb_fd(char *buffer, int fd)
{
	detail *temp;
	temp = head;
	int t = 1;
#if 1
	do{
		if(temp == NULL){
			break;
		}

		if(temp -> next == NULL){
			if(fd == temp -> fd){
                        	strcpy(buffer, temp -> name);
				fd = temp -> fd;
				break;
			}
			fd = temp -> fd;
			printf("any one in online..i send back message to u\n");
			break;
		}

		while(temp -> next != NULL){
			if(fd == temp -> fd){
				strcpy(buffer, temp -> name);
				break;
			}
			t++;
			temp = temp -> next;
		}
		if(fd == temp -> fd)     
			strcpy(buffer, temp -> name);
		else
			strcpy(buffer, "Netw0rk");
	}while(0);
#endif
	return 0;

}

int ap_nd(char *dest, char *sour)
{

	char *temp = dest;
	while(*temp != '\0'){
		temp++;
	}
	*temp++ = ':';
	while(*sour != '\0'){
		*temp++ = *sour++;
	}
	*temp++ = '\0';
	return 0;
}

int delay(int loop)
{
	int i = 0;
	for(i = 0; i < loop; i++);
}						
