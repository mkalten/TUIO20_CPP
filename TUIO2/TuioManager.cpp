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

#include "TuioManager.h"
using namespace TUIO2;

TuioManager::TuioManager() 
	: currentFrameTime(TuioTime::getSystemTime())
	, currentFrame(-1)
    , pointerCount(0)
	, maxPointerID(-1)
	, sessionID(-1)
	, tobjUpdate(false)
	, verbose(false)
	, invert_x(false)
	, invert_y(false)
	, invert_a(false)
{

}

TuioManager::~TuioManager() {
}

TuioObject* TuioManager::createTuioObject() {
    sessionID++;
    TuioObject *tobj = new TuioObject(sessionID);
    tobjList.push_back(tobj);
    tobjUpdate = true;
    return tobj;
}

void TuioManager::addExternalTuioObject(TuioObject *tobj) {
    if (tobj==NULL) return;
    tobjList.push_back(tobj);
    tobjUpdate = true;
    
    TuioPointer *tptr = tobj->getTuioPointer();
    if (tptr!=NULL) pointerCount++;
    
    for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
        (*listener)->tuioAdd(tobj);
    
    if (verbose) std::cout << "add " << tobj->getSessionID() << std::endl;
}

void TuioManager::updateExternalTuioObject(TuioObject *tobj) {
    if (tobj==NULL) return;
    tobjUpdate = true;
    
    for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
        (*listener)->tuioUpdate(tobj);
        
    if (verbose) std::cout << "set " << tobj->getSessionID() << std::endl;
}

void TuioManager::removeExternalTuioObject(TuioObject *tobj) {
    if (tobj==NULL) return;
    tobjList.remove(tobj);
    tobjUpdate = true;
    
    TuioPointer *tptr = tobj->getTuioPointer();
    if (tptr!=NULL) pointerCount--;
    
    for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
        (*listener)->tuioRemove(tobj);
    
    tobjUpdate = true;
    if (verbose) std::cout << "del " << tobj->getSessionID() << std::endl;
}

void TuioManager::removeTuioObject(unsigned int s_id) {

    TuioObject *tobj = NULL;
    for(std::list<TuioObject*>::iterator iter = tobjList.begin();iter!= tobjList.end(); iter++) {
        if((*iter)->getSessionID()==s_id) {
            tobj = *iter;
            break;
        }
    }
    if (tobj==NULL) return;
    else removeTuioObject(tobj);
}

void TuioManager::removeTuioObject(TuioObject *tobj) {
    
    if (tobj== NULL) return;
    
    for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
        (*listener)->tuioRemove(tobj);
    
    if (tobj->containsTuioToken()) {
        if (verbose) std::cout << "del tok " << tobj->getSessionID() << std::endl;
        tobj->deleteTuioToken();
    }
    
    if (tobj->containsTuioPointer()) {
        if (verbose) std::cout << "del ptr " << tobj->getSessionID() << std::endl;
        tobj->deleteTuioPointer();
    }
    
    if (tobj->containsTuioBounds()) {
        if (verbose) std::cout << "del bnd " << tobj->getSessionID() << std::endl;
        tobj->deleteTuioBounds();
    }
    
    if (tobj->containsTuioSymbol()) {
        if (verbose) std::cout << "del sym " << tobj->getSessionID() << std::endl;
        tobj->deleteTuioSymbol();
    }
    
    tobjList.remove(tobj);
    tobjUpdate = true;
    delete tobj;
}

TuioObject* TuioManager::createTuioToken(int sym, float x, float y, float a) {
    return createTuioToken(sym, 0, 0, x, y, a);
}

TuioObject* TuioManager::createTuioToken(int sym, short t_id, short u_id, float x, float y, float a) {
	sessionID++;
    TuioObject *tobj = new TuioObject(sessionID);
	TuioToken *ttok = new TuioToken(currentFrameTime,sessionID, t_id, u_id, sym, x, y, a);
    tobj->setTuioToken(ttok);
	tobjList.push_back(tobj);
	tobjUpdate = true;

	for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->tuioAdd(tobj);

	if (verbose) std::cout << "add tok " << tobj->getSessionID() << std::endl;
	return tobj;
}

