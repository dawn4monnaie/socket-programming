CC=g++ -std=c++11
FLAGS=-g

all: client_pre monitor_pre aws_pre serverA_pre serverB_pre serverC_pre

client_pre: client.o 
	$(CC) $(FLAGS) -o client $^

client.o: client.cpp 
	$(CC) $(FLAGS) -c client.cpp

monitor_pre: monitor.o 
	$(CC) $(FLAGS) -o monitoroutput $^

monitor.o: monitor.cpp 
	$(CC) $(FLAGS) -c monitor.cpp

aws_pre: aws.o 
	$(CC) $(FLAGS) -o awsoutput $^

aws.o: aws.cpp 
	$(CC) $(FLAGS) -c aws.cpp

serverA_pre: serverA.o 
	$(CC) $(FLAGS) -o serverAoutput $^

serverA.o: serverA.cpp 
	$(CC) $(FLAGS) -c serverA.cpp

serverB_pre: serverB.o 
	$(CC) $(FLAGS) -o serverBoutput $^

serverB.o: serverB.cpp 
	$(CC) $(FLAGS) -c serverB.cpp

serverC_pre: serverC.o 
	$(CC) $(FLAGS) -o serverCoutput $^

serverC.o: serverC.cpp 
	$(CC) $(FLAGS) -c serverC.cpp


monitor:
	./monitoroutput

aws:
	./awsoutput

serverA:
	./serverAoutput

serverB:
	./serverBoutput

serverC:
	./serverCoutput



.PHONY: clean
clean:
	rm -f *.o client monitor aws serverA serverB serverC *output






