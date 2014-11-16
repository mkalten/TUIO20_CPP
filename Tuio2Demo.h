/*
 TUIO2 C++ GUI Demo
 Copyright (c) 2009-2014 Martin Kaltenbrunner <martin@tuio.org>
 
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

#ifndef INCLUDED_TUIO2DEMO_H
#define INCLUDED_TUIO2DEMO_H

#include "TuioListener.h"
#include "TuioClient.h"
#include "UdpReceiver.h"
#include "TcpReceiver.h"
#include <list>
#include <math.h>

#include <SDL.h>
#include <SDL_thread.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

using namespace TUIO2;

class Tuio2Demo : public TuioListener { 
	
public:
	Tuio2Demo(int port);
	~Tuio2Demo() {
		tuioClient->disconnect();
		delete tuioClient;
		delete osc_receiver;
	}
	
	void tuioAdd(TuioObject *tobj);
	void tuioUpdate(TuioObject *tobj);
	void tuioRemove(TuioObject *tobj);
	void tuioRefresh(TuioTime frameTime);
	
	void run();
	
private:
	void drawObjects();
	void drawString(char *str);
	void processEvents();
	void initWindow();
	SDL_Surface *window;
	bool verbose, fullscreen, running;
	
	int width, height;
	int screen_width, screen_height;
	int window_width, window_height;

	TuioClient *tuioClient;
	OscReceiver *osc_receiver;
};

#endif /* INCLUDED_Tuio2Demo_H */
