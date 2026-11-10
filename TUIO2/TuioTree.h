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

#ifndef INCLUDED_TUIOTREE_H
#define INCLUDED_TUIOTREE_H

#include "TuioAssociation.h"

namespace TUIO2 {
	
	/**
	 * The TuioTree class encodes a linked tree association (/tuio2/lta),
	 * describing tree structures of connected components. Each tree entry
	 * is a pair of the target session ID and its link ID, while float
	 * node-jump markers indicate back-references to previous nodes.
	 *
	 * @author Martin Kaltenbrunner
	 * @version 2.0.b1
	 */
	class LIBDECL TuioTree : public TuioAssociation {
		
	protected:
		/**
		 * Reflects if the association is physical (true) or logical (false)
		 */
		bool physical;
		/**
		 * The links of this TuioTree as (session ID, link ID) pairs
		 */
		std::list< std::pair<unsigned int,unsigned int> > links;
		/**
		 * The float node-jump markers ordering the tree traversal
		 */
		std::list<float> nodeJumps;
		
	public:
		TuioTree (TuioTime ttime, TuioObject *tobj, bool phy, std::list< std::pair<unsigned int,unsigned int> > lnks, std::list<float> jmps):TuioAssociation(ttime,tobj) {
			physical = phy;
			links = lnks;
			nodeJumps = jmps;
		};
		TuioTree (TuioObject *tobj, bool phy, std::list< std::pair<unsigned int,unsigned int> > lnks, std::list<float> jmps):TuioAssociation(tobj) {
			physical = phy;
			links = lnks;
			nodeJumps = jmps;
		};
		TuioTree (TuioTree *ttre):TuioAssociation(ttre->getTuioTime(),ttre->getContainingTuioObject()) {
			physical = ttre->isPhysical();
			links = ttre->getLinks();
			nodeJumps = ttre->getNodeJumps();
		};

		bool isPhysical() const { return physical; };
		void setPhysical(bool phy) { physical = phy; };
		std::list< std::pair<unsigned int,unsigned int> > getLinks() { return links; };
		void setLinks(std::list< std::pair<unsigned int,unsigned int> > lnks) { links = lnks; };
		std::list<float> getNodeJumps() { return nodeJumps; };
		void setNodeJumps(std::list<float> jmps) { nodeJumps = jmps; };
	};
}
#endif /* INCLUDED_TUIOTREE_H */
