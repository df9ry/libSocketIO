/*
    Project libSocketIO
    Copyright (C) 2015  tania@df9ry.de

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <string>

#include <netinet/in.h>

namespace SocketIO {

/**
 * RAII encapsulation of a socket.
 */
class Socket {
public:
	/**
	 * Constructor.
	 */
	Socket();

	/**
	 * Destructor.
	 */
	~Socket();

	/**
	 * Bind the socket.
	 * @param port Port to bind.
	 * @param ifc Interface to bind. If empty, bind to any interface.
	 */
	void bind(uint16_t port, const std::string& ifc="");

	/**
	 * Directs the socket to listen.
	 * @param backof Buffer for incoming connections.
	 */
	void listen(int backof=5);

	/**
	 * Accept incoming connection.
	 * @return Socket for new connection.
	 */
	Socket&& accept();

	/**
	 * Read from socket.
	 * @param buffer Read buffer.
	 * @param size Size of the buffer.
	 * @return Bytes read or < 0, when eof.
	 */
	int read(uint8_t* buffer, size_t size);

	/**
	 * Write to the socket.
	 * @param buffer Write buffer.
	 * @param length Number of bytes to write.
	 * @return Number of bytes written. If < 0, there was an error.
	 */
	int write(uint8_t* buffer, size_t length);

	/**
	 * Close the socket.
	 */
	void close();

private:
	Socket(int fd);
	int		           m_fd;
	struct sockaddr_in m_addr{};
};

} /* namespace SocketIO */

#endif /* SOCKET_H_ */