TuioObject* TuioManager::addTuioToken(unsigned int s_id, int sym, float x, float y, float a) {
    return addTuioToken(s_id, sym, 0, 0, x, y, a);
}

TuioObject* TuioManager::addTuioToken(unsigned int s_id, short t_id, short u_id, int sym, float x, float y, float a) {
    TuioObject *tobj = NULL;
    for(std::list<TuioObject*>::iterator iter = tobjList.begin();iter!= tobjList.end(); iter++) {
        if((*iter)->getSessionID()==s_id) {
            tobj = *iter;
            break;
        }
    }
    if (tobj==NULL) return NULL;
    
    TuioToken *ttok = new TuioToken(currentFrameTime,tobj->getSessionID(), t_id, u_id, sym, x, y, a);
    tobj->setTuioToken(ttok);
    tobjUpdate = true;
    
    for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
        (*listener)->tuioUpdate(tobj);
    
    if (verbose) std::cout << "add tok " << tobj->getSessionID() << std::endl;
    return tobj;
}

TuioObject* TuioManager::addTuioToken(TuioToken *ttok) {
    if (ttok==NULL) return NULL;
    
    TuioObject *tobj = NULL;
    for(std::list<TuioObject*>::iterator iter = tobjList.begin();iter!= tobjList.end(); iter++) {
        if((*iter)->getSessionID()==ttok->getSessionID()) {
            tobj = *iter;
            break;
        }
    }
    
    if (tobj!=NULL) tobjUpdate = true;
    else tobj = new TuioObject(ttok->getSessionID());
    tobj->setTuioToken(ttok);

    
    for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++) {
        if (tobjUpdate)(*listener)->tuioUpdate(tobj);
        else(*listener)->tuioAdd(tobj);
    }
    
    if (verbose)  std::cout << "add tok " << tobj->getSessionID() << std::endl;
    tobjUpdate = true;
    
    return tobj;
}

void TuioManager::updateTuioToken(TuioToken *ttok, float x, float y, float a) {
	if (ttok==NULL) return;
	if (ttok->getTuioTime()==currentFrameTime) return;

    TuioObject *tobj = NULL;
    for(std::list<TuioObject*>::iterator iter = tobjList.begin();iter!= tobjList.end(); iter++) {
        if((*iter)->getSessionID()==ttok->getSessionID()) {
            tobj = *iter;
            break;
        }
    }
    
    if (tobj==NULL) return;
    ttok->update(currentFrameTime,x,y,a);
    tobjUpdate = true;

    for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
        (*listener)->tuioUpdate(tobj);
		
    if (verbose)std::cout << "set tok " << tobj->getSessionID() << std::endl;
}

void TuioManager::removeTuioToken(TuioToken *ttok) {
    if (ttok==NULL) return;
    
    TuioObject *tobj = NULL;
    for(std::list<TuioObject*>::iterator iter = tobjList.begin();iter!= tobjList.end(); iter++) {
        if((*iter)->getSessionID()==ttok->getSessionID()) {
            tobj = *iter;
            break;
        }
    }
    
    if (tobj==NULL) {
        delete ttok;
        return;
    } else removeTuioObject(tobj);
}

TuioObject* TuioManager::createTuioPointer(float x, float y, float a, float s, float r, float p) {
    return createTuioPointer(0, 0, x, y, a, s, r, p);
}

TuioObject* TuioManager::addTuioPointer(unsigned int s_id, float x, float y, float a, float s, float r, float p) {
    return addTuioPointer(s_id, 0, 0, x, y, a, s, r, p);
}

