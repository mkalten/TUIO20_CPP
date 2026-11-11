#include "test_util.h"
#include "TuioObject.h"
#include "TuioToken.h"
#include "TuioPointer.h"
#include "TuioBounds.h"
#include "TuioSymbol.h"
using namespace TUIO2;

// TuioComponent motion math with a valid dt
TEST(tuiocomponent_update_speeds) {
	TuioObject *tobj = new TuioObject(TuioTime(0,0), 1);
	TuioToken *ttok = new TuioToken(TuioTime(0,0), tobj, 0, 0, 5, 0.0f, 0.0f, 0.0f);
	tobj->setTuioToken(ttok);

	// move +0.1 in x over 100ms -> x_speed 1.0, y_speed 0
	ttok->update(TuioTime(0,100000), 0.1f, 0.0f, 0.0f);
	CHECK_NEAR(ttok->getXSpeed(), 1.0, 1e-4);
	CHECK_NEAR(ttok->getYSpeed(), 0.0, 1e-4);
	CHECK_NEAR(ttok->getMotionSpeed(), 1.0, 1e-4);
	CHECK_EQ(ttok->getTuioState(), TUIO_ACCELERATING);

	// another identical step -> steady speed, zero accel -> stopped
	ttok->update(TuioTime(0,200000), 0.2f, 0.0f, 0.0f);
	CHECK_NEAR(ttok->getXSpeed(), 1.0, 1e-4);

	delete tobj;
}

// the dt==0 guard must not produce NaN/Inf speeds
TEST(tuiocomponent_update_dt0_no_nan) {
	TuioObject *tobj = new TuioObject(TuioTime(0,0), 2);
	TuioToken *ttok = new TuioToken(TuioTime(0,0), tobj, 0, 0, 5, 0.5f, 0.5f, 0.0f);
	tobj->setTuioToken(ttok);

	float lastSpeed = ttok->getXSpeed();
	// same timestamp -> dt==0 -> speeds must stay finite and unchanged
	ttok->update(TuioTime(0,0), 0.9f, 0.9f, 1.0f);
	CHECK(std::isfinite(ttok->getXSpeed()));
	CHECK(std::isfinite(ttok->getYSpeed()));
	CHECK(std::isfinite(ttok->getMotionSpeed()));
	CHECK(std::isfinite(ttok->getMotionAccel()));
	CHECK(std::isfinite(ttok->getRotationSpeed()));
	CHECK(std::isfinite(ttok->getRotationAccel()));
	CHECK_NEAR(ttok->getXSpeed(), lastSpeed, 1e-6);
	// position still updates
	CHECK_NEAR(ttok->getX(), 0.9, 1e-6);

	delete tobj;
}

// the path history is capped at MAX_PATH_SIZE
TEST(tuiocomponent_path_capped) {
	TuioObject *tobj = new TuioObject(TuioTime(0,0), 3);
	TuioToken *ttok = new TuioToken(TuioTime(0,0), tobj, 0, 0, 5, 0.0f, 0.0f, 0.0f);
	tobj->setTuioToken(ttok);

	for (int i=1;i<=MAX_PATH_SIZE+10;i++)
		ttok->update(TuioTime(0,i*1000), (float)i/1000.0f, 0.0f, 0.0f);

	CHECK((int)ttok->getPath().size() <= MAX_PATH_SIZE);
	delete tobj;
}

// component field getters for each TUIO2 component type
TEST(tuiotoken_fields) {
	TuioObject *tobj = new TuioObject(TuioTime(0,0), 4);
	TuioToken *ttok = new TuioToken(TuioTime(0,0), tobj, 2, 3, 5, 0.3f, 0.4f, 0.5f);
	CHECK_EQ(ttok->getTypeID(), 2);
	CHECK_EQ(ttok->getUserID(), 3);
	CHECK_EQ(ttok->getComponentID(), 5u);
	CHECK_NEAR(ttok->getX(), 0.3, 1e-6);
	CHECK_NEAR(ttok->getY(), 0.4, 1e-6);
	CHECK_NEAR(ttok->getAngle(), 0.5, 1e-6);
	delete tobj;
}

TEST(tuiopointer_fields) {
	TuioObject *tobj = new TuioObject(TuioTime(0,0), 5);
	TuioPointer *tptr = new TuioPointer(TuioTime(0,0), tobj, 1, 2, 0, 0.3f, 0.4f, 0.5f, 0.1f, 0.05f, 0.8f);
	CHECK_EQ(tptr->getTypeID(), 1);
	CHECK_EQ(tptr->getUserID(), 2);
	CHECK_NEAR(tptr->getX(), 0.3, 1e-6);
	CHECK_NEAR(tptr->getShear(), 0.1, 1e-6);
	CHECK_NEAR(tptr->getRadius(), 0.05, 1e-6);
	CHECK_NEAR(tptr->getPressure(), 0.8, 1e-6);
	delete tobj;
}

TEST(tuiobounds_fields) {
	TuioObject *tobj = new TuioObject(TuioTime(0,0), 6);
	TuioBounds *tbnd = new TuioBounds(TuioTime(0,0), tobj, 0.3f, 0.4f, 0.5f, 0.2f, 0.1f, 0.02f);
	CHECK_NEAR(tbnd->getX(), 0.3, 1e-6);
	CHECK_NEAR(tbnd->getWidth(), 0.2, 1e-6);
	CHECK_NEAR(tbnd->getHeight(), 0.1, 1e-6);
	CHECK_NEAR(tbnd->getArea(), 0.02, 1e-6);
	CHECK_NEAR(tbnd->getAngle(), 0.5, 1e-6);
	delete tobj;
}

TEST(tuiosymbol_fields) {
	TuioObject *tobj = new TuioObject(TuioTime(0,0), 7);
	TuioSymbol *tsym = new TuioSymbol(TuioTime(0,0), tobj, 1, 1, 9, "tuio2-type", "payload");
	CHECK_EQ(tsym->getTypeID(), 1);
	CHECK_EQ(tsym->getUserID(), 1);
	CHECK_EQ(tsym->getComponentID(), 9u);
	CHECK_EQ(std::string(tsym->getSymbolType()), "tuio2-type");
	CHECK_EQ(std::string(tsym->getSymbolData()), "payload");
	delete tobj;
}
