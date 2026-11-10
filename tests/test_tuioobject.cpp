#include "test_util.h"
#include "TuioObject.h"
#include "TuioToken.h"
#include "TuioPointer.h"
#include "TuioBounds.h"
#include "TuioSymbol.h"
using namespace TUIO2;

TEST(tuioobject_state_and_components) {
	TuioObject *tobj = new TuioObject(TuioTime(0,0), 42);
	CHECK_EQ(tobj->getSessionID(), 42u);
	CHECK(tobj->getTuioToken() == NULL);
	CHECK(tobj->getTuioPointer() == NULL);

	tobj->setTuioToken(new TuioToken(TuioTime(0,0), tobj, 0, 0, 5, 0.3f, 0.4f, 0.5f));
	CHECK(tobj->getTuioToken() != NULL);
	CHECK_EQ(tobj->getTuioToken()->getSymbolID(), 5u);
	CHECK_NEAR(tobj->getTuioToken()->getX(), 0.3, 1e-6);
	CHECK_EQ((int)tobj->getTuioState(), TUIO_ADDED);

	// removing a component marks it removed
	tobj->removeTuioToken(TuioTime(1,0));
	CHECK(tobj->getTuioToken() != NULL);
	CHECK_EQ(tobj->getTuioToken()->getTuioState(), TUIO_REMOVED);

	delete tobj;
}

TEST(tuioobject_all_components) {
	TuioObject *tobj = new TuioObject(TuioTime(0,0), 7);
	tobj->setTuioToken(new TuioToken(TuioTime(0,0), tobj, 0, 0, 1, 0.1f, 0.1f, 0.0f));
	tobj->setTuioPointer(new TuioPointer(TuioTime(0,0), tobj, 0, 0, 0, 0.2f, 0.2f, 0.0f, 0.0f, 0.1f, 0.5f));
	tobj->setTuioBounds(new TuioBounds(TuioTime(0,0), tobj, 0.3f, 0.3f, 0.0f, 0.2f, 0.2f, 0.04f));
	tobj->setTuioSymbol(new TuioSymbol(TuioTime(0,0), tobj, 0, 0, 9, "type", "data"));

	CHECK(tobj->getTuioToken() != NULL);
	CHECK(tobj->getTuioPointer() != NULL);
	CHECK(tobj->getTuioBounds() != NULL);
	CHECK(tobj->getTuioSymbol() != NULL);

	// removing all components marks each removed
	tobj->removeAllTuioComponents(TuioTime(2,0));
	CHECK_EQ(tobj->getTuioToken()->getTuioState(), TUIO_REMOVED);
	CHECK_EQ(tobj->getTuioPointer()->getTuioState(), TUIO_REMOVED);
	CHECK_EQ(tobj->getTuioBounds()->getTuioState(), TUIO_REMOVED);
	CHECK_EQ(tobj->getTuioSymbol()->getTuioState(), TUIO_REMOVED);

	delete tobj;
}

TEST(tuioobject_time_and_source) {
	TuioObject *tobj = new TuioObject(TuioTime(5,0), 21);
	CHECK_EQ(tobj->getTuioTime().getSeconds(), 5L);
	CHECK(tobj->getTuioSource() != NULL);
	delete tobj;
}
