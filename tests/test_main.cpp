/*
 Test runner main for the TUIO library test suite.
*/

#include "test_util.h"

int main() {
	return tuio_test::run_all() ? 1 : 0;
}
