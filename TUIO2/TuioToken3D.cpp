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

#include "TuioToken3D.h"
#include "TuioObject.h"
using namespace TUIO2;

TuioToken3D::TuioToken3D (TuioTime ttime, TuioObject *tobj, unsigned short ti, unsigned short ui, unsigned int c_id, float xp, float yp, float zp, float a, float xa, float ya, float za):TuioComponent3D(ttime,tobj,xp,yp,zp,a,xa,ya,za) {
    type_id = ti;
    user_id = ui;
	component_id = c_id;
}

TuioToken3D::TuioToken3D (TuioObject *tobj, unsigned short ti, unsigned short ui, unsigned int c_id, float xp, float yp, float zp, float a, float xa, float ya, float za):TuioComponent3D(tobj,xp,yp,zp,a,xa,ya,za) {
    type_id = ti;
    user_id = ui;
	component_id = c_id;
}

TuioToken3D::TuioToken3D (TuioToken3D *ttok):TuioComponent3D(ttok->getTuioTime(),ttok->getContainingTuioObject(),ttok->getX(),ttok->getY(),ttok->getZ(),ttok->getAngle(),ttok->getXAxis(),ttok->getYAxis(),ttok->getZAxis()) {
    type_id = ttok->getTypeID();
    user_id = ttok->getUserID();
	component_id = ttok->getComponentID();
}

unsigned int TuioToken3D::getComponentID() const{
	return component_id;
}

unsigned short TuioToken3D::getTypeID() const{
    return type_id;
}

unsigned short TuioToken3D::getUserID() const{
    return user_id;
}

unsigned int TuioToken3D::getTypeUserID() const {
    int tu_id = user_id << 16 | type_id;
    return tu_id;
}

void TuioToken3D::setTypeUserID(unsigned int tu_id) {
    user_id = tu_id >> 16;
    type_id = tu_id & 0x0000FFFF;
}
