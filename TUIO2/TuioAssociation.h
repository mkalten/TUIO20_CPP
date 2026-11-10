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

#ifndef INCLUDED_TUIOASSOCIATION_H
#define INCLUDED_TUIOASSOCIATION_H

#include "TuioContent.h"

namespace TUIO2 {
	
	/**
	 * The TuioAssociation class is the base class for all relationship
	 * descriptors that associate a reference TuioObject to one or more
	 * other components, such as container, link, list and tree
	 * associations.
	 *
	 * @author Martin Kaltenbrunner
	 * @version 2.0.b1
	 */
	class LIBDECL TuioAssociation : public TuioContent {
		
	public:
		/**
		 * This constructor takes a TuioTime and a TuioObject argument.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	tobj	the reference TuioObject of this association
		 */
		TuioAssociation (TuioTime ttime, TuioObject *tobj):TuioContent(ttime,tobj) {};

		/**
		 * This constructor takes a TuioObject argument.
		 *
		 * @param	tobj	the reference TuioObject of this association
		 */
		TuioAssociation (TuioObject *tobj):TuioContent(tobj) {};

		/**
		 * This method refreshes the currentTime after an update.
		 *
		 * @param	ttime	the TuioTime to assign
		 */
		virtual void update(TuioTime ttime) { TuioContent::update(ttime); }

		/**
		 * This method updates this TuioAssociation with the provided time stamp,
		 * marking it as stopped.
		 *
		 * @param	ttime	the TuioTime to assign
		 */
		void stop(TuioTime ttime) { update(ttime); }
	};
}
#endif /* INCLUDED_TUIOASSOCIATION_H */
