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

#include "TuioPointer3D.h"
#include "TuioObject.h"
using namespace TUIO2;

TuioPointer3D::TuioPointer3D (TuioTime ttime, TuioObject *tobj, unsigned short ti, unsigned short ui, unsigned int c_id, float xp, float yp, float zp, float xa, float ya, float za, float r):TuioComponent3D(ttime,tobj,xp,yp,zp,0.0f,xa,ya,za) {
    type_id = ti;
    user_id = ui;
	component_id = c_id;
	radius = r;
}

TuioPointer3D::TuioPointer3D (TuioObject *tobj, unsigned short ti, unsigned short ui, unsigned int c_id, float xp, float yp, float zp, float xa, float ya, float za, float r):TuioComponent3D(tobj,xp,yp,zp,0.0f,xa,ya,za) {
    type_id = ti;
    user_id = ui;
	component_id = c_id;
	radius = r;
}

TuioPointer3D::TuioPointer3D (TuioPointer3D *tptr):TuioComponent3D(tptr->getTuioTime(),tptr->getContainingTuioObject(),tptr->getX(),tptr->getY(),tptr->getZ(),0.0f,tptr->getXAxis(),tptr->getYAxis(),tptr->getZAxis()) {
    type_id = tptr->getTypeID();
    user_id = tptr->getUserID();
	component_id = tptr->getComponentID();
	radius = tptr->getRadius();
}

unsigned int TuioPointer3D::getComponentID() const{
	return component_id;
}

unsigned short TuioPointer3D::getTypeID() const{
    return type_id;
}

unsigned short TuioPointer3D::getUserID() const{
    return user_id;
}

unsigned int TuioPointer3D::getTypeUserID() const {
    int tu_id = user_id << 16 | type_id;
    return tu_id;
}

void TuioPointer3D::setTypeUserID(unsigned int tu_id) {
    user_id = tu_id >> 16;
    type_id = tu_id & 0x0000FFFF;
}

float TuioPointer3D::getRadius() const {
	return radius;
}
