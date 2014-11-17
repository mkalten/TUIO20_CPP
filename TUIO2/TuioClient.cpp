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

#include "TuioClient.h"
#include "UdpReceiver.h"

using namespace TUIO2;
using namespace osc;


TuioClient::TuioClient()
: currentFrame	(-1)
, lateFrame     (false)
, source_count  (0)
, receiver		(NULL)
, local_receiver(true)
{
	receiver = new UdpReceiver();
	initialize();
}

TuioClient::TuioClient(int port)
: currentFrame	(-1)
, lateFrame     (false)
, source_count  (0)
, receiver		(NULL)
, local_receiver(true)
{
	receiver = new UdpReceiver(port);
	initialize();
}

TuioClient::TuioClient(OscReceiver *osc)
: currentFrame	(-1)
, lateFrame     (false)
, source_count  (0)
, receiver		(osc)
, local_receiver(false)
{
	initialize();
}

void TuioClient::initialize()	{	
	receiver->addTuioClient(this);
}

TuioClient::~TuioClient() {
	if (local_receiver) delete receiver;
}

void TuioClient::processOSC( const ReceivedMessage& msg ) {
    try {
        ReceivedMessageArgumentStream args = msg.ArgumentStream();
        
        if( strcmp( msg.AddressPattern(), "/tuio2/frm" ) == 0 ) {

            unsigned int u_fseq;
            int32 fseq,dim;
            TimeTag timetag;
            const char* src_string;
            args >> fseq >> timetag >> src_string >> dim;
            u_fseq = (unsigned int)fseq;
            
            frameTime = TuioTime(timetag);
            frameTime.setFrameID(u_fseq);
            
            // frame sequence
            lateFrame = false;
            if (u_fseq>0) {
                if (u_fseq>currentFrame) currentTime = TuioTime::getSystemTime();
                if ((u_fseq>=currentFrame) || ((currentFrame-u_fseq)>100)) currentFrame = u_fseq;
                else lateFrame = true;
            } else if ((TuioTime::getSystemTime().getTotalMilliseconds()-currentTime.getTotalMilliseconds())>100) {
                currentTime = TuioTime::getSystemTime();
            }
            
            // check if we know that source
            std::string src(src_string);
            std::map<std::string,unsigned int>::iterator iter = sourceList.find(src);
             
             // add a new source
            int source_id = 0;
             if (iter==sourceList.end()) {
                 source_id = source_count;
                 sourceList[src] = source_id;
                 source_count++;
             } else {
             // use the found source_id
                 source_id = sourceList[src];
             }
            
            // frame source
            frameSource.setSourceString(source_id,src_string);

            // frame dimension
            frameSource.setDimension(dim);
            

            
        } else if( strcmp( msg.AddressPattern(), "/tuio2/tok" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id, tu_id, c_id;
            unsigned short t_id, u_id;
            float xpos, ypos, angle, xspeed, yspeed, rspeed, maccel, raccel;
            args >> s_id >> tu_id >> c_id >> xpos >> ypos >> angle >> xspeed >> yspeed >> rspeed >> maccel >> raccel;
            t_id = tu_id >> 16;
            u_id = tu_id & 0x0000FFFF;
            
            TuioObject *tobj = getFrameContainer(frameSource.getSourceID(),(unsigned int)s_id);
            //if (tobj == NULL) std::cout << "new cont " << s_id << " " << frameSource.getSourceID() << std::endl;
            if (tobj == NULL) tobj = new TuioObject(frameTime,&frameSource,(unsigned int)s_id);
            TuioToken *ttok = tobj->getTuioToken();
            if (ttok == NULL) {
                ttok = new TuioToken(frameTime,(unsigned int)s_id,t_id,u_id,c_id,xpos,ypos,angle);
                tobj->setTuioToken(ttok);
                addFrameContainer(tobj);
            } else if ( (ttok->getX()!=xpos) || (ttok->getY()!=ypos) || (ttok->getAngle()!=angle) || (ttok->getXSpeed()!=xspeed) || (ttok->getYSpeed()!=yspeed) || (ttok->getRotationSpeed()!=rspeed) || (ttok->getMotionAccel()!=maccel) || (ttok->getRotationAccel()!=raccel) ) {

                ttok->update(frameTime,xpos,ypos,angle,xspeed,yspeed,rspeed,maccel,raccel);
                tobj->update(frameTime);
                addFrameContainer(tobj);
            }
            
        } else if( strcmp( msg.AddressPattern(), "/tuio2/ptr" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id, tu_id, c_id;
            unsigned short t_id, u_id;
            float xpos, ypos, angle, shear,radius, pressure, xspeed, yspeed, rspeed, maccel, raccel;
            args >> s_id >> tu_id >> c_id >> xpos >> ypos >> angle >> shear >> radius >> pressure >> xspeed >> yspeed >> rspeed >> maccel >> raccel;
            t_id = tu_id >> 16;
            u_id = tu_id & 0x0000FFFF;
            
            TuioObject *tobj = getFrameContainer(frameSource.getSourceID(),(unsigned int)s_id);
            //if (tobj == NULL) std::cout << "new cont " << s_id << " " << frameSource.getSourceID() << std::endl;
            if (tobj == NULL) tobj = new TuioObject(frameTime,&frameSource,(unsigned int)s_id);
            TuioPointer *tptr = tobj->getTuioPointer();
            if (tptr == NULL) {
                tptr = new TuioPointer(frameTime,(unsigned int)s_id,t_id,u_id,c_id,xpos,ypos,angle,shear,radius,pressure);
                tobj->setTuioPointer(tptr);
                addFrameContainer(tobj);
            } else if ( (tptr->getX()!=xpos) || (tptr->getY()!=ypos) || (tptr->getAngle()!=angle) || (tptr->getShear()!=shear) || (tptr->getRadius()!=radius) || (tptr->getPressure()!=pressure) || (tptr->getXSpeed()!=xspeed) || (tptr->getYSpeed()!=yspeed) || (tptr->getMotionAccel()!=maccel) ) {
                
                tptr->update(frameTime,xpos,ypos,angle,shear,radius,pressure,xspeed,yspeed,rspeed,maccel,raccel);
                tobj->update(frameTime);
                addFrameContainer(tobj);
            }
        } else if( strcmp( msg.AddressPattern(), "/tuio2/bnd" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id;
            float xpos, ypos, angle, width, height, area;
            float xspeed, yspeed, rspeed, maccel, raccel;
            args >> s_id >> xpos >> ypos >> angle >> width >> height >> area >> xspeed >> yspeed >> rspeed >> maccel >> raccel;
            
            TuioObject *tobj = getFrameContainer(frameSource.getSourceID(),(unsigned int)s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,&frameSource,(unsigned int)s_id);
            TuioBounds *tbnd = tobj->getTuioBounds();
            if (tbnd == NULL) {
                tbnd = new TuioBounds(frameTime,(unsigned int)s_id,xpos,ypos,angle,width,height,area);
                tobj->setTuioBounds(tbnd);
                addFrameContainer(tobj);
            } else if ( (tbnd->getX()!=xpos) || (tbnd->getY()!=ypos) || (tbnd->getAngle()!=angle) || (tbnd->getWidth()!=width) || (tbnd->getHeight()!=height) || (tbnd->getArea()!=area) || (tbnd->getXSpeed()!=xspeed) || (tbnd->getYSpeed()!=yspeed) || (tbnd->getRotationSpeed()!=rspeed) || (tbnd->getMotionAccel()!=maccel) || (tbnd->getRotationAccel()!=raccel)) {
                
                tbnd->update(frameTime,xpos,ypos,angle,width,height,area,xspeed,yspeed,rspeed,maccel,raccel);
                tobj->update(frameTime);
                addFrameContainer(tobj);
            }
        } else if( strcmp( msg.AddressPattern(), "/tuio2/sym" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id, tu_id, c_id;
            unsigned short t_id, u_id;
            const char* type;
            const char* data;
            args >> s_id >> tu_id >> c_id >> type >> data;
            t_id = tu_id >> 16;
            u_id = tu_id & 0x0000FFFF;
            
            TuioObject *tobj = getFrameContainer(frameSource.getSourceID(),(unsigned int)s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,&frameSource,(unsigned int)s_id);
            TuioSymbol *tsym = tobj->getTuioSymbol();
            if (tsym == NULL) {
                tsym = new TuioSymbol(frameTime,(unsigned int)s_id,t_id,u_id,c_id,type,data);
                tobj->setTuioSymbol(tsym);
                addFrameContainer(tobj);
            } else {
                tsym->update(frameTime);
                tobj->update(frameTime);
                addFrameContainer(tobj);
            }
        } else if( strcmp( msg.AddressPattern(), "/tuio2/alv" ) == 0 ) {
 
            if (lateFrame) return;
            int32 s_id;
            aliveContainerList.clear();
            while(!args.Eos()) {
                args >> s_id;
                aliveContainerList.push_back((unsigned int)s_id);
            }
            
            lockContainerList();
            //find the removed tobjs first
            for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj!=tobjList.end(); tobj++) {
                std::list<unsigned int>::iterator iter = find(aliveContainerList.begin(), aliveContainerList.end(), (*tobj)->getSessionID());
                if (iter == aliveContainerList.end()) {
                    (*tobj)->remove(frameTime);
                    addFrameContainer(*tobj);
                }
            }
            unlockContainerList();
            
            for (std::list<TuioObject*>::iterator iter=frameContainer.begin(); iter!=frameContainer.end(); iter++) {
                TuioObject *tobj = (*iter);
                
                switch (tobj->getTuioState()) {
                    case TUIO_REMOVED:
                        
                        for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener!=listenerList.end(); listener++)
                            (*listener)->tuioRemove(tobj);
                        
                        lockContainerList();
                        for (std::list<TuioObject*>::iterator delcon=tobjList.begin(); delcon!=tobjList.end(); delcon++) {
                            if((*delcon)->getSessionID()==tobj->getSessionID()) {
                                delete *delcon;
                                tobjList.erase(delcon);
                                break;
                            }
                        }
                        unlockContainerList();
                        break;
                    case TUIO_ADDED:
                        
                        //lockContainerList();
                        tobjList.push_back(tobj);
                        //unlockContainerList();
                        for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
                            (*listener)->tuioAdd(tobj);
                        
                        break;
                    default:
                        
                        for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
                            (*listener)->tuioUpdate(tobj);
                }
            }
            
            for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
                (*listener)->tuioRefresh(frameTime);
            
            frameContainer.clear();
        }         
    } catch( Exception& e ){
        std::cerr << "error parsing TUIO2 message: "<< msg.AddressPattern() <<  " - " << e.what() << std::endl;
    }
}

bool TuioClient::isConnected() {	
	return receiver->isConnected();
}

void TuioClient::connect(bool lock) {
			
	//TuioTime::initSession();
	currentTime.reset();
	
	receiver->connect(lock);
	unlockContainerList();
}

void TuioClient::disconnect() {
	
	receiver->disconnect();
	
	aliveContainerList.clear();

	for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++)
		delete (*iter);
	tobjList.clear();
}

TuioObject* TuioClient::getTuioObject(int src_id, unsigned int s_id) {
    lockContainerList();
    for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
        if (((*iter)->getSessionID()==s_id) && ((*iter)->getTuioSource()->getSourceID()==src_id)) {
            unlockContainerList();
            return (*iter);
        }
    }
    unlockContainerList();
    return NULL;
}

