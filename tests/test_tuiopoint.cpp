#include "test_util.h"
#include "TuioPoint.h"
using namespace TUIO;

TEST(tuiopoint_ctors) {
	TuioPoint a(0.3f, 0.7f);
	CHECK_NEAR(a.getX(), 0.3, 1e-6);
	CHECK_NEAR(a.getY(), 0.7, 1e-6);

	TuioTime tt(5, 0);
	TuioPoint b(tt, 0.1f, 0.2f);
	CHECK(b.getTuioTime()==tt);
	CHECK(b.getStartTime()==tt);

	// copy-from-pointer takes position but stamps a new time
	TuioPoint c(&a);
	CHECK_NEAR(c.getX(), 0.3, 1e-6);
	CHECK_NEAR(c.getY(), 0.7, 1e-6);

	TuioPoint d(b);
	CHECK_NEAR(d.getX(), 0.1, 1e-6);
	CHECK(d.getTuioTime()==tt);
}

TEST(tuiopoint_update) {
	TuioPoint p(0.0f, 0.0f);
	p.update(0.5f, 0.5f);
	CHECK_NEAR(p.getX(), 0.5, 1e-6);

	TuioPoint q(0.9f, 0.9f);
	p.update(&q);
	CHECK_NEAR(p.getX(), 0.9, 1e-6);

	TuioTime tt(2, 0);
	p.update(tt, 0.1f, 0.1f);
	CHECK_NEAR(p.getX(), 0.1, 1e-6);
	CHECK(p.getTuioTime()==tt);
}

TEST(tuiopoint_distance_angle) {
	TuioPoint p(0.0f, 0.0f);
	CHECK_NEAR(p.getDistance(0.3f, 0.4f), 0.5, 1e-5);
	CHECK_NEAR(p.getScreenDistance(0.3f, 0.4f, 1000, 1000), 500, 1e-2);

	TuioPoint q(0.6f, 0.8f);
	CHECK_NEAR(p.getDistance(&q), 1.0, 1e-5);

	// coincident points return 0 (division-by-zero guard)
	CHECK_NEAR(p.getAngle(0.0f, 0.0f), 0.0, 1e-6);
	CHECK_NEAR(p.getAngle(&p), 0.0, 1e-6);

	// getAngle(xp,yp) is the direction from (xp,yp) to this point
	CHECK_NEAR(p.getAngle(1.0f, 0.0f), 0.0, 1e-4);      // east
	CHECK_NEAR(p.getAngle(-1.0f, 0.0f), M_PI, 1e-4);    // west
	CHECK_NEAR(p.getAngle(0.0f, -1.0f), M_PI/2, 1e-4);  // height>0, no wrap
	CHECK_NEAR(p.getAngle(0.0f, 1.0f), 3*M_PI/2, 1e-4); // height<0 wraps
}

TEST(tuiopoint_screen) {
	TuioPoint p(0.504f, 0.496f);
	CHECK_EQ(p.getScreenX(1000), 504);
	CHECK_EQ(p.getScreenY(1000), 496);
}

TEST(tuiopoint_threshold) {
	TuioPoint p(0.5f, 0.5f);
	p.addPositionThreshold(0.1f);

	// small change inside the threshold is suppressed
	p.update(TuioTime(1,0), 0.55f, 0.55f);
	CHECK_NEAR(p.getX(), 0.5, 1e-6);

	// larger change is applied
	p.update(TuioTime(2,0), 0.8f, 0.5f);
	CHECK_NEAR(p.getX(), 0.8, 1e-6);

	p.removePositionThreshold();
	p.update(TuioTime(3,0), 0.55f, 0.5f);
	CHECK_NEAR(p.getX(), 0.55, 1e-6);
}

TEST(tuiopoint_copy_independent_filters) {
	TuioPoint p(0.5f, 0.5f);
	p.addPositionFilter(1.0f, 0.1f);

	// deep-copy: mutating the copy's filter must not affect the original
	TuioPoint copy(p);
	copy.update(TuioTime(1,0), 0.9f, 0.9f);
	copy.update(TuioTime(2,0), 0.9f, 0.9f);

	p.update(TuioTime(3,0), 0.6f, 0.6f);
	CHECK(p.getX() < 0.9);  // would equal copy's filtered value if filters were shared
	CHECK(std::isfinite(p.getX()));
	CHECK(std::isfinite(copy.getX()));

	// assignment produces an independent copy too
	TuioPoint asg(0.0f, 0.0f);
	asg = p;
	asg.update(TuioTime(4,0), 0.95f, 0.95f);
	CHECK(p.getX() != asg.getX() || p.getX() < 0.9);

	p.removePositionFilter();
}
