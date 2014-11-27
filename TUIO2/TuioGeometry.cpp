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

#include "TuioGeometry.h"
#include "TuioObject.h"
using namespace TUIO2;

TuioGeometry::TuioGeometry (TuioTime ttime, TuioObject *tobj, std::list<TuioPoint> tpoints):state(TUIO_ADDED) {
	container = tobj;
	points.assign(tpoints.begin(), tpoints.end());
	currentTime = ttime;
	startTime = currentTime;
}

TuioGeometry::TuioGeometry (TuioObject *tobj, std::list<TuioPoint> tpoints):state(TUIO_ADDED) {
	container = tobj;
	points.assign(tpoints.begin(), tpoints.end());
	currentTime = TuioTime::getSystemTime();
	startTime = currentTime;
	
}

TuioGeometry::TuioGeometry (TuioGeometry *tgeom):state(TUIO_ADDED)
{
	container = tgeom->getContainingTuioObject();
	points.assign(tgeom->getPoints().begin(), tgeom->getPoints().end());
}

TuioObject* TuioComponent::getContainingTuioObject() {
	return container;
}

void TuioGeometry::update(TuioTime ttime, std::list<TuioPoint> tpoints) {
	points.assign(tpoints.begin(), tpoints.end());
	currentTime = ttime;
}

void TuioGeometry::update(std::list<TuioPoint> tpoints) {
	points.assign(tpoints.begin(), tpoints.end());
}

void TuioGeometry::update(TuioGeometry *tgeom) {
	points.assign(tgeom->getPoints().begin(), tgeom->getPoints().end());
}

void TuioComponent::remove(TuioTime ttime) {
	currentTime = ttime;
	state = TUIO_REMOVED;
}

unsigned int TuioGeometry::getSessionID() const{
	return container->getSessionID();
}

std::list<TuioPoint> TuioGeometry::getPoints() const {
	return points;
}

int TuioGeometry::getTuioState() const{
	return state;
}

TuioTime TuioGeometry::getTuioTime() const{
	return currentTime;
}

TuioTime TuioGeometry::getStartTime() const{
	return startTime;
}
