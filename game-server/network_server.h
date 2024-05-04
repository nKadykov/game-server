//#pragma once
//#include <SFML/Network.hpp>
//#include <vector>
//#include <iostream>
//
//struct Client {
//	std::string name;
//	sf::IpAddress ip;
//	sf::UdpSocket* p_data_socket;
//	unsigned short port;
//	sf::Packet register_packet;
//	sf::Packet send_packet;
//	bool done = true;
//	sf::Vector2f pos{ 5, 5 };
//};
//
//class NetworkServer
//{
//private:
//	short m_registration_step = 0;
//
//	sf::TcpListener m_listener;
//	sf::TcpSocket m_registration_socket;
//
//	sf::Packet m_packet;
//	
//	sf::Clock m_send_time;
//	sf::Int32 m_send_rate = 4;
//
//	std::vector<Client> m_client_vector;
//
//	sf::Socket::Status acceptConnection();
//	sf::Socket::Status receiveClientData();
//	sf::Socket::Status sendClientData();
//	sf::Socket::Status sendDataPort();
//
//public:
//	NetworkServer();
//	sf::Socket::Status init();
//	sf::Socket::Status registerClient();
//	sf::Socket::Status sendClientRecord();
//	sf::Socket::Status receiveData(unsigned int&);
//	sf::Socket::Status sendData(const sf::Packet&);
//	std::vector<Client> getClientVector() const;
//};

#pragma once
#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <iostream>

struct Client
{
	std::string name;
	sf::IpAddress ip;
	sf::UdpSocket* p_data_socket;
	unsigned short port;
	sf::Packet registration_packet;
	sf::Packet send_packet;
	bool done = true;
	sf::Vector2f position { 5,5 };
};

class NetworkServer
{
public:
	NetworkServer();
	sf::Socket::Status init();
	sf::Socket::Status registerClient();
	sf::Socket::Status sendClientRecord();
	sf::Socket::Status receiveData(unsigned int&);
	sf::Socket::Status sendData(sf::Packet);
	std::vector<Client> getClientVector() const;
	std::vector<Client> m_client_vector;
private:
	short m_registration_step = 0;
	sf::TcpListener m_listener;
	sf::TcpSocket m_registration_socket;
	sf::Packet m_packet;
	sf::Clock m_send_time;
	sf::Int32 m_send_rate = 4;

	sf::Socket::Status acceptConnection();
	sf::Socket::Status receiveClientData();
	sf::Socket::Status sendClientData();
	sf::Socket::Status sendDataPort();
};