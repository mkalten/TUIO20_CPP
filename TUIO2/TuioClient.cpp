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

#include "TuioClient.h"
#include "UdpReceiver.h"
#include <iterator>

using namespace TUIO2;
using namespace osc;


TuioClient::TuioClient()
: lateFrame     (false)
, source_count  (0)
, receiver		(NULL)
, local_receiver(true)
{
	receiver = new UdpReceiver();
	initialize();
}

TuioClient::TuioClient(unsigned short port)
: lateFrame     (false)
, source_count  (0)
, receiver		(NULL)
, local_receiver(true)
{
	receiver = new UdpReceiver(port);
	initialize();
}

TuioClient::TuioClient(OscReceiver *osc)
: lateFrame     (false)
, source_count  (0)
, receiver		(osc)
, local_receiver(false)
{
	initialize();
}

void TuioClient::initialize()	{
	receiver->addTuioClient(this);
}

TuioClient::~TuioClient() {
	if (local_receiver) delete receiver;
}

void TuioClient::processOSC( const ReceivedMessage& msg ) {
    try {
        ReceivedMessageArgumentStream args = msg.ArgumentStream();
        //ReceivedMessage::const_iterator arg = msg.ArgumentsBegin();

        if( strcmp( msg.AddressPattern(), "/tuio2/frm" ) == 0 ) {
            //lockFrame();
            int32 fseq_raw,dim_raw;
            TimeTag timetag;
            const char* src_string;
            args >> fseq_raw >> timetag >> dim_raw >> src_string;

            // check if we know that source
            std::map<std::string,TuioSource*>::iterator iter = sourceList.find(src_string);

            if (iter==sourceList.end()) { // add a new source
                frameSource = new TuioSource(source_count, src_string, (unsigned int)dim_raw);
                sourceList[src_string] = frameSource;
                source_count++;
            } else { // use the found source
                 frameSource = sourceList[src_string];
            }

            unsigned int currentFrameID = (unsigned int)fseq_raw;
            frameTime = TuioTime(timetag);
            frameTime.setFrameID(currentFrameID);

            // discard pending objects left over from a previous (incomplete) frame of
            // this source — a shared frame time marks a split continuation packet
            if (frameSource->getFrameTime() != frameTime) {
                for (std::list<TuioObject*>::iterator iter=frameObjectList.begin(); iter != frameObjectList.end();) {
                    if (((*iter)->getTuioSource()==frameSource) && (std::find(tobjList.begin(),tobjList.end(),*iter)==tobjList.end())) {
                        delete (*iter);
                        iter = frameObjectList.erase(iter);
                    } else iter++;
                }
            }

            // frame sequence
            lateFrame = false;
            unsigned int lastFrameID = frameSource->getFrameTime().getFrameID();
            unsigned int timeDiff = frameTime.getTotalMilliseconds() - frameSource->getFrameTime().getTotalMilliseconds();
            frameSource->setFrameTime(frameTime);

            // drop late frames (but accept the reserved ID 0 and consider a possible reset after 1sec
            if ((currentFrameID<lastFrameID) && (currentFrameID!=0) && (timeDiff<1000)) lateFrame = true;

        } else if( strcmp( msg.AddressPattern(), "/tuio2/tok" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw, tu_id_raw, c_id_raw;
            unsigned short t_id, u_id;
            unsigned int s_id,c_id;
            float xpos, ypos, angle, xspeed, yspeed, rspeed, maccel, raccel;
            args >> s_id_raw >> tu_id_raw >> c_id_raw >> xpos >> ypos >> angle;
            if (!args.Eos()) args >> xspeed >> yspeed >> rspeed >> maccel >> raccel;
            else xspeed = yspeed = rspeed = maccel = raccel = 0.0f;

            s_id = (unsigned int)s_id_raw;
            c_id = (unsigned int)c_id_raw;
            u_id = tu_id_raw >> 16;
            t_id = tu_id_raw & 0x0000FFFF;

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            //if (tobj == NULL) std::cout << "new cont " << s_id << " " << frameSource.getSourceID() << std::endl;
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);
            TuioToken *ttok = tobj->getTuioToken();
            if (ttok == NULL) {
                ttok = new TuioToken(frameTime,tobj,t_id,u_id,c_id,xpos,ypos,angle);
                tobj->setTuioToken(ttok);
                if ((xspeed!=0.0f)||(yspeed!=0.0f)||(rspeed!=0.0f)||(maccel!=0.0f)||(raccel!=0.0f))
                    ttok->update(frameTime,xpos,ypos,angle,xspeed,yspeed,rspeed,maccel,raccel);
            } else if ( (ttok->getX()!=xpos) || (ttok->getY()!=ypos) || (ttok->getAngle()!=angle) || (ttok->getXSpeed()!=xspeed) || (ttok->getYSpeed()!=yspeed) || (ttok->getRotationSpeed()!=rspeed) || (ttok->getMotionAccel()!=maccel) || (ttok->getRotationAccel()!=raccel) ) {

                ttok->update(frameTime,xpos,ypos,angle,xspeed,yspeed,rspeed,maccel,raccel);
            }

        } else if( strcmp( msg.AddressPattern(), "/tuio2/ptr" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw, tu_id_raw, c_id_raw;
            unsigned short t_id, u_id;
            unsigned int s_id,c_id;
            float xpos, ypos, angle, shear,radius, pressure, xspeed, yspeed, pspeed, maccel, paccel;
            args >> s_id_raw >> tu_id_raw >> c_id_raw >> xpos >> ypos >> angle >> shear >> radius >> pressure;
            if (!args.Eos()) args >> xspeed >> yspeed >> pspeed >> maccel >> paccel;
            else xspeed = yspeed = pspeed = maccel = paccel = 0.0f;

            s_id = (unsigned int)s_id_raw;
            c_id = (unsigned int)c_id_raw;
            u_id = tu_id_raw >> 16;
            t_id = tu_id_raw & 0x0000FFFF;

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            //if (tobj == NULL) std::cout << "new cont " << s_id << " " << frameSource.getSourceID() << std::endl;
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);
            TuioPointer *tptr = tobj->getTuioPointer();
            if (tptr == NULL) {
                tptr = new TuioPointer(frameTime,tobj,t_id,u_id,c_id,xpos,ypos,angle,shear,radius,pressure);
                tobj->setTuioPointer(tptr);

            } else if ( (tptr->getX()!=xpos) || (tptr->getY()!=ypos) || (tptr->getAngle()!=angle) || (tptr->getShear()!=shear) || (tptr->getRadius()!=radius) || (tptr->getPressure()!=pressure) || (tptr->getXSpeed()!=xspeed) || (tptr->getYSpeed()!=yspeed) || (tptr->getPressureSpeed()!=pspeed) || (tptr->getMotionAccel()!=maccel) || (tptr->getPressureAccel()!=paccel) ) {

                tptr->update(frameTime,xpos,ypos,angle,shear,radius,pressure,xspeed,yspeed,pspeed,maccel,paccel);
            }
        } else if( strcmp( msg.AddressPattern(), "/tuio2/bnd" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw;
            unsigned int s_id;
            float xpos, ypos, angle, width, height, area;
            float xspeed, yspeed, rspeed, maccel, raccel;
            args >> s_id_raw >> xpos >> ypos >> angle >> width >> height >> area;
            if (!args.Eos()) args >> xspeed >> yspeed >> rspeed >> maccel >> raccel;
            else xspeed = yspeed = rspeed = maccel = raccel = 0.0f;

            s_id = (unsigned int)s_id_raw;
            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);
            TuioBounds *tbnd = tobj->getTuioBounds();
            if (tbnd == NULL) {
                tbnd = new TuioBounds(frameTime,tobj,xpos,ypos,angle,width,height,area);
                tobj->setTuioBounds(tbnd);
            } else if ( (tbnd->getX()!=xpos) || (tbnd->getY()!=ypos) || (tbnd->getAngle()!=angle) || (tbnd->getWidth()!=width) || (tbnd->getHeight()!=height) || (tbnd->getArea()!=area) || (tbnd->getXSpeed()!=xspeed) || (tbnd->getYSpeed()!=yspeed) || (tbnd->getRotationSpeed()!=rspeed) || (tbnd->getMotionAccel()!=maccel) || (tbnd->getRotationAccel()!=raccel)) {

                tbnd->update(frameTime,xpos,ypos,angle,width,height,area,xspeed,yspeed,rspeed,maccel,raccel);
            }
        } else if( strcmp( msg.AddressPattern(), "/tuio2/sym" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw, tu_id_raw, c_id_raw;
            unsigned int s_id, c_id;
            unsigned short t_id, u_id;
            const char* type;
            args >> s_id_raw >> tu_id_raw >> c_id_raw >> type;

            s_id = (unsigned int)s_id_raw;
            c_id = (unsigned int)c_id_raw;
            u_id = tu_id_raw >> 16;
            t_id = tu_id_raw & 0x0000FFFF;

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);
            TuioSymbol *tsym = tobj->getTuioSymbol();
            if (tsym == NULL) {
                tsym = new TuioSymbol(frameTime,tobj,t_id,u_id,c_id,type,"");
                tobj->setTuioSymbol(tsym);
            } else {
                tsym->update(frameTime);
            }

            // the data attribute can be an OSC string or an OSC blob
            ReceivedMessage::const_iterator dataArg = msg.ArgumentsBegin();
            for (int i=0; i<4 && dataArg != msg.ArgumentsEnd(); i++) dataArg++;
            if (dataArg != msg.ArgumentsEnd()) {
                if (dataArg->IsBlob()) {
                    const void *bdata;
                    osc_bundle_element_size_t bsize;
                    dataArg->AsBlobUnchecked(bdata,bsize);
                    tsym->setSymbolDataBlob(bdata,(int)bsize);
                } else if (!tsym->isSymbolDataBlob()) {
                    tsym->setSymbolData(dataArg->AsString());
                }
            }

        } else if( strcmp( msg.AddressPattern(), "/tuio2/chg" ) == 0 ) {
            if (lateFrame) return;
            int32 s_id_raw;
            args >> s_id_raw;
            unsigned int  s_id = (unsigned int)s_id_raw;

            std::list<TuioPoint> pointList;
            while(!args.Eos()) {
                float xpos, ypos;
                args >> xpos >> ypos;
                pointList.push_back(TuioPoint(frameTime, xpos, ypos));
            }

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);

            TuioGeometry *tgeo = tobj->getTuioGeometry();
            if (tgeo == NULL) {
                tgeo = new TuioGeometry(frameTime, tobj);
                tobj->setTuioGeometry(tgeo);
            } tgeo->setConvexHull(pointList);

        } else if( strcmp( msg.AddressPattern(), "/tuio2/ocg" ) == 0 ) {
            if (lateFrame) return;
            int32 s_id_raw;
            args >> s_id_raw;
            unsigned int s_id = (unsigned int)s_id_raw;

            std::list<TuioPoint> pointList;
            while(!args.Eos()) {
                float xpos, ypos;
                args >> xpos >> ypos;
                pointList.push_back(TuioPoint(frameTime, xpos, ypos));
            }

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);

            TuioGeometry *tgeo = tobj->getTuioGeometry();
            if (tgeo == NULL) {
                tgeo = new TuioGeometry(frameTime, tobj);
                tobj->setTuioGeometry(tgeo);
            } tgeo->setOuterContour(pointList);

        } else if( strcmp( msg.AddressPattern(), "/tuio2/icg" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw;
            args >> s_id_raw;
            unsigned int s_id = (unsigned int)s_id_raw;

            // inner contours are float point pairs separated by
            // boolean TRUE markers starting a new contour
            std::list< std::list<TuioPoint> > contourList;
            std::list<TuioPoint> *pointList = NULL;
            ReceivedMessage::const_iterator arg = msg.ArgumentsBegin();
            while (arg != msg.ArgumentsEnd()) {
                if (arg->IsBool()) {
                    contourList.push_back(std::list<TuioPoint>());
                    pointList = &contourList.back();
                    arg++;
                } else if (arg->TypeTag()=='f') {
                    float xpos = arg->AsFloat(); arg++;
                    float ypos = arg->AsFloat(); arg++;
                    if (pointList == NULL) {
                        contourList.push_back(std::list<TuioPoint>());
                        pointList = &contourList.back();
                    }
                    pointList->push_back(TuioPoint(frameTime, xpos, ypos));
                } else arg++;
            }

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);

            TuioGeometry *tgeo = tobj->getTuioGeometry();
            if (tgeo == NULL) {
                tgeo = new TuioGeometry(frameTime, tobj);
                tobj->setTuioGeometry(tgeo);
            } tgeo->setInnerContours(contourList);

        } else if( strcmp( msg.AddressPattern(), "/tuio2/skg" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw;
            args >> s_id_raw;
            unsigned int s_id = (unsigned int)s_id_raw;

            // skeleton is a tree: float point pairs per branch,
            // int32 node indices mark the jump back to a previous node
            std::list< std::list<TuioPoint> > skeleton;
            std::list<unsigned int> jumps;
            std::list<TuioPoint> *branch = NULL;
            ReceivedMessage::const_iterator arg = msg.ArgumentsBegin();
            arg++;
            while (arg != msg.ArgumentsEnd()) {
                if (arg->IsInt32()) {
                    jumps.push_back((unsigned int)arg->AsInt32Unchecked());
                    branch = NULL;
                    arg++;
                } else if (arg->TypeTag()=='f') {
                    float xpos = arg->AsFloat(); arg++;
                    float ypos = arg->AsFloat(); arg++;
                    if (branch == NULL) {
                        skeleton.push_back(std::list<TuioPoint>());
                        branch = &skeleton.back();
                    }
                    branch->push_back(TuioPoint(frameTime, xpos, ypos));
                } else arg++;
            }

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);

            TuioGeometry *tgeo = tobj->getTuioGeometry();
            if (tgeo == NULL) {
                tgeo = new TuioGeometry(frameTime, tobj);
                tobj->setTuioGeometry(tgeo);
            } tgeo->setSkeleton(skeleton);
            tgeo->setSkeletonJumps(jumps);

        } else if( strcmp( msg.AddressPattern(), "/tuio2/s3d" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw;
            args >> s_id_raw;
            unsigned int s_id = (unsigned int)s_id_raw;

            // skeleton is a tree: float xyz triples per branch,
            // int32 node indices mark the jump back to a previous node
            std::list< std::list<float> > skeleton;
            std::list<unsigned int> jumps;
            std::list<float> *branch = NULL;
            ReceivedMessage::const_iterator arg = msg.ArgumentsBegin();
            arg++;
            while (arg != msg.ArgumentsEnd()) {
                if (arg->IsInt32()) {
                    jumps.push_back((unsigned int)arg->AsInt32Unchecked());
                    branch = NULL;
                    arg++;
                } else if (arg->TypeTag()=='f') {
                    float xpos = arg->AsFloat(); arg++;
                    float ypos = arg->AsFloat(); arg++;
                    float zpos = arg->AsFloat(); arg++;
                    if (branch == NULL) {
                        skeleton.push_back(std::list<float>());
                        branch = &skeleton.back();
                    }
                    branch->push_back(xpos);
                    branch->push_back(ypos);
                    branch->push_back(zpos);
                } else arg++;
            }

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);

            TuioGeometry *tgeo = tobj->getTuioGeometry();
            if (tgeo == NULL) {
                tgeo = new TuioGeometry(frameTime, tobj);
                tobj->setTuioGeometry(tgeo);
            } tgeo->setSkeleton3D(skeleton);
            tgeo->setSkeletonJumps3D(jumps);

        } else if( strcmp( msg.AddressPattern(), "/tuio2/svg" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw;
            args >> s_id_raw;
            unsigned int s_id = (unsigned int)s_id_raw;

            std::list<float> vols;
            while(!args.Eos()) {
                float radius;
                args >> radius;
                vols.push_back(radius);
            }

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);

            TuioGeometry *tgeo = tobj->getTuioGeometry();
            if (tgeo == NULL) {
                tgeo = new TuioGeometry(frameTime, tobj);
                tobj->setTuioGeometry(tgeo);
            } tgeo->setSkeletonVolumes(vols);

        } else if( strcmp( msg.AddressPattern(), "/tuio2/arg" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw;
            args >> s_id_raw;
            unsigned int s_id = (unsigned int)s_id_raw;

            std::list<TuioPoint> points;
            std::list<float> widths;
            while(!args.Eos()) {
                float xpos, ypos, width;
                args >> xpos >> ypos >> width;
                points.push_back(TuioPoint(frameTime, xpos, ypos));
                widths.push_back(width);
            }

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);

            TuioGeometry *tgeo = tobj->getTuioGeometry();
            if (tgeo == NULL) {
                tgeo = new TuioGeometry(frameTime, tobj);
                tobj->setTuioGeometry(tgeo);
            } tgeo->setArea(points);
            tgeo->setAreaWidths(widths);

        } else if( strcmp( msg.AddressPattern(), "/tuio2/raw" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw, width_raw;
            args >> s_id_raw >> width_raw;
            unsigned int s_id = (unsigned int)s_id_raw;
            unsigned int width = (unsigned int)width_raw;

            std::string rawData;
            ReceivedMessage::const_iterator arg = msg.ArgumentsBegin();
            arg++; arg++;
            if (arg != msg.ArgumentsEnd() && arg->IsBlob()) {
                const void *bdata;
                osc_bundle_element_size_t bsize;
                arg->AsBlobUnchecked(bdata,bsize);
                rawData.assign((const char*)bdata,bsize);
            }

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);

            TuioGeometry *tgeo = tobj->getTuioGeometry();
            if (tgeo == NULL) {
                tgeo = new TuioGeometry(frameTime, tobj);
                tobj->setTuioGeometry(tgeo);
            } tgeo->setRawData(width,rawData);

        } else if( strcmp( msg.AddressPattern(), "/tuio2/ctl" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw;
            args >> s_id_raw;
            unsigned int s_id = (unsigned int)s_id_raw;

            // control dimensions are an open list of bools and floats
            std::list< std::pair<bool,float> > controls;
            ReceivedMessage::const_iterator arg = msg.ArgumentsBegin();
            arg++;
            while (arg != msg.ArgumentsEnd()) {
                if (arg->IsBool()) controls.push_back(std::make_pair(true, arg->AsBoolUnchecked()?1.0f:0.0f));
                else if (arg->TypeTag()=='f') controls.push_back(std::make_pair(false, arg->AsFloat()));
                arg++;
            }

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);

            TuioControl *tctl = tobj->getTuioControl();
            if (tctl == NULL) {
                tctl = new TuioControl(frameTime, tobj);
                tobj->setTuioControl(tctl);
            } tctl->setControls(controls);

        } else if( strcmp( msg.AddressPattern(), "/tuio2/dat" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw;
            unsigned int s_id;
            const char* mime;
            args >> s_id_raw >> mime;
            s_id = (unsigned int)s_id_raw;

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);

            TuioData *tdat = tobj->getTuioData();
            if (tdat == NULL) {
                tdat = new TuioData(frameTime,tobj,mime,"");
                tobj->setTuioData(tdat);
            } else {
                tdat->update(frameTime);
            }

            // the data attribute can be an OSC string or an OSC blob
            ReceivedMessage::const_iterator darg = msg.ArgumentsBegin();
            darg++; darg++;
            if (darg != msg.ArgumentsEnd()) {
                if (darg->IsBlob()) {
                    const void *bdata;
                    osc_bundle_element_size_t bsize;
                    darg->AsBlobUnchecked(bdata,bsize);
                    tdat->setDataBlob(bdata,(int)bsize);
                } else tdat->setDataContent(darg->AsString());
            }

        } else if( strcmp( msg.AddressPattern(), "/tuio2/sig" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw, c_id_raw;
            args >> s_id_raw >> c_id_raw;
            unsigned int s_id = (unsigned int)s_id_raw;
            unsigned int c_id = (unsigned int)c_id_raw;

            std::list<unsigned int> targets;
            while(!args.Eos()) {
                int32 t_raw;
                args >> t_raw;
                targets.push_back((unsigned int)t_raw);
            }

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);

            TuioSignal *tsig = tobj->getTuioSignal();
            if (tsig == NULL) {
                tsig = new TuioSignal(frameTime,tobj,c_id,targets);
                tobj->setTuioSignal(tsig);
            } else {
                tsig->update(frameTime);
                tsig->setComponentID(c_id);
                tsig->setSignalTargets(targets);
            }

        } else if( strcmp( msg.AddressPattern(), "/tuio2/coa" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw, slot_raw;
            args >> s_id_raw >> slot_raw;
            unsigned int s_id = (unsigned int)s_id_raw;
            unsigned int slot = (unsigned int)slot_raw;

            std::list<unsigned int> contained;
            while(!args.Eos()) {
                int32 t_raw;
                args >> t_raw;
                contained.push_back((unsigned int)t_raw);
            }

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);
            tobj->addTuioAssociation(new TuioContainer(frameTime,tobj,slot,contained));

        } else if( strcmp( msg.AddressPattern(), "/tuio2/lia" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw;
            bool physical;
            args >> s_id_raw >> physical;
            unsigned int s_id = (unsigned int)s_id_raw;

            std::list< std::pair<unsigned int,unsigned int> > links;
            while(!args.Eos()) {
                int32 t_raw, l_raw;
                args >> t_raw >> l_raw;
                links.push_back(std::make_pair((unsigned int)t_raw,(unsigned int)l_raw));
            }

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);
            tobj->addTuioAssociation(new TuioLink(frameTime,tobj,physical,links));

        } else if( strcmp( msg.AddressPattern(), "/tuio2/lla" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw;
            bool physical;
            args >> s_id_raw >> physical;
            unsigned int s_id = (unsigned int)s_id_raw;

            std::list< std::pair<unsigned int,unsigned int> > links;
            while(!args.Eos()) {
                int32 t_raw, l_raw;
                args >> t_raw >> l_raw;
                links.push_back(std::make_pair((unsigned int)t_raw,(unsigned int)l_raw));
            }

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);
            tobj->addTuioAssociation(new TuioList(frameTime,tobj,physical,links));

        } else if( strcmp( msg.AddressPattern(), "/tuio2/lta" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw;
            bool physical;
            args >> s_id_raw >> physical;
            unsigned int s_id = (unsigned int)s_id_raw;

            // tree links are int32 (s_id,l_id) pairs with float node-jump markers
            std::list< std::pair<unsigned int,unsigned int> > links;
            std::list<float> nodeJumps;
            ReceivedMessage::const_iterator arg = msg.ArgumentsBegin();
            arg++; arg++;
            while (arg != msg.ArgumentsEnd()) {
                if (arg->IsInt32()) {
                    int32 t_raw = arg->AsInt32Unchecked(); arg++;
                    int32 l_raw = arg->AsInt32Unchecked(); arg++;
                    links.push_back(std::make_pair((unsigned int)t_raw,(unsigned int)l_raw));
                } else if (arg->TypeTag()=='f') {
                    nodeJumps.push_back(arg->AsFloat());
                    arg++;
                } else arg++;
            }

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);
            tobj->addTuioAssociation(new TuioTree(frameTime,tobj,physical,links,nodeJumps));

        } else if( strcmp( msg.AddressPattern(), "/tuio2/ala" ) == 0 ) {

            if (lateFrame) return;
            associatedObjectList.clear();
            while(!args.Eos()) {
                int32 s_id_raw;
                args >> s_id_raw;
                associatedObjectList.push_back((unsigned int)s_id_raw);
            }

        } else if( strcmp( msg.AddressPattern(), "/tuio2/t3d" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw, tu_id_raw, c_id_raw;
            unsigned int s_id, c_id;
            unsigned short t_id, u_id;
            float xpos, ypos, zpos, angle, xaxis, yaxis, zaxis, xspeed, yspeed, zspeed, rspeed, maccel, raccel;
            args >> s_id_raw >> tu_id_raw >> c_id_raw >> xpos >> ypos >> zpos >> angle >> xaxis >> yaxis >> zaxis;
            if (!args.Eos()) args >> xspeed >> yspeed >> zspeed >> rspeed >> maccel >> raccel;
            else xspeed = yspeed = zspeed = rspeed = maccel = raccel = 0.0f;

            s_id = (unsigned int)s_id_raw;
            c_id = (unsigned int)c_id_raw;
            u_id = tu_id_raw >> 16;
            t_id = tu_id_raw & 0x0000FFFF;

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);
            TuioToken3D *ttok = tobj->getTuioToken3D();
            if (ttok == NULL) {
                ttok = new TuioToken3D(frameTime,tobj,t_id,u_id,c_id,xpos,ypos,zpos,angle,xaxis,yaxis,zaxis);
                tobj->setTuioToken3D(ttok);
            } else {
                ttok->update(frameTime,xpos,ypos,zpos,angle,xaxis,yaxis,zaxis,xspeed,yspeed,zspeed,rspeed,maccel,raccel);
            }

        } else if( strcmp( msg.AddressPattern(), "/tuio2/p3d" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw, tu_id_raw, c_id_raw;
            unsigned int s_id, c_id;
            unsigned short t_id, u_id;
            float xpos, ypos, zpos, xaxis, yaxis, zaxis, radius, xspeed, yspeed, zspeed, rspeed, maccel, raccel;
            args >> s_id_raw >> tu_id_raw >> c_id_raw >> xpos >> ypos >> zpos >> xaxis >> yaxis >> zaxis >> radius;
            if (!args.Eos()) args >> xspeed >> yspeed >> zspeed >> rspeed >> maccel >> raccel;
            else xspeed = yspeed = zspeed = rspeed = maccel = raccel = 0.0f;

            s_id = (unsigned int)s_id_raw;
            c_id = (unsigned int)c_id_raw;
            u_id = tu_id_raw >> 16;
            t_id = tu_id_raw & 0x0000FFFF;

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);
            TuioPointer3D *tptr = tobj->getTuioPointer3D();
            if (tptr == NULL) {
                tptr = new TuioPointer3D(frameTime,tobj,t_id,u_id,c_id,xpos,ypos,zpos,xaxis,yaxis,zaxis,radius);
                tobj->setTuioPointer3D(tptr);
            } else {
                tptr->update(frameTime,xpos,ypos,zpos,0.0f,xaxis,yaxis,zaxis,xspeed,yspeed,zspeed,rspeed,maccel,raccel);
            }

        } else if( strcmp( msg.AddressPattern(), "/tuio2/b3d" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id_raw;
            unsigned int s_id;
            float xpos, ypos, zpos, angle, xaxis, yaxis, zaxis, width, height, depth, volume, xspeed, yspeed, zspeed, rspeed, maccel, raccel;
            args >> s_id_raw >> xpos >> ypos >> zpos >> angle >> xaxis >> yaxis >> zaxis >> width >> height >> depth >> volume;
            if (!args.Eos()) args >> xspeed >> yspeed >> zspeed >> rspeed >> maccel >> raccel;
            else xspeed = yspeed = zspeed = rspeed = maccel = raccel = 0.0f;

            s_id = (unsigned int)s_id_raw;

            TuioObject *tobj = getFrameObject(frameSource->getSourceID(),s_id);
            if (tobj == NULL) tobj = new TuioObject(frameTime,frameSource,s_id);
            addFrameObject(tobj);
            TuioBounds3D *tbnd = tobj->getTuioBounds3D();
            if (tbnd == NULL) {
                tbnd = new TuioBounds3D(frameTime,tobj,xpos,ypos,zpos,angle,xaxis,yaxis,zaxis,width,height,depth,volume);
                tobj->setTuioBounds3D(tbnd);
            } else {
                tbnd->update(frameTime,xpos,ypos,zpos,angle,xaxis,yaxis,zaxis,xspeed,yspeed,zspeed,rspeed,maccel,raccel);
            }

        } else if( strcmp( msg.AddressPattern(), "/tuio2/alv" ) == 0 ) {

            if (lateFrame) return;
            int32 s_id;
            aliveObjectList.clear();
            while(!args.Eos()) {
                args >> s_id;
                aliveObjectList.push_back((unsigned int)s_id);
            }

            lockObjectList();
            //find the removed tobjs first
            for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj!=tobjList.end(); tobj++) {
                if ((*tobj)->getTuioSource()->getSourceID()!=frameSource->getSourceID()) continue;
                std::list<unsigned int>::iterator iter = find(aliveObjectList.begin(), aliveObjectList.end(), (*tobj)->getSessionID());
                if (iter == aliveObjectList.end()) {
                    (*tobj)->remove(frameTime);
                    addFrameObject(*tobj);
                }
            }
            unlockObjectList();

            for (std::list<TuioObject*>::iterator iter=frameObjectList.begin(); iter!=frameObjectList.end(); iter++) {
                TuioObject *tobj = (*iter);

                switch (tobj->getTuioState()) {
                    case TUIO_REMOVED:

                        for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener!=listenerList.end(); listener++)
                            (*listener)->tuioRemove(tobj);

                        lockObjectList();
                        for (std::list<TuioObject*>::iterator delcon=tobjList.begin(); delcon!=tobjList.end(); delcon++) {
                            if (((*delcon)->getSessionID()==tobj->getSessionID()) && ((*delcon)->getTuioSource()->getSourceID()==frameSource->getSourceID())) {
                                delete *delcon;
                                tobjList.erase(delcon);
                                break;
                            }
                        }
                        unlockObjectList();
                        break;
                    default: {

                        // membership in tobjList is the reliable discriminator —
                        // the state alone cannot distinguish a fresh object from one
                        // re-marked ADDED or left in a stale state
                        bool known = false;
                        lockObjectList();
                        for (std::list<TuioObject*>::iterator findobj=tobjList.begin(); findobj!=tobjList.end(); findobj++) {
                            if (((*findobj)->getSessionID()==tobj->getSessionID()) && ((*findobj)->getTuioSource()->getSourceID()==frameSource->getSourceID())) {
                                known = true;
                                break;
                            }
                        }
                        if (!known) tobjList.push_back(tobj);
                        unlockObjectList();

                        for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++) {
                            if (!known) (*listener)->tuioAdd(tobj);
                            else (*listener)->tuioUpdate(tobj);
                        }

                        // commit the object and its components — otherwise objects that
                        // never receive a real update remain in a stale TUIO_ADDED state
                        tobj->commit(frameTime);
                        break;
                    }
                }
            }

            for (std::list<TuioListener*>::iterator listener=listenerList.begin(); listener != listenerList.end(); listener++)
                (*listener)->tuioRefresh(frameTime);

            frameObjectList.clear();
            //unlockFrame();
        }
    } catch( Exception& e ){
        std::cerr << "error parsing TUIO2 message: "<< msg.AddressPattern() <<  " - " << e.what() << std::endl;
        //unlockFrame();
    }
}

