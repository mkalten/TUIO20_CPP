/*
 TUIO2 C++ Test
 Copyright (c) 2005-2014 Martin Kaltenbrunner <martin@tuio.org>
 
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

#ifndef INCLUDED_TUIO2TEST_H
#define INCLUDED_TUIO2TEST_H

#include "TuioServer.h"
#include "TuioPointer.h"
#include "osc/OscTypes.h"
#include <list>
#include <math.h>

#include "FlashSender.h"
#include "TcpSender.h"
#include "WebSockSender.h"

using namespace TUIO2;

class Tuio2Test { 
	
public:
	Tuio2Test(TuioServer *server);
	Tuio2Test() {};
	
	void run();
	TuioServer *tuioServer;

	
private:

	int width, height;
	TuioTime frameTime;

};

#endif /* INCLUDED_TUIO2TEST_H */
