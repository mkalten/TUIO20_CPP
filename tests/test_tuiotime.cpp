#include "test_util.h"
#include "TuioTime.h"
using namespace TUIO;

TEST(tuiotime_ctors) {
	TuioTime t0;
	CHECK_EQ(t0.getSeconds(), 0L);
	CHECK_EQ(t0.getMicroseconds(), 0L);
	CHECK_EQ(t0.getTotalMilliseconds(), 0L);

	// msec ctor takes total milliseconds
	TuioTime t1(1500);
	CHECK_EQ(t1.getSeconds(), 1L);
	CHECK_EQ(t1.getMicroseconds(), 500000L);
	CHECK_EQ(t1.getTotalMilliseconds(), 1500L);

	TuioTime t2(3, 250000);
	CHECK_EQ(t2.getSeconds(), 3L);
	CHECK_EQ(t2.getMicroseconds(), 250000L);
	CHECK_EQ(t2.getTotalMilliseconds(), 3250L);
}

TEST(tuiotime_arithmetic) {
	TuioTime t(2, 500000);

	// operator+(long us): micro-second carry is computed per component
	TuioTime sum = t + 400000;
	CHECK_EQ(sum.getSeconds(), 2L);
	CHECK_EQ(sum.getTotalMilliseconds(), 2900L);

	// operator+(TuioTime) normalizes the sum
	TuioTime sum2 = t + TuioTime(1, 700000);
	CHECK_EQ(sum2.getSeconds(), 4L);
	CHECK_EQ(sum2.getMicroseconds(), 200000L);

	// operator- borrows correctly
	TuioTime diff = t - 600000;
	CHECK_EQ(diff.getSeconds(), 1L);
	CHECK_EQ(diff.getMicroseconds(), 900000L);

	TuioTime diff2 = t - TuioTime(1, 700000);
	CHECK_EQ(diff2.getSeconds(), 0L);
	CHECK_EQ(diff2.getMicroseconds(), 800000L);

	// operators return new objects, the original is unchanged
	CHECK_EQ(t.getMicroseconds(), 500000L);
}

TEST(tuiotime_compare_reset) {
	TuioTime a(1, 500000), b(1, 500000), c(2, 0);
	CHECK(a==b);
	CHECK(!(a!=b));
	CHECK(a!=c);
	CHECK(!(a==c));

	b = c;
	CHECK(b==c);
	b.reset();
	CHECK_EQ(b.getSeconds(), 0L);
	CHECK_EQ(b.getMicroseconds(), 0L);
}

TEST(tuiotime_session) {
	TuioTime::initSession();
	TuioTime start = TuioTime::getStartTime();
	TuioTime session = TuioTime::getSessionTime();
	CHECK(session.getTotalMilliseconds() >= 0);

	// session time is monotonic
	TuioTime later = TuioTime::getSessionTime();
	CHECK(later.getTotalMilliseconds() >= session.getTotalMilliseconds());

	TuioTime sys = TuioTime::getSystemTime();
	CHECK(sys.getTotalMilliseconds() >= start.getTotalMilliseconds());
	CHECK(sys.getTotalMilliseconds() > 0);
}
