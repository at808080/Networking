#include <iostream>
#include <WS2tcpip.h> //WinSock is the framework windows uses to access networking sockets

#pragma comment (lib, "ws2_32.lib")

int main()
{
	//Initialise winsock
	WSADATA wsdata;
	WORD ver = MAKEWORD(2, 2);

	int wsok = WSAStartup(ver, &wsdata);
	if (wsok != 0)
	{
		std::cerr << "Can't init winsock - quitting" << std::endl;
		return 0;
	}


	//Create a socket to bind to
	SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listeningSocket == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket - quitting" << std::endl;
		return 0;
	}

	//Bind ip address and port to the socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listeningSocket, (sockaddr*)&hint, sizeof(hint)); //cast the address of the hint to type pointer sockaddr

	//Tell winsock that the socket is for listening
	listen(listeningSocket, SOMAXCONN); //SOMAXCONN = number of maximum connections in backlog
	
	fd_set master;
	FD_ZERO(&master);

	FD_SET(listeningSocket, &master);

	while (true)
	{
		fd_set copyOfSet = master; 
		int socketCount = select(0, &copyOfSet, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++)
		{
			//server can either accept connection from client or receive message from client
			SOCKET incomingSocket = copyOfSet.fd_array[i];
			if (incomingSocket == listeningSocket)
			{
				//accept the new connection and add it to list of connected clients
				SOCKET clientSocket = accept(listeningSocket, nullptr, nullptr);
				FD_SET(clientSocket, &master);

				std::string welcomeMessage = "You are connected to the chat\n";
				send(clientSocket, welcomeMessage.c_str(), welcomeMessage.size() + 1, 0);

			}
			else
			{
				char buffer[4096];
				ZeroMemory(buffer, 4096);
				//accept the new message and send it to all other clients  
				int bytesIn = recv(incomingSocket, buffer, 4096, 0);
				
				if (bytesIn <= 0)
				{
					closesocket(incomingSocket);
					FD_CLR(incomingSocket, &master);
				}
				else
				{
					for (int i = 0; i < master.fd_count; i++)
					{
						
						SOCKET outgoingSocket = master.fd_array[i];
						if (outgoingSocket != listeningSocket && outgoingSocket != incomingSocket)
						{
							send(outgoingSocket, buffer, bytesIn, 0);
						}
					}
				}
			}
		}
	}

	//Shut Down winsock
	WSACleanup();

	return 0;
}

/*

#include <iostream>
#include <WS2tcpip.h> //WinSock is the framework windows uses to access networking sockets

#pragma comment (lib, "ws2_32.lib")

int main()
{
	//Initialise winsock
	WSADATA wsdata;
	WORD ver = MAKEWORD(2, 2);

	int wsok = WSAStartup(ver, &wsdata);
	if (wsok != 0)
	{
		std::cerr << "Can't init winsock - quitting" << std::endl;
		return 0;
	}


	//Create a socket to bind to
	SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listeningSocket == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket - quitting" << std::endl;
		return 0;
	}

	//Bind ip address and port to the socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listeningSocket, (sockaddr*)&hint, sizeof(hint)); //cast the address of the hint to type pointer sockaddr

	//Tell winsock that the socket is for listening
	listen(listeningSocket, SOMAXCONN); //SOMAXCONN = number of maximum connections in backlog

	std::cout << "Server live and listening\n";

	//Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listeningSocket, (sockaddr*)&client, &clientSize);
	//if (clientSocket == INVALID_SOCKET)
	char host[NI_MAXHOST]; //client's remote name
	char service[NI_MAXHOST];  //service (port) the client is connected on
	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << host << " connected on port " << service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
	}

	//Close the listening socket
	closesocket(listeningSocket);

	//While loop: accept and echo message back to client
	char buffer[4096];
	while (true)
	{
		ZeroMemory(buffer, 4096);

		//wait for the client to send data
		int bytesReceived = recv(clientSocket, buffer, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Error receiving bytes from socket " << std::endl;
		}
		if (bytesReceived == 0)
		{
			std::cout << "Client disconnected " << std::endl;
		}

		//echo the message back to the client
		send(clientSocket, buffer, bytesReceived + 1, 0);
	}

	//Close socket bound to
	closesocket(clientSocket);

	//Shut Down winsock
	WSACleanup();

	return 0;
}

*/