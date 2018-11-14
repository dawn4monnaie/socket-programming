
/*
 * for client:
 *  send: LINK_ID, SIZE, POWER
 *  recv: end2end_delay
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>

#include <string>
#include <iostream>
#include <string.h>
#include <vector>
#include <fstream>
#include <map>
#include <sstream>


using std::cout;
using std::endl;
using std::string;
using std::fstream;
using std::ifstream;
using std::vector;
using std::map;
using std::getline;
using std::ios;

#define AWSPORT "25616"   //aws TCP port # 25000+xxx
#define HOST "localhost"


void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

int main(int argc, char* argv[]){

    if (argc != 4){
        // cout<<"Parameter format should be <LINK_ID> <SIZE> <POWER>"<<endl;
        system("PAUSE");
        return 1;
    }

    //set up TCP --from Beej
    int sockfd = 0;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo(HOST, AWSPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect. \n");
        exit(0);
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure
    printf("The client is up and running. \n");


	string LINK_ID2=argv[1];
    string SIZE2=argv[2];
    string POWER2=argv[3];

    const char* LINK_ID=LINK_ID2.c_str();
    const char* SIZE=SIZE2.c_str();
    const char* POWER=POWER2.c_str();

	// printf("LINK ID:%s\n", LINK_ID);
	// printf("SIZE:%s\n", SIZE);	
	// printf("POWER:%s\n", POWER);

	int length1 = LINK_ID2.length();
	int length2 = SIZE2.length();	
	int length3 = POWER2.length();

	char sendBuff[20];
	memset(sendBuff, 0, 20);
	strcat(sendBuff, LINK_ID);
    send(sockfd, sendBuff, 20, 0);

	memset(sendBuff, 0, 20);
	strcat(sendBuff, SIZE);
    send(sockfd, sendBuff, 20, 0);

	memset(sendBuff, 0, 20);
	strcat(sendBuff, POWER);
    send(sockfd, sendBuff, 20, 0);
    printf("The client has sent the  ID= %s, size=%s, and power=%s to AWS.\n", LINK_ID, SIZE, POWER);


    // cout << "Begin to Receving" <<endl;
    
    char flag[1];

    recv(sockfd, flag, 1, 0);

    if (flag[0] == '1'){
        char end2end_delay[20];
        recv(sockfd, end2end_delay, 20, 0);
        printf("The delay for link %s is %s ms\n", LINK_ID, end2end_delay);
    } else if (flag[0] == '2'){
        printf("Found no matches for link %s \n", LINK_ID);
    } else {
        printf("Debug \n");
    }

    return 0;

}
