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

#include "TuioContent.h"
#include "TuioObject.h"
using namespace TUIO2;

TuioContent::TuioContent (TuioTime ttime, TuioObject *tobj) {
	currentTime = TuioTime(ttime);
	state = TUIO_ADDED;
	container = tobj;
}

TuioContent::TuioContent (TuioObject *tobj) {
	currentTime = TuioTime::getSystemTime();
	state = TUIO_ADDED;
	container = tobj;
}

void TuioContent::setContainingTuioObject (TuioObject *tobj) {
	container = tobj;
}

TuioObject* TuioContent::getContainingTuioObject () {
	return container;
}

TuioTime TuioContent::getTuioTime() {
	return currentTime;
}

int TuioContent::getTuioState() {
	return (int)state;
}

unsigned int TuioContent::getSessionID() {
	return container ? container->getSessionID() : 0;
}

void TuioContent::update(TuioTime ttime) {
	currentTime = ttime;
	if (state == TUIO_ADDED) state = TUIO_IDLE;
	if (container) container->update(ttime);
}

void TuioContent::remove(TuioTime ttime) {
	state = TUIO_REMOVED;
	currentTime = ttime;
	if (container) container->update(ttime);
}
