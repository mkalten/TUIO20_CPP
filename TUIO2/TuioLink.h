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

#ifndef INCLUDED_TUIOLINK_H
#define INCLUDED_TUIOLINK_H

#include "TuioAssociation.h"

namespace TUIO2 {
	
	/**
	 * The TuioLink class encodes a link association (/tuio2/lia),
	 * describing physical or logical connections between the reference
	 * component and a list of linked components. Each link is a pair of
	 * the target session ID and its link ID (input/output port pair).
	 *
	 * @author Martin Kaltenbrunner
	 * @version 2.0.b1
	 */
	class LIBDECL TuioLink : public TuioAssociation {
		
	protected:
		/**
		 * Reflects if the association is physical (true) or logical (false)
		 */
		bool physical;
		/**
		 * The links of this TuioLink as (session ID, link ID) pairs
		 */
		std::list< std::pair<unsigned int,unsigned int> > links;
		
	public:
		TuioLink (TuioTime ttime, TuioObject *tobj, bool phy, std::list< std::pair<unsigned int,unsigned int> > lnks):TuioAssociation(ttime,tobj) {
			physical = phy;
			links = lnks;
		};
		TuioLink (TuioObject *tobj, bool phy, std::list< std::pair<unsigned int,unsigned int> > lnks):TuioAssociation(tobj) {
			physical = phy;
			links = lnks;
		};
		TuioLink (TuioLink *tlnk):TuioAssociation(tlnk->getTuioTime(),tlnk->getContainingTuioObject()) {
			physical = tlnk->isPhysical();
			links = tlnk->getLinks();
		};

		/**
		 * Tests if this is a physical association.
		 * @return	true if physical, false if logical
		 */
		bool isPhysical() const { return physical; };

		/**
		 * Assigns the physical/logical state of this TuioLink.
		 * @param	phy	the physical state to assign
		 */
		void setPhysical(bool phy) { physical = phy; };

		/**
		 * Returns the links of this TuioLink as (session ID, link ID) pairs.
		 * @return	the link pairs
		 */
		std::list< std::pair<unsigned int,unsigned int> > getLinks() { return links; };

		/**
		 * Assigns the links of this TuioLink.
		 * @param	lnks	the link pairs to assign
		 */
		void setLinks(std::list< std::pair<unsigned int,unsigned int> > lnks) { links = lnks; };
	};
}
#endif /* INCLUDED_TUIOLINK_H */
