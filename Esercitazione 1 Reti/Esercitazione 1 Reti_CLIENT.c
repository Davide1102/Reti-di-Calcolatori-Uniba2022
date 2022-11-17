#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif


#define BUFF 512
#include <stdio.h>


void ErrorHandler(char *errorMessage);
void ClearWinSock();
void ClearArray(char* a, int j);


int main() {

	#if defined WIN32
	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("Error at WSAStartup()\n");
		return -1;
	}
	#endif
	
	int byterecv;
	int clientSocket; //crezione della socket
	//controllo errori
	clientSocket = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(clientSocket < 0 ) 
	{
		ErrorHandler("socket creation failed\n");
		closesocket(clientSocket);
		ClearWinSock();
		return -1;
	}
	
	
	struct sockaddr_in sad;
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	
	
	puts("Inserire il numero di porta di un server");
	
	short portNum;
	scanf("%hu", &portNum);
	fflush(stdin);
	sad.sin_port = htons(portNum);

	//richiesta di connessione del client al server
	if (connect(clientSocket, (struct sockaddr*) &sad, sizeof(sad)) <0) 
	{
		ErrorHandler("Failed to connect.");
		closesocket(clientSocket);
		ClearWinSock();
		return -1;
	}
	
	char msg[BUFF];
	ClearArray(msg, BUFF);
	
	//ricezione della stringa di connessione
	if(recv(clientSocket, msg, BUFF-1, 0) <=0 ) {
		
		ErrorHandler("Messaggio non ricevuto");
		closesocket(clientSocket);
		ClearWinSock();
		return -1;		
	}
	
	else {

		puts(msg);
	}
	ClearArray(msg, BUFF);	
	
	int flag_chiusura = 1; //flag che gestisce il caso di chiusura della comunicazione 1= vero, 0 = falso
	
	while(flag_chiusura) {
		
		//invio al server delle due stringhe da concatenare
		for (int i = 0; i<2; i++)
		{
			puts("Inserire una stringa da inviare al server");
			gets(msg);
			fflush(stdin);
			
			if(send(clientSocket, msg, strlen(msg), 0) != strlen(msg))
			{
				ErrorHandler("Messaggio non inviato");
				closesocket(clientSocket);
				ClearWinSock();
				return -1;					
			}
			
		ClearArray(msg, BUFF);
			
		}
		
		
		
		//ricezione stringa concatenata
		if((byterecv = recv(clientSocket, msg, BUFF-1, 0)) <=0) {
			ErrorHandler("Messaggio non inviato");
			closesocket(clientSocket);
			ClearWinSock();
			return -1;							
		}
		else {
			msg[byterecv] = '\0'; //si pone il terminatore per la stampa
			puts(msg);
		}
		
		//gestione uscita della comunicazione
		if(!strcmp("bye", msg)) {
			flag_chiusura = 0;
		}
		ClearArray(msg, BUFF);
	}

	//chiusura della socket	
	closesocket(clientSocket);
	ClearWinSock();
	
	return 0;
}


void ErrorHandler(char *errorMessage) {
	printf("%s",errorMessage);
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

