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

#include "TuioObject.h"

using namespace TUIO2;

TuioObject::TuioObject (unsigned int s_id) {
    currentTime = TuioTime::getSystemTime();
    session_id = s_id;
    startTime = currentTime;
    
    token = NULL;
    pointer = NULL;
    bounds = NULL;
    symbol = NULL;
    token3d = NULL;
    pointer3d = NULL;
    bounds3d = NULL;
    geometry = NULL;
    control = NULL;
    data = NULL;
    signal = NULL;
    associations.clear();
    state = TUIO_ADDED;
}

TuioObject::TuioObject (TuioTime ttime, unsigned int s_id) {
    currentTime = ttime;
    session_id = s_id;
    startTime = currentTime;
    
    token = NULL;
    pointer = NULL;
    bounds = NULL;
    symbol = NULL;
    token3d = NULL;
    pointer3d = NULL;
    bounds3d = NULL;
    geometry = NULL;
    control = NULL;
    data = NULL;
    signal = NULL;
    associations.clear();
    state = TUIO_ADDED;
}

TuioObject::TuioObject (TuioTime ttime, TuioSource *src, unsigned int s_id) {
    currentTime = ttime;
    session_id = s_id;
    startTime = currentTime;
    
    setTuioSource(src);
    token = NULL;
    pointer = NULL;
    bounds = NULL;
    symbol = NULL;
    token3d = NULL;
    pointer3d = NULL;
    bounds3d = NULL;
    geometry = NULL;
    control = NULL;
    data = NULL;
    signal = NULL;
    associations.clear();
    state = TUIO_ADDED;
}

TuioObject::~TuioObject() {
    deleteAllTuioComponents();
}

unsigned int TuioObject::getSessionID() {
    return session_id;
}

void TuioObject::setTuioSource(TuioSource *src) {
    source.setSourceString(src->getSourceID(),src->getSourceString());
}

TuioSource* TuioObject::getTuioSource() {
    return &source;
}

void TuioObject::setTuioToken (TuioToken *ttok) {
    token = ttok;
    token->setContainingTuioObject(this);
    currentTime = TuioTime::getSystemTime();
    state = TUIO_ADDED;
}

void TuioObject::setTuioPointer (TuioPointer *tptr) {
    pointer = tptr;
    pointer->setContainingTuioObject(this);
    currentTime = TuioTime::getSystemTime();
    state = TUIO_ADDED;
}

void TuioObject::setTuioBounds (TuioBounds *tbnd) {
	bounds = tbnd;
    bounds->setContainingTuioObject(this);
    currentTime = TuioTime::getSystemTime();
    state = TUIO_ADDED;
}

void TuioObject::setTuioSymbol (TuioSymbol *tsym) {
    symbol = tsym;
    symbol->setContainingTuioObject(this);
    currentTime = TuioTime::getSystemTime();
    state = TUIO_ADDED;
}

void TuioObject::setTuioToken3D (TuioToken3D *ttok) {
    token3d = ttok;
    token3d->setContainingTuioObject(this);
    currentTime = TuioTime::getSystemTime();
    state = TUIO_ADDED;
}

void TuioObject::setTuioPointer3D (TuioPointer3D *tptr) {
    pointer3d = tptr;
    pointer3d->setContainingTuioObject(this);
    currentTime = TuioTime::getSystemTime();
    state = TUIO_ADDED;
}

void TuioObject::setTuioBounds3D (TuioBounds3D *tbnd) {
    bounds3d = tbnd;
    bounds3d->setContainingTuioObject(this);
    currentTime = TuioTime::getSystemTime();
    state = TUIO_ADDED;
}

void TuioObject::setTuioGeometry (TuioGeometry *tgeo) {
    geometry = tgeo;
    geometry->setContainingTuioObject(this);
    currentTime = TuioTime::getSystemTime();
    state = TUIO_ADDED;
}

void TuioObject::setTuioControl (TuioControl *tctl) {
    control = tctl;
    control->setContainingTuioObject(this);
    currentTime = TuioTime::getSystemTime();
    state = TUIO_ADDED;
}

void TuioObject::setTuioData (TuioData *tdat) {
    data = tdat;
    data->setContainingTuioObject(this);
    currentTime = TuioTime::getSystemTime();
    state = TUIO_ADDED;
}

void TuioObject::setTuioSignal (TuioSignal *tsig) {
    signal = tsig;
    signal->setContainingTuioObject(this);
    currentTime = TuioTime::getSystemTime();
    state = TUIO_ADDED;
}

