/*
 ============================================================================
 Name        : SERVER_UDP2.c
 Author      : Simona Pia
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#endif
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define BUFF 200
#define PORT 48000

void ClearWinSock();

int main()
{
	//inizializzazione della winsock.h
	#if defined WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
	if (iResult != 0)
	{
		printf ("error at WSASturtup\n");
		return EXIT_FAILURE;
	}
	#endif

	char msg[BUFF];
	int serverSocket;
	struct sockaddr_in ServAddr;
	struct sockaddr_in ClntAddr;
	int cliAddrLen;
	int recvSize;
	char upperChar[20];

	//creo la socket del server
	if((serverSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP) ) < 0)
	{
		puts("socket() fallita");
		return -1;
	}

	//costruisco l'indirizzo del server
	memset(&ServAddr, 0, sizeof(ServAddr));
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_port = htons(PORT);
	ServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//effettuo la bind della socket
	if((bind (serverSocket, (struct sockaddr *)&ServAddr, sizeof(ServAddr))) < 0)
	{
		puts("Errore nel bind()");
		return -1;
	}


	while (1)
	{
		//ricevo il messaggio iniziale dal client
		cliAddrLen = sizeof(ClntAddr);
		recvSize = recvfrom(serverSocket, msg, BUFF, 0, (struct sockaddr*) &ClntAddr, &cliAddrLen);

		msg[recvSize] = '\0';

		//ricerca del nome del client
		struct hostent *host;
		struct in_addr addr;
		addr.s_addr = inet_addr(inet_ntoa(ClntAddr.sin_addr));

		host = gethostbyaddr((char*) &addr, 4, AF_INET);
		char* canonical_name = host->h_name;

		printf("E' stato ricevuto %s dal client di nome %s e indirizzo %s\n", msg,  canonical_name,  inet_ntoa(ClntAddr.sin_addr) );

		//invio la stringa al client
		if(sendto(serverSocket, "OK", strlen("OK"), 0, (struct sockaddr*) &ClntAddr, sizeof(ClntAddr)) != strlen("OK"))
		{
			puts("Sono stati inviati byte diversi da quelli intenzionati ad inviare");
		}

		int N, i;

		//ricevo il numero delle vocali
		cliAddrLen = sizeof(ClntAddr);
		recvSize = recvfrom(serverSocket, msg, BUFF, 0, (struct sockaddr*) &ClntAddr, &cliAddrLen);

		//conversione da stringa a intero
		N=atoi(msg);

		for (i=0; i<N; i++)
		{
			//ricevo e invio le vocali in maiuscolo al client
			cliAddrLen = sizeof(ClntAddr);
			recvSize = recvfrom(serverSocket, upperChar, strlen(upperChar), 0, (struct sockaddr*) &ClntAddr, &cliAddrLen);

			upperChar[0] = toupper(upperChar[0]);
			upperChar[1]='\0';

			if(sendto(serverSocket, upperChar, strlen(upperChar), 0, (struct sockaddr*) &ClntAddr, sizeof(ClntAddr)) != strlen(upperChar))
			{
				puts("Sono stati inviati byte diversi da quelli intenzionati ad inviare");
			}

		}

	}


}



void ClearWinSock()
{
	#if defined WIN32
	WSACleanup();
	#endif
}

