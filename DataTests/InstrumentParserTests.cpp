#include <boost/test/unit_test.hpp>

#include <fstream>
#include <sstream>

#include "Data/InstrumentParser.h"

BOOST_AUTO_TEST_SUITE(InstrumentParserTests)

using namespace cf;
using namespace cf::Data;

BOOST_AUTO_TEST_CASE(BasicTest)
{
	std::string filename = "instruments_basic.testdata";
	
	std::ostringstream oss;
	oss
		<< "["
			<< "instrument { "
				<< "name: \"piano\","
				<< "patches: ["
					<< "patch {"
						<< "name: \"regular\""
						<< "keyswitch: F#2"
						<< "length: 0.1,"
						<< "attack: 0.2,"
						<< "weight: 0.3,"
					<< "},"
					<< "patch { }" // dummy patch
				<< "]"
			<< "},"
			<< "instrument {  }" // dummy instrument
		<< "]";

	//std::cout << oss.str();

	{
		std::ofstream ofs(filename, std::ios_base::trunc);
		ofs << oss.str();
	}

	InstrumentParser parser;
	BOOST_CHECK(parser.parse(filename));
	auto instruments = parser.Instruments();

	// Instrument
	BOOST_REQUIRE_GT(instruments.size(), 0);
	BOOST_CHECK_EQUAL(instruments.size(), 2);
	auto instrument = instruments["piano"];
	BOOST_CHECK_EQUAL(instrument.name, "piano"); // Check that it was really found

	// Patch
	BOOST_REQUIRE_GT(instrument.patches.size(), 0);
	BOOST_CHECK_EQUAL(instrument.patches.size(), 2);
	auto patch = instrument.patches[0];

	BOOST_CHECK_EQUAL(patch.name, "regular");
	BOOST_CHECK_EQUAL(patch.keyswitch, 42);
	BOOST_CHECK_EQUAL(patch.length, 0.1);
	BOOST_CHECK_EQUAL(patch.attack, 0.2);
	BOOST_CHECK_EQUAL(patch.weight, 0.3);

}

BOOST_AUTO_TEST_SUITE_END()
