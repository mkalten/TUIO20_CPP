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

#include "TuioToken.h"

using namespace TUIO2;

TuioToken::TuioToken (TuioTime ttime, unsigned int si, short ti, short ui, int sym, float xp, float yp, float a):TuioComponent(ttime, si, xp, yp,a) {
    type_id = ti;
    user_id = ui;
    symbol_id = sym;
}

TuioToken::TuioToken (unsigned int si, short ti, short ui, int sym, float xp, float yp, float a):TuioComponent(si, xp, yp, a) {
    type_id = ti;
    user_id = ui;
    symbol_id = sym;
}

TuioToken::TuioToken (unsigned int si, int sym, float xp, float yp, float a):TuioComponent(si, xp, yp, a) {
    type_id = 0;
    user_id = 0;
    symbol_id = sym;
}

TuioToken::TuioToken (TuioToken *ttok):TuioComponent(ttok) {
	symbol_id = ttok->getSymbolID();
}

void TuioToken::stop (TuioTime ttime) {
	update(ttime,xpos,ypos,angle);
}

int TuioToken::getSymbolID() const{ 
	return symbol_id;
}

short TuioToken::getTypeID() const{
    return type_id;
};

short TuioToken::getUserID() const{
    return user_id;
};

int TuioToken::getTypeUserID() const {
    int tu_id = user_id << 16 | type_id;
    return tu_id;
}

void TuioToken::setTypeUserID(int tu_id) {
    user_id = tu_id >> 16;
    type_id = tu_id & 0x0000FFFF;
}
