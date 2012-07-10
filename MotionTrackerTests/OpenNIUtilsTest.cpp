#include <boost/test/unit_test.hpp>

#include "OpenNIUtils.h"

BOOST_AUTO_TEST_SUITE(OpenNIUtilsTests)

using namespace cf::MotionTracker;

BOOST_AUTO_TEST_CASE(Stati)
{
	std::ostream nullStream(nullptr);
	OpenNIUtils utils(nullStream);

	BOOST_CHECK(CheckXnStatus(utils, XN_STATUS_OK, "Should not fail"));
	BOOST_CHECK(!CheckXnStatus(utils, XN_STATUS_ERROR, "Failing on purpose"));
}

BOOST_AUTO_TEST_CASE(ErrorCatching)
{
	std::ostream nullStream(nullptr);
	OpenNIUtils utils(nullStream);

	utils.ResetErrors();
	BOOST_CHECK(!utils.ErrorsOccurred());

	CheckXnStatus(utils, XN_STATUS_ERROR, "Setting error state on purpose");
	BOOST_CHECK(utils.ErrorsOccurred());

	CheckXnStatus(utils, XN_STATUS_OK, "ok status");
	BOOST_CHECK(utils.ErrorsOccurred());

	utils.ResetErrors();
	BOOST_CHECK(!utils.ErrorsOccurred());
}

BOOST_AUTO_TEST_SUITE_END()