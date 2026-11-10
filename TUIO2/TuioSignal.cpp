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

TuioSignal::TuioSignal (TuioTime ttime, TuioObject *tobj, unsigned int sid, std::list<unsigned int> trgts):TuioContent(ttime,tobj) {
	signal_id = sid;
	targets = trgts;
}

TuioSignal::TuioSignal (TuioObject *tobj, unsigned int sid, std::list<unsigned int> trgts):TuioContent(tobj) {
	signal_id = sid;
	targets = trgts;
}

TuioSignal::TuioSignal (TuioSignal *tsig):TuioContent(tsig->getTuioTime(),tsig->getContainingTuioObject()) {
	signal_id = tsig->getSignalID();
	targets = tsig->getSignalTargets();
}

void TuioSignal::update(TuioTime ttime) {
	TuioContent::update(ttime);
}

void TuioSignal::stop(TuioTime ttime) {
	update(ttime);
}

unsigned int TuioSignal::getSignalID() const {
	return signal_id;
}

void TuioSignal::setSignalID(unsigned int sid) {
	signal_id = sid;
}

std::list<unsigned int> TuioSignal::getSignalTargets() {
	return targets;
}

void TuioSignal::setSignalTargets(std::list<unsigned int> trgts) {
	targets = trgts;
}
