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

#ifndef INCLUDED_TUIOLIST_H
#define INCLUDED_TUIOLIST_H

#include "TuioAssociation.h"

namespace TUIO2 {
	
	/**
	 * The TuioList class encodes a linked list association (/tuio2/lla),
	 * describing consecutive chains of connected components.
	 * Each list entry is a pair of the target session ID and its link ID.
	 *
	 * @author Martin Kaltenbrunner
	 * @version 2.0.b1
	 */
	class LIBDECL TuioList : public TuioAssociation {
		
	protected:
		/**
		 * Reflects if the association is physical (true) or logical (false)
		 */
		bool physical;
		/**
		 * The ordered links of this TuioList as (session ID, link ID) pairs
		 */
		std::list< std::pair<unsigned int,unsigned int> > links;
		
	public:
		TuioList (TuioTime ttime, TuioObject *tobj, bool phy, std::list< std::pair<unsigned int,unsigned int> > lnks):TuioAssociation(ttime,tobj) {
			physical = phy;
			links = lnks;
		};
		TuioList (TuioObject *tobj, bool phy, std::list< std::pair<unsigned int,unsigned int> > lnks):TuioAssociation(tobj) {
			physical = phy;
			links = lnks;
		};
		TuioList (TuioList *tlst):TuioAssociation(tlst->getTuioTime(),tlst->getContainingTuioObject()) {
			physical = tlst->isPhysical();
			links = tlst->getLinks();
		};

		bool isPhysical() const { return physical; };
		void setPhysical(bool phy) { physical = phy; };
		std::list< std::pair<unsigned int,unsigned int> > getLinks() { return links; };
		void setLinks(std::list< std::pair<unsigned int,unsigned int> > lnks) { links = lnks; };
	};
}
#endif /* INCLUDED_TUIOLIST_H */