TuioObject* TuioManager::createTuioPointer(short t_id, short u_id, float x, float y, float a, float s, float r, float p) {
	sessionID++;
    TuioObject *tobj = new TuioObject(sessionID);
	
	int pointerID = pointerCount;
	if ((pointerCount<=maxPointerID) && (freePointerList.size()>0)) {
        std::list<TuioPointer*>::iterator closestPointer = freePointerList.begin();
		
		for(std::list<TuioPointer*>::iterator iter = freePointerList.begin();iter!= freePointerList.end(); iter++) {
			if((*iter)->getDistance(x,y)<(*closestPointer)->getDistance(x,y)) closestPointer = iter;
		}
		
		TuioPointer *freePointer = (*closestPointer);
        pointerID = (*closestPointer)->getPointerID();
        freePointerList.erase(closestPointer);
        delete freePointer;
	} else maxPointerID = pointerID;	
	
	TuioPointer *tptr = new TuioPointer(currentFrameTime, sessionID, t_id, u_id, pointerID, x, y, a, s, r, p);
    tobj->setTuioPointer(tptr);
    pointerCount++;
	tobjList.push_back(tobj);
	tobjUpdate = true;

	for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->tuioAdd(tobj);
	
	if (verbose)std::cout << "add ptr " << tobj->getSessionID() << std::endl;
	return tobj;
}

TuioObject* TuioManager::addTuioPointer(unsigned int s_id, short t_id, short u_id, float x, float y, float a, float s, float r, float p) {
    TuioObject *tobj = NULL;
    for(std::list<TuioObject*>::iterator iter = tobjList.begin();iter!= tobjList.end(); iter++) {
        if((*iter)->getSessionID()==s_id) {
            tobj = *iter;
            break;
        }
    }
    
    if (tobj==NULL) return NULL;
    int pointerID = pointerCount;
    if ((pointerCount<=maxPointerID) && (freePointerList.size()>0)) {
        std::list<TuioPointer*>::iterator closestPointer = freePointerList.begin();
        
        for(std::list<TuioPointer*>::iterator iter = freePointerList.begin();iter!= freePointerList.end(); iter++) {
            if((*iter)->getDistance(x,y)<(*closestPointer)->getDistance(x,y)) closestPointer = iter;
        }
        
        TuioPointer *freePointer = (*closestPointer);
        pointerID = (*closestPointer)->getPointerID();
        freePointerList.erase(closestPointer);
        delete freePointer;
    } else maxPointerID = pointerID;
    
    TuioPointer *tptr = new TuioPointer(currentFrameTime, sessionID, t_id, u_id, pointerID, x, y, a, s, r, p);
    tobj->setTuioPointer(tptr);
    pointerCount++;
    tobjUpdate = true;
    
    for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
        (*listener)->tuioAdd(tobj);
    
    if (verbose)std::cout << "add ptr " << tobj->getSessionID() << std::endl;
    return tobj;
}


TuioObject* TuioManager::createTuioPointer(int p_id, float x, float y, float a, float s, float r, float p) {

    return createTuioPointer(p_id, 0, 0, x, y, a, s, r, p);
}

TuioObject* TuioManager::addTuioPointer(unsigned int s_id, int p_id, float x, float y, float a, float s, float r, float p) {
    
    return addTuioPointer(s_id, p_id, 0, 0, x, y, a, s, r, p);
}

TuioObject* TuioManager::createTuioPointer(int p_id, short t_id, short u_id, float x, float y, float a, float s, float r, float p) {
    sessionID++;
    TuioObject *tobj = new TuioObject(sessionID);
    
    TuioPointer *tptr = new TuioPointer(currentFrameTime, sessionID, t_id, u_id, p_id, x, y, a, s, r, p);
    tobj->setTuioPointer(tptr);
    pointerCount++;
    tobjList.push_back(tobj);
    tobjUpdate = true;
    
    for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
        (*listener)->tuioAdd(tobj);
    
    if (verbose)std::cout << "add ptr " << tobj->getSessionID() << std::endl;
    return tobj;
}

