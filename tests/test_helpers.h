/*
 Shared helpers for the TUIO2 library tests:
 a TuioListener that records callbacks, an OscSender that captures
 the serialized OSC data, and an OscReceiver stub exposing ProcessPacket.
*/

#ifndef INCLUDED_TEST_HELPERS_H
#define INCLUDED_TEST_HELPERS_H

#include "test_util.h"
#include "TuioListener.h"
#include "OscSender.h"
#include "OscReceiver.h"
#include "TuioDispatcher.h"
#include "TuioClient.h"

using namespace TUIO;
using namespace TUIO2;

// records every listener callback with the last received object state
class RecordingListener : public TuioListener {
public:
	int addObj = 0, updObj = 0, delObj = 0;
	int refreshCount = 0;

	unsigned int lastSessionID = 0;
	float lastX = -1, lastY = -1, lastAngle = -1;
	bool hadToken = false, hadPointer = false, hadBounds = false, hadSymbol = false;

	void record(TuioObject *tobj) {
		lastSessionID = tobj->getSessionID();
		hadToken = (tobj->getTuioToken()!=NULL);
		hadPointer = (tobj->getTuioPointer()!=NULL);
		hadBounds = (tobj->getTuioBounds()!=NULL);
		hadSymbol = (tobj->getTuioSymbol()!=NULL);
		if (hadToken) {
			lastX = tobj->getTuioToken()->getX();
			lastY = tobj->getTuioToken()->getY();
			lastAngle = tobj->getTuioToken()->getAngle();
		} else if (hadPointer) {
			lastX = tobj->getTuioPointer()->getX();
			lastY = tobj->getTuioPointer()->getY();
		} else if (hadBounds) {
			lastX = tobj->getTuioBounds()->getX();
			lastY = tobj->getTuioBounds()->getY();
			lastAngle = tobj->getTuioBounds()->getAngle();
		}
	}

	void tuioAdd(TuioObject *tobj) { addObj++; record(tobj); }
	void tuioUpdate(TuioObject *tobj) { updObj++; record(tobj); }
	void tuioRemove(TuioObject *tobj) { delObj++; record(tobj); }
	void tuioRefresh(TuioTime) { refreshCount++; }
};

// captures the serialized OSC data instead of sending it
// lastPacket holds the most recent packet, captured accumulates all packets
// since reset() — the server may send multiple packets per frame
class CaptureSender : public OscSender {
public:
	std::vector<char> lastPacket;
	std::string captured;
	std::vector< std::vector<char> > allPackets;
	int packets = 0;

	CaptureSender(unsigned int bufSize=8192) { buffer_size = bufSize; local = true; }
	bool sendOscPacket(osc::OutboundPacketStream *bundle) {
		lastPacket.assign(bundle->Data(), bundle->Data()+bundle->Size());
		captured.append(bundle->Data(), bundle->Size());
		allPackets.push_back(lastPacket);
		packets++;
		return true;
	}
	void reset() { lastPacket.clear(); captured.clear(); allPackets.clear(); packets = 0; }
	bool isConnected() { return true; }
	const char* tuio_type() { return "TEST"; }
};

// minimal OscReceiver that allows feeding packets directly
class StubReceiver : public OscReceiver {
public:
	void connect(bool lock=false) { (void)lock; connected = true; }
	void disconnect() { connected = false; }
};

// exposes the protected TuioDispatcher list for direct population
class TestDispatcher : public TuioDispatcher {
public:
	~TestDispatcher() {
		for (std::list<TuioObject*>::iterator i=tobjList.begin(); i!=tobjList.end(); i++) delete *i;
	}
	int listenerCount() { return (int)listenerList.size(); }
	void injectObject(TuioObject *tobj) { lockObjectList(); tobjList.push_back(tobj); unlockObjectList(); }
};

#endif /* INCLUDED_TEST_HELPERS_H */
