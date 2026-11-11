/*
 TUIO2 C++ Library
 Copyright (c) 2009-2026 Martin Kaltenbrunner <martin@tuio.org>
 
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

TuioServer::TuioServer(const char *host, unsigned short port)
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

void TuioServer::setDimension(unsigned short w, unsigned short h) {
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

            if ((*tobj)->containsTuioGeometry()) {
                TuioGeometry *tgeo = (*tobj)->getTuioGeometry();
                if  ((full_update) || (tgeo->getTuioTime()==currentFrameTime)) addGeometryMessage(tgeo);
            }

            if ((*tobj)->containsTuioControl()) {
                TuioControl *tctl = (*tobj)->getTuioControl();
                if  ((full_update) || (tctl->getTuioTime()==currentFrameTime)) addControlMessage(tctl);
            }

            if ((*tobj)->containsTuioData()) {
                TuioData *tdat = (*tobj)->getTuioData();
                if  ((full_update) || (tdat->getTuioTime()==currentFrameTime)) addDataMessage(tdat);
            }

            if ((*tobj)->containsTuioSignal()) {
                TuioSignal *tsig = (*tobj)->getTuioSignal();
                if  ((full_update) || (tsig->getTuioTime()==currentFrameTime)) addSignalMessage(tsig);
            }

            if ((*tobj)->containsTuioToken3D()) {
                TuioToken3D *ttok = (*tobj)->getTuioToken3D();
                if  ((full_update) || (ttok->getTuioTime()==currentFrameTime)) addToken3DMessage(ttok);
            }

            if ((*tobj)->containsTuioPointer3D()) {
                TuioPointer3D *tptr = (*tobj)->getTuioPointer3D();
                if  ((full_update) || (tptr->getTuioTime()==currentFrameTime)) addPointer3DMessage(tptr);
            }

            if ((*tobj)->containsTuioBounds3D()) {
                TuioBounds3D *tbnd = (*tobj)->getTuioBounds3D();
                if  ((full_update) || (tbnd->getTuioTime()==currentFrameTime)) addBounds3DMessage(tbnd);
            }

            if ((*tobj)->containsTuioAssociation()) {
                std::list<TuioAssociation*> assocs = (*tobj)->getTuioAssociationList();
                for (std::list<TuioAssociation*>::iterator it = assocs.begin(); it != assocs.end(); it++)
                    if  ((full_update) || ((*it)->getTuioTime()==currentFrameTime)) addAssociationMessage(*it);
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
    (*oscPacket) << osc::BeginMessage( "/tuio2/frm") << 0 << TuioTime::getSystemTimeTag() << (int32)source->getDimension() << source->getSourceName() << osc::EndMessage;
    (*oscPacket) << osc::BeginMessage( "/tuio2/alv") << osc::EndMessage;
    (*oscPacket) << osc::EndBundle;
    deliverOscPacket( oscPacket );
}

void TuioServer::sendFullTuioBundle() {

}

void TuioServer::checkBundleCapacity(int msg_size) {

    unsigned long size = msg_size + ALV_MESSAGE_SIZE + 4*tobjList.size();

    if ((oscPacket->Capacity()-oscPacket->Size())<size) {
        (*oscPacket) << osc::EndBundle;
        deliverOscPacket( oscPacket ); // send the intermediate bundle without final ALV message!
        startTuioBundle(currentFrame);
    }
}

void TuioServer::startTuioBundle(unsigned int fseq) {

    oscPacket->Clear();
    (*oscPacket) << osc::BeginBundleImmediate;
    if (source) (*oscPacket) << osc::BeginMessage( "/tuio2/frm") << (int32)currentFrame << frameTimeTag << (int32)source->getDimension() << source->getSourceName();
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
	(*oscPacket) << (int32)ttok->getSessionID() << (int32)ttok->getTypeUserID() << (int32)ttok->getComponentID() << xpos << ypos << angle;
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
    (*oscPacket) << (int32)tptr->getSessionID() << (int32)tptr->getTypeUserID() << (int32)tptr->getComponentID();
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
    (*oscPacket) << (int32)tsym->getSessionID() << (int32)tsym->getTypeUserID() << (int32)tsym->getComponentID();
    (*oscPacket) << tsym->getSymbolType();
    if (tsym->isSymbolDataBlob()) {
        int bsize;
        const void *bdata = tsym->getSymbolDataBlob(&bsize);
        (*oscPacket) << osc::Blob(bdata,bsize);
    } else (*oscPacket) << tsym->getSymbolData();
    (*oscPacket) << osc::EndMessage;
}

void TuioServer::addGeometryMessage(TuioGeometry *tgeo) {

    std::list<TuioPoint> hull = tgeo->getConvexHull();
    if (!hull.empty()) {
        (*oscPacket) << osc::BeginMessage( "/tuio2/chg") << (int32)tgeo->getSessionID();
        for (std::list<TuioPoint>::iterator p = hull.begin(); p != hull.end(); p++)
            (*oscPacket) << p->getX() << p->getY();
        (*oscPacket) << osc::EndMessage;
    }

    std::list<TuioPoint> outer = tgeo->getOuterContour();
    if (!outer.empty()) {
        (*oscPacket) << osc::BeginMessage( "/tuio2/ocg") << (int32)tgeo->getSessionID();
        for (std::list<TuioPoint>::iterator p = outer.begin(); p != outer.end(); p++)
            (*oscPacket) << p->getX() << p->getY();
        (*oscPacket) << osc::EndMessage;
    }

    std::list< std::list<TuioPoint> > inner = tgeo->getInnerContours();
    if (!inner.empty()) {
        (*oscPacket) << osc::BeginMessage( "/tuio2/icg") << (int32)tgeo->getSessionID();
        bool first = true;
        for (std::list< std::list<TuioPoint> >::iterator c = inner.begin(); c != inner.end(); c++) {
            if (!first) (*oscPacket) << true;
            for (std::list<TuioPoint>::iterator p = c->begin(); p != c->end(); p++)
                (*oscPacket) << p->getX() << p->getY();
            first = false;
        }
        (*oscPacket) << osc::EndMessage;
    }

    std::list< std::list<TuioPoint> > skltn = tgeo->getSkeleton();
    std::list<unsigned int> jumps = tgeo->getSkeletonJumps();
    if (!skltn.empty()) {
        (*oscPacket) << osc::BeginMessage( "/tuio2/skg") << (int32)tgeo->getSessionID();
        std::list<unsigned int>::iterator j = jumps.begin();
        bool first = true;
        for (std::list< std::list<TuioPoint> >::iterator b = skltn.begin(); b != skltn.end(); b++) {
            if (!first && j != jumps.end()) (*oscPacket) << (int32)(*j++);
            for (std::list<TuioPoint>::iterator p = b->begin(); p != b->end(); p++)
                (*oscPacket) << p->getX() << p->getY();
            first = false;
        }
        (*oscPacket) << osc::EndMessage;
    }

    std::list< std::list<float> > skltn3d = tgeo->getSkeleton3D();
    std::list<unsigned int> jumps3d = tgeo->getSkeletonJumps3D();
    if (!skltn3d.empty()) {
        (*oscPacket) << osc::BeginMessage( "/tuio2/s3d") << (int32)tgeo->getSessionID();
        std::list<unsigned int>::iterator j = jumps3d.begin();
        bool first = true;
        for (std::list< std::list<float> >::iterator b = skltn3d.begin(); b != skltn3d.end(); b++) {
            if (!first && j != jumps3d.end()) (*oscPacket) << (int32)(*j++);
            for (std::list<float>::iterator p = b->begin(); p != b->end(); p++)
                (*oscPacket) << (*p);
            first = false;
        }
        (*oscPacket) << osc::EndMessage;
    }

    std::list<float> vols = tgeo->getSkeletonVolumes();
    if (!vols.empty()) {
        (*oscPacket) << osc::BeginMessage( "/tuio2/svg") << (int32)tgeo->getSessionID();
        for (std::list<float>::iterator v = vols.begin(); v != vols.end(); v++)
            (*oscPacket) << (*v);
        (*oscPacket) << osc::EndMessage;
    }

    std::list<TuioPoint> points = tgeo->getArea();
    std::list<float> widths = tgeo->getAreaWidths();
    if (!points.empty()) {
        (*oscPacket) << osc::BeginMessage( "/tuio2/arg") << (int32)tgeo->getSessionID();
        std::list<float>::iterator w = widths.begin();
        for (std::list<TuioPoint>::iterator p = points.begin(); p != points.end(); p++) {
            (*oscPacket) << p->getX() << p->getY();
            if (w != widths.end()) (*oscPacket) << (*w++);
            else (*oscPacket) << 0.0f;
        }
        (*oscPacket) << osc::EndMessage;
    }

    std::string rawData = tgeo->getRawData();
    if (!rawData.empty()) {
        (*oscPacket) << osc::BeginMessage( "/tuio2/raw") << (int32)tgeo->getSessionID() << (int32)tgeo->getRawWidth()
            << osc::Blob(rawData.data(),(osc_bundle_element_size_t)rawData.size());
        (*oscPacket) << osc::EndMessage;
    }
}

void TuioServer::addControlMessage(TuioControl *tctl) {

    std::list< std::pair<bool,float> > controls = tctl->getControls();
    if (controls.empty()) return;

    (*oscPacket) << osc::BeginMessage( "/tuio2/ctl") << (int32)tctl->getSessionID();
    for (std::list< std::pair<bool,float> >::iterator it = controls.begin(); it != controls.end(); it++) {
        if (it->first) (*oscPacket) << (it->second != 0.0f);
        else (*oscPacket) << it->second;
    }
    (*oscPacket) << osc::EndMessage;
}

void TuioServer::addDataMessage(TuioData *tdat) {

    (*oscPacket) << osc::BeginMessage( "/tuio2/dat") << (int32)tdat->getSessionID() << tdat->getMimeType();
    if (tdat->isDataBlob()) {
        int bsize;
        const void *bdata = tdat->getDataBlob(&bsize);
        (*oscPacket) << osc::Blob(bdata,bsize);
    } else (*oscPacket) << tdat->getDataContent();
    (*oscPacket) << osc::EndMessage;
}

void TuioServer::addSignalMessage(TuioSignal *tsig) {

    (*oscPacket) << osc::BeginMessage( "/tuio2/sig") << (int32)tsig->getSessionID() << (int32)tsig->getComponentID();
    std::list<unsigned int> targets = tsig->getSignalTargets();
    for (std::list<unsigned int>::iterator it = targets.begin(); it != targets.end(); it++)
        (*oscPacket) << (int32)(*it);
    (*oscPacket) << osc::EndMessage;
}

void TuioServer::addAssociationMessage(TuioAssociation *tasc) {

    TuioContainer *tcnr = dynamic_cast<TuioContainer*>(tasc);
    if (tcnr != NULL) {
        (*oscPacket) << osc::BeginMessage( "/tuio2/coa") << (int32)tasc->getSessionID() << (int32)tcnr->getSlot();
        std::list<unsigned int> contained = tcnr->getContained();
        for (std::list<unsigned int>::iterator it = contained.begin(); it != contained.end(); it++)
            (*oscPacket) << (int32)(*it);
        (*oscPacket) << osc::EndMessage;
        return;
    }

    TuioLink *tlnk = dynamic_cast<TuioLink*>(tasc);
    if (tlnk != NULL) {
        (*oscPacket) << osc::BeginMessage( "/tuio2/lia") << (int32)tasc->getSessionID() << tlnk->isPhysical();
        std::list< std::pair<unsigned int,unsigned int> > links = tlnk->getLinks();
        for (std::list< std::pair<unsigned int,unsigned int> >::iterator it = links.begin(); it != links.end(); it++)
            (*oscPacket) << (int32)it->first << (int32)it->second;
        (*oscPacket) << osc::EndMessage;
        return;
    }

    TuioTree *ttre = dynamic_cast<TuioTree*>(tasc);
    if (ttre != NULL) {
        (*oscPacket) << osc::BeginMessage( "/tuio2/lta") << (int32)tasc->getSessionID() << ttre->isPhysical();
        std::list< std::pair<unsigned int,unsigned int> > links = ttre->getLinks();
        std::list<float> jumps = ttre->getNodeJumps();
        std::list<float>::iterator j = jumps.begin();
        std::list< std::pair<unsigned int,unsigned int> >::iterator it = links.begin();
        while (it != links.end() || j != jumps.end()) {
            if (j != jumps.end()) { (*oscPacket) << (*j++); continue; }
            (*oscPacket) << (int32)it->first << (int32)it->second; it++;
        }
        (*oscPacket) << osc::EndMessage;
        return;
    }

    TuioList *tlst = dynamic_cast<TuioList*>(tasc);
    if (tlst != NULL) {
        (*oscPacket) << osc::BeginMessage( "/tuio2/lla") << (int32)tasc->getSessionID() << tlst->isPhysical();
        std::list< std::pair<unsigned int,unsigned int> > links = tlst->getLinks();
        for (std::list< std::pair<unsigned int,unsigned int> >::iterator it = links.begin(); it != links.end(); it++)
            (*oscPacket) << (int32)it->first << (int32)it->second;
        (*oscPacket) << osc::EndMessage;
        return;
    }
}

void TuioServer::addToken3DMessage(TuioToken3D *ttok) {

    (*oscPacket) << osc::BeginMessage( "/tuio2/t3d");
    (*oscPacket) << (int32)ttok->getSessionID() << (int32)ttok->getTypeUserID() << (int32)ttok->getComponentID()
        << ttok->getX() << ttok->getY() << ttok->getZ() << ttok->getAngle()
        << ttok->getXAxis() << ttok->getYAxis() << ttok->getZAxis()
        << ttok->getXSpeed() << ttok->getYSpeed() << ttok->getZSpeed()
        << ttok->getRotationSpeed() << ttok->getMotionAccel() << ttok->getRotationAccel();
    (*oscPacket) << osc::EndMessage;
}

void TuioServer::addPointer3DMessage(TuioPointer3D *tptr) {

    (*oscPacket) << osc::BeginMessage( "/tuio2/p3d");
    (*oscPacket) << (int32)tptr->getSessionID() << (int32)tptr->getTypeUserID() << (int32)tptr->getComponentID()
        << tptr->getX() << tptr->getY() << tptr->getZ()
        << tptr->getXAxis() << tptr->getYAxis() << tptr->getZAxis() << tptr->getRadius()
        << tptr->getXSpeed() << tptr->getYSpeed() << tptr->getZSpeed()
        << tptr->getRotationSpeed() << tptr->getMotionAccel() << tptr->getRotationAccel();
    (*oscPacket) << osc::EndMessage;
}

void TuioServer::addBounds3DMessage(TuioBounds3D *tbnd) {

    (*oscPacket) << osc::BeginMessage( "/tuio2/b3d");
    (*oscPacket) << (int32)tbnd->getSessionID()
        << tbnd->getX() << tbnd->getY() << tbnd->getZ() << tbnd->getAngle()
        << tbnd->getXAxis() << tbnd->getYAxis() << tbnd->getZAxis()
        << tbnd->getWidth() << tbnd->getHeight() << tbnd->getDepth() << tbnd->getVolume()
        << tbnd->getXSpeed() << tbnd->getYSpeed() << tbnd->getZSpeed()
        << tbnd->getRotationSpeed() << tbnd->getMotionAccel() << tbnd->getRotationAccel();
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