TuioObject* TuioManager::addTuioPointer(unsigned int s_id, int p_id, short t_id, short u_id, float x, float y, float a, float s, float r, float p) {
    TuioObject *tobj = NULL;
    for(std::list<TuioObject*>::iterator iter = tobjList.begin();iter!= tobjList.end(); iter++) {
        if((*iter)->getSessionID()==s_id) {
            tobj = *iter;
            break;
        }
    }
    
    if (tobj==NULL) return NULL;
    TuioPointer *tptr = new TuioPointer(currentFrameTime, sessionID, t_id, u_id, p_id, x, y, a, s, r, p);
    tobj->setTuioPointer(tptr);
    pointerCount++;
    tobjUpdate = true;
    
    for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
        (*listener)->tuioAdd(tobj);
    
    if (verbose)std::cout << "add ptr " << tobj->getSessionID() << std::endl;
    return tobj;
}

TuioObject* TuioManager::addTuioPointer(TuioPointer *tptr) {
	if (tptr==NULL) return NULL;
    
    TuioObject *tobj = NULL;
    for(std::list<TuioObject*>::iterator iter = tobjList.begin();iter!= tobjList.end(); iter++) {
        if((*iter)->getSessionID()==tptr->getSessionID()) {
            tobj = *iter;
            break;
        }
    }
    
    if (tobj!=NULL) tobjUpdate = true;
    else tobj = new TuioObject(tptr->getSessionID());
    tobj->setTuioPointer(tptr);
    pointerCount++;

    for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++) {
        if (tobjUpdate)(*listener)->tuioUpdate(tobj);
        else(*listener)->tuioAdd(tobj);
    }
    
    if (verbose) std::cout << "add ptr " << tobj->getSessionID() << std::endl;
    tobjUpdate = true;
    
    return tobj;
}

void TuioManager::updateTuioPointer(TuioPointer *tptr, float x, float y, float a, float s, float r, float p) {
	if (tptr==NULL) return;
	if (tptr->getTuioTime()==currentFrameTime) return;
    
    TuioObject *tobj = NULL;
    for(std::list<TuioObject*>::iterator iter = tobjList.begin();iter!= tobjList.end(); iter++) {
        if((*iter)->getSessionID()==tptr->getSessionID()) {
            tobj = *iter;
            break;
        }
    }
    
    if (tobj==NULL) return;
	tptr->update(currentFrameTime,x,y,a,s,r,p);
	tobjUpdate = true;

    for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
        (*listener)->tuioUpdate(tobj);

    if (verbose)std::cout << "set ptr " << tobj->getSessionID() << std::endl;
}

void TuioManager::removeTuioPointer(TuioPointer *tptr) {
	if (tptr==NULL) return;
    pointerCount--;
    
    TuioObject *tobj = NULL;
    for(std::list<TuioObject*>::iterator iter = tobjList.begin();iter!= tobjList.end(); iter++) {
        if((*iter)->getSessionID()==tptr->getSessionID()) {
            tobj = *iter;
            break;
        }
    }
    
    if (tptr->getPointerID()==maxPointerID) {
        maxPointerID = -1;
        
        if (pointerCount>0) {
        
            for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj != tobjList.end(); tobj++) {
                if ((*tobj)->containsTuioPointer()) {
                    int pointerID = (*tobj)->getTuioPointer()->getPointerID();
                    if (pointerID>maxPointerID) maxPointerID=pointerID;
                }
            }
            
            freePointerBuffer.clear();
            for (std::list<TuioPointer*>::iterator flist=freePointerList.begin(); flist != freePointerList.end(); flist++) {
                TuioPointer *freePointer = (*flist);
                if (freePointer->getPointerID()<=maxPointerID) freePointerBuffer.push_back(new TuioPointer(freePointer));
            }
            
            freePointerList = freePointerBuffer;
            
        } else {
            for (std::list<TuioPointer*>::iterator flist=freePointerList.begin(); flist != freePointerList.end(); flist++) {
                TuioPointer *freePointer = (*flist);
                delete freePointer;
            }
            freePointerList.clear();
        }
    } else if (tptr->getPointerID()<maxPointerID) {
        freePointerList.push_back(new TuioPointer(tptr));
    }
    
    if (tobj==NULL) {
        delete tptr;
        return;
    } else removeTuioObject(tobj);
    

}

