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

#ifndef INCLUDED_TUIOSIGNAL_H
#define INCLUDED_TUIOSIGNAL_H

#include "TuioContent.h"

namespace TUIO2 {
	
	/**
	 * The TuioSignal class encodes the trigger signal of a /tuio2/sig
	 * message, sent from a reference component to a list of target
	 * components identified by their session IDs.
	 *
	 * @author Martin Kaltenbrunner
	 * @version 2.0.b1
	 */
	class LIBDECL TuioSignal : public TuioContent {
		
	protected:
		/**
		 * The signal type ID of this TuioSignal
		 */
		unsigned int signal_id;
		/**
		 * The session IDs of the signal target components
		 */
		std::list<unsigned int> targets;
		
	public:
		/**
		 * This constructor takes a TuioTime argument, a TuioObject argument
		 * as well as the signal ID and target session IDs of this TuioSignal.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	tobj	the TuioObject to assign
		 * @param	sid	the signal ID to assign
		 * @param	trgts	the target session IDs to assign
		 */
		TuioSignal (TuioTime ttime, TuioObject *tobj, unsigned int sid, std::list<unsigned int> trgts);

		/**
		 * This constructor takes a TuioObject argument as well as the
		 * signal ID and target session IDs of this TuioSignal.
		 *
		 * @param	tobj	the TuioObject to assign
		 * @param	sid	the signal ID to assign
		 * @param	trgts	the target session IDs to assign
		 */
		TuioSignal (TuioObject *tobj, unsigned int sid, std::list<unsigned int> trgts);

		/**
		 * This constructor takes a TuioSignal argument and copies
		 * all of its attributes to the newly created TuioSignal.
		 *
		 * @param	tsig	the TuioSignal to assign
		 */
		TuioSignal (TuioSignal *tsig);

		/**
		 * This method refreshes the currentTime after an update.
		 *
		 * @param	ttime	the TuioTime to assign
		 */
		void update(TuioTime ttime);


		/**
		 * Returns the signal ID of this TuioSignal.
		 *
		 * @return	the signal ID
		 */
		unsigned int getSignalID() const;

		/**
		 * Assigns the signal ID of this TuioSignal.
		 *
		 * @param	sid	the signal ID to assign
		 */
		void setSignalID(unsigned int sid);

		/**
		 * Returns the target session IDs of this TuioSignal.
		 *
		 * @return	the signal target session IDs
		 */
		std::list<unsigned int> getSignalTargets();

		/**
		 * Assigns the target session IDs of this TuioSignal.
		 *
		 * @param	trgts	the target session IDs to assign
		 */
		void setSignalTargets(std::list<unsigned int> trgts);
	};
}
#endif /* INCLUDED_TUIOSIGNAL_H */
