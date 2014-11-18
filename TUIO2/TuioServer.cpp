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

#include "TuioServer.h"
#include "UdpSender.h"

using namespace TUIO2;
using namespace osc;

TuioServer::TuioServer() 
	:local_sender			(true)
	,full_update			(false)	
	,periodic_update		(false)
	,source                 (NULL)
{
	primary_sender = new UdpSender();
	initialize();
}

TuioServer::TuioServer(const char *host, int port) 
:local_sender			(true)
,full_update			(false)	
,periodic_update		(false)
,source                 (NULL)
{
	primary_sender = new UdpSender(host,port);
	initialize();
}

TuioServer::TuioServer(OscSender *oscsend)
	:primary_sender					(oscsend)
	,local_sender			(false)
	,full_update			(false)	
	,periodic_update		(false)
	,source                 (NULL)
{
	initialize();
}

void TuioServer::initialize() {
	
	senderList.push_back(primary_sender);
	int size = primary_sender->getBufferSize();
	oscBuffer = new char[size];
	oscPacket = new osc::OutboundPacketStream(oscBuffer,size);
	fullBuffer = new char[size];
	fullPacket = new osc::OutboundPacketStream(oscBuffer,size);
	
	updateTime = TuioTime(currentFrameTime);
	//sendEmptyTuioBundle();
	
	invert_x = false;
	invert_y = false;
	invert_a = false;	
}

TuioServer::~TuioServer() {

	initTuioFrame(TuioTime::getSystemTime());
	stopUntouchedMovingObjects();
	
	initTuioFrame(TuioTime::getSystemTime());
	removeUntouchedStoppedObjects();
	sendEmptyTuioBundle();
	
	delete []oscBuffer;
	delete oscPacket;
	delete []fullBuffer;
	delete fullPacket;
	
	if (local_sender) delete primary_sender;
}

void TuioServer::addOscSender(OscSender *sender) {

	// add source address if previously local
	/*if ((source_name) && (primary_sender->isLocal()) && (senderList.size()==1)) {
		setSourceName(source_name);
	}*/ 
	
	// resize packets to smallest transport method
	unsigned int size = sender->getBufferSize();
	if (size<oscPacket->Capacity()) {
		osc::OutboundPacketStream *temp = oscPacket;
		oscPacket = new osc::OutboundPacketStream(oscBuffer,size);
		delete temp;
		temp = fullPacket;
		fullPacket = new osc::OutboundPacketStream(oscBuffer,size);
		delete temp;
		
	}
	
	senderList.push_back(sender);
}

void TuioServer::deliverOscPacket(osc::OutboundPacketStream  *packet) {

	for (unsigned int i=0;i<senderList.size();i++)
		senderList[i]->sendOscPacket(packet);
}

void TuioServer::setSourceName(const char* name) {
    if (source==NULL) source = new TuioSource(name);
    //else source->setSourceName(name);
}

void TuioServer::setDimension(short w, short h) {
    if (source!=NULL) source->setDimension(w,h);
    
    // decoder test
    /*int dim = source->getDimension();
    short width = dim >> 16;
    short height = dim & 0x0000FFFF;
    printf("dim: %i %i\n",width,height);*/
}

void TuioServer::commitTuioFrame() {
	TuioManager::commitTuioFrame();
		
	if(tobjUpdate) {
		startTuioBundle(currentFrame);
		for (std::list<TuioObject*>::iterator tobj = tobjList.begin(); tobj!=tobjList.end(); tobj++) {
			
            if ((*tobj)->containsTuioToken()) {
                TuioToken *ttok = (*tobj)->getTuioToken();
                if  ((full_update) || (ttok->getTuioTime()==currentFrameTime)) addTokenMessage(ttok);
            }
            
            if ((*tobj)->containsTuioPointer()) {
                TuioPointer *tptr = (*tobj)->getTuioPointer();
                if  ((full_update) || (tptr->getTuioTime()==currentFrameTime)) addPointerMessage(tptr);
            }
            
            if ((*tobj)->containsTuioBounds()) {
                TuioBounds *tbnd = (*tobj)->getTuioBounds();
                if  ((full_update) || (tbnd->getTuioTime()==currentFrameTime)) addBoundsMessage(tbnd);
            }
            
            if ((*tobj)->containsTuioSymbol()) {
                TuioSymbol *tsym = (*tobj)->getTuioSymbol();
                if  ((full_update) || (tsym->getTuioTime()==currentFrameTime)) addSymbolMessage(tsym);
            }
            
            
        }
		updateTime = TuioTime(currentFrameTime);
		sendTuioBundle();
	} else if (periodic_update) {
		
		TuioTime timeCheck = currentFrameTime - updateTime;
		if(timeCheck.getSeconds()>=update_interval) {
			updateTime = TuioTime(currentFrameTime);
			startTuioBundle(currentFrame);
			if (full_update) {
				for (std::list<TuioObject*>::iterator tobj = tobjList.begin(); tobj!=tobjList.end(); tobj++) {
					// start a new packet if we exceed the packet capacity
					if ((oscPacket->Capacity()-oscPacket->Size())<TOK_MESSAGE_SIZE) {
						sendTuioBundle();
						startTuioBundle(currentFrame);
					}
					addTokenMessage((*tobj)->getTuioToken());
				}
			}
			sendTuioBundle();
		}
	}
	tobjUpdate = false;
}

