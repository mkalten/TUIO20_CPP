/*
 TUIO2 C++ Library
 Copyright (c) 2009-2014 Martin Kaltenbrunner <martin@tuio.org>
 
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

#ifndef WIN32
#include <pthread.h>
#else
#include <windows.h>
#endif

namespace TUIO2 {
	
	/**
	 * <p>The TuioDispatcher generates TUIO events which are broadcasted to all 
	 * registered classes that implement the {@link TuioListener} interface.</p> 
	 *
	 * @author Martin Kaltenbrunner
	 * @version 2.0.a0
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
         * Returns the TuioObject corresponding to the provided Session ID
         * or NULL if the Session ID does not refer to an active TuioObject
         *
         * @return  an active TuioObject corresponding to the provided Session ID or NULL
         */
        TuioObject* getTuioObject(int s_id);
        
		/**
		 * Returns the TuioToken corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioToken
		 *
		 * @return  an active TuioToken corresponding to the provided Session ID or NULL
		 */
		TuioToken* getTuioToken(int s_id);

		/**
		 * Returns the TuioPointer corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioPointer
		 *
		 * @return  an active TuioPointer corresponding to the provided Session ID or NULL
		 */
		TuioPointer* getTuioPointer(int s_id);

		/**
		 * Returns the TuioBounds corresponding to the provided Session ID
		 * or NULL if the Session ID does not refer to an active TuioBounds
		 *
		 * @return  an active TuioBounds corresponding to the provided Session ID or NULL
		 */
		TuioBounds* getTuioBounds(int s_id);
        
        /**
         * Returns the TuioSymbol corresponding to the provided Session ID
         * or NULL if the Session ID does not refer to an active TuioSymbol
         *
         * @return  an active TuioSymbol corresponding to the provided Session ID or NULL
         */
        TuioSymbol* getTuioSymbol(int s_id);
		
		/**
		 * Locks the TuioToken list in order to avoid updates during access
		 */
		void lockContainerList();

		/**
		 * Releases the lock of the TuioToken list
		 */
		void unlockContainerList();
		
	protected:
		std::list<TuioListener*> listenerList;
		std::list<TuioObject*> tobjList;
		
#ifndef WIN32
		pthread_mutex_t tobjMutex;
#else
		HANDLE tobjMutex;
#endif	
				
	};
}
#endif /* INCLUDED_TUIODISPATCHER_H */