bool TuioClient::isConnected() {
	return receiver->isConnected();
}

void TuioClient::connect(bool lock) {

	receiver->connect(lock);
}

void TuioClient::processBundleBegin() {

	// pending objects are reconciled at /frm time instead of per-packet:
	// a frame may legitimately span multiple packets (capacity split)
}

void TuioClient::disconnect() {

	receiver->disconnect();
	aliveObjectList.clear();
	associatedObjectList.clear();
	for (std::list<TuioObject*>::iterator iter=frameObjectList.begin(); iter != frameObjectList.end(); iter++) {
		if (std::find(tobjList.begin(),tobjList.end(),*iter)==tobjList.end())
			delete (*iter);
	}
	frameObjectList.clear();

	for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++)
		delete (*iter);
	tobjList.clear();

	for (std::map<std::string,TuioSource*>::iterator iter=sourceList.begin(); iter != sourceList.end(); iter++)
		delete (*iter).second;
	sourceList.clear();
	source_count = 0;
}

TuioObject* TuioClient::getTuioObject(unsigned int src_id, unsigned int s_id) {
    lockObjectList();
    for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
        if (((*iter)->getSessionID()==s_id) && ((*iter)->getTuioSource()->getSourceID()==src_id)) {
            unlockObjectList();
            return (*iter);
        }
    }
    unlockObjectList();
    return NULL;
}