void TuioServer::sendEmptyTuioBundle() {
    oscPacket->Clear();
    (*oscPacket) << osc::BeginBundleImmediate;
    (*oscPacket) << osc::BeginMessage( "/tuio2/frm") << 0 << TuioTime::getSystemTimeTag() << source->getSourceName() << source->getDimension() << osc::EndMessage;
    (*oscPacket) << osc::BeginMessage( "/tuio2/alv") << osc::EndMessage;
    (*oscPacket) << osc::EndBundle;
    deliverOscPacket( oscPacket );
}

void TuioServer::checkBundleCapacity(int msg_size) {
    
    int size = msg_size + ALV_MESSAGE_SIZE + 4*tobjList.size();
    
    if ((oscPacket->Capacity()-oscPacket->Size())<size) {
        (*oscPacket) << osc::EndBundle;
        deliverOscPacket( oscPacket ); // send the intermediate bundle without final ALV message!
        startTuioBundle(currentFrame);
    }
}

void TuioServer::startTuioBundle(unsigned int fseq) {
    
    oscPacket->Clear();
    (*oscPacket) << osc::BeginBundleImmediate;
    if (source) (*oscPacket) << osc::BeginMessage( "/tuio2/frm") << (int32)currentFrame << frameTimeTag << source->getSourceName() << source->getDimension();
    (*oscPacket) << osc::EndMessage;
}

void TuioServer::addTokenMessage(TuioToken *ttok) {
	
    // start a new packet if we exceed the packet capacity
    checkBundleCapacity(TOK_MESSAGE_SIZE);
    
	float xpos = ttok->getX();
	float xvel = ttok->getXSpeed();
	if (invert_x) {
		xpos = 1 - xpos;
		xvel = -1 * xvel;
	}
	float ypos = ttok->getY();
	float yvel = ttok->getYSpeed();
	if (invert_y) {
		ypos = 1 - ypos;
		yvel = -1 * yvel;
	}
	float angle = ttok->getAngle();
	float rvel = ttok->getRotationSpeed();
	if (invert_a) {
		angle = 2.0f*(float)M_PI - angle;
		rvel = -1 * rvel;
	}
	
	(*oscPacket) << osc::BeginMessage( "/tuio2/tok");
	(*oscPacket) << (int32)ttok->getSessionID() << (int32)ttok->getTypeUserID() << (int32)ttok->getSymbolID() << xpos << ypos << angle;
	(*oscPacket) << xvel << yvel << rvel << ttok->getMotionAccel() << ttok->getRotationAccel();	
	(*oscPacket) << osc::EndMessage;
}

void TuioServer::addPointerMessage(TuioPointer *tptr) {
    
    // start a new packet if we exceed the packet capacity
    checkBundleCapacity(PTR_MESSAGE_SIZE);
    
    float xpos = tptr->getX();
    float xvel = tptr->getXSpeed();
    if (invert_x) {
        xpos = 1 - xpos;
        xvel = -1 * xvel;
    }
    float ypos = tptr->getY();
    float yvel = tptr->getYSpeed();
    if (invert_y) {
        ypos = 1 - ypos;
        yvel = -1 * yvel;
    }
    
    (*oscPacket) << osc::BeginMessage( "/tuio2/ptr");
    (*oscPacket) << (int32)tptr->getSessionID() << (int32)tptr->getTypeUserID() << (int32)tptr->getPointerID();
    (*oscPacket) << xpos << ypos << tptr->getAngle() << tptr->getShear() << tptr->getRadius() << tptr->getPressure();
    (*oscPacket) << xvel << yvel << tptr->getPressureSpeed() << tptr->getMotionAccel() << tptr->getPressureAccel();
    (*oscPacket) << osc::EndMessage;
}

