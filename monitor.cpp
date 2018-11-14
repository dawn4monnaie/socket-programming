
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
#include <string.h>
#include <vector>
#include <fstream>
#include <map>
#include <sstream>
#include <iostream>

using std::endl;
using std::string;
using std::fstream;
using std::ifstream;
using std::vector;
using std::map;
using std::getline;
using std::ios;
using std::cout;

#define AWSPORT "26616"   //aws TCP port # 25000+xxx
#define HOST "localhost"


void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}



int main(void){

    //set up TCP --from Beej
    int sockfd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    int yes = 1;
    char s[INET6_ADDRSTRLEN], s2[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP


    if ((rv = getaddrinfo(HOST, AWSPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("monitor: socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("monitor: connect");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "monitor: failed to connect. \n");
        exit(0);
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure
    printf("The monitor is up and running. \n");


    while(true) {

	
        char type[1];
        recv(sockfd, type, 1, 0);

        if (type[0] == '0') {
            char LINK_ID[20];
            char SIZE[20];
            char POWER[20];

            recv(sockfd, LINK_ID, 20, 0);
            recv(sockfd, SIZE, 20, 0);
            recv(sockfd, POWER, 20, 0);
            printf("The monitor received link ID=%s, size=%s, and power=%s from the AWS \n", LINK_ID, SIZE, POWER);
		    type[0] == '-1';
        } else if (type[0] == '1') {
            char LINK_ID[20];
            char transmission_delay[20];
            char propagation_delay[20];
            char end2end_delay[20];

            recv(sockfd, LINK_ID, 20, 0);
            recv(sockfd, transmission_delay, 20, 0);
            recv(sockfd, propagation_delay, 20, 0);
            recv(sockfd, end2end_delay, 20, 0);

            printf("The result for link %s: Tt = %s ms, Tp = %s ms, Delay = %s ms \n", LINK_ID, transmission_delay, propagation_delay, end2end_delay);
		    type[0] == '-1';
        } else if (type[0] == '2'){
            char link_id[20];
            recv(sockfd, link_id, 20, 0);
            printf("Found no matches for link %s \n", link_id);
		    type[0] == '-1';
        } 
	

    }

}