TuioObject* TuioManager::createTuioBounds(float x, float y, float a, float w, float h, float f) {
    sessionID++;
    TuioObject *tobj = new TuioObject(sessionID);
    
    TuioBounds *tbnd = new TuioBounds(currentFrameTime, sessionID, x, y, a, w, h, f);
    tobj->setTuioBounds(tbnd);
    pointerCount++;
    tobjList.push_back(tobj);
    tobjUpdate = true;
    
    for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
        (*listener)->tuioAdd(tobj);
    
    if (verbose)std::cout << "add bnd " << tobj->getSessionID() << std::endl;
    
    return tobj;
}

TuioObject* TuioManager::addTuioBounds(unsigned int s_id, float x, float y, float a, float w, float h, float f) {
    TuioObject *tobj = NULL;
    for(std::list<TuioObject*>::iterator iter = tobjList.begin();iter!= tobjList.end(); iter++) {
        if((*iter)->getSessionID()==s_id) {
            tobj = *iter;
            break;
        }
    }
    if (tobj==NULL) return NULL;
	
	TuioBounds *tbnd = new TuioBounds(currentFrameTime, sessionID, x, y, a, w, h, f);
    tobj->setTuioBounds(tbnd);
    pointerCount++;
    tobjUpdate = true;
	
    for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
        (*listener)->tuioAdd(tobj);
    
    if (verbose)std::cout << "add bnd " << tobj->getSessionID() << std::endl;
	
	return tobj;
}

TuioObject* TuioManager::addTuioBounds(TuioBounds *tbnd) {
	if (tbnd==NULL) return NULL;

    TuioObject *tobj = NULL;
    for(std::list<TuioObject*>::iterator iter = tobjList.begin();iter!= tobjList.end(); iter++) {
        if((*iter)->getSessionID()==tbnd->getSessionID()) {
            tobj = *iter;
            break;
        }
    }
    
    if (tobj!=NULL) tobjUpdate = true;
    else tobj = new TuioObject(tbnd->getSessionID());
    tobj->setTuioBounds(tbnd);
	
    for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++) {
        if (tobjUpdate)(*listener)->tuioUpdate(tobj);
        else(*listener)->tuioAdd(tobj);
    }
    
    if (verbose) std::cout << "add bnd " << tobj->getSessionID() << std::endl;
    tobjUpdate = true;
    
    return tobj;
}

void TuioManager::updateTuioBounds(TuioBounds *tbnd, float x, float y, float a, float w, float h, float f) {
	if (tbnd==NULL) return;
	if (tbnd->getTuioTime()==currentFrameTime) return;
    
    
    TuioObject *tobj = NULL;
    for(std::list<TuioObject*>::iterator iter = tobjList.begin();iter!= tobjList.end(); iter++) {
        if((*iter)->getSessionID()==tbnd->getSessionID()) {
            tobj = *iter;
            break;
        }
    }
    
	tbnd->update(currentFrameTime,x,y,a,w,h,f);
    tobjUpdate = true;
    
    for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
        (*listener)->tuioUpdate(tobj);
    
    if (verbose)std::cout << "set bnd " << tobj->getSessionID() << std::endl;
}

void TuioManager::removeTuioBounds(TuioBounds *tbnd) {
	if (tbnd==NULL) return;
	
    TuioObject *tobj = NULL;
    for(std::list<TuioObject*>::iterator iter = tobjList.begin();iter!= tobjList.end(); iter++) {
        if((*iter)->getSessionID()==tbnd->getSessionID()) {
            tobj = *iter;
            break;
        }
    }
    
    if (tobj==NULL) {
        delete tbnd;
        return;
    } else removeTuioObject(tobj);
}

TuioObject* TuioManager::createTuioSymbol(short t_id, short u_id, int sym, const char *type, const char *data) {
    sessionID++;
    TuioObject *tobj = new TuioObject(sessionID);
    
    TuioSymbol *tsym = new TuioSymbol(currentFrameTime, sessionID, t_id, u_id, sym, type, data);
    tobj->setTuioSymbol(tsym);
    pointerCount++;
    tobjList.push_back(tobj);
    tobjUpdate = true;
    
    for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
        (*listener)->tuioAdd(tobj);
    
    if (verbose)std::cout << "add sym " << tobj->getSessionID() << std::endl;
    
    return tobj;
}

