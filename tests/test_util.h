/*
 Minimal unit test harness for the TUIO library tests.
 Provides self-registering TEST cases, CHECK/CHECK_EQ/CHECK_NEAR
 assertions and a waitFor helper for the asynchronous transport tests.
*/

#ifndef INCLUDED_TEST_UTIL_H
#define INCLUDED_TEST_UTIL_H

#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>
#include <functional>

namespace tuio_test {

struct TestCase {
	const char *name;
	void (*fn)();
};

inline std::vector<TestCase>& registry() {
	static std::vector<TestCase> r;
	return r;
}

struct Registrar {
	Registrar(const char *name, void (*fn)()) {
		registry().push_back(TestCase{name,fn});
	}
};

inline int& check_count() { static int c = 0; return c; }
inline int& fail_count()  { static int f = 0; return f; }

inline void report_fail(const char *expr, const char *file, int line) {
	fail_count()++;
	std::cerr << "    FAIL " << file << ":" << line
	          << "  CHECK(" << expr << ")" << std::endl;
}

template<typename A, typename B>
inline void check_eq(const A &a, const B &b, const char *ea, const char *eb,
                     const char *file, int line) {
	check_count()++;
	if (!(a==b)) {
		fail_count()++;
		std::cerr << "    FAIL " << file << ":" << line
		          << "  CHECK_EQ(" << ea << ", " << eb << ")"
		          << "  [" << a << " != " << b << "]" << std::endl;
	}
}

inline void check_near(double a, double b, double eps,
                       const char *ea, const char *eb,
                       const char *file, int line) {
	check_count()++;
	if (!(std::fabs(a-b) <= eps)) {
		fail_count()++;
		std::cerr << "    FAIL " << file << ":" << line
		          << "  CHECK_NEAR(" << ea << ", " << eb << ", " << eps << ")"
		          << "  [" << a << " vs " << b << "]" << std::endl;
	}
}

// polls the predicate every millisecond until it returns true or the timeout expires
template<typename Pred>
inline bool waitFor(Pred pred, int timeout_ms) {
	std::chrono::steady_clock::time_point deadline =
		std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
	while (std::chrono::steady_clock::now() < deadline) {
		if (pred()) return true;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	return pred();
}

inline int run_all() {
	int failed_tests = 0;
	for (std::vector<TestCase>::iterator t = registry().begin(); t != registry().end(); ++t) {
		int before = fail_count();
		std::cout << "[TEST] " << t->name << " ... " << std::flush;
		try { t->fn(); }
		catch (std::exception &e) {
			fail_count()++;
			std::cerr << "    EXCEPTION " << e.what() << std::endl;
		} catch (...) {
			fail_count()++;
			std::cerr << "    EXCEPTION (unknown)" << std::endl;
		}
		if (fail_count() > before) { failed_tests++; std::cout << "FAIL" << std::endl; }
		else std::cout << "ok" << std::endl;
	}
	std::cout << check_count() << " checks, " << fail_count() << " failed, "
	          << failed_tests << "/" << registry().size() << " tests failed" << std::endl;
	return failed_tests;
}

} // namespace tuio_test

#define TEST(name) \
	static void name(); \
	static tuio_test::Registrar registrar_##name(#name, name); \
	static void name()

#define CHECK(cond) do { tuio_test::check_count()++; \
	if (!(cond)) tuio_test::report_fail(#cond,__FILE__,__LINE__); } while(0)

#define CHECK_EQ(a,b) tuio_test::check_eq((a),(b),#a,#b,__FILE__,__LINE__)

#define CHECK_NEAR(a,b,eps) tuio_test::check_near((a),(b),(eps),#a,#b,__FILE__,__LINE__)

// polls the predicate for up to timeout milliseconds
#define WAIT_FOR(pred,timeout_ms) CHECK(tuio_test::waitFor([&]{ return (pred); },timeout_ms))

#endif /* INCLUDED_TEST_UTIL_H */