TuioToken* TuioClient::getTuioToken(int src_id, unsigned int s_id) {
	lockContainerList();
	for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
		if (((*iter)->getSessionID()==s_id) && ((*iter)->getTuioSource()->getSourceID()==src_id)) {
			unlockContainerList();
            return (*iter)->getTuioToken();
		}
	}	
	unlockContainerList();
	return NULL;
}

TuioPointer* TuioClient::getTuioPointer(int src_id, unsigned int s_id) {
    lockContainerList();
    for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
        if (((*iter)->getSessionID()==s_id) && ((*iter)->getTuioSource()->getSourceID()==src_id)) {
            unlockContainerList();
            return (*iter)->getTuioPointer();
        }
    }
    unlockContainerList();
    return NULL;
}

TuioBounds* TuioClient::getTuioBounds(int src_id, unsigned int s_id) {
    lockContainerList();
    for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
        if (((*iter)->getSessionID()==s_id) && ((*iter)->getTuioSource()->getSourceID()==src_id)) {
            unlockContainerList();
            return (*iter)->getTuioBounds();
        }
    }
    unlockContainerList();
    return NULL;
}

TuioSymbol* TuioClient::getTuioSymbol(int src_id, unsigned int s_id) {
    lockContainerList();
    for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
        if (((*iter)->getSessionID()==s_id) && ((*iter)->getTuioSource()->getSourceID()==src_id)) {
            unlockContainerList();
            return (*iter)->getTuioSymbol();
        }
    }
    unlockContainerList();
    return NULL;
}

