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

#include "TuioComponent3D.h"
#include "TuioObject.h"
using namespace TUIO2;

TuioComponent3D::TuioComponent3D (TuioTime ttime, TuioObject *tobj, float xp, float yp, float zp, float a, float xa, float ya, float za):TuioComponent(ttime,tobj,xp,yp,a) {
	zpos = zp;
	zspeed = 0.0f;
	xaxis = xa;
	yaxis = ya;
	zaxis = za;
}

TuioComponent3D::TuioComponent3D (TuioObject *tobj, float xp, float yp, float zp, float a, float xa, float ya, float za):TuioComponent(tobj,xp,yp,a) {
	zpos = zp;
	zspeed = 0.0f;
	xaxis = xa;
	yaxis = ya;
	zaxis = za;
}

TuioComponent3D::TuioComponent3D (TuioComponent3D *tcon):TuioComponent(tcon->getTuioTime(),tcon->getContainingTuioObject(),tcon->getX(),tcon->getY(),tcon->getAngle()) {
	zpos = tcon->getZ();
	zspeed = tcon->getZSpeed();
	xaxis = tcon->getXAxis();
	yaxis = tcon->getYAxis();
	zaxis = tcon->getZAxis();
}

void TuioComponent3D::update (TuioTime ttime, float xp, float yp, float zp, float a, float xa, float ya, float za) {
	
	float dz = zp-zpos;
	TuioTime diffTime = ttime - currentTime;
	float dt = diffTime.getTotalMilliseconds()/1000.0f;
	if (dt>0) zspeed = dz/dt;
	else zspeed = 0.0f;

	TuioComponent::update(ttime,xp,yp,a);
	zpos = zp;
	xaxis = xa;
	yaxis = ya;
	zaxis = za;
}

void TuioComponent3D::update (TuioTime ttime, float xp, float yp, float zp, float a, float xa, float ya, float za, float xs, float ys, float zs, float rs, float ma, float ra) {
	
	TuioComponent::update(ttime,xp,yp,a,xs,ys,rs,ma,ra);
	zpos = zp;
	zspeed = zs;
	xaxis = xa;
	yaxis = ya;
	zaxis = za;
}

float TuioComponent3D::getZ() const {
	return zpos;
}

float TuioComponent3D::getZSpeed() const {
	return zspeed;
}

float TuioComponent3D::getXAxis() const {
	return xaxis;
}

float TuioComponent3D::getYAxis() const {
	return yaxis;
}

float TuioComponent3D::getZAxis() const {
	return zaxis;
}
