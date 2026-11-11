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

#include "TuioData.h"
#include "TuioObject.h"
using namespace TUIO2;

TuioData::TuioData (TuioTime ttime, TuioObject *tobj, const char *mime, const char *data):TuioContent(ttime,tobj) {
	mime_type = std::string(mime);
	data_content = std::string(data);
	data_is_blob = false;
}

TuioData::TuioData (TuioObject *tobj, const char *mime, const char *data):TuioContent(tobj) {
	mime_type = std::string(mime);
	data_content = std::string(data);
	data_is_blob = false;
}

TuioData::TuioData (TuioData *tdat):TuioContent(tdat->getTuioTime(),tdat->getContainingTuioObject()) {
	mime_type = tdat->getMimeType();
	data_content = tdat->getDataContent();
	data_is_blob = tdat->isDataBlob();
	int bsize;
	const void *bdata = tdat->getDataBlob(&bsize);
	if (bdata != NULL) data_blob.assign((const char*)bdata, bsize);
}

void TuioData::update(TuioTime ttime) {
	TuioContent::update(ttime);
}


const char* TuioData::getMimeType() const {
	return mime_type.c_str();
}

const char* TuioData::getDataContent() const {
	return data_content.c_str();
}

void TuioData::setDataContent(const char *data) {
	data_is_blob = false;
	data_content = std::string(data);
	data_blob.clear();
}

bool TuioData::isDataBlob() const {
	return data_is_blob;
}

void TuioData::setDataBlob(const void *data, int size) {
	data_is_blob = true;
	if (data != NULL && size > 0) data_blob.assign((const char*)data, size);
	else data_blob.clear();
}

const void* TuioData::getDataBlob(int *size) const {
	if (!data_is_blob || data_blob.empty()) {
		if (size != NULL) *size = 0;
		return NULL;
	}
	if (size != NULL) *size = (int)data_blob.size();
	return data_blob.data();
}
