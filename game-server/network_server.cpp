#include "network_server.h"

NetworkServer::NetworkServer()
{
}

sf::Socket::Status NetworkServer::init()
{
	if (m_listener.listen(sf::Socket::AnyPort) == sf::Socket::Status::Done) {
		std::cout << "Port -" << m_listener.getLocalPort() << std::endl;
		return sf::Socket::Status();
	}
	else {
		return sf::Socket::Status::Error;
	}
}

sf::Socket::Status NetworkServer::registerClient()
{
	acceptConnection();
	receiveClientData();
	sendClientData();
	sendDataPort();
	if (m_registration_step == 5) { 
		m_registration_step = 0; 
		return sf::Socket::Status::Done; 
	}
	else {
		return sf::Socket::Status::NotReady;
	}
}

sf::Socket::Status NetworkServer::acceptConnection()
{
	if (m_registration_step == 0)
	{
		if (m_listener.isBlocking()) {
			m_listener.setBlocking(false);
		}

		if (m_listener.accept(m_registration_socket) == sf::Socket::Status::Done)
		{
			std::cout << "acceptIncomingConnection(): Accepted new connection\n";
			m_registration_step = 1;
			return sf::Socket::Status::Done;
		}
		else {
			return sf::Socket::Status::NotReady;
		}

		return sf::Socket::Status::NotReady;
	}
}

sf::Socket::Status NetworkServer::receiveClientData()
{
	if (m_registration_step == 1) {
		if (m_registration_socket.isBlocking()) {
			m_registration_socket.setBlocking(false);
		}

		if (m_registration_socket.receive(m_packet) == sf::Socket::Status::Done) {
			if (m_packet.getDataSize() > 0) {
				std::string name;
				if (m_packet >> name) {
					Client new_client;
					m_client_vector.push_back(new_client);
					m_client_vector.back().name = name;
					m_client_vector.back().ip = m_registration_socket.getRemoteAddress();
					m_client_vector.back().p_data_socket = new sf::UdpSocket;
					if (m_client_vector.back().p_data_socket->bind(sf::Socket::AnyPort) != sf::Socket::Status::Done)
						std::cout << "(!)receiveClientRegData(): Failed to bind port to the new client-dedicated data port\n";
				}
				else {
					std::cout << "(!)receiveClientRegData(): Failed to read client name from received m_packet\n";
					return sf::Socket::Status::Error;
				}
				sf::Uint16 port;
				if (m_packet >> port) {
					m_client_vector.back().port = static_cast<unsigned short>(port);
				}
				else {
					std::cout << "(!)receiveClientRegData(): Failed to read client data socket port from received m_packet\n";
					return sf::Socket::Status::Error;
				}

				if (!m_packet.endOfPacket()) {
					std::cout << "(!)receiveClientRegData(): Client registration data received, but something left, data probably corrupted\n";
				}
			}
			else {
				std::cout << "(!)receiveClientRegData(): Error, received m_packet is empty\n";
				return sf::Socket::Status::Error;
			}

			std::cout << "receiveClientRegData(): Client registration data received. New client: " << m_client_vector.back().name << std::endl;
			m_registration_step = 2;
			for (int i = 0; i < m_client_vector.size() - 1; i++)
				m_client_vector[i].done = false;
			m_packet.clear();
			return sf::Socket::Status::Done;
		}
		else {
			return sf::Socket::Status::NotReady;
		}
	}
}

sf::Socket::Status NetworkServer::sendClientData()
{
	if (m_registration_step == 2) {
		if (m_client_vector.size() > 1) {
			for (int i = 0; i < m_client_vector.size() - 1; i++) {
				if (!m_client_vector[i].done) {
					if (m_client_vector[i].p_data_socket->isBlocking()) {
						m_client_vector[i].p_data_socket->setBlocking(false);
					}
					sf::IpAddress temp_ip = m_client_vector[i].ip;
					unsigned short tempPort = m_client_vector[i].port;

					if (m_client_vector[i].send_packet.getDataSize() == 0)
						m_client_vector[i].send_packet << "NEW" << m_client_vector.back().name;

					if (m_client_vector[i].p_data_socket->send(m_client_vector[i].send_packet, temp_ip, tempPort) == sf::Socket::Status::Done)
					{
						m_client_vector[i].done = true;

						bool allIsDone = true;
						for (int k = 0; k < m_client_vector.size() - 1; k++)
						{
							if (!m_client_vector[k].done) allIsDone = false;
						}
						if (allIsDone)
						{
							for (int j = 0; j < m_client_vector.size(); j++)
							{
								m_client_vector[j].send_packet.clear();
								m_client_vector[j].done = false;
							}
							m_registration_step = 3;
							return sf::Socket::Status::Done;
						}
					}
				}
			}
			return sf::Socket::Status::NotReady;
		}
		else {
			m_registration_step = 3;
			return sf::Socket::Status::Done;
		}
	}
}

