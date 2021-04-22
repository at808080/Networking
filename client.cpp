#include <iostream>
#include <WS2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

int main()
{
	std::string serverIP = "127.0.0.1";
	int serverListeningPort = 54000;

	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		std::cerr << "Can't start winsock, err #" << wsResult << std::endl;
		return 0;
	}

	//Create socket 
	//This socket is the identifier the client program will use to communicate with the network layer
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket, error #" << WSAGetLastError() << std::endl;
		WSACleanup();
	}

	//Hint structure to tell winsock what server and port to connect o
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(serverListeningPort); //host ot network short
	inet_pton(AF_INET, serverIP.c_str(), &hint.sin_addr); //pointer to string to number

	//Actually connect to the server
	int connectionResult = connect(clientSocket, (sockaddr*)&hint, sizeof(hint));
	if (connectionResult == SOCKET_ERROR)
	{
		std::cerr << "Can't connect to the sserver, error # " << WSAGetLastError() << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return 0;
	}

	//Receive data 
	char buffer[4096];
	std::string userInput = " ";
	while (userInput.size() > 0)
	{
		std::cout << "Enter text:";
		std::getline(std::cin, userInput);
		if (userInput.size() > 0)
		{
			//send the text
			int sendresult = send(clientSocket, userInput.c_str(), userInput.size() + 1, 0);
			if (sendresult != SOCKET_ERROR)
			{
				//
				ZeroMemory(buffer, 4096);
				//echo response to console
				int bytesreceived = recv(clientSocket, buffer, 4096, 0);
				if (bytesreceived > 0)
				{
					std::cout << "Server's response:" << std::string(buffer, 0, bytesreceived) << std::endl;
				}

			}
		}
		
		
	}
	closesocket(clientSocket);
	WSACleanup();

	return 0;
}