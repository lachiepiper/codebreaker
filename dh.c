#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "sha256.h"


#define G 15
#define P 97
/*
Disclaimer: Much of this code was reproduced from beeg's guide to socket
programming. http://beej.us/guide/bgnet/html/multi/index.html
Some was also taken from https://app.lms.unimelb.edu.au/bbcswebdav/pid-6960267-dt-content-rid-59698248_2/courses/COMP30023_2019_SM1/client-1.2.c
as well as https://www.geeksforgeeks.org/implementation-diffie-hellman-algorithm/?fbclid=IwAR0LHyWCcw4fvq4oZy8cnOHzqXqd7dvDqQJetoGsfwQEaC3ZecRVoLknJAM
*/

int setup_client_socket(const int port, const char* server_name,
						struct sockaddr_in* serv_addr);

int modulo(int a, int b, int n);

int main(int argc, char const *argv[]) {

	char* server;
	int port = 7800;
	int sockfd;
	unsigned char b;
	int i = 0;
	int j = 0;
    int n;
	int gbmodp = 0;
	char buffer[2048];
	char gbmodp_str[4];
    memset(buffer, 0, 2048);
	FILE* fp = fopen("DiffieHellman.c", "r");
	char c;
	BYTE file_str[1000000];
	unsigned char* hash;
	BYTE * hash_byte;

    char * first_input = "piperl\n";
	server = "172.26.37.44";

	// while((c = fgetc(fp))!= EOF){
	// 	file_str[i] = c;
	// 	i++;
	// }

	BYTE real_str[i];
	for(j = 0; j<i; j++){
		real_str[j] = file_str[j];
	}
	i=0;
	j=0;

	//please lord let this work
	hash = sha256_process(real_str);
	b = atoi(argv[1]);

	int num = 1;
	int a = G;

	gbmodp = modulo(G,b,P);



	//Power function to return value of a ^ b mod P
	//gbmodp = modulo(G, b, P);


	sprintf(gbmodp_str, "%d\n", gbmodp);

     struct sockaddr_in serv_addr;
     bzero(&serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = inet_addr("172.26.37.44");
     serv_addr.sin_port = htons(atoi("7800"));

     int status;
     struct addrinfo hints;
     struct addrinfo *servinfo;  // will point to the results
     memset(&hints, 0, sizeof(hints));
     hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
     hints.ai_socktype = SOCK_STREAM; // TCP stream sockets


     if ((status = getaddrinfo("172.26.37.44", "7800", &hints, &servinfo)) != 0) {
         //fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
         exit(1);
     }

    sockfd = socket(servinfo->ai_family,  servinfo->ai_socktype, servinfo->ai_protocol);


    if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) < 0){
        printf("connect failed\n");
        exit(1);
    }


    n = write(sockfd, first_input, strlen(first_input));
    printf("first input: %s\n",first_input );
    if (n < 0) {
		perror("write");
		exit(EXIT_FAILURE);
	}

	char out_string[3];
	out_string[0] = gbmodp_str[0];
	out_string[1] = gbmodp_str[1];
	out_string[2] = '\0';

    n = write(sockfd, gbmodp_str, strlen(gbmodp_str));
    if (n < 0) {
		perror("write");
		exit(EXIT_FAILURE);
	}

    n = read(sockfd, buffer, 2048);
    if (n < 0) {
		perror("read");
		exit(EXIT_FAILURE);
	}

	int buf_int = atoi(buffer);

	int gbmodpa = modulo(buf_int, b, P);

	char gbmodpa_str[4];
	sprintf(gbmodpa_str, "%d\n", gbmodpa);

	n = write(sockfd, gbmodpa_str, strlen(gbmodpa_str));
	n = read(sockfd, buffer, 2048);
    if (n < 0) {
		perror("read");
		exit(EXIT_FAILURE);
	}

	/* Close to let server know that we've finished sending our message */
	close(sockfd);


    return 0;
}

/* Create and return a socket bound to the given port and server */
int setup_client_socket(const int port, const char* server_name,
						struct sockaddr_in* serv_addr) {
	int sockfd;
	struct hostent* server;

	server = gethostbyname(server_name);
	if (!server) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(EXIT_FAILURE);
	}
	bzero((char*)serv_addr, sizeof(serv_addr));
	serv_addr->sin_family = AF_INET;
	bcopy(server->h_addr_list[0], (char*)&serv_addr->sin_addr.s_addr,
		  server->h_length);
	serv_addr->sin_port = htons(port);

	/* Create socket */
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	return sockfd;
}

//Power function to return value of a ^ b mod P
int modulo(int a, int b, int n){
    long long x=1, y=a;
    while (b > 0) {
        if (b%2 == 1) {
            x = (x*y) % n; // multiplying with base
        }
        y = (y*y) % n; // squaring the base
        b /= 2;
    }
    return x % n;
}

/*
condenses sha256 functionality we were given
*/
unsigned char* sha256_process(BYTE* text){
    int i = 0;

    BYTE* buffer = (BYTE*) malloc(SHA256_BLOCK_SIZE);
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, text, sizeof(text)/sizeof(char));
    sha256_final(&ctx, buffer);
    return buffer;
}
