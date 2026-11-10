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
	 * The TuioGeometry class encodes the detailed contour geometry
	 * of a TuioObject component. It can be assigned to a TuioObject
	 * to describe its convex hull, its outer contour as well as
	 * one or more inner contours as lists of TuioPoints.
	 *
	 * @author Martin Kaltenbrunner
	 * @version 2.0.b1
	 */
	class LIBDECL TuioGeometry : public TuioContent {
		
	protected:
		/**
		 * The convex hull of this TuioGeometry represented as a list of TuioPoints
		 */
		std::list<TuioPoint> convexHull;
		/**
		 * The outer contour of this TuioGeometry represented as a list of TuioPoints
		 */
		std::list<TuioPoint> outerContour;
		/**
		 * The inner contours of this TuioGeometry represented as a list of TuioPoint lists
		 */
		std::list< std::list<TuioPoint> > innerContours;
		/**
		 * The skeleton of this TuioGeometry as a list of point lists,
		 * one per leaf-to-node branch
		 */
		std::list< std::list<TuioPoint> > skeleton;
		/**
		 * The node indices where each skeleton branch continues from
		 */
		std::list<unsigned int> skeletonJumps;
		/**
		 * The volume radius of each skeleton point (normalized to the sensor height)
		 */
		std::list<float> skeletonVolumes;
		/**
		 * The span start points of the area description
		 */
		std::list<TuioPoint> areaPoints;
		/**
		 * The span lengths of the area description
		 */
		std::list<float> areaWidths;
		/**
		 * The raw region data of this TuioGeometry (8bit samples)
		 */
		std::string rawData;
		/**
		 * The relative width between raw region data points
		 */
		unsigned int rawWidth;
		
	public:
		/**
		 * This constructor takes a TuioTime argument and a TuioObject argument
		 * and creates an empty TuioGeometry.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	tobj	the TuioObject to assign
		 */
		TuioGeometry (TuioTime ttime, TuioObject *tobj);

		/**
		 * This constructor takes a TuioObject argument
		 * and creates an empty TuioGeometry.
		 *
		 * @param	tobj	the TuioObject to assign
		 */
		TuioGeometry (TuioObject *tobj);

		/**
		 * This constructor takes a TuioGeometry argument and copies
		 * all of its attributes to the newly created TuioGeometry.
		 *
		 * @param	tgeo	the TuioGeometry to assign
		 */
		TuioGeometry (TuioGeometry *tgeo);

		/**
		 * This method refreshes the currentTime after an update.
		 *
		 * @param	ttime	the TuioTime to assign
		 */
		void update(TuioTime ttime);

		/**
		 * This method updates the TuioGeometry with the provided time stamp,
		 * marking it as stopped.
		 *
		 * @param	ttime	the TuioTime to assign
		 */
		void stop(TuioTime ttime);

		/**
		 * Assigns the provided list of TuioPoints as the convex hull
		 * of this TuioGeometry.
		 *
		 * @param	hull	the convex hull point list to assign
		 */
		void setConvexHull(std::list<TuioPoint> hull);

		/**
		 * Returns the convex hull of this TuioGeometry.
		 *
		 * @return	the convex hull point list
		 */
		std::list<TuioPoint> getConvexHull();

		/**
		 * Assigns the provided list of TuioPoints as the outer contour
		 * of this TuioGeometry.
		 *
		 * @param	contour	the outer contour point list to assign
		 */
		void setOuterContour(std::list<TuioPoint> contour);

		/**
		 * Returns the outer contour of this TuioGeometry.
		 *
		 * @return	the outer contour point list
		 */
		std::list<TuioPoint> getOuterContour();

		/**
		 * Assigns the provided list of TuioPoint lists as the inner contours
		 * of this TuioGeometry.
		 *
		 * @param	contours	the inner contour point lists to assign
		 */
		void setInnerContours(std::list< std::list<TuioPoint> > contours);

		/**
		 * Returns the inner contours of this TuioGeometry.
		 *
		 * @return	the inner contour point lists
		 */
		std::list< std::list<TuioPoint> > getInnerContours();

		/**
		 * Assigns the skeleton branches of this TuioGeometry.
		 * @param	skltn	the skeleton branch point lists
		 */
		void setSkeleton(std::list< std::list<TuioPoint> > skltn);
		/**
		 * Returns the skeleton branches of this TuioGeometry.
		 * @return	the skeleton branch point lists
		 */
		std::list< std::list<TuioPoint> > getSkeleton();
		/**
		 * Assigns the skeleton node jump indices of this TuioGeometry.
		 * @param	jumps	the node jump indices
		 */
		void setSkeletonJumps(std::list<unsigned int> jumps);
		/**
		 * Returns the skeleton node jump indices of this TuioGeometry.
		 * @return	the node jump indices
		 */
		std::list<unsigned int> getSkeletonJumps();
		/**
		 * Assigns the per-point skeleton volume radii of this TuioGeometry.
		 * @param	vols	the skeleton volume radii
		 */
		void setSkeletonVolumes(std::list<float> vols);
		/**
		 * Returns the per-point skeleton volume radii of this TuioGeometry.
		 * @return	the skeleton volume radii
		 */
		std::list<float> getSkeletonVolumes();
		/**
		 * Assigns the area span points of this TuioGeometry.
		 * @param	points	the area span start points
		 */
		void setArea(std::list<TuioPoint> points);
		/**
		 * Returns the area span points of this TuioGeometry.
		 * @return	the area span start points
		 */
		std::list<TuioPoint> getArea();
		/**
		 * Assigns the area span widths of this TuioGeometry.
		 * @param	widths	the area span lengths
		 */
		void setAreaWidths(std::list<float> widths);
		/**
		 * Returns the area span widths of this TuioGeometry.
		 * @return	the area span lengths
		 */
		std::list<float> getAreaWidths();
		/**
		 * Assigns the raw region data of this TuioGeometry.
		 * @param	width	the relative width between data points
		 * @param	data	the raw 8bit sample data
		 */
		void setRawData(unsigned int width, std::string data);
		/**
		 * Returns the raw region data of this TuioGeometry.
		 * @return	the raw 8bit sample data
		 */
		std::string getRawData();
		/**
		 * Returns the relative width between raw region data points.
		 * @return	the raw data width
		 */
		unsigned int getRawWidth();
	};
}
#endif /* INCLUDED_TUIOGEOMETRY_H */
