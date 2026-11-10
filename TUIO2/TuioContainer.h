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

#ifndef INCLUDED_TUIOCONTAINER_H
#define INCLUDED_TUIOCONTAINER_H

#include "TuioAssociation.h"

namespace TUIO2 {
	
	/**
	 * The TuioContainer class encodes a container association (/tuio2/coa),
	 * associating one or more components to a container reference component.
	 *
	 * @author Martin Kaltenbrunner
	 * @version 2.0.b1
	 */
	class LIBDECL TuioContainer : public TuioAssociation {
		
	protected:
		/**
		 * The entry point slot of the associated components
		 */
		unsigned int slot;
		/**
		 * The session IDs of the components contained in this container
		 */
		std::list<unsigned int> contained;
		
	public:
		TuioContainer (TuioTime ttime, TuioObject *tobj, unsigned int sl, std::list<unsigned int> ctn):TuioAssociation(ttime,tobj) {
			slot = sl;
			contained = ctn;
		};
		TuioContainer (TuioObject *tobj, unsigned int sl, std::list<unsigned int> ctn):TuioAssociation(tobj) {
			slot = sl;
			contained = ctn;
		};
		TuioContainer (TuioContainer *tcnr):TuioAssociation(tcnr->getTuioTime(),tcnr->getContainingTuioObject()) {
			slot = tcnr->getSlot();
			contained = tcnr->getContained();
		};

		/**
		 * Returns the container slot of this TuioContainer.
		 * @return	the container slot
		 */
		unsigned int getSlot() const { return slot; };

		/**
		 * Assigns the container slot of this TuioContainer.
		 * @param	sl	the slot to assign
		 */
		void setSlot(unsigned int sl) { slot = sl; };

		/**
		 * Returns the contained component session IDs of this TuioContainer.
		 * @return	the contained session IDs
		 */
		std::list<unsigned int> getContained() { return contained; };

		/**
		 * Assigns the contained component session IDs of this TuioContainer.
		 * @param	ctn	the contained session IDs to assign
		 */
		void setContained(std::list<unsigned int> ctn) { contained = ctn; };
	};
}
#endif /* INCLUDED_TUIOCONTAINER_H */