void TuioServer::addBoundsMessage(TuioBounds *tbnd) {
	
    // start a new packet if we exceed the packet capacity
    checkBundleCapacity(BND_MESSAGE_SIZE);
    
	float xpos = tbnd->getX();
	float xvel = tbnd->getXSpeed();
	if (invert_x) {
		xpos = 1 - xpos;
		xvel = -1 * xvel;
	}
	float ypos = tbnd->getY();
	float yvel = tbnd->getYSpeed();
	if (invert_y) {
		ypos = 1 - ypos;
		yvel = -1 * yvel;
	}
	float angle = tbnd->getAngle();
	float rvel = tbnd->getRotationSpeed();
	if (invert_a) {
		angle = 2.0f*(float)M_PI - angle;
		rvel = -1 * rvel;
	}
	
	(*oscPacket) << osc::BeginMessage( "/tuio2/bnd");
	(*oscPacket) << (int32)tbnd->getSessionID() << xpos << ypos << angle << tbnd->getWidth() << tbnd->getHeight() << tbnd->getArea();
	(*oscPacket) << xvel << yvel  << rvel << tbnd->getMotionAccel()  << tbnd->getRotationAccel();	
	(*oscPacket) << osc::EndMessage;
}

void TuioServer::addSymbolMessage(TuioSymbol *tsym) {
    
    // start a new packet if we exceed the packet capacity
    checkBundleCapacity(SYM_MESSAGE_SIZE);
    
    (*oscPacket) << osc::BeginMessage( "/tuio2/sym");
    (*oscPacket) << (int32)tsym->getSessionID() << (int32)tsym->getTypeUserID() << (int32)tsym->getSymbolID();
    (*oscPacket) << tsym->getSymbolType() << tsym->getSymbolData();
    (*oscPacket) << osc::EndMessage;
}

void TuioServer::sendTuioBundle() {
    
    //int before = oscPacket->Capacity()-oscPacket->Size();
    (*oscPacket) << osc::BeginMessage( "/tuio2/alv");
    
    for(std::list<TuioObject*>::iterator tobj = tobjList.begin();tobj!= tobjList.end(); tobj++)
        (*oscPacket) << (int32)(*tobj)->getSessionID();
    
    (*oscPacket) << osc::EndMessage;
    //int after = oscPacket->Capacity()-oscPacket->Size();
    //printf("ALV_MESSAGE_SIZE: %i\n",before-after);
    
	(*oscPacket) << osc::EndBundle;
	deliverOscPacket( oscPacket );
}

