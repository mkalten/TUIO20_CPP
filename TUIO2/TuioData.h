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

#ifndef INCLUDED_TUIODATA_H
#define INCLUDED_TUIODATA_H

#include "TuioContent.h"

namespace TUIO2 {
	
	/**
	 * The TuioData class encodes the arbitrary data content of a
	 * /tuio2/dat message, associated to a TuioObject component.
	 * The data attribute carries a MIME type descriptor as well as
	 * its actual content, which can be an OSC string or an OSC blob.
	 *
	 * @author Martin Kaltenbrunner
	 * @version 2.0.b1
	 */
	class LIBDECL TuioData : public TuioContent {
		
	protected:
		/**
		 * The MIME type descriptor of this TuioData
		 */
		std::string mime_type;
		/**
		 * The data content when transmitted as OSC string
		 */
		std::string data_content;
		/**
		 * The data content when transmitted as OSC blob
		 */
		std::string data_blob;
		/**
		 * Reflects if the data content was transmitted as OSC blob
		 */
		bool data_is_blob;
		
	public:
		/**
		 * This constructor takes a TuioTime argument, a TuioObject argument
		 * as well as the MIME type and content of this TuioData.
		 *
		 * @param	ttime	the TuioTime to assign
		 * @param	tobj	the TuioObject to assign
		 * @param	mime	the MIME type descriptor
		 * @param	data	the data content to assign
		 */
		TuioData (TuioTime ttime, TuioObject *tobj, const char *mime, const char *data);

		/**
		 * This constructor takes a TuioObject argument as well as the
		 * MIME type and content of this TuioData.
		 *
		 * @param	tobj	the TuioObject to assign
		 * @param	mime	the MIME type descriptor
		 * @param	data	the data content to assign
		 */
		TuioData (TuioObject *tobj, const char *mime, const char *data);

		/**
		 * This constructor takes a TuioData argument and copies
		 * all of its attributes to the newly created TuioData.
		 *
		 * @param	tdat	the TuioData to assign
		 */
		TuioData (TuioData *tdat);

		/**
		 * This method refreshes the currentTime after an update.
		 *
		 * @param	ttime	the TuioTime to assign
		 */
		void update(TuioTime ttime);


		/**
		 * Returns the MIME type descriptor of this TuioData.
		 *
		 * @return	the MIME type descriptor
		 */
		const char* getMimeType() const;

		/**
		 * Returns the string content of this TuioData.
		 *
		 * @return	the data content
		 */
		const char* getDataContent() const;

		/**
		 * This method assigns string content to this TuioData.
		 *
		 * @param	data	the data content to assign
		 */
		void setDataContent(const char *data);

		/**
		 * This method tests if the data content was transmitted as OSC blob.
		 * @return	true if the data content is a blob
		 */
		bool isDataBlob() const;

		/**
		 * This method assigns OSC blob content to this TuioData.
		 *
		 * @param	data	the blob data to assign
		 * @param	size	the blob data size
		 */
		void setDataBlob(const void *data, int size);

		/**
		 * This method returns the data blob of this TuioData.
		 * @param	size	returns the blob data size
		 * @return	the blob data
		 */
		const void* getDataBlob(int *size) const;
	};
}
#endif /* INCLUDED_TUIODATA_H */
