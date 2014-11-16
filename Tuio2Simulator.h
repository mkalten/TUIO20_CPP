/*
 TUIO C++ Server Demo
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

#ifndef INCLUDED_TUIO2SIMULATOR_H
#define INCLUDED_TUIO2SIMULATOR_H

#include "TuioServer.h"
#include "TuioPointer.h"
#include "osc/OscTypes.h"
#include <list>
#include <math.h>

#include "FlashSender.h"
#include "TcpSender.h"

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

class Tuio2Simulator { 
	
public:
	Tuio2Simulator(TuioServer *server);
	~Tuio2Simulator() {};
	
	void run();
	TuioServer *tuioServer;
	std::list<TuioPointer*> stickyPointerList;
	std::list<TuioPointer*> jointPointerList;
	std::list<TuioPointer*> activePointerList;
	
private:
	void drawFrame();
	void drawString(const char *str);
	void processEvents();
	void initWindow();

	SDL_Surface *window;
	bool verbose, fullupdate, periodic, fullscreen, running, help;
	
	int width, height;
	int screen_width, screen_height;
	int window_width, window_height;
	TuioTime frameTime;
	
	void mousePressed(float x, float y);
	void mouseReleased(float x, float y);
	void mouseDragged(float x, float y);
	//int s_id;
};

#endif /* INCLUDED_TUIO2SIMULATOR_H */
