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

#ifndef INCLUDED_TUIOCONTENT_H
#define INCLUDED_TUIOCONTENT_H

#ifndef TUIO_IDLE
#define TUIO_IDLE 0
#define TUIO_ADDED 1
#define TUIO_ACCELERATING 2
#define TUIO_DECELERATING 3
#define TUIO_ROTATING 4
#define TUIO_STOPPED 5
#define TUIO_REMOVED 6
#endif

#include "LibExport.h"
#include "TuioTime.h"
#include <list>
#include <string>

namespace TUIO2 {

	class TuioObject;

	/**
	 * The TuioContent class is the base class for all non-positional
	 * content that can be associated to a TuioObject via its session ID,
	 * such as symbols, geometry descriptors, control dimensions,
	 * arbitrary data payloads and trigger signals.
	 *
	 * @author Martin Kaltenbrunner
	 * @version 2.0.b1
	 */
	class LIBDECL TuioContent {
		
	protected:
		/**
		 * The TuioObject that contains this TuioContent
		 */
		TuioObject *container;
		/**
		 * The time stamp of the last update represented as TuioTime
		 */
		TuioTime currentTime;
		/**
		 * Reflects the current state of the TuioContent
		 */
		unsigned char state;
		
	public:
		/**
		 * This constructor takes a TuioTime and a TuioObject argument.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	tobj	the TuioObject that contains this TuioContent
		 */
		TuioContent (TuioTime ttime, TuioObject *tobj);

		/**
		 * This constructor takes a TuioObject argument.
		 *
		 * @param	tobj	the TuioObject that contains this TuioContent
		 */
		TuioContent (TuioObject *tobj);

		/**
		 * This method assigns the containing TuioObject.
		 *
		 * @param	tobj	the TuioObject to assign
		 */
		void setContainingTuioObject(TuioObject *tobj);

		/**
		 * Returns the containing TuioObject.
		 *
		 * @return	the containing TuioObject
		 */
		TuioObject* getContainingTuioObject();

		/**
		 * Returns the time stamp of the last update of this TuioContent.
		 *
		 * @return	the TuioTime of the last update
		 */
		TuioTime getTuioTime();

		/**
		 * Returns the current state of this TuioContent.
		 *
		 * @return	the state of this TuioContent
		 */
		int getTuioState();

		/**
		 * Returns the Session ID of the containing TuioObject.
		 *
		 * @return	the Session ID of the containing TuioObject
		 */
		unsigned int getSessionID();

		/**
		 * This method refreshes the currentTime of this TuioContent.
		 *
		 * @param	ttime	the TuioTime to assign
		 */
		virtual void update(TuioTime ttime);

		/**
		 * This method marks this TuioContent as removed.
		 *
		 * @param	ttime	the TuioTime to assign
		 */
		virtual void remove(TuioTime ttime);
	};
}
#endif /* INCLUDED_TUIOCONTENT_H */
