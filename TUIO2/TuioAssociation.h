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
		 * A Link ID is comprised of two 16bit unsigned integer values
		 * embedded into a single 32bit integer value: the output port
		 * within the initial two bytes and the input port of the connected
		 * component within the last two bytes.
		 *
		 * @param	l_id	the Link ID to decode
		 * @return	the output port of the provided Link ID
		 */
		static unsigned short getLinkOutputPort(unsigned int l_id) { return (unsigned short)(l_id>>16); };

		/**
		 * Returns the input port of the provided Link ID.
		 *
		 * @param	l_id	the Link ID to decode
		 * @return	the input port of the provided Link ID
		 */
		static unsigned short getLinkInputPort(unsigned int l_id) { return (unsigned short)(l_id&0xFFFF); };

		/**
		 * Encodes a Link ID from the provided output and input ports.
		 *
		 * @param	output	the output port to encode
		 * @param	input	the input port to encode
		 * @return	the combined Link ID
		 */
		static unsigned int setLinkPort(unsigned short output, unsigned short input) { return (((unsigned int)output)<<16)|((unsigned int)input); };
	};
}
#endif /* INCLUDED_TUIOASSOCIATION_H */
