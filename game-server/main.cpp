#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>

#include "network_server.h"

NetworkServer server;


int main()
{
	server.init();


	sf::Packet packet;
	packet << "DATA";
	for (int i = 0; i < server.getClientVector().size(); i++)
	{
		packet << server.getClientVector()[i].name << server.getClientVector()[i].position.x << server.getClientVector()[i].position.y;
	}

	while (true)
	{
		server.registerClient();
		server.sendClientRecord();

		if (server.sendData(packet) == sf::Socket::Status::Done)
		{
			packet.clear();
			packet << "DATA";
			for (int i = 0; i < server.getClientVector().size(); i++)
			{
				packet << server.getClientVector()[i].name << server.getClientVector()[i].position.x << server.getClientVector()[i].position.y;
			}
		}


		unsigned int client_index;
		if (server.receiveData(client_index) == sf::Socket::Status::Done)
		{
			if (server.getClientVector()[client_index].registration_packet.getDataSize() > 0)
			{
				std::string s;
				if (server.getClientVector()[client_index].registration_packet >> s)
				{
					if (s == "DATA")
					{
						float x, y;
						if (server.getClientVector()[client_index].registration_packet >> x)
						{
							server.getClientVector()[client_index].position.x = x;
						}
						if (server.getClientVector()[client_index].registration_packet >> y)
						{
							server.getClientVector()[client_index].position.y = y;
						}
						server.getClientVector()[client_index].registration_packet.clear();
					}
				}
			}
		}
	}


	getchar();
	return 0;
}