std::list<TuioObject*> TuioClient::getTuioObjectList(int src_id) {
    lockContainerList();
    std::list<TuioObject*> listBuffer;
    for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj != tobjList.end(); tobj++) {
        if ((*tobj)->getTuioSource()->getSourceID()==src_id) listBuffer.push_back(*tobj);
    }
    unlockContainerList();
    return listBuffer;
}

std::list<TuioToken*> TuioClient::getTuioTokenList(int src_id) {
    lockContainerList();
    std::list<TuioToken*> listBuffer;
    for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj != tobjList.end(); tobj++) {
        if ((*tobj)->getTuioSource()->getSourceID()==src_id) {
            TuioToken *ttok = (*tobj)->getTuioToken();
            if (ttok!=NULL) listBuffer.push_back(ttok);
        }
    }
    unlockContainerList();
    return listBuffer;
}

std::list<TuioPointer*> TuioClient::getTuioPointerList(int src_id) {
    lockContainerList();
    std::list<TuioPointer*> listBuffer;
    for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj != tobjList.end(); tobj++) {
        if ((*tobj)->getTuioSource()->getSourceID()==src_id) {
            TuioPointer *tptr = (*tobj)->getTuioPointer();
            if (tptr!=NULL) listBuffer.push_back(tptr);
        }
    }
    unlockContainerList();
    return listBuffer;
}