TuioObject* TuioManager::addTuioSymbol(unsigned int s_id, short t_id, short u_id, int sym, const char *type, const char *data) {
    TuioObject *tobj = NULL;
    for(std::list<TuioObject*>::iterator iter = tobjList.begin();iter!= tobjList.end(); iter++) {
        if((*iter)->getSessionID()==s_id) {
            tobj = *iter;
            break;
        }
    }
    if (tobj==NULL) return NULL;
    
    TuioSymbol *tsym = new TuioSymbol(currentFrameTime, sessionID, t_id, u_id, sym, type, data);
    tobj->setTuioSymbol(tsym);
    pointerCount++;
    tobjUpdate = true;
    
    for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
        (*listener)->tuioAdd(tobj);
    
    if (verbose)std::cout << "add sym " << tobj->getSessionID() << std::endl;
    
    return tobj;
}

TuioObject* TuioManager::addTuioSymbol(TuioSymbol *tsym) {
    if (tsym==NULL) return NULL;
    
    TuioObject *tobj = NULL;
    for(std::list<TuioObject*>::iterator iter = tobjList.begin();iter!= tobjList.end(); iter++) {
        if((*iter)->getSessionID()==tsym->getSessionID()) {
            tobj = *iter;
            break;
        }
    }
    
    if (tobj!=NULL) tobjUpdate = true;
    else tobj = new TuioObject(tsym->getSessionID());
    tobj->setTuioSymbol(tsym);
    
    for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++) {
        if (tobjUpdate)(*listener)->tuioUpdate(tobj);
        else(*listener)->tuioAdd(tobj);
    }
    
    if (verbose) std::cout << "add sym " << tobj->getSessionID() << std::endl;
    tobjUpdate = true;
    
    return tobj;
}

void TuioManager::removeTuioSymbol(TuioSymbol *tsym) {
    if (tsym==NULL) return;
    
    TuioObject *tobj = NULL;
    for(std::list<TuioObject*>::iterator iter = tobjList.begin();iter!= tobjList.end(); iter++) {
        if((*iter)->getSessionID()==tsym->getSessionID()) {
            tobj = *iter;
            break;
        }
    }
    
    if (tobj==NULL) {
        delete tsym;
        return;
    } else removeTuioObject(tobj);
}

int TuioManager::getSessionID() {
	sessionID++;
    if (sessionID==UINT_MAX) sessionID = 0;
	return sessionID;
}

int TuioManager::getFrameID() {
	return currentFrame;
}

TuioTime TuioManager::getFrameTime() {
	return currentFrameTime;
}

void TuioManager::initTuioFrame(TuioTime ttime) {
    frameTimeTag = TuioTime::getSystemTimeTag();
	currentFrameTime = TuioTime(ttime);
	currentFrame++;
    if (currentFrame==UINT_MAX) currentFrame = 0;
}

void TuioManager::commitTuioFrame() {
	for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
		(*listener)->tuioRefresh(currentFrameTime);
    
    if (verbose) std::cout << "refresh " << currentFrame << " " << currentFrameTime.getTotalMilliseconds() << std::endl;
}

TuioToken* TuioManager::getClosestTuioToken(float xp, float yp) {
	
	TuioToken *closestToken = NULL;
	float closestDistance = 1.0f;
	
	for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
        TuioToken *ttok = (*iter)->getTuioToken();
        if (ttok==NULL) continue;
		float distance = ttok->getDistance(xp,yp);
		if(distance<closestDistance) {
			closestToken = ttok;
			closestDistance = distance;
		}
	}
	
	return closestToken;
}

