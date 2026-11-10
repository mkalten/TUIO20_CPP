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

#ifndef INCLUDED_TUIOCONTROL_H
#define INCLUDED_TUIOCONTROL_H

#include "TuioContent.h"

namespace TUIO2 {
	
	/**
	 * The TuioControl class encodes the additional control dimensions
	 * of a /tuio2/ctl message, associated to a TuioObject component.
	 * Each control dimension is a pair (isBool,value) describing a
	 * discrete bool (0/1) or a continuous float (-1.0f ... 1.0f).
	 *
	 * @author Martin Kaltenbrunner
	 * @version 2.0.b1
	 */
	class LIBDECL TuioControl : public TuioContent {
		
	protected:
		/**
		 * The ordered list of control dimensions (isBool,value)
		 */
		std::list< std::pair<bool,float> > controls;
		
	public:
		/**
		 * This constructor takes a TuioTime argument and a TuioObject argument
		 * and creates an empty TuioControl.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	tobj	the TuioObject to assign
		 */
		TuioControl (TuioTime ttime, TuioObject *tobj);

		/**
		 * This constructor takes a TuioObject argument
		 * and creates an empty TuioControl.
		 *
		 * @param	tobj	the TuioObject to assign
		 */
		TuioControl (TuioObject *tobj);

		/**
		 * This constructor takes a TuioControl argument and copies
		 * all of its attributes to the newly created TuioControl.
		 *
		 * @param	tctl	the TuioControl to assign
		 */
		TuioControl (TuioControl *tctl);

		/**
		 * This method refreshes the currentTime after an update.
		 *
		 * @param	ttime	the TuioTime to assign
		 */
		void update(TuioTime ttime);

		/**
		 * This method updates the TuioControl with the provided time stamp,
		 * marking it as stopped.
		 *
		 * @param	ttime	the TuioTime to assign
		 */
		void stop(TuioTime ttime);

		/**
		 * Assigns the control dimension list of this TuioControl.
		 * Each entry is a pair (isBool,value) where boolean dimensions
		 * are stored as 0/1 float values.
		 *
		 * @param	ctls	the control dimensions to assign
		 */
		void setControls(std::list< std::pair<bool,float> > ctls);

		/**
		 * Returns the control dimension list of this TuioControl.
		 *
		 * @return	the control dimensions
		 */
		std::list< std::pair<bool,float> > getControls();
	};
}
#endif /* INCLUDED_TUIOCONTROL_H */
