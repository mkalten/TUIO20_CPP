/*
 TUIO2 C++ Example - part of the reacTIVision project
 Copyright (c) 2009-2017 Martin Kaltenbrunner <martin@tuio.org>
 
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

#include "Tuio2Dump.h"

void Tuio2Dump::tuioAdd(TuioObject *tobj) {

	if(tobj->containsNewTuioToken()) std::cout << "add tok " << tobj->getTuioToken()->getSessionID() << std::endl;
	if(tobj->containsNewTuioPointer()) std::cout << "add ptr " << tobj->getTuioPointer()->getSessionID() << std::endl;
	if(tobj->containsNewTuioBounds()) std::cout << "add bnd " << tobj->getTuioBounds()->getSessionID() << std::endl;
	if(tobj->containsNewTuioSymbol()) std::cout << "add sym " << tobj->getTuioSymbol()->getSessionID() << std::endl;
	//std::cout << "add obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << "/"<<  tobj->getTuioSourceID() << ") "<< tobj->getX() << " " << tobj->getY() << " " << tobj->getAngle() << std::endl;
	std::cout << std::flush;
}

void Tuio2Dump::tuioUpdate(TuioObject *tobj) {
	if(tobj->containsTuioToken()) std::cout << "set tok " << tobj->getTuioToken()->getSessionID() << std::endl;
	if(tobj->containsTuioPointer()) std::cout << "set ptr " << tobj->getTuioPointer()->getSessionID() << std::endl;
	if(tobj->containsTuioBounds()) std::cout << "set bnd " << tobj->getTuioBounds()->getSessionID() << std::endl;
	if(tobj->containsTuioSymbol()) std::cout << "set sym " << tobj->getTuioSymbol()->getSessionID() << std::endl;
	//std::cout << "set obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << "/"<<  tobj->getTuioSourceID() << ") "<< tobj->getX() << " " << tobj->getY() << " " << tobj->getAngle() << " " << tobj->getMotionSpeed() << " " << tobj->getRotationSpeed() << " " << tobj->getMotionAccel() << " " << tobj->getRotationAccel() << std::endl;	
	std::cout << std::flush;
}

void Tuio2Dump::tuioRemove(TuioObject *tobj) {
	if(tobj->containsTuioToken()) std::cout << "del tok " << tobj->getSessionID() << std::endl;
	if(tobj->containsTuioPointer()) std::cout << "del ptr " << tobj->getSessionID() << std::endl;
	if(tobj->containsTuioBounds()) std::cout << "del bnd " << tobj->getSessionID() << std::endl;
	if(tobj->containsTuioSymbol()) std::cout << "del sym " << tobj->getSessionID() << std::endl;
	//std::cout << "del obj " << tobj->getSymbolID() << " (" << tobj->getSessionID() << "/"<<  tobj->getTuioSourceID() << ")" << std::endl;
	std::cout << std::flush;
}

void Tuio2Dump::tuioRefresh(TuioTime frameTime) {
	std::cout << "refresh " << frameTime.getFrameID() << " "<< frameTime.getTotalMilliseconds() << std::endl;
	std::cout << std::flush;
}

int main(int argc, char* argv[])
{
	if( argc >= 2 && strcmp( argv[1], "-h" ) == 0 ){
        	std::cout << "usage: TuioDump [port]\n";
        	return 0;
	}

	int port = 3333;
	if( argc == 2 ) port = atoi( argv[1] );
	else if( argc == 3 ) port = atoi( argv[2] );

	Tuio2Dump dump;
	OscReceiver *receiver;
	receiver = new UdpReceiver(port);
	//if (argc<3) receiver = new TcpReceiver(port);
	//else if (argc==3) receiver = new TcpReceiver(argv[1], port);
	// receiver = new DevReceiver(0);
	TuioClient client(receiver);
	client.addTuioListener(&dump);
	client.connect(true);

	delete receiver;
	return 0;
}


