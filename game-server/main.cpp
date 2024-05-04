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
	for (int i = 0; i < server.m_client_vector.size(); i++)
	{
		packet << server.m_client_vector[i].name << server.m_client_vector[i].position.x << server.m_client_vector[i].position.y;
	}

	while (true)
	{
		server.registerClient();
		server.sendClientRecord();

		if (server.sendData(packet) == sf::Socket::Status::Done)
		{
			packet.clear();
			packet << "DATA";
			for (int i = 0; i < server.m_client_vector.size(); i++)
			{
				packet << server.m_client_vector[i].name << server.m_client_vector[i].position.x << server.m_client_vector[i].position.y;
			}
		}


		unsigned int client_index;
		if (server.receiveData(client_index) == sf::Socket::Status::Done)
		{
			if (server.m_client_vector[client_index].registration_packet.getDataSize() > 0)
			{
				std::string s;
				if (server.m_client_vector[client_index].registration_packet >> s)
				{
					if (s == "DATA")
					{
						float x, y;
						if (server.m_client_vector[client_index].registration_packet >> x)
						{
							server.m_client_vector[client_index].position.x = x;
						}
						if (server.m_client_vector[client_index].registration_packet >> y)
						{
							server.m_client_vector[client_index].position.y = y;
						}
						server.m_client_vector[client_index].registration_packet.clear();
					}
				}
			}
		}
	}


	getchar();
	return 0;
}