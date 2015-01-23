/*
 TUIO2 C++ Library
 Copyright (c) 2009-2014 Martin Kaltenbrunner <martin@tuio.org>
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3.0 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library.
*/

#include "WebSockSender.h"

#include <stdio.h>
#include "base64.h"
#include "sha1.h"

#ifdef  WIN32
#ifndef int32_t
typedef DWORD int32_t;
#endif
#endif

using namespace TUIO2;

WebSockSender::WebSockSender()
	:TcpSender( 8080 )
{
	local = true;
	buffer_size = MAX_TCP_SIZE;
	port_no = 8080;
}

WebSockSender::WebSockSender(int port)
	:TcpSender( port )
{
	local = true;
	buffer_size = MAX_TCP_SIZE;
	port_no = port;
}

WebSockSender::~WebSockSender()
{
}

bool WebSockSender::sendOscPacket (osc::OutboundPacketStream *bundle) {
	if (!connected) return false; 
	if ( bundle->Size() > buffer_size ) return false;
	if ( bundle->Size() == 0 ) return false;
	
#ifdef OSC_HOST_LITTLE_ENDIAN             
	data_size[0] =  bundle->Size()>>24;
	data_size[1] = (bundle->Size()>>16) & 255;
	data_size[2] = (bundle->Size()>>8) & 255;
	data_size[3] = (bundle->Size()) & 255;
#else
	*((int32_t*)data_size) = bundle->Size();
#endif

#ifdef WIN32
	std::list<SOCKET>::iterator client;
#else
	std::list<int>::iterator client;
#endif
	
	for (client = tcp_client_list.begin(); client!=tcp_client_list.end(); client++) {
		int len = bundle->Size();
		// add WebSocket header on top
		uint8_t header[4] = {
			0x82,
			(uint8_t)( len & 0xFF), 
			(uint8_t)((len >>8) & 0xFF),
			(uint8_t)( len & 0xFF)
		};
		int hs = 2;
		if (len > 125) { hs = 4; header[1] = 126; }
		memcpy(&data_buffer[0], &header, hs);
		memcpy(&data_buffer[hs], bundle->Data(), bundle->Size());
		send((*client),data_buffer, hs+bundle->Size(),0);
	}

	return true;
}

void WebSockSender::newClient( int tcp_client ) {

	// socket -> file descriptor
	FILE* conn = fdopen( tcp_client, "r+" );

	// websocket challenge-response
	uint8_t digest[SHA1_HASH_SIZE];
	char buf[1024] = "...";
	char key[1024];

	// read client handshake challenge
	while ((buf[0] != 0) && (buf[0] != '\r')) {
		fgets( buf, sizeof(buf), conn );
		if (strncmp(buf,"Sec-WebSocket-Key: ",19) == 0) {
			strncpy(key,buf+19,sizeof(key));
			key[strlen(buf)-21] = 0;
			break;
		}
	}

	strncat(key,"258EAFA5-E914-47DA-95CA-C5AB0DC85B11",sizeof(key)-strlen(key));
	sha1(digest,(uint8_t*)key,strlen(key));

	snprintf(buf, sizeof(buf),
		"HTTP/1.1 101 Switching Protocols\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Accept: %s\r\n\r\n",
		base64( digest, SHA1_HASH_SIZE ).c_str() ); 

	send(tcp_client,buf, strlen(buf),0);
}

