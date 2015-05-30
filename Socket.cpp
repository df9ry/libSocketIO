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

Socket::Socket() {
	m_fd = ::socket(AF_INET6, SOCK_STREAM, 0);
	if (m_fd == -1)
		throw runtime_error("Unable to create socket");
	memset(&m_addr, 0x00, sizeof(m_addr));
}

Socket::Socket(int fd): m_fd{fd} {
	memset(&m_addr, 0x00, sizeof(m_addr));
}

Socket::Socket(Socket&& other) {
	close();
	m_fd = other.m_fd;
	other.m_fd = -1;
	memcpy(&m_addr, &other.m_addr, sizeof(m_addr));
	memset(&other.m_addr, 0x00, sizeof(other.m_addr));
}

Socket& Socket::operator=(Socket&& other) {
	close();
	m_fd = other.m_fd;
	other.m_fd = -1;
	memcpy(&m_addr, &other.m_addr, sizeof(m_addr));
	memset(&other.m_addr, 0x00, sizeof(other.m_addr));
	return *this;
}

Socket::~Socket() {
	close();
}

void Socket::bind(uint16_t port, const std::string& ifc) {
	memset(&m_addr, 0x00, sizeof(m_addr));
	m_addr.sin6_family = AF_INET6;
	if (ifc.empty())
		::inet_pton(AF_INET6, ifc.c_str(),
				&m_addr.sin6_addr);
	else
		m_addr.sin6_addr = in6addr_any;
	m_addr.sin6_port = ::htons(port);
	if (::bind(m_fd, (struct sockaddr *) &m_addr, sizeof(m_addr)) < 0)
		throw runtime_error("Unable to bind");
}

void Socket::listen(int backof) {
	::listen(m_fd, backof);
}

unique_ptr<Socket> Socket::accept() {
	auto s = unique_ptr<Socket>(new Socket(-1));
	socklen_t clilen = sizeof(s.get()->m_addr);
	s.get()->m_fd = ::accept(m_fd, (struct sockaddr *) &s.get()->m_addr, &clilen);
	if (s.get()->m_fd == -1)
		throw runtime_error("Unable to accept");
	return s;
}

int Socket::read(uint8_t* buffer, size_t size) {
	return ::read(m_fd, buffer, size);
}

int Socket::write(uint8_t* buffer, size_t length) {
	return ::write(m_fd, buffer, length);
}

void Socket::close() {
	if (m_fd != -1) {
		::close(m_fd);
		m_fd = -1;
	}
}

string Socket::toString() const {
	char str[INET6_ADDRSTRLEN];
	if  (m_addr.sin6_family == AF_INET) {
		::inet_ntop(AF_INET, &(m_addr.sin6_addr), str, INET_ADDRSTRLEN);
		return string(str) + "/" + to_string(::ntohs(m_addr.sin6_port));
	} else { // AF_INET6
		::inet_ntop(AF_INET6, &(m_addr.sin6_addr), str, INET6_ADDRSTRLEN);
		return string(str) + "/" + to_string(::ntohs(m_addr.sin6_port));
	}
}

string Socket::toStringPeer() const {
	socklen_t len;
	struct sockaddr_storage addr;
	char ipstr[INET6_ADDRSTRLEN];
	int port;

	len = sizeof addr;
	getpeername(m_fd, (struct sockaddr*)&addr, &len);

	// deal with both IPv4 and IPv6:
	if (addr.ss_family == AF_INET) {
	    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
	    port = ::ntohs(s->sin_port);
	    ::inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
	} else { // AF_INET6
	    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
	    port = ntohs(s->sin6_port);
	    ::inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
	}
	return string(ipstr) + "/" + to_string(port);
}

} /* namespace SocketIO */
