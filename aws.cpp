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
#include <signal.h>

#include <iostream>
#include <string.h>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

#define TCPPORT_MONITOR "26616"   //TCP port
#define TCPPORT_CLIENT "25616"   //TCP port
#define UDPPORT "24616"		//UDP port
#define HOST "localhost"
#define PORTA "21616"
#define PORTB "22616"
#define PORTC "23616"
#define BACKLOG 10

string link_info;
string delay_string;

void sigchld_handler(int s) {
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}




void connectLookupServer(char* LINK_ID, char ch, vector<string> &res) {
    char* backserver_port;
    if(ch == 'A')
        backserver_port = PORTA;
    else if (ch == 'B')
        backserver_port = PORTB;
	else
		cout << "port type error" <<endl;

	// cout << ch << " " << backserver_port << endl;
    int mysock=0;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    //set up UDP -- from Beej
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;  // set the type to UDP

    if ((rv = getaddrinfo(HOST, backserver_port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((mysock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "talker: failed to bind socket\n");
        exit(2);
    }

    sendto(mysock, LINK_ID, 20, 0, p->ai_addr,p->ai_addrlen);
    printf("The AWS sent link ID=%s to Backend-Server %c using UDP over port %s. \n", LINK_ID, ch, backserver_port);


    char Exist[20];
    recvfrom(mysock, Exist, 20, 0 , NULL, NULL);
    printf("The AWS received %c matches from Backend-Server %c using UDP over port %s. \n", Exist[0], ch, backserver_port);
    
    res.push_back(Exist);
    // cout << ch << " Exist: "<< Exist << endl;
    if (Exist[0] == '1'){
		// cout << "begin to receive the details from server  " << ch << endl;

        char LinkID[20];
        char Bandwidth[20];
        char Length[20];
        char Velocity[20];
        char NoisePower[20];
		// cout << ch << " Debug 0" << endl;
        recvfrom(mysock, LinkID, 20, 0,NULL, NULL);
		// cout << ch << " Debug 1" << endl;
        recvfrom(mysock, Bandwidth, 20, 0, NULL, NULL);
		// cout << ch << " Debug 2" << endl;
        recvfrom(mysock, Length, 20, 0, NULL, NULL);
		// cout << ch << " Debug 3" << endl;
        recvfrom(mysock, Velocity, 20, 0, NULL, NULL);
		// cout << ch << " Debug 4" << endl;
        recvfrom(mysock, NoisePower, 20, 0, NULL, NULL);
		// cout << ch << " Debug 5" << endl;
	    // cout << "In " << ch << ":"<< LinkID << " " << Bandwidth<<" "<< Length<< " "<<Velocity << " "<<NoisePower << endl;

        res.push_back(LinkID);
        res.push_back(Bandwidth);
        res.push_back(Length);
        res.push_back(Velocity);
        res.push_back(NoisePower);
		// cout << "received finished" << endl;
	    // cout <<  "In " << ch << "(push_back):" << res[1] << " " << res[2] << " " << res[3] << " "<< res[4] << " "<< res[5] << endl;
    }

   
}



void connectComputingServer(const char* LINK_ID, const char* SIZE, const char* SignalPOWER, const char* Bandwidth, const char* Length, const char* Velocity, const char* NoisePower, vector<string> &res) {
    char* backserver_port;
    backserver_port = PORTC;
    int mysock=0;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    if ((rv = getaddrinfo(HOST, backserver_port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((mysock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "talker: failed to bind socket\n");
        exit(2);
    }

    char sendBuff[20];
            
	// cout << "send to C: "<<LINK_ID<<" "<< SIZE<< " "<<SignalPOWER<< " "<<Bandwidth<< " " <<Length<< " " << Velocity<< " " << NoisePower << endl;
	memset(sendBuff, 0, 20);
	strcat(sendBuff, LINK_ID);
    sendto(mysock, sendBuff, 20, 0, p->ai_addr,p->ai_addrlen);
    
	memset(sendBuff, 0, 20);
	strcat(sendBuff, SIZE);
    sendto(mysock, sendBuff, 20, 0, p->ai_addr,p->ai_addrlen);
    
    memset(sendBuff, 0, 20);
	strcat(sendBuff, SignalPOWER);
    sendto(mysock, sendBuff, 20, 0, p->ai_addr,p->ai_addrlen);

    memset(sendBuff, 0, 20);
	strcat(sendBuff, Bandwidth);
    sendto(mysock, sendBuff, 20, 0, p->ai_addr,p->ai_addrlen);

    memset(sendBuff, 0, 20);
	strcat(sendBuff, Length);
    sendto(mysock, sendBuff, 20, 0, p->ai_addr,p->ai_addrlen);
    
    memset(sendBuff, 0, 20);
	strcat(sendBuff, Velocity);
    sendto(mysock, sendBuff, 20, 0, p->ai_addr,p->ai_addrlen);
    
    memset(sendBuff, 0, 20);
	strcat(sendBuff, NoisePower);
    sendto(mysock, sendBuff, 20, 0, p->ai_addr,p->ai_addrlen);
    
    printf("The AWS sent link ID=%s, size=%s, power=%s, and "
           "link information to Backend-Server C using UDP over "
           "port %s \n", LINK_ID, SIZE, SignalPOWER, backserver_port);

    char transmission_delay[20];
    char propagation_delay[20];
    char end2end_delay[20];
    recvfrom(mysock, transmission_delay, 20, 0, NULL, NULL);
    recvfrom(mysock, propagation_delay, 20, 0, NULL, NULL);
    recvfrom(mysock, end2end_delay, 20, 0, NULL, NULL);
    printf("The AWS received outputs from Backend-Server C using UDP over port %s. \n", backserver_port);

	// cout << "In function transmission_delay:" << transmission_delay << " "<< propagation_delay << "  "<<end2end_delay << endl;

    res.push_back(transmission_delay);
    res.push_back(propagation_delay);
    res.push_back(end2end_delay);


}



int main() {

	    

    //set up TCP --from Beej
    int sockfd, sockfd2, new_fd, new_fd2;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, hints2, *servinfo, *servinfo2, *p, *p2;
    struct sockaddr_storage their_addr, their_addr2; // connector's address information
    socklen_t sin_size, sin_size2;
    int yes = 1, yes2 = 1;
    char s[INET6_ADDRSTRLEN], s2[INET6_ADDRSTRLEN];
    int rv, rv2;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    memset(&hints2, 0, sizeof hints2);
    hints2.ai_family = AF_UNSPEC;
    hints2.ai_socktype = SOCK_STREAM;
    hints2.ai_flags = AI_PASSIVE; // use my IP


    if ((rv = getaddrinfo(HOST, TCPPORT_CLIENT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }
    freeaddrinfo(servinfo); // all done with this structure


    if ((rv2 = getaddrinfo(HOST, TCPPORT_MONITOR, &hints2, &servinfo2)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv2));
        return 1;
    }
    for (p2 = servinfo2; p2 != NULL; p2 = p2->ai_next) {
        if ((sockfd2 = socket(p2->ai_family, p2->ai_socktype, p2->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd2, SOL_SOCKET, SO_REUSEADDR, &yes2, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd2, p2->ai_addr, p2->ai_addrlen) == -1) {
            close(sockfd2);
            perror("server: bind");
            continue;
        }
        break;
    }
    if (p2 == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }
    freeaddrinfo(servinfo2); // all done with this structure




    //listen
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    if (listen(sockfd2, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

	printf( "The AWS is up and running. \n");


    // Monitor
    sin_size2 = sizeof their_addr2;
    new_fd2 = accept(sockfd2, (struct sockaddr *) &their_addr2, &sin_size2);
    if (new_fd2 == -1) {
        perror("accept");
        exit(1);
    }

    inet_ntop(their_addr2.ss_family, get_in_addr((struct sockaddr *) &their_addr2), s2, sizeof s2);
    struct sockaddr_in addrTheir2;
    memset(&addrTheir2, 0, sizeof(addrTheir2));
    int len2 = sizeof(addrTheir2);
    getpeername(new_fd2, (struct sockaddr *) &addrTheir2, (socklen_t *) &len2);
    int monitor_port = addrTheir2.sin_port;





    while(true) {
        // Client
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            exit(1);
        }
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *) &their_addr), s, sizeof s);
        struct sockaddr_in addrTheir;
        memset(&addrTheir, 0, sizeof(addrTheir));
        int len = sizeof(addrTheir);
        getpeername(new_fd, (struct sockaddr *) &addrTheir, (socklen_t *) &len);
        int client_port = addrTheir.sin_port;

	    char LINK_ID[20], SIZE[20], POWER[20];
        recv(new_fd, LINK_ID, 20, 0);
        recv(new_fd, SIZE, 20, 0);
        recv(new_fd, (char *)&POWER, 20, 0);
        printf("The AWS received  link ID=%s, size=%s, and power=%s from the client using TCP over port %d \n", LINK_ID, SIZE, POWER, client_port);


        char flag[1];
	    memset(flag, 0, 1);
	    flag[0] = '0';
        send(new_fd2, flag, 1, 0);

	    char sendBuff[20];
	    memset(sendBuff, 0, 20);
	    strcat(sendBuff, LINK_ID);
	    // puts(sendBuff);
        send(new_fd2, sendBuff, 20, 0);

	    memset(sendBuff, 0, 20);
	    strcat(sendBuff, SIZE);
	    // puts(sendBuff);
        send(new_fd2, sendBuff, 20, 0);

	    memset(sendBuff, 0, 20);
	    strcat(sendBuff, POWER);
	    // puts(sendBuff);
        send(new_fd2, POWER, 20, 0);

        printf("The AWS sent link ID=%s, size=%s, and power=%s to the monitor using TCP over port %d \n", LINK_ID, SIZE, POWER, monitor_port);

	    // cout << "Begin to lookup serverA/B" << endl;

        vector<string> link_info;

        vector<string> resA, resB;
        connectLookupServer(LINK_ID, 'A', resA);
        connectLookupServer(LINK_ID, 'B', resB);

        if (resA[0] == "1"){
		    // cout << "received the details from A" << endl;
            link_info = resA;
        } else if (resB[0] == "1"){
		    // cout << "received the details from B" << endl;
            link_info = resB;
        } else {
		    // cout << "no details received" << endl;
	    }

        // printf("The AWS received %s matches from Backend-Server A using UDP over port %s \n", resA[0], PORTA);
        // printf("The AWS received %s matches from Backend-Server B using UDP over port %s \n", resB[0], PORTB);

        if (resA[0] == "1" || resB[0] == "1") {
		
            // cout << "before connectComputerServer: " << link_info[2] << " " << link_info[3] << " " << link_info[4] << " " << link_info[5] << endl;

            vector<string> resC;
            connectComputingServer(LINK_ID, SIZE, POWER, link_info[2].c_str(), link_info[3].c_str(), link_info[4].c_str(), link_info[5].c_str(), resC);
            memset(flag, 0, 1);
            flag[0] = '1';
            send(new_fd, flag, 1, 0);

            // cout << "Debug sent information to C" << endl;
            // cout << resC[0] << " " << resC[1] << " " << resC[2] << endl;
                
            char sendBuff[20];
                
            memset(sendBuff, 0, 20);
            strcat(sendBuff, resC[2].c_str());
            send(new_fd, sendBuff, 20, 0);
            //“The AWS sent delay=<Delay>ms to the client using TCP over port <port number>”
            printf("The AWS sent delay=%s ms to the client using TCP over port %d \n", resC[2].c_str(), client_port);

            memset(flag, 0, 1);
            flag[0] = '1';
            send(new_fd2, flag, 1, 0);


            memset(sendBuff, 0, 20);
            strcat(sendBuff, LINK_ID);
            send(new_fd2, sendBuff, 20, 0);

            memset(sendBuff, 0, 20);
            strcat(sendBuff, resC[0].c_str());
            send(new_fd2, sendBuff, 20, 0);

            memset(sendBuff, 0, 20);
            strcat(sendBuff, resC[1].c_str());
            send(new_fd2, sendBuff, 20, 0);
            
            memset(sendBuff, 0, 20);
            strcat(sendBuff, resC[2].c_str());
            send(new_fd2, sendBuff, 20, 0);

            printf("The AWS sent detailed results to the monitor using TCP over port %d. \n", monitor_port);

        } else {
            flag[0] = '2';
            send(new_fd, flag, 1, 0);

            send(new_fd2, flag, 1, 0);
            char sendBuff[20];
	        memset(sendBuff, 0, 20);
	        strcat(sendBuff, LINK_ID);
            send(new_fd2, sendBuff, 20, 0);
            printf("The AWS sent No Match to the monitor and the client using TCP over ports %d and %d, respectively \n", client_port, monitor_port);

        }

        close(new_fd);
        // close(new_fd2);

    }


}



