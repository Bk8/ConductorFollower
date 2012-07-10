#include <boost/test/unit_test.hpp>

#include <fstream>
#include <sstream>

#include "Data/ScoreParser.h"

BOOST_AUTO_TEST_SUITE(ScoreParserTests)

using namespace cf;
using namespace cf::Data;

BOOST_AUTO_TEST_CASE(BasicTest)
{
	std::string filename = "score_basic.testdata";
	
	std::ostringstream oss;
	oss
		<< "score { "
			<< "name: \"Never gonna give you up\","
			<< "midi_file: \"/home/user/nggyu.mid\","
			<< "instrument_file: \"/home/user/instr.def\","
			<< "tracks: ["
				<< "track {"
					<< "name: \"piano\","
					<< "instrument: \"piano\""
				<< "},"
				<< "track { }" // dummy track
			<< "]"
		<< "}";

	//std::cout << oss.str();

	{
		std::ofstream ofs(filename, std::ios_base::trunc);
		ofs << oss.str();
	}

	ScoreParser parser;
	BOOST_CHECK(parser.parse(filename));
	auto score = parser.data();

	// Score
	BOOST_CHECK_EQUAL(score.name, "Never gonna give you up");
	BOOST_CHECK_EQUAL(score.midiFile, "/home/user/nggyu.mid");
	BOOST_CHECK_EQUAL(score.instrumentFile, "/home/user/instr.def");
	
	// Track
	BOOST_REQUIRE_GT(score.tracks.size(), 0);
	BOOST_CHECK_EQUAL(score.tracks.size(), 2);
	auto track = score.tracks[0];
	BOOST_CHECK_EQUAL(track.name, "piano");
	BOOST_CHECK_EQUAL(track.instrument, "piano");
}

BOOST_AUTO_TEST_SUITE_END()
