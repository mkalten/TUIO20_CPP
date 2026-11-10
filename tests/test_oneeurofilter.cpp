#include "test_util.h"
#include "OneEuroFilter.h"
using namespace TUIO;

TEST(lowpass_basics) {
	LowPassFilter lp(1.0);
	// first call initializes to the input value
	double r = lp.filter(5.0, 0.5);
	CHECK_NEAR(r, 5.0, 1e-9);
	// subsequent calls smooth toward the input: 5 + 0.5*(7-5) = 6
	r = lp.filter(7.0, 0.5);
	CHECK_NEAR(r, 6.0, 1e-9);
}

TEST(oneeuro_ctor_validation) {
	bool threw = false;
	try { OneEuroFilter f(0.0); } catch (std::range_error&) { threw = true; }
	CHECK(threw);
	threw = false;
	try { OneEuroFilter f(60.0, 0.0); } catch (std::range_error&) { threw = true; }
	CHECK(threw);
	threw = false;
	try { OneEuroFilter f(60.0, 1.0, 0.0); } catch (std::range_error&) { threw = true; }
	CHECK(threw);

	// valid parameters do not throw
	OneEuroFilter ok(60.0, 1.0, 0.5, 1.0);
	CHECK(std::isfinite(ok.filter(1.0, 0.016)));
}

TEST(oneeuro_converges) {
	OneEuroFilter f(60.0, 1.0, 0.5, 1.0);
	// repeated same-dt updates converge toward the step input
	double r = 0.0;
	for (int i = 0; i < 100; i++) r = f.filter(1.0, 1.0/60.0);
	CHECK_NEAR(r, 1.0, 1e-3);
	CHECK(r <= 1.0);
}

TEST(oneeuro_dt_guard) {
	OneEuroFilter f(60.0, 1.0, 0.5, 1.0);
	double first = f.filter(2.0, 0.016);
	// dt<=0 must not produce NaN (division-by-zero guard in filter)
	double r = f.filter(5.0, 0.0);
	CHECK(std::isfinite(r));
	r = f.filter(5.0, -1.0);
	CHECK(std::isfinite(r));
	CHECK(std::isfinite(first));
}

TEST(oneeuro_copy_independence) {
	OneEuroFilter f(60.0, 1.0, 0.5, 1.0);
	for (int i = 0; i < 10; i++) f.filter(0.5, 1.0/60.0);

	OneEuroFilter copy(f);
	OneEuroFilter asg(120.0, 2.0, 1.0, 2.0);
	asg = f;

	// mutating the copies must not disturb the original's state
	for (int i = 0; i < 10; i++) copy.filter(10.0, 1.0/60.0);
	for (int i = 0; i < 10; i++) asg.filter(-10.0, 1.0/60.0);

	double orig = f.filter(0.5, 1.0/60.0);
	double cp   = copy.filter(0.5, 1.0/60.0);
	double ag   = asg.filter(0.5, 1.0/60.0);
	CHECK(std::isfinite(orig) && std::isfinite(cp) && std::isfinite(ag));
	// the copy chased 10.0, the assignment -10.0: they must differ from the original
	CHECK(orig != cp);
	CHECK(orig != ag);
}
