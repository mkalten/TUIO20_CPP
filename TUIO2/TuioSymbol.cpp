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

#include "TuioSymbol.h"
#include "TuioObject.h"

using namespace TUIO2;

TuioSymbol::TuioSymbol (TuioTime ttime, TuioObject *tobj, unsigned short ti, unsigned short ui, unsigned int sym, const char *type, const char *data):TuioContent(ttime,tobj) {
    currentTime = ttime;
    startTime = currentTime;
    type_id = ti;
    user_id = ui;
    symbol_id = sym;
    symbol_type = std::string(type);
    symbol_data = std::string(data);
    symbol_is_blob = false;
    state = TUIO_ADDED;
}
	
TuioSymbol::TuioSymbol (TuioObject *tobj, unsigned short ti, unsigned short ui, unsigned int sym, const char *type, const char *data):TuioContent(tobj) {
    currentTime = TuioTime::getSystemTime();
    startTime = currentTime;
    type_id = ti;
    user_id = ui;
    symbol_id = sym;
    symbol_type = std::string(type);
    symbol_data = std::string(data);
    symbol_is_blob = false;
    state = TUIO_ADDED;
}

TuioSymbol::TuioSymbol (TuioSymbol *tsym):TuioContent(tsym->getTuioTime(),tsym->getContainingTuioObject()) {
    currentTime = TuioTime::getSystemTime();
    startTime = currentTime;
    session_id = tsym->getSessionID();
    type_id = tsym->getTypeID();
    user_id = tsym->getUserID();
    symbol_id = tsym->getSymbolID();
    symbol_type = tsym->getSymbolType();
    symbol_data = tsym->getSymbolData();
    symbol_is_blob = tsym->isSymbolDataBlob();
    int bsize;
    const void *bdata = tsym->getSymbolDataBlob(&bsize);
    if (bdata != NULL) symbol_blob.assign((const char*)bdata, bsize);
    state = TUIO_ADDED;
}

unsigned int TuioSymbol::getSymbolID() const {
    return symbol_id;
}
        
unsigned short TuioSymbol::getTypeID() const {
    return type_id;
}
        
unsigned short TuioSymbol::getUserID() const {
    return user_id;
}

unsigned int TuioSymbol::getTypeUserID() const {
    int tu_id = user_id << 16 | type_id;
    return tu_id;
}

void TuioSymbol::setTypeUserID(unsigned int tu_id) {
    user_id = tu_id >> 16;
    type_id = tu_id & 0x0000FFFF;
}

const char* TuioSymbol::getSymbolType() const {
    return symbol_type.c_str();
}
        
const char* TuioSymbol::getSymbolData() const {
    return symbol_data.c_str();
}

void TuioSymbol::update(TuioTime ttime) {
    TuioContent::update(ttime);

}



bool TuioSymbol::isSymbolDataBlob() const {
    return symbol_is_blob;
}

void TuioSymbol::setSymbolDataBlob(const void *data, int size) {
    symbol_is_blob = true;
    if (data != NULL && size > 0) symbol_blob.assign((const char*)data, size);
    else symbol_blob.clear();
}

const void* TuioSymbol::getSymbolDataBlob(int *size) const {
    if (!symbol_is_blob || symbol_blob.empty()) {
        if (size != NULL) *size = 0;
        return NULL;
    }
    if (size != NULL) *size = (int)symbol_blob.size();
    return symbol_blob.data();
}

void TuioSymbol::setSymbolData(const char *data) {
    symbol_is_blob = false;
    symbol_data = std::string(data);
    symbol_blob.clear();
}
