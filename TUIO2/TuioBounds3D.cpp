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

#include "TuioBounds3D.h"
#include "TuioObject.h"
using namespace TUIO2;

TuioBounds3D::TuioBounds3D (TuioTime ttime, TuioObject *tobj, float xp, float yp, float zp, float a, float xa, float ya, float za, float w, float h, float d, float v):TuioComponent3D(ttime,tobj,xp,yp,zp,a,xa,ya,za) {
	width = w;
	height = h;
	depth = d;
	volume = v;
}

TuioBounds3D::TuioBounds3D (TuioObject *tobj, float xp, float yp, float zp, float a, float xa, float ya, float za, float w, float h, float d, float v):TuioComponent3D(tobj,xp,yp,zp,a,xa,ya,za) {
	width = w;
	height = h;
	depth = d;
	volume = v;
}

TuioBounds3D::TuioBounds3D (TuioBounds3D *tbnd):TuioComponent3D(tbnd->getTuioTime(),tbnd->getContainingTuioObject(),tbnd->getX(),tbnd->getY(),tbnd->getZ(),tbnd->getAngle(),tbnd->getXAxis(),tbnd->getYAxis(),tbnd->getZAxis()) {
	width = tbnd->getWidth();
	height = tbnd->getHeight();
	depth = tbnd->getDepth();
	volume = tbnd->getVolume();
}

float TuioBounds3D::getWidth() const {
	return width;
}

float TuioBounds3D::getHeight() const {
	return height;
}

float TuioBounds3D::getDepth() const {
	return depth;
}

float TuioBounds3D::getVolume() const {
	return volume;
}
