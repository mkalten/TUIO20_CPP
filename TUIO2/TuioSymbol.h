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

#ifndef INCLUDED_TUIOSYMBOL_H
#define INCLUDED_TUIOSYMBOL_H
//#pragma clang diagnostic ignored "-Woverloaded-virtual"

#include "TuioTime.h"
#include "TuioContent.h"
#include <string>

namespace TUIO2 {

	/**
     * The TuioSymbol class encapsulates /tuio2/sym TUIO symbol.
	 *
	 * @author Martin Kaltenbrunner
	 * @version 2.0.b1
	 */
	class LIBDECL TuioSymbol : public TuioContent {

	protected:
        /**
         * The unique Session ID that is assigned to each TUIO tobj.
         */
        unsigned int session_id;
        /**
         * The individual symbol ID number that is assigned to each TuioToken.
         */
        unsigned int symbol_id;
        /**
         * The symbol type ID that is assigned to each TuioToken.
         */
        unsigned short type_id;
        /**
         * The user ID that is assigned to each TuioToken.
         */
        unsigned short user_id;
        /**
         * The symbol type descriptor
         */
        std::string symbol_type;
        /**
         * The actual symbol data
         */
        std::string symbol_data;
        /**
         * The symbol data when transmitted as OSC blob
         */
        std::string symbol_blob;
        /**
         * Reflects if the symbol data was transmitted as OSC blob
         */
        bool symbol_is_blob;
        /**
         * The time stamp of the last update represented as TuioTime (time since session start)
         */
        TuioTime currentTime;
	/**
	 * The creation time of this TuioPoint represented as TuioTime (time since session start)
	 */
	TuioTime startTime;
        /**
         * Reflects the current state of the TuioSymbol
         */
        unsigned char state;

	public:
        /**
         * This constructor takes a TuioTime argument and assigns it along with the provided
         * Session ID, Type ID, User ID and Symbol ID as well as the Symbol type and data.
         *
         * @param	ttime	the TuioTime to assign
         * @param	tobj	the TuioObject to assign
         * @param	ti	the Type ID  to assign
         * @param	ui	the User ID  to assign
         * @param	sym	the Symbol ID  to assign
         * @param	type	the symbol type descriptor
         * @param	data	the symbol data to assign
         */
		TuioSymbol (TuioTime ttime, TuioObject *tobj, unsigned short ti, unsigned short ui, unsigned int sym, const char *type, const char *data);

        /**
         * This constructor assigns the provided Session ID, Type ID, User ID and Symbol ID
         * as well as the Symbol type and data.
         *
         * @param	tobj	the TuioObject to assign
         * @param	ti	the Type ID  to assign
         * @param	ui	the User ID  to assign
         * @param	sym	the Symbol ID  to assign
         * @param	type	the symbol type descriptor
         * @param	data	the symbol data to assign
         */
        TuioSymbol (TuioObject *tobj, unsigned short ti, unsigned short ui, unsigned int sym, const char *type, const char *data);
		
		/**
		 * This constructor takes a TuioSymbol argument and sets its attributes
		 * to the ones of the provided TuioSymbol.
		 *
		 * @param	tsym	the TuioSymbol to assign
		 */
		TuioSymbol (TuioSymbol *tsym);
        
        /**
         * Returns the Symbol ID of this TuioSymbol.
         * @return	the Symbol ID of this TuioSymbol
         */
        unsigned int getSymbolID() const;
        
        /**
         * Returns the Type ID of this TuioSymbol.
         * @return	the Type ID of this TuioSymbol
         */
        unsigned short getTypeID() const;
        
        /**
         * Returns the User ID of this TuioSymbol.
         * @return	the User ID of this TuioSymbol
         */
        unsigned short getUserID() const;
        
        /**
         * Returns the encoded Type & User ID of this TuioSymbol.
         * @return	the encoded Type & User ID of this TuioSymbol
         */
        unsigned int getTypeUserID() const;
        
        /**
         * Decodes and assigns the Type & User ID to this TuioSymbol.
         * @param tu_id   the encoded Type & User ID of this TuioSymbol
         */
        void setTypeUserID(unsigned int tu_id);
        
        /**
         * Returns the Type string of this TuioSymbol.
         * @return	the Type string of this TuioSymbol
         */
        const char* getSymbolType() const;
        
        /**
         * Returns the actual data of this TuioSymbol.
         * @return	the actual data of this TuioSymbol
         */
        const char* getSymbolData() const;

        /**
         * This method assigns string data to this TuioSymbol.
         *
         * @param	data	the symbol data to assign
         */
        void setSymbolData(const char *data);

        /**
         * This method tests if the symbol data was transmitted as OSC blob.
         * @return	true if the symbol data is a blob
         */
        bool isSymbolDataBlob() const;

        /**
         * This method assigns OSC blob data to this TuioSymbol.
         *
         * @param	data	the blob data to assign
         * @param	size	the blob data size
         */
        void setSymbolDataBlob(const void *data, int size);

        /**
         * This method returns the symbol blob data of this TuioSymbol.
         * @param	size	returns the blob data size
         * @return	the blob data
         */
        const void* getSymbolDataBlob(int *size) const;
 
        /**
         * This method refreshes the currentTime after an update
         */
        void update(TuioTime ttime);

	};
}
#endif
