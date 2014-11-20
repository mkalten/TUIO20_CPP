/*
	TUIO2 C++ Server Demo
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

#include "Tuio2Simulator.h"
void Tuio2Simulator::drawFrame() {

	if(!running) return;
	glClear(GL_COLOR_BUFFER_BIT);
	char id[3];

	// draw the cursors
	std::list<TuioPointer*> pointerList = tuioServer->getTuioPointerList();
	for (std::list<TuioPointer*>::iterator iter = pointerList.begin(); iter!=pointerList.end(); iter++) {
		TuioPointer *tptr = (*iter);
		std::list<TuioPoint> path = tptr->getPath();
		if (path.size()>0) {

			TuioPoint last_point = path.front();
			glBegin(GL_LINES);
			glColor3f(0.0, 0.0, 1.0);

			for (std::list<TuioPoint>::iterator point = path.begin(); point!=path.end(); point++) {
				glVertex3f(last_point.getScreenX(width), last_point.getScreenY(height), 0.0f);
				glVertex3f(point->getScreenX(width), point->getScreenY(height), 0.0f);
				last_point.update(point->getX(),point->getY());
			}
			glEnd();

			// draw the finger tip
			glColor3f(0.75, 0.75, 0.75);
			std::list<TuioPointer*>::iterator joint = std::find( jointPointerList.begin(), jointPointerList.end(), tptr );
			if( joint != jointPointerList.end() ) {
				glColor3f(0.5, 0.5, 0.5);
			}
			glPushMatrix();
			glTranslatef(last_point.getScreenX(width), last_point.getScreenY(height), 0.0f);
			glBegin(GL_TRIANGLE_FAN);
			for(double a = 0.0f; a <= 2*M_PI; a += 0.2f) {
				glVertex2d(cos(a) * height/100.0f, sin(a) * height/100.0f);
			}
			glEnd();
			glPopMatrix();

			glColor3f(0.0, 0.0, 0.0);
			glRasterPos2f(tptr->getScreenX(width),tptr->getScreenY(height));
			sprintf(id,"%d",tptr->getPointerID());
			drawString(id);
		}
	}

	if (help) {
		glColor3f(0.0, 0.0, 1.0);
		glRasterPos2f(10,20);
		drawString("h - toggle help display");
		glRasterPos2f(10,35);
		if (verbose) drawString("v - disable verbose mode");
		else drawString("v - enable verbose mode");
		glRasterPos2f(10,50);
		drawString("r - reset session");
		glRasterPos2f(10,65);
		if (tuioServer->fullUpdateEnabled()) drawString("f - disable full update");
		else drawString("f - enable full update");
		glRasterPos2f(10,80);
		if (tuioServer->periodicMessagesEnabled()) drawString("p - disable periodic messages");
		else drawString("p - enable periodic messages");
		glRasterPos2f(10,95);
		drawString("SHIFT click - create persistent cursor");
		glRasterPos2f(10,110);
		drawString("CTRL click - add to cursor group");
		glRasterPos2f(10,125);
		if (fullscreen) drawString("F1 - exit fullscreen mode");
		else drawString("F1 - enter fullscreen mode");
		glRasterPos2f(10,140);
		drawString("ESC - Quit");
	}

	SDL_GL_SwapBuffers();
}

void Tuio2Simulator::drawString(const char *str) {
	if (str && strlen(str)) {
		while (*str) {
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *str);
			str++;
		}
	}
}

void Tuio2Simulator::processEvents()
{
    SDL_Event event;

    while( SDL_PollEvent( &event ) ) {

        switch( event.type ) {
		case SDL_KEYDOWN:
			if( event.key.keysym.sym == SDLK_ESCAPE ){
				running = false;
				SDL_Quit();
			} else if( event.key.keysym.sym == SDLK_F1 ){
				fullscreen=!fullscreen;
				initWindow();
			} else if( event.key.keysym.sym == SDLK_f ){
				fullupdate=!tuioServer->fullUpdateEnabled();
				if (fullupdate) tuioServer->enableFullUpdate();
				else tuioServer->disableFullUpdate();
			} else if( event.key.keysym.sym == SDLK_p ){
				periodic=!tuioServer->periodicMessagesEnabled();
				if (periodic) tuioServer->enablePeriodicMessages();
				else tuioServer->disablePeriodicMessages();
			} else if( event.key.keysym.sym == SDLK_v ){
				verbose = !verbose;
				tuioServer->setVerbose(verbose);
			} else if( event.key.keysym.sym == SDLK_h ){
				help = !help;
			} else if( event.key.keysym.sym == SDLK_r ){
				tuioServer->resetTuioObjectList();
				stickyPointerList.clear();
				jointPointerList.clear();
				activePointerList.clear();
			}
			break;

		case SDL_MOUSEMOTION:
			if (event.button.button == SDL_BUTTON_LEFT) mouseDragged((float)event.button.x/width, (float)event.button.y/height);
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT) mousePressed((float)event.button.x/width, (float)event.button.y/height);
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT) mouseReleased((float)event.button.x/width, (float)event.button.y/height);
			break;
		case SDL_QUIT:
			running = false;
			SDL_ShowCursor(true);
			SDL_Quit();
			break;
        }
    }
}

void Tuio2Simulator::mousePressed(float x, float y) {
	//printf("pressed %f %f\n",x,y);

	TuioPointer *match = NULL;
	float distance  = 0.01f;
	for (std::list<TuioPointer*>::iterator iter = stickyPointerList.begin(); iter!=stickyPointerList.end(); iter++) {
		TuioPointer *tptr = (*iter);
		float test = tptr->getDistance(x,y);
		if ((test < distance) && (test < 8.0f/width)) {
			distance = test;
			match = tptr;
		}
	}

	Uint8 *keystate = SDL_GetKeyState(NULL);
	if ((keystate[SDLK_LSHIFT]) || (keystate[SDLK_RSHIFT]))  {

		if (match!=NULL) {
			std::list<TuioPointer*>::iterator joint = std::find( jointPointerList.begin(), jointPointerList.end(), match );
			if( joint != jointPointerList.end() ) {
				jointPointerList.erase( joint );
			}
			stickyPointerList.remove(match);
			stickyPointerList.remove(match);
			tuioServer->removeTuioPointer(match);
		} else {
			TuioObject *tobj = tuioServer->createTuioPointer(x,y,0,0,0,0);
			stickyPointerList.push_back(tobj->getTuioPointer());
			activePointerList.push_back(tobj->getTuioPointer());
		}
	} else if ((keystate[SDLK_LCTRL]) || (keystate[SDLK_RCTRL])) {

		if (match!=NULL) {
			std::list<TuioPointer*>::iterator joint = std::find( jointPointerList.begin(), jointPointerList.end(), match );
			if( joint != jointPointerList.end() ) {
				jointPointerList.remove( match );
			} else jointPointerList.push_back(match);
		}
	} else {
		if (match==NULL) {
			TuioObject *tobj = tuioServer->createTuioPointer(x,y,0,0,0,0);
			activePointerList.push_back(tobj->getTuioPointer());
        } else {
            activePointerList.push_back(match);
        }
	}
}

void Tuio2Simulator::mouseDragged(float x, float y) {
	//printf("dragged %f %f\n",x,y);

	TuioPointer *pointer = NULL;
	float distance  = width;
	for (std::list<TuioPointer*>::iterator iter = activePointerList.begin(); iter!=activePointerList.end(); iter++) {
		TuioPointer *tptr = (*iter);
		float test = tptr->getDistance(x,y);
		if (test<distance) {
			distance = test;
			pointer = tptr;
		}
	}

	if (pointer==NULL) return;
	if (pointer->getTuioTime()==frameTime) return;

	std::list<TuioPointer*>::iterator joint = std::find( jointPointerList.begin(), jointPointerList.end(), pointer );
	if( joint != jointPointerList.end() ) {
		float dx = x-pointer->getX();
		float dy = y-pointer->getY();
		for (std::list<TuioPointer*>::iterator iter = jointPointerList.begin(); iter!=jointPointerList.end(); iter++) {
			TuioPointer *jointPointer = (*iter);
			 tuioServer->updateTuioPointer(jointPointer,jointPointer->getX()+dx,jointPointer->getY()+dy,0,0,0,0);
		}
	} else tuioServer->updateTuioPointer(pointer,x,y,0,0,0,0);
}

void Tuio2Simulator::mouseReleased(float x, float y) {
	//printf("released %f %f\n",x,y);

	TuioPointer *pointer = NULL;
	float distance  = 0.01f;
	for (std::list<TuioPointer*>::iterator iter = stickyPointerList.begin(); iter!=stickyPointerList.end(); iter++) {
		TuioPointer *tptr = (*iter);
		float test = tptr->getDistance(x,y);
		if (test<distance) {
			distance = test;
			pointer = tptr;
		}
	}

	if (pointer!=NULL) {
		activePointerList.remove(pointer);
		return;
	}

	distance = 0.01f;
	for (std::list<TuioPointer*>::iterator iter = activePointerList.begin(); iter!=activePointerList.end(); iter++) {
		TuioPointer *tptr = (*iter);
		float test = tptr->getDistance(x,y);
		if (test<distance) {
			distance = test;
			pointer = tptr;
		}
	}

	if (pointer!=NULL) {
		activePointerList.remove(pointer);
		tuioServer->removeTuioPointer(pointer);
	}
}

Tuio2Simulator::Tuio2Simulator(TuioServer *server)
	: verbose		(false)
	, fullupdate	(false)
	, periodic		(false)
	, fullscreen	(false)
	, help			(false)
	, screen_width	(1024)
	, screen_height	(768)
	, window_width	(640)
	, window_height	(480)
{

	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		std::cerr << "SDL could not be initialized: " <<  SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
    
    screen_width = SDL_GetVideoInfo()->current_w;
    screen_height = SDL_GetVideoInfo()->current_h;

    //TuioTime::initSession();
    frameTime = TuioTime::getSystemTime();

    tuioServer = server;
    tuioServer->setSourceName("t2sim");
    
    initWindow();
}

void Tuio2Simulator::initWindow() {

	int videoFlags = SDL_OPENGL | SDL_DOUBLEBUF;
	if( fullscreen ) {
		videoFlags |= SDL_FULLSCREEN;
		SDL_ShowCursor(false);
		width = screen_width;
		height = screen_height;
	} else {
		width = window_width;
		height = window_height;
	}

    tuioServer->setDimension(width,height);
	window = SDL_SetVideoMode(width, height, 32, videoFlags);
	if ( window == NULL ) {
		std::cerr << "Could not open window: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}

	SDL_ShowCursor(true);
	SDL_WM_SetCaption("Tuio2Simulator", NULL);

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, (float)width,  (float)height, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Tuio2Simulator::run() {
	running=true;
	while (running) {
		frameTime = TuioTime::getSystemTime();
		tuioServer->initTuioFrame(frameTime);
		processEvents();
		tuioServer->stopUntouchedMovingObjects();
		tuioServer->commitTuioFrame();
		drawFrame();

		// simulate 50Hz compensating the previous processing time
		int delay = 20 - (TuioTime::getSystemTime().getTotalMilliseconds() - frameTime.getTotalMilliseconds());
		if (delay>0) SDL_Delay(delay);
	}
}

int main(int argc, char* argv[])
{
/*	if (( argc != 1) && ( argc != 3)) {
        	std::cout << "usage: Tuio2Simulator [host] [port]\n";
        	return 0;
	}*/