TuioToken* TuioClient::getTuioToken(unsigned int src_id, unsigned int s_id) {
	lockObjectList();
	for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
		if (((*iter)->getSessionID()==s_id) && ((*iter)->getTuioSource()->getSourceID()==src_id)) {
			unlockObjectList();
            return (*iter)->getTuioToken();
		}
	}
	unlockObjectList();
	return NULL;
}

TuioPointer* TuioClient::getTuioPointer(unsigned int src_id, unsigned int s_id) {
    lockObjectList();
    for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
        if (((*iter)->getSessionID()==s_id) && ((*iter)->getTuioSource()->getSourceID()==src_id)) {
            unlockObjectList();
            return (*iter)->getTuioPointer();
        }
    }
    unlockObjectList();
    return NULL;
}

TuioBounds* TuioClient::getTuioBounds(unsigned int src_id, unsigned int s_id) {
    lockObjectList();
    for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
        if (((*iter)->getSessionID()==s_id) && ((*iter)->getTuioSource()->getSourceID()==src_id)) {
            unlockObjectList();
            return (*iter)->getTuioBounds();
        }
    }
    unlockObjectList();
    return NULL;
}

TuioSymbol* TuioClient::getTuioSymbol(unsigned int src_id, unsigned int s_id) {
    lockObjectList();
    for (std::list<TuioObject*>::iterator iter=tobjList.begin(); iter != tobjList.end(); iter++) {
        if (((*iter)->getSessionID()==s_id) && ((*iter)->getTuioSource()->getSourceID()==src_id)) {
            unlockObjectList();
            return (*iter)->getTuioSymbol();
        }
    }
    unlockObjectList();
    return NULL;
}

