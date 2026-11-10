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

#include "TuioDispatcher.h"

#include <iostream>
#include <list>
#include <algorithm>
#include <cstring>

using namespace TUIO2;

TuioDispatcher::TuioDispatcher() {
#ifdef WIN32
	tobjMutex = CreateMutex(NULL,FALSE,"tobjMutex");
#else
	pthread_mutex_init(&tobjMutex,NULL);
#endif
}

TuioDispatcher::~TuioDispatcher() {
#ifdef WIN32
	CloseHandle(tobjMutex);
#else
	pthread_mutex_destroy(&tobjMutex);
#endif
}

void TuioDispatcher::lockObjectList() {
#ifdef WIN32
	WaitForSingleObject(tobjMutex, INFINITE);
#else
	pthread_mutex_lock(&tobjMutex);
#endif
}

void TuioDispatcher::unlockObjectList() {
#ifdef WIN32
	ReleaseMutex(tobjMutex);
#else
	pthread_mutex_unlock(&tobjMutex);
#endif
}

void TuioDispatcher::addTuioListener(TuioListener *listener) {
	listenerList.push_back(listener);
}

void TuioDispatcher::removeTuioListener(TuioListener *listener) {
	std::list<TuioListener*>::iterator result = find(listenerList.begin(),listenerList.end(),listener);
	if (result!=listenerList.end()) listenerList.remove(listener);
}

void TuioDispatcher::removeAllTuioListeners() {	
	listenerList.clear();
}

TuioObject* TuioDispatcher::getTuioObject(unsigned int s_id) {
    lockObjectList();
    for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj!=tobjList.end(); tobj++) {
        if((*tobj)->getSessionID()==s_id) {
            unlockObjectList();
            return (*tobj);
        }
    }
    unlockObjectList();
    return NULL;
}

TuioToken* TuioDispatcher::getTuioToken(unsigned int s_id) {
    TuioObject *tobj = getTuioObject(s_id);
    if (tobj==NULL) return NULL;
    
    return tobj->getTuioToken();
}

TuioPointer* TuioDispatcher::getTuioPointer(unsigned int s_id) {
    TuioObject *tobj = getTuioObject(s_id);
    if (tobj==NULL) return NULL;
    
    return tobj->getTuioPointer();
}

TuioBounds* TuioDispatcher::getTuioBounds(unsigned int s_id) {
    TuioObject *tobj = getTuioObject(s_id);
    if (tobj==NULL) return NULL;
    
    return tobj->getTuioBounds();
}

TuioSymbol* TuioDispatcher::getTuioSymbol(unsigned int s_id) {
    TuioObject *tobj = getTuioObject(s_id);
    if (tobj==NULL) return NULL;
    
    return tobj->getTuioSymbol();
}

std::list<TuioObject*> TuioDispatcher::getTuioObjectList() {
    std::list<TuioObject*> listBuffer;
    lockObjectList();
    for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj!=tobjList.end(); tobj++) {
        listBuffer.push_back(*tobj);
    }
    unlockObjectList();
    return listBuffer;
}

std::list<TuioToken*> TuioDispatcher::getTuioTokenList() {
	std::list<TuioToken*> listBuffer;
    lockObjectList();
    for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj!=tobjList.end(); tobj++) {
        TuioToken *ttok = (*tobj)->getTuioToken();
        if (ttok!=NULL) listBuffer.push_back(ttok);
    }
    unlockObjectList();
	return listBuffer;
}

std::list<TuioPointer*> TuioDispatcher::getTuioPointerList() {
    std::list<TuioPointer*> listBuffer;
    lockObjectList();
    for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj!=tobjList.end(); tobj++) {
        TuioPointer *tptr = (*tobj)->getTuioPointer();
        if (tptr!=NULL) listBuffer.push_back(tptr);
    }
    unlockObjectList();
    return listBuffer;
}

std::list<TuioBounds*> TuioDispatcher::getTuioBoundsList() {
    std::list<TuioBounds*> listBuffer;
    lockObjectList();
    for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj!=tobjList.end(); tobj++) {
        TuioBounds *tbnd = (*tobj)->getTuioBounds();
        if (tbnd!=NULL) listBuffer.push_back(tbnd);
    }
    unlockObjectList();
    return listBuffer;
}

std::list<TuioGeometry*> TuioDispatcher::getTuioGeometryList() {
    std::list<TuioGeometry*> listBuffer;
    for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
        if ((*iter)->getTuioGeometry()!=NULL) listBuffer.push_back((*iter)->getTuioGeometry());
    }
    return listBuffer;
}

std::list<TuioControl*> TuioDispatcher::getTuioControlList() {
    std::list<TuioControl*> listBuffer;
    for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
        if ((*iter)->getTuioControl()!=NULL) listBuffer.push_back((*iter)->getTuioControl());
    }
    return listBuffer;
}

std::list<TuioData*> TuioDispatcher::getTuioDataList() {
    std::list<TuioData*> listBuffer;
    for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
        if ((*iter)->getTuioData()!=NULL) listBuffer.push_back((*iter)->getTuioData());
    }
    return listBuffer;
}

std::list<TuioSignal*> TuioDispatcher::getTuioSignalList() {
    std::list<TuioSignal*> listBuffer;
    for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
        if ((*iter)->getTuioSignal()!=NULL) listBuffer.push_back((*iter)->getTuioSignal());
    }
    return listBuffer;
}

std::list<TuioAssociation*> TuioDispatcher::getTuioAssociationList() {
    std::list<TuioAssociation*> listBuffer;
    for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
        std::list<TuioAssociation*> assocs = (*iter)->getTuioAssociationList();
        listBuffer.insert(listBuffer.end(), assocs.begin(), assocs.end());
    }
    return listBuffer;
}

std::list<TuioToken3D*> TuioDispatcher::getTuioToken3DList() {
    std::list<TuioToken3D*> listBuffer;
    for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
        if ((*iter)->getTuioToken3D()!=NULL) listBuffer.push_back((*iter)->getTuioToken3D());
    }
    return listBuffer;
}

std::list<TuioPointer3D*> TuioDispatcher::getTuioPointer3DList() {
    std::list<TuioPointer3D*> listBuffer;
    for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
        if ((*iter)->getTuioPointer3D()!=NULL) listBuffer.push_back((*iter)->getTuioPointer3D());
    }
    return listBuffer;
}

std::list<TuioBounds3D*> TuioDispatcher::getTuioBounds3DList() {
    std::list<TuioBounds3D*> listBuffer;
    for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
        if ((*iter)->getTuioBounds3D()!=NULL) listBuffer.push_back((*iter)->getTuioBounds3D());
    }
    return listBuffer;
}

std::list<TuioSymbol*> TuioDispatcher::getTuioSymbolList() {
    std::list<TuioSymbol*> listBuffer;
    lockObjectList();
    for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj!=tobjList.end(); tobj++) {
        TuioSymbol *tsym = (*tobj)->getTuioSymbol();
        if (tsym!=NULL) listBuffer.push_back(tsym);
    }
    unlockObjectList();
    return listBuffer;
}
