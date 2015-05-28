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

#include "Socket.h"

#include <stdexcept>

#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

namespace SocketIO {

Socket::Socket(): m_fd{socket(AF_INET, SOCK_STREAM, 0)} {
	if (m_fd < 0)
		throw runtime_error("Unable to create socket");
	memset(&m_addr, 0x00, sizeof(m_addr));
}

Socket::Socket(int fd): m_fd{fd} {
	memset(&m_addr, 0x00, sizeof(m_addr));
}

Socket::~Socket() {
	close();
}

void Socket::bind(uint16_t port, const std::string& ifc) {
	memset(&m_addr, 0x00, sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	if (ifc.empty())
		inet_aton(ifc.c_str(), &m_addr.sin_addr);
	else
		m_addr.sin_addr.s_addr = INADDR_ANY;
	m_addr.sin_port = htons(port);
	if (::bind(m_fd, (struct sockaddr *) &m_addr, sizeof(m_addr)) < 0)
		throw runtime_error("Unable to bind");
}

void Socket::listen(int backof) {
	::listen(m_fd, backof);
}

Socket&& Socket::accept() {
	Socket s(-1);
	socklen_t clilen{sizeof(s.m_addr)};
	s.m_fd = ::accept(m_fd, (struct sockaddr *) &s.m_addr, &clilen);
	if (s.m_fd < 0)
		throw runtime_error("Unable to accept");
	return move(s);
}

int Socket::read(uint8_t* buffer, size_t size) {
	return ::read(m_fd, buffer, size);
}

int Socket::write(uint8_t* buffer, size_t length) {
	return ::write(m_fd, buffer, length);
}

void Socket::close() {
	if (m_fd >= 0)
		::close(m_fd);
	m_fd = -1;
}

} /* namespace SocketIO */
