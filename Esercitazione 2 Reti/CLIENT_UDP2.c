/*
 ============================================================================
 Name        : CLIENT_UDP2.c
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
#define BUFF 200


int getvocali(char vett[], int n);
void ClearWinSock();


int main() {

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


	int clientSocket;
	char msg[BUFF];
	int numVocali;

	//struct per indirizzo server
	struct sockaddr_in ServAddr;
	struct sockaddr_in recvAddr;
	unsigned int fromSize;
	int recvSize;


	//variabili per la risoluzione del DNS
	struct hostent *host;
	char name[BUFF];
	int port;


	puts("Inserire nome del server");
	host = gethostbyname(gets(name));
	fflush(stdin);

	if(host == NULL)
	{
		puts("gethostbyname() fallita");

		return -1;
	}

	//puntatore di tipo in_addr per convertire l'indirizzo in notazione puntata
	struct in_addr* ina = (struct in_addr*) host->h_addr_list[0];
	printf("Risultato di gethostbyname(%s): %s\n", name, inet_ntoa(*ina));


	puts("Inserire numero di porta del server");
	scanf("%d", &port);
	fflush(stdin);

	//memorizzazione delle informazioni indirizzo server
	memset(&ServAddr, 0, sizeof(ServAddr));
	ServAddr.sin_family = PF_INET;
	ServAddr.sin_port = htons(port);
	ServAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*ina));


	//creazione della socket
	if((clientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		puts("Errore nella creazione della socket");
		return -1;
	}


	//invio della stringa iniziale
	if(sendto(clientSocket, "Ciao" , strlen("Ciao"), 0, (struct sockaddr*) &ServAddr, sizeof(ServAddr)) != strlen("Ciao"))
	{
		puts("E' stato inviato un numero di byte diversi da quelli considerati");

		return -1;
	}


	recvSize = sizeof(recvAddr);
	fromSize = recvfrom(clientSocket, msg, BUFF, 0, (struct sockaddr*) &recvAddr, &recvSize);


	if(ServAddr.sin_addr.s_addr != recvAddr.sin_addr.s_addr)
	{
		puts("Error: received a packet from unknown source.");

		return -1;
	}

	msg[fromSize] = '\0';
	printf("Received: %s\n", msg);

	puts("Scrivere una stringa da inviare al server");

	gets(msg);

	fflush(stdin);

	numVocali = getvocali(msg, strlen(msg));

	int i;

	char numVoc[BUFF];

	//converto il numero delle vocali da intero a stringa e lo invio al server
	itoa(numVocali, numVoc, 10);

	if(sendto(clientSocket, numVoc, strlen(numVoc), 0, (struct sockaddr*) &ServAddr, sizeof(ServAddr)) != strlen(numVoc))
	{
		puts("E' stato inviato un numero di byte diversi da quelli considerati");
	}

	for(i = 0; i < strlen(msg); i++)
	{
		if(sendto(clientSocket, &msg[i], strlen(&msg[i]), 0, (struct sockaddr*) &ServAddr, sizeof(ServAddr)) != strlen(&msg[i]))
		{
			puts("E' stato inviato un numero di byte diversi da quelli considerati");

		}
	}

	char recvVoc[2];

	for(i = 0; i < numVocali; i++)
	{
		recvSize = sizeof(recvAddr);
		fromSize = recvfrom(clientSocket, recvVoc, 2, 0, (struct sockaddr*) &recvAddr, &recvSize);

		if(ServAddr.sin_addr.s_addr != recvAddr.sin_addr.s_addr)
		{
			puts("Error: received a packet from unknown source.");
		}

		recvSize=strlen(recvVoc);

		recvVoc[1]='\0';

		printf("%s\n", recvVoc);

	}

	closesocket(clientSocket);
	ClearWinSock();
	system("pause");
	return 0;


}


int getvocali(char vett[], int n)
{
	int j = 0, i;
	for (i=0; i < n; i++)
	{
		if(vett[i] == 'a' || vett[i] == 'e' || vett[i] == 'i' || vett[i] == 'o' || vett[i] == 'u' ||
			vett[i] == 'A' || vett[i] == 'E' || vett[i] == 'I' || vett[i] == 'O' || vett[i] == 'U' )
		{
			vett[j] = vett[i];
			j++;
		}
	}

	vett[j] = '\0';
	return j;
}


void ClearWinSock()
{
	#if defined WIN32
	WSACleanup();
	#endif
}
