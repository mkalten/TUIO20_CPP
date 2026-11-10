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

#ifndef INCLUDED_TUIOBOUNDS3D_H
#define INCLUDED_TUIOBOUNDS3D_H

#include "TuioComponent3D.h"

namespace TUIO2 {
	
	/**
	 * The TuioBounds3D class encodes the alternative 3D representation
	 * for untagged generic objects (/tuio2/b3d), adding the Z coordinate,
	 * orientation axis, depth and volume to the common TuioBounds attributes.
	 *
	 * @author Martin Kaltenbrunner
	 * @version 2.0.b1
	 */
	class LIBDECL TuioBounds3D : public TuioComponent3D {
		
	protected:
		/**
		 * The width of this TuioBounds3D
		 */
		float width;
		/**
		 * The height of this TuioBounds3D
		 */
		float height;
		/**
		 * The depth of this TuioBounds3D
		 */
		float depth;
		/**
		 * The volume of this TuioBounds3D
		 */
		float volume;
		
	public:
		/**
		 * This constructor takes a TuioTime argument, a TuioObject argument
		 * as well as the initial position, angle, orientation axis and
		 * the bounds dimensions.
		 */
		TuioBounds3D (TuioTime ttime, TuioObject *tobj, float xp, float yp, float zp, float a, float xa, float ya, float za, float w, float h, float d, float v);

		/**
		 * This constructor takes a TuioObject argument
		 * as well as the initial position, angle, orientation axis and
		 * the bounds dimensions.
		 */
		TuioBounds3D (TuioObject *tobj, float xp, float yp, float zp, float a, float xa, float ya, float za, float w, float h, float d, float v);

		/**
		 * This constructor takes a TuioBounds3D argument and copies
		 * all of its attributes to the newly created TuioBounds3D.
		 */
		TuioBounds3D (TuioBounds3D *tbnd);

		float getWidth() const;
		float getHeight() const;
		float getDepth() const;
		float getVolume() const;
	};
}
#endif /* INCLUDED_TUIOBOUNDS3D_H */