std::list<TuioObject*> TuioClient::getTuioObjectList(unsigned int src_id) {
    lockObjectList();
    std::list<TuioObject*> listBuffer;
    for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj != tobjList.end(); tobj++) {
        if ((*tobj)->getTuioSource()->getSourceID()==src_id) listBuffer.push_back(*tobj);
    }
    unlockObjectList();
    return listBuffer;
}

std::list<TuioToken*> TuioClient::getTuioTokenList(unsigned int src_id) {
    lockObjectList();
    std::list<TuioToken*> listBuffer;
    for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj != tobjList.end(); tobj++) {
        if ((*tobj)->getTuioSource()->getSourceID()==src_id) {
            TuioToken *ttok = (*tobj)->getTuioToken();
            if (ttok!=NULL) listBuffer.push_back(ttok);
        }
    }
    unlockObjectList();
    return listBuffer;
}

std::list<TuioPointer*> TuioClient::getTuioPointerList(unsigned int src_id) {
    lockObjectList();
    std::list<TuioPointer*> listBuffer;
    for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj != tobjList.end(); tobj++) {
        if ((*tobj)->getTuioSource()->getSourceID()==src_id) {
            TuioPointer *tptr = (*tobj)->getTuioPointer();
            if (tptr!=NULL) listBuffer.push_back(tptr);
        }
    }
    unlockObjectList();
    return listBuffer;
}