std::list<TuioBounds*> TuioClient::getTuioBoundsList(int src_id) {
    lockContainerList();
    std::list<TuioBounds*> listBuffer;
    for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj != tobjList.end(); tobj++) {
        if ((*tobj)->getTuioSource()->getSourceID()==src_id) {
            TuioBounds *tbnd = (*tobj)->getTuioBounds();
            if (tbnd!=NULL) listBuffer.push_back(tbnd);
        }
    }
    unlockContainerList();
    return listBuffer;
}

std::list<TuioSymbol*> TuioClient::getTuioSymbolList(int src_id) {
    lockContainerList();
    std::list<TuioSymbol*> listBuffer;
    for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj != tobjList.end(); tobj++) {
        if ((*tobj)->getTuioSource()->getSourceID()==src_id) {
            TuioSymbol *tsym = (*tobj)->getTuioSymbol();
            if (tsym!=NULL) listBuffer.push_back(tsym);
        }
    }
    unlockContainerList();
    return listBuffer;
}

void TuioClient::addFrameContainer(TuioObject *tobj) {
    for (std::list<TuioObject*>::iterator iter=frameContainer.begin(); iter != frameContainer.end(); iter++) {
        if ((*iter)->getSessionID()==tobj->getSessionID()) return;
    }
    
    frameContainer.push_back(tobj);
}

TuioObject* TuioClient::getFrameContainer(int src_id, unsigned int s_id) {
    for (std::list<TuioObject*>::iterator tobj=frameContainer.begin(); tobj != frameContainer.end(); tobj++) {
        if ((*tobj)->getSessionID()==s_id) return *tobj;
    }
    
    for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj != tobjList.end(); tobj++) {
        if (((*tobj)->getSessionID()==s_id) && ((*tobj)->getTuioSourceID()==src_id)) return *tobj;
    }
    
    return NULL;
}