#ifndef __MACOSX__
	glutInit(&argc,argv);
#else
    if ((argc>1) && ((std::string(argv[1]).find("-NSDocumentRevisionsDebugMode")==0 ) || (std::string(argv[1]).find("-psn_")==0))) argc = 1;
#endif

	TuioServer *server;
	if( argc == 3 ) {
        printf("3a\n");
		server = new TuioServer(argv[1],atoi(argv[2]));
	} else server = new TuioServer(); // default is UDP port 3333 on localhost

	// add an additional TUIO/TCP sender
	OscSender *tcp_sender;
	if( argc == 2 ) {
        printf("2\n");
		tcp_sender = new TcpSender(atoi(argv[1]));
	} else if ( argc == 3 ) {
        printf("3b\n");
		tcp_sender = new TcpSender(argv[1],atoi(argv[2]));
	} else tcp_sender = new TcpSender(3333);
	server->addOscSender(tcp_sender);

	// add an additional TUIO/FLC sender
	OscSender *flash_sender = new FlashSender();
	server->addOscSender(flash_sender);

	Tuio2Simulator *app = new Tuio2Simulator(server);
	app->run();

	delete app;
	delete server;
#ifndef LINUX
	delete flash_sender;
#endif
	delete tcp_sender;
	return 0;
}