sf::Socket::Status NetworkServer::sendDataPort()
{
	if (m_registration_step == 3)
	{
		if (m_registration_socket.isBlocking()) {
			m_registration_socket.setBlocking(false);
		}

		if (m_packet.getDataSize() == 0) {
			m_packet << static_cast<sf::Uint16>(m_client_vector.back().p_data_socket->getLocalPort());
		}

		if (m_registration_socket.send(m_packet) == sf::Socket::Status::Done) {
			std::cout << "sendDedicatedDataPort(): Dedicated data port sent\n";
			m_registration_step = 4;
			m_packet.clear();
			return sf::Socket::Status::Done;
		}
		else {
			return sf::Socket::Status::NotReady;
		}
	}
}

sf::Socket::Status NetworkServer::sendClientRecord()
{
	if (m_registration_step == 4) {
		if (m_registration_socket.isBlocking()) {
			m_registration_socket.setBlocking(false);
		}

		if (m_packet.getDataSize() == 0) {
			if (m_client_vector.size() > 1) {
				for (int i = 0; i < m_client_vector.size() - 1; i++)
					m_packet << m_client_vector[i].name;
			}
			else m_packet << "FIRST";
		}

		if (m_registration_socket.send(m_packet) == sf::Socket::Status::Done) {
			std::cout << "sendConnectedClientsRecords(): Connected clients records sent to new client\n";
			m_registration_step = 5;
			m_registration_socket.disconnect();
			return sf::Socket::Status::Done;
		}
		else {
			return sf::Socket::Status::NotReady;
		}
	}
}

sf::Socket::Status NetworkServer::receiveData(unsigned int& client_index)
{
	for (int i = 0; i < m_client_vector.size(); i++) {
		if (m_client_vector[i].p_data_socket->isBlocking()) m_client_vector[i].p_data_socket->setBlocking(false);
		sf::IpAddress temp_ip = m_client_vector[i].ip;
		unsigned short tempPort = m_client_vector[i].port;

		if (m_client_vector[i].p_data_socket->receive(m_client_vector[i].registration_packet, temp_ip, tempPort) == sf::Socket::Status::Done) {
			client_index = i;

			return sf::Socket::Status::Done;
		}
	}

	return sf::Socket::Status::NotReady;
}

sf::Socket::Status NetworkServer::sendData(sf::Packet data_packet)
{
	if (m_send_time.getElapsedTime().asMilliseconds() > m_send_rate) {
		for (int i = 0; i < m_client_vector.size(); i++) {
			if (!m_client_vector[i].done) {
				if (m_client_vector[i].p_data_socket->isBlocking()) {
					m_client_vector[i].p_data_socket->setBlocking(false);
				}
				sf::IpAddress temp_ip = m_client_vector[i].ip;
				unsigned short tempPort = m_client_vector[i].port;

				if (m_client_vector[i].send_packet.getDataSize() == 0) {
					m_client_vector[i].send_packet = data_packet;
				}

				if (m_client_vector[i].p_data_socket->send(m_client_vector[i].send_packet, temp_ip, tempPort) == sf::Socket::Status::Done) {
					m_client_vector[i].done = true;
				}

				bool allIsDone = true;

				for (int j = 0; j < m_client_vector.size(); j++) {
					if (m_client_vector[j].done == false) allIsDone = false;
				}

				if (allIsDone) {
					for (int j = 0; j < m_client_vector.size(); j++) {
						m_client_vector[j].send_packet.clear();
						m_client_vector[j].done = false;
					}
					m_send_time.restart();
					return sf::Socket::Status::Done;
				}
			}

		}
		return sf::Socket::Status::NotReady;

	}
	else {
		return sf::Socket::Status::NotReady;
	}
}

std::vector<Client> NetworkServer::getClientVector() const {
	return m_client_vector;
}