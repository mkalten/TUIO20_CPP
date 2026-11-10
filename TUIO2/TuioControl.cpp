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

#include "TuioControl.h"
#include "TuioObject.h"
using namespace TUIO2;

TuioControl::TuioControl (TuioTime ttime, TuioObject *tobj):TuioContent(ttime,tobj) {

}

TuioControl::TuioControl (TuioObject *tobj):TuioContent(tobj) {

}

TuioControl::TuioControl (TuioControl *tctl):TuioContent(tctl->getTuioTime(),tctl->getContainingTuioObject()) {
	controls = tctl->getControls();
}

void TuioControl::update(TuioTime ttime) {
	TuioContent::update(ttime);
}

void TuioControl::stop(TuioTime ttime) {
	update(ttime);
}

void TuioControl::setControls(std::list< std::pair<bool,float> > ctls) {
	controls = ctls;
}

std::list< std::pair<bool,float> > TuioControl::getControls() {
	return controls;
}