TuioPointer* TuioManager::getClosestTuioPointer(float xp, float yp) {

	TuioPointer *closestPointer = NULL;
	float closestDistance = 1.0f;

    for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
        TuioPointer *tptr = (*iter)->getTuioPointer();
        if (tptr==NULL) continue;
		float distance = tptr->getDistance(xp,yp);
		if(distance<closestDistance) {
			closestPointer = tptr;
			closestDistance = distance;
		}
	}
	
	return closestPointer;
}

TuioBounds* TuioManager::getClosestTuioBounds(float xp, float yp) {
	
	TuioBounds *closestBounds = NULL;
	float closestDistance = 1.0f;
	
    for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter!=tobjList.end(); iter++) {
        TuioBounds *tbnd = (*iter)->getTuioBounds();
        if (tbnd==NULL) continue;
		float distance = tbnd->getDistance(xp,yp);
		if(distance<closestDistance) {
			closestBounds = tbnd;
			closestDistance = distance;
		}
	}
	
	return closestBounds;
}

std::list<TuioObject*> TuioManager::getUntouchedObjects() {
	
	std::list<TuioObject*> untouched;
	for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj!=tobjList.end(); tobj++) {
		
        bool touched = false;
        if ((*tobj)->containsTuioToken()) {
            if ((*tobj)->getTuioToken()->getTuioTime()==currentFrameTime) touched=true;
        }
        
        if ((*tobj)->containsTuioPointer()) {
            if ((*tobj)->getTuioPointer()->getTuioTime()==currentFrameTime) touched=true;
        }
        
        if ((*tobj)->containsTuioBounds()) {
            if ((*tobj)->getTuioToken()->getTuioTime()==currentFrameTime) touched=true;
        }
        
        if ((*tobj)->containsTuioSymbol()) {
            if ((*tobj)->getTuioSymbol()->getTuioTime()==currentFrameTime) touched=true;
        }
        
        if (!touched) untouched.push_back(*tobj);
		
	}	
	return untouched;
}

void TuioManager::stopUntouchedMovingObjects() {
	
	for (std::list<TuioObject*>::iterator tobj = tobjList.begin(); tobj!=tobjList.end(); tobj++) {
        
        if ((*tobj)->containsTuioToken()) {
            TuioToken *ttok = (*tobj)->getTuioToken();
            if ((ttok->getTuioTime()!=currentFrameTime) && (ttok->isMoving())) {
                ttok->stop(currentFrameTime);
                if (verbose) std::cout << "set tok " << ttok->getSessionID()  << std::endl;
                tobjUpdate = true;
            }
		}
        
        if ((*tobj)->containsTuioPointer()) {
            TuioPointer *tptr = (*tobj)->getTuioPointer();
            if ((tptr->getTuioTime()!=currentFrameTime) && (tptr->isMoving())) {
                tptr->stop(currentFrameTime);
                if (verbose) std::cout << "set ptr " << tptr->getSessionID()  << std::endl;
                tobjUpdate = true;
            }
        }
        
        if ((*tobj)->containsTuioBounds()) {
            TuioBounds *tbnd = (*tobj)->getTuioBounds();
            if ((tbnd->getTuioTime()!=currentFrameTime) && (tbnd->isMoving())) {
                tbnd->stop(currentFrameTime);
                if (verbose) std::cout << "set bnd " << tbnd->getSessionID()  << std::endl;
                tobjUpdate = true;
            }
        }
        
	}
}

void TuioManager::removeUntouchedStoppedObjects() {
	std::list<TuioObject*>::iterator iter = tobjList.begin();
	while (iter!=tobjList.end()) {
		TuioObject *tobj = (*iter);
		if ((tobj->getTuioTime()!=currentFrameTime) && (!tobj->isMoving())) {
			removeTuioObject(tobj);
			iter = tobjList.begin();
		} else iter++;
	}
}

void TuioManager::resetTuioObjectList() {
	
    pointerCount=0;
	std::list<TuioObject*>::iterator tobj = tobjList.begin();
	while (tobj!=tobjList.end()) {
		removeTuioObject((*tobj));
		tobj = tobjList.begin();
	}
}
