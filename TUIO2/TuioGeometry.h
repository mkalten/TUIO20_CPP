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

#ifndef INCLUDED_TUIOGEOMETRY_H
#define INCLUDED_TUIOGEOMETRY_H

#include "TuioContent.h"
#include "TuioPoint.h"

namespace TUIO2 {
	
	/**
	 * The TuioGeometry class encodes the optional geometry descriptors
	 * that can be associated to an existing component instance, such as
	 * the convex hull (CHG), outer (OCG) and inner (ICG) contours, the
	 * skeleton graph (SKG) with its node jumps and skeleton volumes (SVG),
	 * the area spans (ARG) as well as raw region data (RAW).
	 *
	 * @author Martin Kaltenbrunner
	 * @version 2.0.b1
	 */
	class LIBDECL TuioGeometry : public TuioContent {
		
	protected:
		/**
		 * The convex hull point list of this TuioGeometry
		 */
		std::list<TuioPoint> convexHull;
		/**
		 * The outer contour point list of this TuioGeometry
		 */
		std::list<TuioPoint> outerContour;
		/**
		 * The inner contour point lists of this TuioGeometry
		 */
		std::list< std::list<TuioPoint> > innerContours;
		/**
		 * The skeleton branches of this TuioGeometry as point lists
		 */
		std::list< std::list<TuioPoint> > skeleton;
		/**
		 * The node-jump markers between skeleton branches
		 */
		std::list<unsigned int> skeletonJumps;
		/**
		 * The skeleton node volumes (radii) of this TuioGeometry
		 */
		std::list<float> skeletonVolumes;
		/**
		 * The area span points of this TuioGeometry
		 */
		std::list<TuioPoint> areaPoints;
		/**
		 * The area span widths of this TuioGeometry
		 */
		std::list<float> areaWidths;
		/**
		 * The raw region data of this TuioGeometry
		 */
		std::string rawData;
		/**
		 * The row width of the raw region data
		 */
		unsigned int rawWidth;
		
	public:
		TuioGeometry (TuioTime ttime, TuioObject *tobj);
		TuioGeometry (TuioObject *tobj);
		TuioGeometry (TuioGeometry *tgeo);

		virtual void update(TuioTime ttime);

		void setConvexHull(std::list<TuioPoint> hull);
		std::list<TuioPoint> getConvexHull();
		void setOuterContour(std::list<TuioPoint> contour);
		std::list<TuioPoint> getOuterContour();
		void setInnerContours(std::list< std::list<TuioPoint> > contours);
		std::list< std::list<TuioPoint> > getInnerContours();
		void setSkeleton(std::list< std::list<TuioPoint> > skltn);
		std::list< std::list<TuioPoint> > getSkeleton();
		void setSkeletonJumps(std::list<unsigned int> jumps);
		std::list<unsigned int> getSkeletonJumps();
		void setSkeletonVolumes(std::list<float> vols);
		std::list<float> getSkeletonVolumes();
		void setArea(std::list<TuioPoint> points);
		std::list<TuioPoint> getArea();
		void setAreaWidths(std::list<float> widths);
		std::list<float> getAreaWidths();
		void setRawData(unsigned int width, std::string data);
		std::string getRawData();
		unsigned int getRawWidth();
	};
}
#endif /* INCLUDED_TUIOGEOMETRY_H */
