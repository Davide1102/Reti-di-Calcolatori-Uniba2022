#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif


#include <stdio.h>
#include <stdlib.h> // for atoi()
#include <string.h>
#define PROTOPORT 5193 // default protocol port number
#define QLEN 2 // size of request queue
#define BUFF 512
void ErrorHandler(char *errorMessage);

void ClearArray(char* a, int j);


void ClearWinSock();

int main(){

	#if defined WIN32
	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		ErrorHandler("Error at WSAStartup()\n");
		return -1;
	}
	#endif
	
	int serverSocket;
	char msgConn[] = "Connessione avvenuta";
	

	
	
	serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //Creazione della socket di benvenuto e verifica errori

	if(serverSocket < 0 ) {		
	ErrorHandler("socket creation failed\n");
	closesocket(serverSocket);
	ClearWinSock();
	return -1;
	}
	
	struct sockaddr_in sad;
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	sad.sin_port = htons( 5193 );
	//assegnazione dell'indirizzo ip alla socket
	if (bind(serverSocket, (struct sockaddr*) &sad,sizeof(sad)) <0) {
		ErrorHandler("bind() failed.\n");
		closesocket(serverSocket);
		ClearWinSock();
		return -1;
	}
	
	//impostazione del server alla modalità di ascolto
	if (listen (serverSocket, QLEN) < 0) {
		ErrorHandler("listen() failed.\n");
		closesocket(serverSocket);
		ClearWinSock();
		return -1;
	}

	struct sockaddr_in cad; //definizione della struct per l'indirizzo client
	int clientSocket; //descrtittore socket dedicata
	int clientLen; //dimensione indirizzo client
	printf( "Waiting for a client to connect...");
	
	while(1){
		
		clientLen = sizeof(cad); //si ricava la dimensione dell'indirizzo client
		if ( (clientSocket=accept(serverSocket, (struct sockaddr *)&cad, &clientLen)) < 0) {
			ErrorHandler("accept() failed.\n");
			closesocket(serverSocket);
			ClearWinSock();
			return -1;
		}
		else 
		{
			printf("Connessione stabilita con il client: %s", inet_ntoa(cad.sin_addr));
		}
		
		//invio stringa di conferma della connessione
		if(send(clientSocket, msgConn, strlen(msgConn), 0) != strlen(msgConn)) {
			ErrorHandler("Messaggio non inviato.\n");
			closesocket(serverSocket);
			ClearWinSock();
			return -1;			
		}
		
		int flag_chiusura = 1; //flag che gestisce il caso di chiusura della comunicazione 1= vero, 0= falso

		//gestione della concatenazione delle stringhe ricevute dal client
		while (flag_chiusura) {
			
			char msg1[BUFF];
			char msg2[BUFF];
			
			ClearArray(msg1, BUFF);
			ClearArray(msg2, BUFF);
			
			if(recv(clientSocket, msg1, BUFF-1, 0) <= 0) {
			ErrorHandler("Messaggio non ricevuto.\n");
			closesocket(serverSocket);
			ClearWinSock();
			return -1;					
			}
			
			if(recv(clientSocket, msg2, BUFF-1, 0) <= 0) {
			ErrorHandler("Messaggio non ricevuto.\n");
			closesocket(serverSocket);
			ClearWinSock();
			return -1;					
			}	
			//controllo sulla parola di uscita del client
			if(!strcmp("quit", msg1) || !strcmp("quit" ,msg2)) {
				
				flag_chiusura = 0;
				
				if(send(clientSocket, "bye", strlen("bye"), 0) != strlen("bye")) {
					ErrorHandler("Messaggio non inviato.\n");
					closesocket(serverSocket);
					ClearWinSock();
					return -1;			
				}				
				
			}
			
			else
			{
				strcat(msg1, msg2);	//concatenazione delle stringhe ricevute
				
				msg1[strlen(msg1)] = '\0';
				
				if(send(clientSocket, msg1, strlen(msg1), 0) != strlen(msg1)) {
					ErrorHandler("Messaggio non inviato.\n");
					closesocket(serverSocket);
					ClearWinSock();
					return -1;						
				}				
			}
			
			ClearArray(msg1, BUFF);
			ClearArray(msg2, BUFF);			
		}		
	}
	
	
	system("pause");
	return 0;	
}


void ErrorHandler(char *errorMessage) {
printf ("%s", errorMessage);
}


void ClearWinSock() {
#if defined WIN32
WSACleanup();
#endif
}

//funzione per pulire l'array
void ClearArray(char* a, int j) {
	
	for(int i = 0; i < j; i++) {
		
		a[i] = 0;
	}
}
