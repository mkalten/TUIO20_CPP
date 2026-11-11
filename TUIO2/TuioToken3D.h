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

#ifndef INCLUDED_TUIOTOKEN3D_H
#define INCLUDED_TUIOTOKEN3D_H

#include "TuioComponent3D.h"

namespace TUIO2 {
	
	/**
	 * The TuioToken3D class encodes the alternative 3D representation
	 * for tagged physical objects (/tuio2/t3d), adding the Z coordinate
	 * and the orientation axis to the common TuioToken attributes.
	 *
	 * @author Martin Kaltenbrunner
	 * @version 2.0.b1
	 */
	class LIBDECL TuioToken3D : public TuioComponent3D {
		
	protected:
		/**
		 * The symbol ID that is assigned to each TuioToken3D
		 */
		unsigned int component_id;
		/**
		 * The type ID that is assigned to each TuioToken3D
		 */
		unsigned short type_id;
		/**
		 * The user ID that is assigned to each TuioToken3D
		 */
		unsigned short user_id;
		
	public:
		/**
		 * This constructor takes a TuioTime argument, a TuioObject argument,
		 * the Type/User/Symbol IDs as well as the initial position, angle
		 * and orientation axis.
		 */
		TuioToken3D (TuioTime ttime, TuioObject *tobj, unsigned short ti, unsigned short ui, unsigned int c_id, float xp, float yp, float zp, float a, float xa, float ya, float za);

		/**
		 * This constructor takes a TuioObject argument,
		 * the Type/User/Symbol IDs as well as the initial position, angle
		 * and orientation axis.
		 */
		TuioToken3D (TuioObject *tobj, unsigned short ti, unsigned short ui, unsigned int c_id, float xp, float yp, float zp, float a, float xa, float ya, float za);

		/**
		 * This constructor takes a TuioToken3D argument and copies
		 * all of its attributes to the newly created TuioToken3D.
		 */
		TuioToken3D (TuioToken3D *ttok);

		/**
		 * Returns the Component ID of this TuioToken3D.
		 * @return	the Component ID
		 */
		unsigned int getComponentID() const;

		/**
		 * Returns the Type ID of this TuioToken3D.
		 * @return	the Type ID
		 */
		unsigned short getTypeID() const;

		/**
		 * Returns the User ID of this TuioToken3D.
		 * @return	the User ID (0 encodes the "no user" state)
		 */
		unsigned short getUserID() const;

		/**
		 * Returns the combined Type/User ID of this TuioToken3D.
		 * @return	the combined Type/User ID
		 */
		unsigned int getTypeUserID() const;

		/**
		 * Assigns the combined Type/User ID of this TuioToken3D.
		 * @param	tu_id	the combined Type/User ID to assign
		 */
		void setTypeUserID(unsigned int tu_id);
	};
}
#endif /* INCLUDED_TUIOTOKEN3D_H */