void TuioServer::sendFullMessages() {
/*
	// prepare the pointer packet
	fullPacket->Clear();
	(*fullPacket) << osc::BeginBundleImmediate;
	if (source!=NULL) (*fullPacket) << osc::BeginMessage( "/tuio/2Dcur") << "source" << source_name << osc::EndMessage;
	// add the pointer alive message
	(*fullPacket) << osc::BeginMessage( "/tuio/2Dcur") << "alive";
	for (std::list<TuioPointer*>::iterator tuioPointer = pointerList.begin(); tuioPointer!=pointerList.end(); tuioPointer++)
		(*fullPacket) << (int32)((*tuioPointer)->getSessionID());	
	(*fullPacket) << osc::EndMessage;	
	
	// add all current pointer set messages
	for (std::list<TuioPointer*>::iterator tuioPointer = pointerList.begin(); tuioPointer!=pointerList.end(); tuioPointer++) {
		
		// start a new packet if we exceed the packet capacity
		if ((fullPacket->Capacity()-fullPacket->Size())<CUR_MESSAGE_SIZE) {
			
			// add the immediate fseq message and send the pointer packet
			(*fullPacket) << osc::BeginMessage( "/tuio/2Dcur") << "fseq" << -1 << osc::EndMessage;
			(*fullPacket) << osc::EndBundle;
			deliverOscPacket( fullPacket );
			
			// prepare the new pointer packet
			fullPacket->Clear();	
			(*fullPacket) << osc::BeginBundleImmediate;
			if (source_name) (*fullPacket) << osc::BeginMessage( "/tuio/2Dcur") << "source" << source_name << osc::EndMessage;
			// add the pointer alive message
			(*fullPacket) << osc::BeginMessage( "/tuio/2Dcur") << "alive";
			for (std::list<TuioPointer*>::iterator tuioPointer = pointerList.begin(); tuioPointer!=pointerList.end(); tuioPointer++)
				(*fullPacket) << (int32)((*tuioPointer)->getSessionID());	
			(*fullPacket) << osc::EndMessage;				
		}
		
		float xpos = (*tuioPointer)->getX();
		float xvel = (*tuioPointer)->getXSpeed();
		if (invert_x) {
			xpos = 1 - xpos;
			xvel = -1 * xvel;
		}
		float ypos = (*tuioPointer)->getY();
		float yvel = (*tuioPointer)->getYSpeed();
		if (invert_y) {
			ypos = 1 - ypos;
			yvel = -1 * yvel;
		}
		
		// add the actual pointer set message
		(*fullPacket) << osc::BeginMessage( "/tuio/2Dcur") << "set";
		(*fullPacket) << (int32)((*tuioPointer)->getSessionID()) << xpos << ypos;
		(*fullPacket) << xvel << yvel <<(*tuioPointer)->getMotionAccel();	
		(*fullPacket) << osc::EndMessage;	
	}
	
	// add the immediate fseq message and send the pointer packet
	(*fullPacket) << osc::BeginMessage( "/tuio/2Dcur") << "fseq" << -1 << osc::EndMessage;
	(*fullPacket) << osc::EndBundle;
	deliverOscPacket( fullPacket );
	
	// prepare the object packet
	fullPacket->Clear();
	(*fullPacket) << osc::BeginBundleImmediate;
	if (source_name) (*fullPacket) << osc::BeginMessage( "/tuio/2Dobj") << "source" << source_name << osc::EndMessage;
	// add the object alive message
	(*fullPacket) << osc::BeginMessage( "/tuio/2Dobj") << "alive";
	for (std::list<TuioToken*>::iterator TuioToken = tokenList.begin(); TuioToken!=tokenList.end(); TuioToken++)
		(*fullPacket) << (int32)((*TuioToken)->getSessionID());	
	(*fullPacket) << osc::EndMessage;	
	
	for (std::list<TuioToken*>::iterator tuioToken = tokenList.begin(); tuioToken!=tokenList.end(); tuioToken++) {
		
		// start a new packet if we exceed the packet capacity
		if ((fullPacket->Capacity()-fullPacket->Size())<OBJ_MESSAGE_SIZE) {
			// add the immediate fseq message and send the object packet
			(*fullPacket) << osc::BeginMessage( "/tuio/2Dobj") << "fseq" << -1 << osc::EndMessage;
			(*fullPacket) << osc::EndBundle;
			deliverOscPacket( fullPacket );
			
			// prepare the new object packet
			fullPacket->Clear();	
			(*fullPacket) << osc::BeginBundleImmediate;
			if (source_name) (*fullPacket) << osc::BeginMessage( "/tuio/2Dobj") << "source" << source_name << osc::EndMessage;
			// add the object alive message
			(*fullPacket) << osc::BeginMessage( "/tuio/2Dobj") << "alive";
			for (std::list<TuioToken*>::iterator tuioToken = tokenList.begin(); tuioToken!=tokenList.end(); tuioToken++)
				(*fullPacket) << (int32)((*tuioToken)->getSessionID());
			(*fullPacket) << osc::EndMessage;	
		}
		
		float xpos = (*tuioToken)->getX();
		float xvel = (*tuioToken)->getXSpeed();
		if (invert_x) {
			xpos = 1 - xpos;
			xvel = -1 * xvel;
		}
		float ypos = (*tuioToken)->getY();
		float yvel = (*tuioToken)->getYSpeed();
		if (invert_y) {
			ypos = 1 - ypos;
			yvel = -1 * yvel;
		}
		float angle = (*tuioToken)->getAngle();
		float rvel = (*tuioToken)->getRotationSpeed();
		if (invert_a) {
			angle =  2.0f*(float)M_PI - angle;
			rvel = -1 * rvel;
		}
		
		// add the actual object set message
		(*fullPacket) << osc::BeginMessage( "/tuio/2Dobj") << "set";
		(*fullPacket) << (int32)((*tuioToken)->getSessionID()) << (*tuioToken)->getSymbolID() << xpos << ypos << angle;
		(*fullPacket) << xvel << yvel << rvel << (*tuioToken)->getMotionAccel() << (*tuioToken)->getRotationAccel();
		(*fullPacket) << osc::EndMessage;
		
	}
	// add the immediate fseq message and send the object packet
	(*fullPacket) << osc::BeginMessage( "/tuio/2Dobj") << "fseq" << -1 << osc::EndMessage;
	(*fullPacket) << osc::EndBundle;
	deliverOscPacket( fullPacket );
	
	// prepare the blob packet
	fullPacket->Clear();
	(*fullPacket) << osc::BeginBundleImmediate;
	if (source_name) (*fullPacket) << osc::BeginMessage( "/tuio/2Dbnd") << "source" << source_name << osc::EndMessage;
	// add the object alive message
	(*fullPacket) << osc::BeginMessage( "/tuio/2Dbnd") << "alive";
	for (std::list<TuioBounds*>::iterator tuioBounds = blobList.begin(); tuioBounds!=blobList.end(); tuioBounds++)
		(*fullPacket) << (int32)((*tuioBounds)->getSessionID());	
	(*fullPacket) << osc::EndMessage;	
	
	for (std::list<TuioBounds*>::iterator tuioBounds = blobList.begin(); tuioBounds!=blobList.end(); tuioBounds++) {
		
		// start a new packet if we exceed the packet capacity
		if ((fullPacket->Capacity()-fullPacket->Size())<BLB_MESSAGE_SIZE) {
			// add the immediate fseq message and send the object packet
			(*fullPacket) << osc::BeginMessage( "/tuio/2Dbnd") << "fseq" << -1 << osc::EndMessage;
			(*fullPacket) << osc::EndBundle;
			deliverOscPacket( fullPacket );
			
			// prepare the new blob packet
			fullPacket->Clear();	
			(*fullPacket) << osc::BeginBundleImmediate;
			if (source_name) (*fullPacket) << osc::BeginMessage( "/tuio/2Dbnd") << "source" << source_name << osc::EndMessage;
			// add the blob alive message
			(*fullPacket) << osc::BeginMessage( "/tuio/2Dbnd") << "alive";
			for (std::list<TuioBounds*>::iterator tuioBounds = blobList.begin(); tuioBounds!=blobList.end(); tuioBounds++)
				(*fullPacket) << (int32)((*tuioBounds)->getSessionID());	
			(*fullPacket) << osc::EndMessage;	
		}
		
		float xpos = (*tuioBounds)->getX();
		float xvel = (*tuioBounds)->getXSpeed();
		if (invert_x) {
			xpos = 1 - xpos;
			xvel = -1 * xvel;
		}
		float ypos = (*tuioBounds)->getY();
		float yvel = (*tuioBounds)->getYSpeed();
		if (invert_y) {
			ypos = 1 - ypos;
			yvel = -1 * yvel;
		}
		float angle = (*tuioBounds)->getAngle();
		float rvel = (*tuioBounds)->getRotationSpeed();
		if (invert_a) {
			angle = 2.0f*(float)M_PI - angle;
			rvel = -1 * rvel;
		}		
		
		// add the actual blob set message
		(*fullPacket) << osc::BeginMessage( "/tuio/2Dbnd") << "set";
		(*fullPacket) << (int32)((*tuioBounds)->getSessionID()) << xpos << ypos  << angle << (*tuioBounds)->getWidth() << (*tuioBounds)->getHeight() << (*tuioBounds)->getArea();
		(*fullPacket) << xvel << yvel << rvel << (*tuioBounds)->getMotionAccel() << (*tuioBounds)->getRotationAccel();	
		(*fullPacket) << osc::EndMessage;
		
	}
	// add the immediate fseq message and send the blob packet
	(*fullPacket) << osc::BeginMessage( "/tuio/2Dbnd") << "fseq" << -1 << osc::EndMessage;
	(*fullPacket) << osc::EndBundle;
	deliverOscPacket( fullPacket );
*/
}



