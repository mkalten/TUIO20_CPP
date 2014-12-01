/*
	TUIO2 C++ Test
    Copyright (c) 2009-2014 Martin Kaltenbrunner <martin@tuio.org>
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "Tuio2Test.h"

Tuio2Test::Tuio2Test(TuioServer *server)
	: width	(1024)
	, height(768)
{
    tuioServer = server;
    tuioServer->setSourceName("t2test");
	tuioServer->setDimension(width,height);
	tuioServer->setVerbose(true);
}



void Tuio2Test::run() {
	
	frameTime = TuioTime::getSystemTime();
	tuioServer->initTuioFrame(frameTime);
	TuioObject *tobj = tuioServer->createTuioObject();
	tuioServer->addTuioPointer(tobj,0.0,0.0,0.0,0.0,0.0,0.0);
	tuioServer->addTuioToken(tobj,0,0.0,0.0,0.0);
	tuioServer->addTuioBounds(tobj,0,0,0,0,0,0);
	tuioServer->addTuioSymbol(tobj,0,0,0,"type0","data0");
	tuioServer->commitTuioFrame();
	usleep(10000);
	
	frameTime = TuioTime::getSystemTime();
	tuioServer->initTuioFrame(frameTime);
	TuioObject *to = tuioServer->createTuioPointer(0.1,0.1,0.1,0.1,0.1,0.1);
	TuioPointer *pointer = to->getTuioPointer();
	to = tuioServer->createTuioToken(1,0.1,0.1,0.1);
	TuioToken *token = to->getTuioToken();
	to = tuioServer->createTuioBounds(0,0,0,0,0,0);
	TuioBounds *bounds = to->getTuioBounds();
	to = tuioServer->createTuioSymbol(0,0,0,"type1","data1");
	TuioSymbol *symbol = to->getTuioSymbol();
	tuioServer->commitTuioFrame();
	usleep(100000);
	
	frameTime = TuioTime::getSystemTime();
	tuioServer->initTuioFrame(frameTime);
	TuioPointer *tp = tobj->getTuioPointer();
	tuioServer->updateTuioPointer(tp,0.01,0.01,0.01,0.01,0.01,0.01);
	TuioToken *tt = tobj->getTuioToken();
	tuioServer->updateTuioToken(tt,0.01,0.01,0.01);
	TuioBounds *tb = tobj->getTuioBounds();
	tuioServer->updateTuioBounds(tb,0.01,0.01,0.01,0.01,0.01,0.01);
	tuioServer->commitTuioFrame();
	usleep(100000);
	
	frameTime = TuioTime::getSystemTime();
	tuioServer->initTuioFrame(frameTime);
	tuioServer->updateTuioPointer(pointer,0.11,0.11,0.11,0.11,0.11,0.11);
	tuioServer->updateTuioToken(token,0.11,0.11,0.11);
	tuioServer->updateTuioBounds(bounds,0.11,0.11,0.11,0.11,0.11,0.11);
	tuioServer->commitTuioFrame();
	usleep(100000);
	
	frameTime = TuioTime::getSystemTime();
	tuioServer->initTuioFrame(frameTime);
	tuioServer->removeTuioObject(tobj);
	tuioServer->commitTuioFrame();
	usleep(100000);
	
	frameTime = TuioTime::getSystemTime();
	tuioServer->initTuioFrame(frameTime);
	tuioServer->removeTuioPointer(pointer);
	tuioServer->removeTuioToken(token);
	tuioServer->removeTuioBounds(bounds);
	tuioServer->removeTuioSymbol(symbol);
	tuioServer->commitTuioFrame();
}

int main(int argc, char* argv[])
{
 	TuioServer *server;
	if( argc == 3 ) {
		server = new TuioServer(argv[1],atoi(argv[2]));
	} else server = new TuioServer(); // default is UDP port 3333 on localhost

	// add an additional TUIO/TCP sender
	OscSender *tcp_sender;
	if( argc == 2 ) {
		tcp_sender = new TcpSender(atoi(argv[1]));
	} else if ( argc == 3 ) {
		tcp_sender = new TcpSender(argv[1],atoi(argv[2]));
	} else tcp_sender = new TcpSender(3333);
	server->addOscSender(tcp_sender);

	// add an additional TUIO/FLC sender
	OscSender *flash_sender = new FlashSender();
	server->addOscSender(flash_sender);

	Tuio2Test *test = new Tuio2Test(server);
	test->run();

	delete test;
	delete server;
#ifndef LINUX
	delete flash_sender;
#endif
	delete tcp_sender;
	return 0;
}


