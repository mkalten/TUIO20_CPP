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

#ifndef INCLUDED_TUIODISPATCHER_H
#define INCLUDED_TUIODISPATCHER_H

#include "TuioListener.h"

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace TUIO2 {

	/**
	 * <p>The TuioDispatcher generates TUIO events which are broadcasted to all
	 * registered classes that implement the {@link TuioListener} interface.</p>
	 *
	 * @author Martin Kaltenbrunner
	 * @version 2.0.b1
	 */
	class LIBDECL TuioDispatcher {

	public:
		/**
		 * This constructor creates a TuioDispatcher
		 *
		 */
		TuioDispatcher();

		/**
		 * The destructor is doing nothing in particular.
		 */
		~TuioDispatcher();

		/**
		 * Adds the provided TuioListener to the list of registered TUIO event listeners
		 *
		 * @param  listener  the TuioListener to add
		 */
		void addTuioListener(TuioListener *listener);

		/**
		 * Removes the provided TuioListener from the list of registered TUIO event listeners
		 *
		 * @param  listener  the TuioListener to remove
		 */
		void removeTuioListener(TuioListener *listener);

		/**
		 * Removes all TuioListener from the list of registered TUIO event listeners
		 */
		void removeAllTuioListeners();

		/**
		 * Returns a List of all currently active TuioObject
		 *
		 * @return  a List of all currently active TuioObject
		 */
		std::list<TuioObject*> getTuioObjectList();

		/**
		 * Returns a List of all currently active TuioTokens
		 *
		 * @return  a List of all currently active TuioTokens
		 */
		std::list<TuioToken*> getTuioTokenList();

		/**
		 * Returns a List of all currently active TuioPointers
		 *
		 * @return  a List of all currently active TuioPointers
		 */
		std::list<TuioPointer*> getTuioPointerList();

		/**
		 * Returns a List of all currently active TuioBounds
		 *
		 * @return  a List of all currently active TuioBounds
		 */
		std::list<TuioBounds*> getTuioBoundsList();

		/**
		* Returns a List of all currently active TuioSymbols
		*
		* @return  a List of all currently active TuioSymbols
		*/
		std::list<TuioSymbol*> getTuioSymbolList();

        /**
		* Returns a List of all currently active TuioGeometry
		*
		* @return  a List of all currently active TuioGeometry
		*/
		std::list<TuioGeometry*> getTuioGeometryList();

        /**
		* Returns a List of all currently active TuioControl
		*
		* @return  a List of all currently active TuioControl
		*/
		std::list<TuioControl*> getTuioControlList();

        /**
		* Returns a List of all currently active TuioData
		*
		* @return  a List of all currently active TuioData
		*/
		std::list<TuioData*> getTuioDataList();

        /**
		* Returns a List of all currently active TuioSignal
		*
		* @return  a List of all currently active TuioSignal
		*/
		std::list<TuioSignal*> getTuioSignalList();

        /**
		* Returns a List of all currently active TuioAssociations
		*
		* @return  a List of all currently active TuioAssociations
		*/
		std::list<TuioAssociation*> getTuioAssociationList();

        /**
		* Returns a List of all currently active TuioToken3D
		*
		* @return  a List of all currently active TuioToken3D
		*/
		std::list<TuioToken3D*> getTuioToken3DList();

        /**
		* Returns a List of all currently active TuioPointer3D
		*
		* @return  a List of all currently active TuioPointer3D
		*/
		std::list<TuioPointer3D*> getTuioPointer3DList();

        /**
		* Returns a List of all currently active TuioBounds3D
		*
		* @return  a List of all currently active TuioBounds3D
		*/
		std::list<TuioBounds3D*> getTuioBounds3DList();

		/**
		* Returns the TuioObject corresponding to the provided Session ID
		* or NULL if the Session ID does not refer to an active TuioObject
		*
		* @return  an active TuioObject corresponding to the provided Session ID or NULL
		*/
		TuioObject* getTuioObject(unsigned int s_id);

		/**
		 * Returns the TuioToken corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioToken
		 *
		 * @return  an active TuioToken corresponding to the provided Session ID or NULL
		 */
		TuioToken* getTuioToken(unsigned int s_id);

		/**
		 * Returns the TuioPointer corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioPointer
		 *
		 * @return  an active TuioPointer corresponding to the provided Session ID or NULL
		 */
		TuioPointer* getTuioPointer(unsigned int s_id);

		/**
		 * Returns the TuioBounds corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioBounds
		 *
		 * @return  an active TuioBounds corresponding to the provided Session ID or NULL
		 */
		TuioBounds* getTuioBounds(unsigned int s_id);

		/**
		* Returns the TuioSymbol corresponding to the provided Session ID
		* or NULL if the Session ID does not refer to an active TuioSymbol
		*
		* @return  an active TuioSymbol corresponding to the provided Session ID or NULL
		*/
		TuioSymbol* getTuioSymbol(unsigned int s_id);

		/**
		 * Locks the TuioObject list in order to avoid updates during access
		 */
		void lockObjectList();

		/**
		 * Releases the lock of the TuioObject list
		 */
		void unlockObjectList();

	protected:
		std::list<TuioListener*> listenerList;
		std::list<TuioObject*> tobjList;

#ifdef WIN32
		HANDLE tobjMutex;
#else
		pthread_mutex_t tobjMutex;
#endif

	};
}
#endif /* INCLUDED_TUIODISPATCHER_H */
