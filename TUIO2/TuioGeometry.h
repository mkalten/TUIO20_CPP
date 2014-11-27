/*
 TUIO2 C++ Library
 Copyright (c) 2009-2014 Martin Kaltenbrunner <martin@tuio.org>
 
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

#include "TuioPoint.h"
#include "TuioTime.h"

#include <list>
#include <string>

#define TUIO_IDLE 0
#define TUIO_ADDED 1
#define TUIO_ACCELERATING 2
#define TUIO_DECELERATING 3
#define TUIO_ROTATING 4
#define TUIO_STOPPED 5
#define TUIO_REMOVED 6

namespace TUIO2 {
	
	/**
	 * The abstract TuioGeometry class defines common attributes that apply to all subclasses {@link TuioOCG}.
	 *
	 * @author Ivan Dilchovski
	 * @version 2.0.a0
	 */
	
	class TuioObject;
	class LIBDECL TuioGeometry {
		
	protected:
		/**
		 * The TuioObject that contains this TUIO geometry.
		 */
		TuioObject *container;
		/**
		 * The list containing the points defining the geometry
		 */
		std::list<TuioPoint> points;
		/**
		 * The time stamp of the last update represented as TuioTime (time since session start)
		 */
		TuioTime currentTime;
		/**
		 * The creation time of this TuioPoint represented as TuioTime (time since session start)
		 */
		TuioTime startTime;
		/**
		 * Reflects the current state of the TuioComponent
		 */
		int state;
		
	public:
		using TuioPoint::update;
		
		/**
		 * This constructor takes a TuioTime argument and assigns it along with the provided
		 * Session ID and list of geometry points to the newly created TuioComponent.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	tobj	the TuioObject to assign
		 * @param	tpoints	the TuioPoints forming the basic geometry
		 */
		TuioGeometry (TuioTime ttime, TuioObject *tobj, std::list<TuioPoint> tpoints);
		
		/**
		 * This constructor takes the provided Session ID and
		 * list of geometry points to the newly created TuioComponent.
		 *
		 * @param	tobj	the TuioObject to assign
		 * @param	tpoints	the TuioPoints forming the basic geometry
		 */
		TuioGeometry (TuioObject *tobj, std::list<TuioPoint> tpoints);
		
		/**
		 * This constructor takes the atttibutes of the provided TuioGeometry
		 * and assigs these values to the newly created TuioGeometry.
		 *
		 * @param	tcon	the TuioGeometry to assign
		 */
		TuioGeometry (TuioGeometry *tgeom);
		
		/**
		 * The destructor is doing nothing in particular.
		 */
		virtual ~TuioGeometry(){};
		
		/**
		 * Returns the TuioObject containing this TuioGeometry.
		 * @return	the TuioObject containing this TuioGeometry
		 */
		virtual TuioObject* getContainingTuioObject();
		
		/**
		 * Takes a TuioTime argument and assigns it along with the provided
		 * list of points to the private TuioComponent attributes.
		 * The speed and accleration values are calculated accordingly.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	tpoints tpoints	the TuioPoints forming the basic geometry
		 */
		virtual void update (TuioTime ttime, std::list<TuioPoint> tpoints);
		
		/**
		 * Assigns the provided list of geometry points
		 * to the private TuioGeometry attributes. The TuioTime time stamp remains unchanged.
		 *
		 * @param	tpoints tpoints	the TuioPoints forming the basic geometry
		 */
		virtual void update (std::list<TuioPoint> tpoints);
		
		/**
		 * Takes the atttibutes of the provided TuioGeometry
		 * and assigs these values to this TuioGeometry.
		 * The TuioTime time stamp of this TuioGeometry remains unchanged.
		 *
		 * @param	tcon	the TuioGeometry to assign
		 */
		virtual void update(TuioGeometry *tgeom);
		
		/**
		 * Assigns the REMOVE state to this TuioGeometry and sets
		 * its TuioTime time stamp to the provided TuioTime argument.
		 *
		 * @param	ttime	the TuioTime to assign
		 */
		virtual void remove(TuioTime ttime);
		
		/**
		 * Returns the Session ID of this TuioGeometry.
		 * @return	the Session ID of this TuioGeometry
		 */
		virtual unsigned int getSessionID() const;
		
		/**
		 * Returns the points of this TuioGeometry.
		 * @return	the points of this TuioGeometry
		 */
		virtual std::list<TuioPoint> getPoints() const;
		
		/**
		 * Returns the TUIO state of this TuioComponent.
		 * @return	the TUIO state of this TuioComponent
		 */
		virtual int getTuioState() const;
		
		/**
		 * Returns current time stamp of this TuioGeometry as TuioTime
		 *
		 * @return	the  time stamp of this TuioGeometry as TuioTime
		 */
		virtual TuioTime getTuioTime() const;
		/**
		 * Returns the start time of this TuioGeometry as TuioTime.
		 *
		 * @return	the start time of this TuioGeometry as TuioTime
		 */
		virtual TuioTime getStartTime() const;
	};
}
#endif // INCLUDED_TUIOCOMPONENT_H