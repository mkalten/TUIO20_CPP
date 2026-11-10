#include "test_helpers.h"
#include "TuioManager.h"
using namespace TUIO2;

TEST(manager_add_update_remove_token) {
	TuioManager mgr;

	TuioObject *tobj = mgr.createTuioToken(5, 0.3f, 0.4f, 0.5f);
	CHECK(tobj != NULL);
	CHECK(tobj->getTuioToken() != NULL);
	CHECK_EQ((int)mgr.getTuioObjectList().size(), 1);
	CHECK_EQ((int)mgr.getTuioTokenList().size(), 1);

	// updates are no-ops within the same frame — advance to the next frame
	mgr.initTuioFrame(TuioTime(1,0));
	mgr.updateTuioToken(tobj->getTuioToken(), 0.6f, 0.4f, 0.5f);
	CHECK_NEAR(tobj->getTuioToken()->getX(), 0.6, 1e-6);

	mgr.removeTuioToken(tobj->getTuioToken());
	CHECK_EQ((int)mgr.getTuioTokenList().size(), 0);
}

TEST(manager_remove_object) {
	TuioManager mgr;
	TuioObject *tobj = mgr.createTuioBounds(0.3f, 0.4f, 0.0f, 0.2f, 0.2f, 0.04f);
	CHECK_EQ((int)mgr.getTuioBoundsList().size(), 1);

	mgr.removeTuioObject(tobj);
	CHECK_EQ((int)mgr.getTuioObjectList().size(), 0);
	CHECK_EQ((int)mgr.getTuioBoundsList().size(), 0);
}

TEST(manager_multi_component_object) {
	TuioManager mgr;
	TuioObject *tobj = mgr.createTuioObject();
	CHECK(tobj != NULL);

	mgr.addTuioToken(tobj, 5, 0.1f, 0.1f, 0.0f);
	mgr.addTuioBounds(tobj, 0.2f, 0.2f, 0.0f, 0.1f, 0.1f, 0.01f);

	CHECK(tobj->getTuioToken() != NULL);
	CHECK(tobj->getTuioBounds() != NULL);
	CHECK_EQ((int)mgr.getTuioTokenList().size(), 1);
	CHECK_EQ((int)mgr.getTuioBoundsList().size(), 1);
}

TEST(manager_frame_cycle) {
	TuioManager mgr;
	mgr.initTuioFrame(TuioTime::getSystemTime());
	mgr.createTuioPointer(0.3f, 0.4f, 0.0f, 0.0f, 0.1f, 0.5f);
	mgr.commitTuioFrame();
	CHECK_EQ((int)mgr.getTuioPointerList().size(), 1);
}
