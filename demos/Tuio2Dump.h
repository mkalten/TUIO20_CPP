/*
 TUIO2 C++ Example - part of the reacTIVision project
 Copyright (c) 2009-2017 Martin Kaltenbrunner <martin@tuio.org>
 
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef INCLUDED_TUIO2DUMP_H
#define INCLUDED_TUIO2DUMP_H

#include "TuioListener.h"
#include "TuioClient.h"
#include "UdpReceiver.h"
#include "TcpReceiver.h"
#include <math.h>

using namespace TUIO2;

class Tuio2Dump : public TuioListener {

	public:
	void tuioAdd(TuioObject *tobj);
	void tuioUpdate(TuioObject *tobj);
	void tuioRemove(TuioObject *tobj);
	void tuioRefresh(TuioTime frameTime);
};

#endif /* INCLUDED_TUIO2DUMP_H */