void TuioObject::addTuioAssociation (TuioAssociation *tasc) {
    associations.push_back(tasc);
    tasc->setContainingTuioObject(this);
    currentTime = TuioTime::getSystemTime();
    state = TUIO_ADDED;
}

void TuioObject::removeAllTuioComponents(TuioTime ttime) {
    removeTuioToken(ttime);
    removeTuioPointer(ttime);
    removeTuioBounds(ttime);
    removeTuioSymbol(ttime);
    removeTuioToken3D(ttime);
    removeTuioPointer3D(ttime);
    removeTuioBounds3D(ttime);
    removeTuioGeometry(ttime);
    removeTuioControl(ttime);
    removeTuioData(ttime);
    removeTuioSignal(ttime);
    removeTuioAssociations(ttime);
}

void TuioObject::removeTuioToken (TuioTime ttime) {
    if (token != NULL) token->remove(ttime);
    currentTime = ttime;
 }

void TuioObject::removeTuioPointer (TuioTime ttime) {
    if (pointer != NULL) pointer->remove(ttime);
    currentTime = ttime;
}

void TuioObject::removeTuioBounds (TuioTime ttime) {
    if (bounds != NULL) bounds->remove(ttime);
    currentTime = ttime;
}

void TuioObject::removeTuioSymbol (TuioTime ttime) {
    if (symbol != NULL) symbol->remove(ttime);
    currentTime = ttime;
}

void TuioObject::removeTuioToken3D (TuioTime ttime) {
    if (token3d != NULL) token3d->remove(ttime);
    currentTime = ttime;
}

void TuioObject::removeTuioPointer3D (TuioTime ttime) {
    if (pointer3d != NULL) pointer3d->remove(ttime);
    currentTime = ttime;
}

void TuioObject::removeTuioBounds3D (TuioTime ttime) {
    if (bounds3d != NULL) bounds3d->remove(ttime);
    currentTime = ttime;
}

void TuioObject::removeTuioGeometry (TuioTime ttime) {
    if (geometry != NULL) geometry->remove(ttime);
    currentTime = ttime;
}

void TuioObject::removeTuioControl (TuioTime ttime) {
    if (control != NULL) control->remove(ttime);
    currentTime = ttime;
}

void TuioObject::removeTuioData (TuioTime ttime) {
    if (data != NULL) data->remove(ttime);
    currentTime = ttime;
}

void TuioObject::removeTuioSignal (TuioTime ttime) {
    if (signal != NULL) signal->remove(ttime);
    currentTime = ttime;
}

void TuioObject::removeTuioAssociations (TuioTime ttime) {
    for (std::list<TuioAssociation*>::iterator it = associations.begin(); it != associations.end(); it++)
        (*it)->remove(ttime);
    currentTime = ttime;
}

void TuioObject::deleteAllTuioComponents() {
    deleteTuioToken();
    deleteTuioPointer();
    deleteTuioBounds();
    deleteTuioSymbol();
    deleteTuioToken3D();
    deleteTuioPointer3D();
    deleteTuioBounds3D();
    deleteTuioGeometry();
    deleteTuioControl();
    deleteTuioData();
    deleteTuioSignal();
    deleteTuioAssociations();
}

void TuioObject::deleteTuioToken () {
    if (token != NULL) {
        delete token;
        token = NULL;
        //currentTime = TuioTime::getSessionTime();
    }
}

void TuioObject::deleteTuioPointer () {
    if (pointer != NULL) {
        delete pointer;
        pointer = NULL;
        //currentTime = TuioTime::getSessionTime();
    }
}

void TuioObject::deleteTuioBounds () {
    if (bounds != NULL) {
        delete bounds;
        bounds = NULL;
        //currentTime = TuioTime::getSessionTime();
    }
}

void TuioObject::deleteTuioSymbol () {
    if (symbol != NULL) {
        delete symbol;
        symbol = NULL;
    token3d = NULL;
    pointer3d = NULL;
    bounds3d = NULL;
        //currentTime = TuioTime::getSessionTime();
    }
}

void TuioObject::deleteTuioToken3D () {
    if (token3d != NULL) {
        delete token3d;
        token3d = NULL;
    }
}

void TuioObject::deleteTuioPointer3D () {
    if (pointer3d != NULL) {
        delete pointer3d;
        pointer3d = NULL;
    }
}

