#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define MYPORT "3491"
#define BACKLOG 10

int connection = 1;

struct thread_info {
	int socket1;
	int socket2;
};

void* connection1(void* arg) {
	struct thread_info* data = (struct thread_info*)arg;
	int i, o;
	char msg[141];
	char snd[148];
	memset(msg, '\0', sizeof(msg));
	memset(snd, '\0', sizeof(snd));
	while (connection) {
		if (recv(data->socket1, msg, 141, 0) <= 0) {
			connection = 0;
		}
		else {
			snd[0] = 'U';
			snd[1] = 's';
			snd[2] = 'e';
			snd[3] = 'r';
			snd[4] = '1';
			snd[5] = ':';
			snd[6] = ' ';
			i = 7;
			o = 0;
			while (msg[o] != '\0')
				snd[i++] = msg[o++];
			send(data->socket1, snd, strlen(snd), 0);
			send(data->socket2, snd, strlen(snd), 0);
			memset(msg, '\0', sizeof(msg));
			memset(snd, '\0', sizeof(snd));
		}
	}
    
    pthread_exit(NULL);
}

void* connection2(void* arg) {
	struct thread_info* data = (struct thread_info*)arg;
	int i, o;
	char msg[141];
	char snd[148];
	memset(msg, '\0', sizeof(msg));
	memset(snd, '\0', sizeof(snd));
	while (connection) {
		if (recv(data->socket2, msg, 141, 0) <= 0) {
			connection = 0;
		}
		else {
			snd[0] = 'U';
			snd[1] = 's';
			snd[2] = 'e';
			snd[3] = 'r';
			snd[4] = '2';
			snd[5] = ':';
			snd[6] = ' ';
			i = 7;
			o = 0;
			while (msg[o] != '\0')
				snd[i++] = msg[o++];
			send(data->socket2, snd, strlen(snd), 0);
			send(data->socket1, snd, strlen(snd), 0);
			memset(msg, '\0', sizeof(msg));
			memset(snd, '\0', sizeof(snd));
		}
	}
    
    pthread_exit(NULL);
}


int main(int argc, char *argv[]) {

	struct sockaddr_storage user_address1, user_address2;
    socklen_t address_size1, address_size2;
    struct addrinfo hints, *res;
    int mysock, guest_sock1, guest_sock2;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, MYPORT, &hints, &res) != 0) {
        printf("some error idk");
        return 1;
    }

	if ((mysock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		printf("socket error");
		return 2;
	}
    
    if (bind(mysock, res->ai_addr, res->ai_addrlen) == -1) {
		printf("bind error");
		return 3;
	}

    if (listen(mysock, BACKLOG) == -1) {
		printf("listen error");
		return 4;
	}


    address_size1 = sizeof user_address1;
    guest_sock1 = accept(mysock, (struct sockaddr *)&user_address1, &address_size1);
	if (guest_sock1 == -1) {
		printf("can't make socket1");
		return 5;
		}

    address_size2 = sizeof user_address2;
    guest_sock2 = accept(mysock, (struct sockaddr *)&user_address2, &address_size2);
	if (guest_sock2 == -1) {
		printf("can't make socket2");
		return 5;
		}



	struct thread_info data1;
	data1.socket1 = guest_sock1;
	data1.socket2 = guest_sock2;



	struct thread_info data2;
	data2.socket1 = guest_sock1;
	data2.socket2 = guest_sock2;




	pthread_t thread1, thread2;



	pthread_create(&thread1, NULL, connection1, (void*)&data1);
	pthread_create(&thread2, NULL, connection2, (void*)&data2);



	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);




	///////
	
	/////////////


		freeaddrinfo(res);
		close(mysock);
		close(guest_sock1);
		close(guest_sock2);
		close(data1.socket1);
		close(data1.socket2);
		close(data2.socket1);
		close(data2.socket2);

//////////////////////////////

    

	///////////////////////////////

	return 0;
}


// For one thing, you can’t use close() to close a socket—you need to use closesocket(),
// instead. Also, select() only works with socket descriptors, not file descriptors (like 0 for stdin).

// gcc nya.c -o nya -lws2_32 -pthread