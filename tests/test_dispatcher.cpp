#include "test_helpers.h"
#include "TuioDispatcher.h"
#include "TuioObject.h"
#include "TuioToken.h"
#include "TuioPointer.h"
#include "TuioBounds.h"
#include "TuioSymbol.h"
using namespace TUIO2;

TEST(dispatcher_listeners) {
	TestDispatcher disp;
	RecordingListener l1, l2;
	disp.addTuioListener(&l1);
	disp.addTuioListener(&l2);
	CHECK_EQ(disp.listenerCount(), 2);

	disp.removeTuioListener(&l1);
	CHECK_EQ(disp.listenerCount(), 1);
	disp.removeAllTuioListeners();
	CHECK_EQ(disp.listenerCount(), 0);
}

TEST(dispatcher_getters_counts) {
	TestDispatcher disp;

	TuioObject *tokObj = new TuioObject(TuioTime(0,0), 11);
	tokObj->setTuioToken(new TuioToken(TuioTime(0,0), tokObj, 0, 0, 5, 0.1f, 0.2f, 0.3f));
	disp.injectObject(tokObj);

	TuioObject *ptrObj = new TuioObject(TuioTime(0,0), 12);
	ptrObj->setTuioPointer(new TuioPointer(TuioTime(0,0), ptrObj, 0, 0, 0, 0.5f, 0.5f, 0.0f, 0.0f, 0.1f, 0.5f));
	disp.injectObject(ptrObj);

	TuioObject *bndObj = new TuioObject(TuioTime(0,0), 13);
	bndObj->setTuioBounds(new TuioBounds(TuioTime(0,0), bndObj, 0.5f, 0.5f, 0.0f, 0.1f, 0.1f, 0.01f));
	disp.injectObject(bndObj);

	TuioObject *symObj = new TuioObject(TuioTime(0,0), 14);
	symObj->setTuioSymbol(new TuioSymbol(TuioTime(0,0), symObj, 0, 0, 7, "type", "data"));
	disp.injectObject(symObj);

	CHECK_EQ((int)disp.getTuioObjectList().size(), 4);
	CHECK_EQ((int)disp.getTuioTokenList().size(), 1);
	CHECK_EQ((int)disp.getTuioPointerList().size(), 1);
	CHECK_EQ((int)disp.getTuioBoundsList().size(), 1);
	CHECK_EQ((int)disp.getTuioSymbolList().size(), 1);

	TuioObject *found = disp.getTuioObject(11);
	CHECK(found != NULL);
	CHECK(found->getTuioToken() != NULL);
	CHECK_EQ(found->getTuioToken()->getSymbolID(), 5u);
	CHECK(disp.getTuioObject(99) == NULL);
	CHECK(disp.getTuioToken(11) != NULL);
	CHECK(disp.getTuioPointer(12) != NULL);
	CHECK(disp.getTuioBounds(13) != NULL);
	CHECK(disp.getTuioSymbol(14) != NULL);
}

TEST(dispatcher_lock_unlock) {
	TestDispatcher disp;
	// balanced lock/unlock pairs must not deadlock
	disp.lockObjectList();
	disp.unlockObjectList();
	CHECK(true);
}