void TuioObject::deleteTuioBounds3D () {
    if (bounds3d != NULL) {
        delete bounds3d;
        bounds3d = NULL;
    }
}

void TuioObject::deleteTuioGeometry () {
    if (geometry != NULL) {
        delete geometry;
        geometry = NULL;
    control = NULL;
    data = NULL;
    signal = NULL;
    associations.clear();
    }
}

void TuioObject::deleteTuioControl () {
    if (control != NULL) {
        delete control;
        control = NULL;
    }
}

void TuioObject::deleteTuioData () {
    if (data != NULL) {
        delete data;
        data = NULL;
    }
}

void TuioObject::deleteTuioSignal () {
    if (signal != NULL) {
        delete signal;
        signal = NULL;
    associations.clear();
    }
}

void TuioObject::deleteTuioAssociations () {
    for (std::list<TuioAssociation*>::iterator it = associations.begin(); it != associations.end(); it++)
        delete (*it);
    associations.clear();
}

void TuioObject::clearAllTuioComponents() {
    clearTuioToken();
    clearTuioPointer();
    clearTuioBounds();
    clearTuioSymbol();
    clearTuioToken3D();
    clearTuioPointer3D();
    clearTuioBounds3D();
    clearTuioGeometry();
    clearTuioControl();
    clearTuioData();
    clearTuioSignal();
    clearTuioAssociations();
}

void TuioObject::clearTuioToken () {
    if (token != NULL) {
        token = NULL;
        //currentTime = TuioTime::getSessionTime();
    }
}

void TuioObject::clearTuioPointer () {
    if (pointer != NULL) {
        pointer = NULL;
        //currentTime = TuioTime::getSessionTime();
    }
}

void TuioObject::clearTuioBounds () {
    if (bounds != NULL) {
        bounds = NULL;
        //currentTime = TuioTime::getSessionTime();
    }
}

void TuioObject::clearTuioSymbol () {
    if (symbol != NULL) {
        symbol = NULL;
    token3d = NULL;
    pointer3d = NULL;
    bounds3d = NULL;
        //currentTime = TuioTime::getSessionTime();
    }
}

void TuioObject::clearTuioToken3D () {
    if (token3d != NULL) {
        token3d = NULL;
    }
}

void TuioObject::clearTuioPointer3D () {
    if (pointer3d != NULL) {
        pointer3d = NULL;
    }
}

void TuioObject::clearTuioBounds3D () {
    if (bounds3d != NULL) {
        bounds3d = NULL;
    }
}

void TuioObject::clearTuioGeometry () {
    if (geometry != NULL) {
        geometry = NULL;
    control = NULL;
    data = NULL;
    signal = NULL;
    associations.clear();
    }
}

void TuioObject::clearTuioControl () {
    if (control != NULL) {
        control = NULL;
    }
}

void TuioObject::clearTuioData () {
    if (data != NULL) {
        data = NULL;
    }
}

void TuioObject::clearTuioSignal () {
    if (signal != NULL) {
        signal = NULL;
    associations.clear();
    }
}

void TuioObject::clearTuioAssociations () {
    associations.clear();
}

bool TuioObject::containsAnyTuioComponent () {
    if (token != NULL) return true;
    else if (pointer != NULL) return true;
    else if (bounds != NULL) return true;
    else if (symbol != NULL) return true;
    else return false;
}

bool TuioObject::containsTuioToken () {
    if (token != NULL) return true;
    else return false;
}

bool TuioObject::containsTuioPointer () {
    if (pointer != NULL) return true;
    else return false;
}

bool TuioObject::containsTuioBounds () {
    if (bounds != NULL) return true;
    else return false;
}

bool TuioObject::containsTuioSymbol () {
    if (symbol != NULL) return true;
    else return false;
}

bool TuioObject::containsTuioToken3D () {
    return (token3d != NULL);
}

bool TuioObject::containsTuioPointer3D () {
    return (pointer3d != NULL);
}

bool TuioObject::containsTuioBounds3D () {
    return (bounds3d != NULL);
}

bool TuioObject::containsTuioGeometry () {
    return (geometry != NULL);
}

bool TuioObject::containsTuioControl () {
    return (control != NULL);
}

bool TuioObject::containsTuioData () {
    return (data != NULL);
}

bool TuioObject::containsTuioSignal () {
    return (signal != NULL);
}

bool TuioObject::containsTuioAssociation () {
    return (!associations.empty());
}

