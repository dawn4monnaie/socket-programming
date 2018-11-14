//
// Created by Dawn on 2018/11/10.
//


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

#include <string>
#include <iostream>
#include <string.h>
#include <vector>
#include <fstream>
#include <map>
#include <sstream>
#include <math.h>



using std::cout;
using std::endl;
using std::string;
using std::fstream;
using std::ifstream;
using std::vector;
using std::map;
using std::getline;
using std::ios;
using std::to_string;


#define MYPORT "23616"   //my port number for ServerA
#define HOST "localhost"


float transmission_delay;
float propagation_delay;
float end2end_delay;

const int TEN = 10;
const int THOUSAND = 1000;


void compute(int file_size, int signal_power, float bandwidth, float length, float velocity, float noise_power){

    float signal_watt = pow(TEN, float(signal_power) / TEN) / THOUSAND;
    float noise_watt = pow(TEN, float(noise_power) / TEN) / THOUSAND;

	velocity *= 10000000.0;

    // B * log(1 + S / N );
    float capacity = (bandwidth * THOUSAND * THOUSAND) * log2(1 + signal_watt / noise_watt); // bits per second

    transmission_delay = float(file_size) / capacity * THOUSAND;

    propagation_delay = (float(length) / velocity) * THOUSAND * THOUSAND; // millisecond

    end2end_delay = transmission_delay + propagation_delay;

}


int main(void){
    // set up UDP  -- From Beej
    int sockfd;
    int rv;
    struct addrinfo hints;  // the struct addrinfo have already been filled with relevant info
    struct addrinfo *servinfo; //point out the result
    struct sockaddr_storage their_addr;
    struct addrinfo *p;  //tempoary point
    socklen_t addr_len;
    memset(&hints, 0, sizeof hints);  // make sure the struct is empty
    hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM; // UDP dgram sockets
    hints.ai_flags = AI_PASSIVE; // use my IP
    if ((rv = getaddrinfo(HOST, MYPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 0;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("serverC: socket");
            continue;
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("serverC: bind");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "serverC: failed to bind socket\n");
        return 0;
    }

    freeaddrinfo(servinfo);
    printf( "The Server C is up and running using UDP on port %s.\n", MYPORT);


    while(1) {

        addr_len = sizeof their_addr;
        char *link_info;
        char *client_input;

        char LINK_ID[20];
        char SIZE[20];
        char SignalPOWER[20];
        char Bandwidth[20];
        char Length[20];
        char Velocity[20];
        char NoisePower[20];

        recvfrom(sockfd, LINK_ID, 20, 0, (struct sockaddr *) &their_addr, &addr_len);
        recvfrom(sockfd, SIZE, 20, 0, (struct sockaddr *) &their_addr, &addr_len);
        recvfrom(sockfd, SignalPOWER, 20, 0, (struct sockaddr *) &their_addr, &addr_len);
        recvfrom(sockfd, Bandwidth, 20, 0, (struct sockaddr *) &their_addr, &addr_len);
        recvfrom(sockfd, Length, 20, 0, (struct sockaddr *) &their_addr, &addr_len);
        recvfrom(sockfd, Velocity, 20, 0, (struct sockaddr *) &their_addr, &addr_len);
        recvfrom(sockfd, NoisePower, 20, 0, (struct sockaddr *) &their_addr, &addr_len);

        printf("The Server C received link information of link %s, file size %s, and signal power %s \n", LINK_ID, SIZE,SignalPOWER);

        //int file_size, int signal_power, float bandwidth, float length, float velocity, float noise_power
	    // cout << atof(SIZE) << " "<< atof(SignalPOWER) << " " << atof(Bandwidth) << " " << atof(Length) << " "<< atof(Velocity) << " " << atof(NoisePower) << endl;

        compute(atof(SIZE), atof(SignalPOWER), atof(Bandwidth), atof(Length), atof(Velocity), atof(NoisePower));
        printf("The server C finished the calculation for link %s. \n", LINK_ID);


	    // cout << transmission_delay << " " << propagation_delay << " " << end2end_delay << endl;
        char sendBuff[20];
            
	    memset(sendBuff, 0, 20);
	    sprintf(sendBuff, "%.2f", transmission_delay);
        sendto(sockfd, sendBuff, 20 , 0,(struct sockaddr *) &their_addr, addr_len);

	    memset(sendBuff, 0, 20);
	    sprintf(sendBuff, "%.2f", propagation_delay);
        sendto(sockfd, sendBuff, 20 , 0,(struct sockaddr *) &their_addr, addr_len);

        memset(sendBuff, 0, 20);
	    sprintf(sendBuff, "%.2f", end2end_delay);
        sendto(sockfd, sendBuff, 20 , 0,(struct sockaddr *) &their_addr, addr_len);

        printf("The Server C finished sending the output to AWS. \n");

    }
}


