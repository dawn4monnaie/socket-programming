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
#include <iterator>


using std::cout;
using std::endl;
using std::string;
using std::fstream;
using std::ifstream;
using std::vector;
using std::map;
using std::istream_iterator;
using std::istringstream;
using std::getline;
using std::ios;
using std::to_string;


#define MYPORT "21616"   //my port number for ServerA
#define HOST "localhost"

char* search(char* LINK_ID, map<string, string> link_storage){
    if(link_storage.find(LINK_ID) != link_storage.end()){
        return "1";
    } else{
        return "0";
    }
}


vector<string> split(const string &str, char delim, bool skip_empty = true) {
    istringstream iss(str);
    vector<string> elems;
    for (string item; getline(iss, item, delim); )
        if (skip_empty && item.empty())
            continue;
        else
            elems.push_back(item);
    return elems;
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
            perror("serverA: socket");
            continue;
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("serverA: bind");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "serverA: failed to bind socket\n");
        return 0;
    }
    freeaddrinfo(servinfo);


    // read database file
    string filename = "./database_a.csv";
    ifstream fin(filename.c_str());
    string line;
    map<string, string> link_storage;
    if(!fin) {
        std::cerr << "Cannot open the File" << endl;
        return false;
    }

    while(getline(fin, line)){
        vector<string> context = split(line, ',');
	string key(context[0]);
        link_storage[key] = line;
    }


    printf( "The Server A is up and running using UDP on port %s.\n", MYPORT);

    while(1){
        addr_len = sizeof their_addr;
        char LINK_ID[20];

        recvfrom(sockfd, LINK_ID, 20 , 0,(struct sockaddr *)&their_addr, &addr_len);
        printf("The Server A received input %s. \n", LINK_ID);

	    string link_id_copy(LINK_ID);
        char* Exist = search(LINK_ID, link_storage);
        printf("The server A has found %s match. \n", Exist);
	    char existBuff[20];
	    memset(existBuff, 0, 20);
	    strcat(existBuff, Exist);
        sendto(sockfd, existBuff, 20 , 0,(struct sockaddr *) &their_addr, addr_len);

	    // cout << link_id_copy << endl;
        //send back to aws

        if(Exist=="1") {
	        // cout << link_id_copy << endl;
		    // cout << "Exist -> sending details " << endl;
            const char* LinkID;
            const char* Bandwidth;
            const char* Length;
            const char* Velocity;
            const char* NoisePower;
	        // cout << link_id_copy << endl;
		    // cout << link_storage[link_id_copy] << endl;

            vector<string> elements = split(link_storage[link_id_copy], ',');

	        // cout << elements[0] << " " << elements[1] << " " << elements[2] << " " << elements[3] << " " << elements[4] << endl;
            LinkID = elements[0].c_str();
            Bandwidth = elements[1].c_str();
            Length = elements[2].c_str();
            Velocity = elements[3].c_str();
            NoisePower = elements[4].c_str();


		    // printf("%s, %s, %s, %s, %s\n", LinkID, Bandwidth, Length, Velocity, NoisePower);

            char sendBuff[20];
            
	        memset(sendBuff, 0, 20);
	        strcat(sendBuff, LinkID);
            sendto(sockfd, sendBuff, 20, 0,(struct sockaddr *) &their_addr, addr_len);

            memset(sendBuff, 0, 20);
	        strcat(sendBuff, Bandwidth);
            sendto(sockfd, sendBuff, 20, 0,(struct sockaddr *) &their_addr, addr_len);

            memset(sendBuff, 0, 20);
	        strcat(sendBuff, Length);
            sendto(sockfd, sendBuff, 20, 0,(struct sockaddr *) &their_addr, addr_len);

            memset(sendBuff, 0, 20);
	        strcat(sendBuff, Velocity);
            sendto(sockfd, sendBuff, 20, 0,(struct sockaddr *) &their_addr, addr_len);

            memset(sendBuff, 0, 20);
	        strcat(sendBuff, NoisePower);
            sendto(sockfd, sendBuff, 20, 0,(struct sockaddr *) &their_addr, addr_len);

        }
        
        printf("The Server A finished sending the output to AWS. \n");

    }


}
