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

#include "TuioGeometry.h"
#include "TuioObject.h"
using namespace TUIO2;

TuioGeometry::TuioGeometry (TuioTime ttime, TuioObject *tobj):TuioContent(ttime,tobj) {

}

TuioGeometry::TuioGeometry (TuioObject *tobj):TuioContent(tobj) {

}

TuioGeometry::TuioGeometry (TuioGeometry *tgeo):TuioContent(tgeo->getTuioTime(),tgeo->getContainingTuioObject()) {
	convexHull = tgeo->getConvexHull();
	outerContour = tgeo->getOuterContour();
	innerContours = tgeo->getInnerContours();
	skeleton = tgeo->getSkeleton();
	skeletonJumps = tgeo->getSkeletonJumps();
	skeletonVolumes = tgeo->getSkeletonVolumes();
	areaPoints = tgeo->getArea();
	areaWidths = tgeo->getAreaWidths();
	rawData = tgeo->getRawData();
	rawWidth = tgeo->getRawWidth();
}

void TuioGeometry::update(TuioTime ttime) {
	TuioContent::update(ttime);
}


void TuioGeometry::setConvexHull(std::list<TuioPoint> hull) {
	convexHull = hull;
}

std::list<TuioPoint> TuioGeometry::getConvexHull() {
	return convexHull;
}

void TuioGeometry::setOuterContour(std::list<TuioPoint> contour) {
	outerContour = contour;
}

std::list<TuioPoint> TuioGeometry::getOuterContour() {
	return outerContour;
}

void TuioGeometry::setInnerContours(std::list< std::list<TuioPoint> > contours) {
	innerContours = contours;
}

std::list< std::list<TuioPoint> > TuioGeometry::getInnerContours() {
	return innerContours;
}

void TuioGeometry::setSkeleton(std::list< std::list<TuioPoint> > skltn) {
	skeleton = skltn;
}

std::list< std::list<TuioPoint> > TuioGeometry::getSkeleton() {
	return skeleton;
}

void TuioGeometry::setSkeletonJumps(std::list<unsigned int> jumps) {
	skeletonJumps = jumps;
}

std::list<unsigned int> TuioGeometry::getSkeletonJumps() {
	return skeletonJumps;
}

void TuioGeometry::setSkeletonVolumes(std::list<float> vols) {
	skeletonVolumes = vols;
}

std::list<float> TuioGeometry::getSkeletonVolumes() {
	return skeletonVolumes;
}

void TuioGeometry::setArea(std::list<TuioPoint> points) {
	areaPoints = points;
}

std::list<TuioPoint> TuioGeometry::getArea() {
	return areaPoints;
}

void TuioGeometry::setAreaWidths(std::list<float> widths) {
	areaWidths = widths;
}

std::list<float> TuioGeometry::getAreaWidths() {
	return areaWidths;
}

void TuioGeometry::setRawData(unsigned int width, std::string data) {
	rawWidth = width;
	rawData = data;
}

std::string TuioGeometry::getRawData() {
	return rawData;
}

unsigned int TuioGeometry::getRawWidth() {
	return rawWidth;
}