std::list<TuioBounds*> TuioClient::getTuioBoundsList(unsigned int src_id) {
    lockObjectList();
    std::list<TuioBounds*> listBuffer;
    for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj != tobjList.end(); tobj++) {
        if ((*tobj)->getTuioSource()->getSourceID()==src_id) {
            TuioBounds *tbnd = (*tobj)->getTuioBounds();
            if (tbnd!=NULL) listBuffer.push_back(tbnd);
        }
    }
    unlockObjectList();
    return listBuffer;
}

std::list<TuioSymbol*> TuioClient::getTuioSymbolList(unsigned int src_id) {
    lockObjectList();
    std::list<TuioSymbol*> listBuffer;
    for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj != tobjList.end(); tobj++) {
        if ((*tobj)->getTuioSource()->getSourceID()==src_id) {
            TuioSymbol *tsym = (*tobj)->getTuioSymbol();
            if (tsym!=NULL) listBuffer.push_back(tsym);
        }
    }
    unlockObjectList();
    return listBuffer;
}

void TuioClient::addFrameObject(TuioObject *tobj) {
    for (std::list<TuioObject*>::iterator iter=frameObjectList.begin(); iter != frameObjectList.end(); iter++) {
        if ((*iter)->getSessionID()==tobj->getSessionID()) return;
    }

    frameObjectList.push_back(tobj);
}

TuioObject* TuioClient::getFrameObject(unsigned int src_id, unsigned int s_id) {
    for (std::list<TuioObject*>::iterator tobj=frameObjectList.begin(); tobj != frameObjectList.end(); tobj++) {
        if ((*tobj)->getSessionID()==s_id) return *tobj;
    }

    for (std::list<TuioObject*>::iterator tobj=tobjList.begin(); tobj != tobjList.end(); tobj++) {
        if (((*tobj)->getSessionID()==s_id) && ((*tobj)->getTuioSource()->getSourceID()==src_id)) return *tobj;
    }

    return NULL;
}

void TuioClient::lockFrame() {
#ifdef WIN32
    WaitForSingleObject(frameMutex, INFINITE);
#else
    pthread_mutex_lock(&frameMutex);
#endif
}

void TuioClient::unlockFrame() {
#ifdef WIN32
    ReleaseMutex(frameMutex);
#else
    pthread_mutex_unlock(&frameMutex);
#endif
}
