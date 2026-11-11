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

#ifndef INCLUDED_TUIOCOMPONENT3D_H
#define INCLUDED_TUIOCOMPONENT3D_H

#include "TuioComponent.h"

namespace TUIO2 {
	
	/**
	 * The TuioComponent3D class extends the 2D TuioComponent with the
	 * additional Z coordinate and the orientation axis required by the
	 * alternative 3D component representations (T3D, P3D, B3D).
	 *
	 * @author Martin Kaltenbrunner
	 * @version 2.0.b1
	 */
	class LIBDECL TuioComponent3D : public TuioComponent {
		
	protected:
		/**
		 * The Z coordinate of this component
		 */
		float zpos;
		/**
		 * The velocity of the Z coordinate
		 */
		float zspeed;
		/**
		 * The X axis orientation component
		 */
		float xaxis;
		/**
		 * The Y axis orientation component
		 */
		float yaxis;
		/**
		 * The Z axis orientation component
		 */
		float zaxis;
		
	public:
		/**
		 * This constructor takes a TuioTime argument and a TuioObject argument
		 * as well as the initial position, angle and orientation axis.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	tobj	the TuioObject that contains this component
		 * @param	xp	the X coordinate to assign
		 * @param	yp	the Y coordinate to assign
		 * @param	zp	the Z coordinate to assign
		 * @param	a	the angle to assign
		 * @param	xa	the X axis to assign
		 * @param	ya	the Y axis to assign
		 * @param	za	the Z axis to assign
		 */
		TuioComponent3D (TuioTime ttime, TuioObject *tobj, float xp, float yp, float zp, float a, float xa, float ya, float za);

		/**
		 * This constructor takes a TuioObject argument as well as
		 * the initial position, angle and orientation axis.
		 */
		TuioComponent3D (TuioObject *tobj, float xp, float yp, float zp, float a, float xa, float ya, float za);

		/**
		 * This constructor takes a TuioComponent3D argument and copies
		 * all of its attributes to the newly created component.
		 */
		TuioComponent3D (TuioComponent3D *tcon);

		/**
		 * This method updates the component's position, angle and orientation axis
		 * and calculates its speed and acceleration values.
		 */
		virtual void update (TuioTime ttime, float xp, float yp, float zp, float a, float xa, float ya, float za);

		/**
		 * This method updates the component with the provided speed and acceleration values.
		 */
		virtual void update (TuioTime ttime, float xp, float yp, float zp, float a, float xa, float ya, float za, float xs, float ys, float zs, float rs, float ma, float ra);

		/**
		 * This method updates the component's position, angle and orientation axis
		 * with the provided time stamp, marking it as stopped.
		 *
		 * @param	ttime	the TuioTime to assign
		 */
		virtual void stop(TuioTime ttime);

		/**
		 * Returns the Z coordinate of this component.
		 * The Z-axis values are relative to the sensor height.
		 * @return	the Z coordinate
		 */
		float getZ() const;

		/**
		 * Returns the Z coordinate velocity of this component.
		 * @return	the Z velocity
		 */
		float getZSpeed() const;

		/**
		 * Returns the X axis orientation component.
		 * @return	the X axis
		 */
		float getXAxis() const;

		/**
		 * Returns the Y axis orientation component.
		 * @return	the Y axis
		 */
		float getYAxis() const;

		/**
		 * Returns the Z axis orientation component.
		 * @return	the Z axis
		 */
		float getZAxis() const;
	};
}
#endif /* INCLUDED_TUIOCOMPONENT3D_H */
