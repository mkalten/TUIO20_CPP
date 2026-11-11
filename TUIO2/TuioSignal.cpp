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

#include "TuioSignal.h"
#include "TuioObject.h"
using namespace TUIO2;

TuioSignal::TuioSignal (TuioTime ttime, TuioObject *tobj, unsigned int c_id, std::list<unsigned int> trgts):TuioContent(ttime,tobj) {
	component_id = c_id;
	targets = trgts;
}

TuioSignal::TuioSignal (TuioObject *tobj, unsigned int c_id, std::list<unsigned int> trgts):TuioContent(tobj) {
	component_id = c_id;
	targets = trgts;
}

TuioSignal::TuioSignal (TuioSignal *tsig):TuioContent(tsig->getTuioTime(),tsig->getContainingTuioObject()) {
	component_id = tsig->getComponentID();
	targets = tsig->getSignalTargets();
}

void TuioSignal::update(TuioTime ttime) {
	TuioContent::update(ttime);
}


unsigned int TuioSignal::getComponentID() const {
	return component_id;
}

void TuioSignal::setComponentID(unsigned int c_id) {
	component_id = c_id;
}

std::list<unsigned int> TuioSignal::getSignalTargets() {
	return targets;
}

void TuioSignal::setSignalTargets(std::list<unsigned int> trgts) {
	targets = trgts;
}
