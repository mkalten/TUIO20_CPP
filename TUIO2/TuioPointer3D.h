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

#ifndef INCLUDED_TUIOPOINTER3D_H
#define INCLUDED_TUIOPOINTER3D_H

#include "TuioComponent3D.h"

namespace TUIO2 {
	
	/**
	 * The TuioPointer3D class encodes the alternative 3D representation
	 * for pointing gestures (/tuio2/p3d), adding the Z coordinate and
	 * the pointing direction vector to the common TuioPointer attributes.
	 *
	 * @author Martin Kaltenbrunner
	 * @version 2.0.b1
	 */
	class LIBDECL TuioPointer3D : public TuioComponent3D {
		
	protected:
		/**
		 * The pointer ID that is assigned to each TuioPointer3D
		 */
		unsigned int pointer_id;
		/**
		 * The type ID that is assigned to each TuioPointer3D
		 */
		unsigned short type_id;
		/**
		 * The user ID that is assigned to each TuioPointer3D
		 */
		unsigned short user_id;
		/**
		 * The pointer's spherical region of influence
		 */
		float radius;
		
	public:
		/**
		 * This constructor takes a TuioTime argument, a TuioObject argument,
		 * the Type/User/Pointer IDs as well as the initial position,
		 * direction vector and radius.
		 */
		TuioPointer3D (TuioTime ttime, TuioObject *tobj, unsigned short ti, unsigned short ui, unsigned int pi, float xp, float yp, float zp, float xa, float ya, float za, float r);

		/**
		 * This constructor takes a TuioObject argument,
		 * the Type/User/Pointer IDs as well as the initial position,
		 * direction vector and radius.
		 */
		TuioPointer3D (TuioObject *tobj, unsigned short ti, unsigned short ui, unsigned int pi, float xp, float yp, float zp, float xa, float ya, float za, float r);

		/**
		 * This constructor takes a TuioPointer3D argument and copies
		 * all of its attributes to the newly created TuioPointer3D.
		 */
		TuioPointer3D (TuioPointer3D *tptr);

		unsigned int getPointerID() const;
		unsigned short getTypeID() const;
		unsigned short getUserID() const;
		unsigned int getTypeUserID() const;
		void setTypeUserID(unsigned int tu_id);

		/**
		 * Returns the pointer's region of influence radius.
		 * @return	the pointer radius
		 */
		float getRadius() const;
	};
}
#endif /* INCLUDED_TUIOPOINTER3D_H */