bool TuioObject::containsNewTuioToken () {
    if (token == NULL) return false;
    else if (token->getTuioState()==TUIO_ADDED) return true;
    else return false;
}

bool TuioObject::containsNewTuioPointer () {
    if (pointer == NULL) return false;
    else if (pointer->getTuioState()==TUIO_ADDED) return true;
    else return false;
}

bool TuioObject::containsNewTuioBounds () {
    if (bounds == NULL) return false;
    else if (bounds->getTuioState()==TUIO_ADDED) return true;
    else return false;
}

bool TuioObject::containsNewTuioSymbol () {
    if (symbol == NULL) return false;
    else if (symbol->getTuioState()==TUIO_ADDED) return true;
    else return false;
}

bool TuioObject::containsNewTuioToken3D () {
    return (token3d != NULL) && (token3d->getTuioState() == TUIO_ADDED);
}

bool TuioObject::containsNewTuioPointer3D () {
    return (pointer3d != NULL) && (pointer3d->getTuioState() == TUIO_ADDED);
}

bool TuioObject::containsNewTuioBounds3D () {
    return (bounds3d != NULL) && (bounds3d->getTuioState() == TUIO_ADDED);
}

bool TuioObject::containsNewTuioGeometry () {
    return (geometry != NULL) && (geometry->getTuioState() == TUIO_ADDED);
}

bool TuioObject::containsNewTuioControl () {
    return (control != NULL) && (control->getTuioState() == TUIO_ADDED);
}

bool TuioObject::containsNewTuioData () {
    return (data != NULL) && (data->getTuioState() == TUIO_ADDED);
}

bool TuioObject::containsNewTuioSignal () {
    return (signal != NULL) && (signal->getTuioState() == TUIO_ADDED);
}

bool TuioObject::containsNewTuioAssociation () {
    for (std::list<TuioAssociation*>::iterator it = associations.begin(); it != associations.end(); it++)
        if ((*it)->getTuioState() == TUIO_ADDED) return true;
    return false;
}

TuioToken* TuioObject::getTuioToken () {
    return token;
}

TuioPointer* TuioObject::getTuioPointer () {
    return pointer;
}

TuioBounds* TuioObject::getTuioBounds () {
    return bounds;
}

TuioSymbol* TuioObject::getTuioSymbol () {
    return symbol;
}

TuioToken3D* TuioObject::getTuioToken3D () {
    return token3d;
}

TuioPointer3D* TuioObject::getTuioPointer3D () {
    return pointer3d;
}

TuioBounds3D* TuioObject::getTuioBounds3D () {
    return bounds3d;
}

TuioGeometry* TuioObject::getTuioGeometry () {
    return geometry;
}

TuioControl* TuioObject::getTuioControl () {
    return control;
}

TuioData* TuioObject::getTuioData () {
    return data;
}

TuioSignal* TuioObject::getTuioSignal () {
    return signal;
}

std::list<TuioAssociation*> TuioObject::getTuioAssociationList () {
    return associations;
}

void TuioObject::deleteTuioAssociation (TuioAssociation *tasc) {
    for (std::list<TuioAssociation*>::iterator it = associations.begin(); it != associations.end(); it++) {
        if (*it == tasc) {
            associations.erase(it);
            delete tasc;
            return;
        }
    }
}

void TuioObject::stop(TuioTime ttime){
    if (token!=NULL) token->stop(ttime);
    if (pointer!=NULL) pointer->stop(ttime);
    if (bounds!=NULL) bounds->stop(ttime);
    currentTime = ttime;
}

void TuioObject::remove(TuioTime ttime){
    if (token!=NULL) token->remove(ttime);
    if (pointer!=NULL) pointer->remove(ttime);
    if (bounds!=NULL) bounds->remove(ttime);
    currentTime = ttime;
    state = TUIO_REMOVED;
}

void TuioObject::update(TuioTime ttime){
    currentTime = ttime;
    state = TUIO_IDLE;
}

bool TuioObject::isMoving(){
    if ((token!=NULL) && token->isMoving()) return true;
    if ((pointer!=NULL) && pointer->isMoving()) return true;
    if ((bounds!=NULL) && bounds->isMoving()) return true;
    return false;
}

TuioTime TuioObject::getTuioTime() const{
    return currentTime;
}

TuioTime TuioObject::getStartTime() const{
	return startTime;
}

int TuioObject::getTuioState() const{
    return state;
}

