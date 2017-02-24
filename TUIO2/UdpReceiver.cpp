/*
 TUIO2 C++ Library
 Copyright (c) 2009-2017 Martin Kaltenbrunner <martin@tuio.org>
 
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

#include "UdpReceiver.h"

using namespace TUIO2;
using namespace osc;

#ifdef WIN32
static DWORD WINAPI ClientThreadFunc( LPVOID obj )
#else
static void* ClientThreadFunc( void* obj )
#endif
{
	static_cast<UdpReceiver*>(obj)->socket->Run();
	return 0;
};

UdpReceiver::UdpReceiver(unsigned short port):locked (false) {
	try {
		socket = new UdpListeningReceiveSocket(IpEndpointName( IpEndpointName::ANY_ADDRESS, port ), this );
	} catch (std::exception &e) {
		std::cerr << "could not bind to UDP port " << port << std::endl;
		socket = NULL;
	}

	if (socket!=NULL) {
		if (!socket->IsBound()) {
			delete socket;
			socket = NULL;
		} else std::cout << "listening to TUIO/UDP messages on port " << port << std::endl;
	}
}

UdpReceiver::~UdpReceiver() {
	delete socket;
}

void UdpReceiver::connect(bool lk) {

	if (connected) return;
	if (socket==NULL) return;
	locked = lk;

	if (!locked) {
#ifdef WIN32
		DWORD threadId;
		thread = CreateThread( 0, 0, ClientThreadFunc, this, 0, &threadId );
#else
		pthread_create(&thread , NULL, ClientThreadFunc, this);
#endif
	} else socket->Run();

	connected = true;
}

void UdpReceiver::disconnect() {

	if (!connected) return;
	if (socket==NULL) {
		connected = false;
		locked = false;
		return;
	}
	socket->Break();

	if (!locked) {
#ifdef WIN32
		if( thread ) CloseHandle( thread );
#endif
		thread = 0;
	} else locked = false;

	connected = false;
}
