#include "MidiReader.h"

#include <boost/make_shared.hpp>

#include "ScoreFollower/ScoreEvent.h"

using namespace cf;
using namespace cf::ScoreFollower;

MidiReader::MidiReader()
{
}

MidiReader::~MidiReader()
{
}

void
MidiReader::OpenFile(std::string const & filename)
{
	File file(String(filename.c_str()));
	FileInputStream stream(file);
	file_.readFrom(stream);
	file_.convertTimestampTicksToSeconds();
}

sf::TrackReaderPtr
MidiReader::Track(int index)
{
	assert(index < TrackCount());
	return boost::make_shared<TrackReaderImpl>(*file_.getTrack(index));
}

sf::TempoReaderPtr
MidiReader::TempoTrack()
{
	return boost::make_shared<TempoReaderImpl>(file_);
}

MidiReader::TempoReaderImpl::TempoReaderImpl(MidiFile const & file)
	: timeFormat_(file.getTimeFormat())
	, current_(0)
{
	file.findAllTempoEvents(sequence_);
	count_ = sequence_.getNumEvents();
}

bool
MidiReader::TempoReaderImpl::NextEvent(cf::ScoreFollower::score_time_t & timestamp, tempo_t & data)
{
	assert(current_ < count_);
	
	seconds_t seconds(sequence_.getEventTime(current_));
	timestamp = time::duration_cast<score_time_t>(seconds);
	
	auto ePtr = sequence_.getEventPointer(current_);
	double dqDuration = ePtr->message.getTempoSecondsPerQuarterNote();
	seconds_t qDuration(dqDuration);
	data = time::duration_cast<tempo_t>(qDuration);

	++current_;
	return current_ < count_;
}

MidiReader::TrackReaderImpl::TrackReaderImpl(MidiMessageSequence const & sequence)
	: sequence_(sequence)
	, count_(sequence.getNumEvents())
	, current_(0)
{}

bool
MidiReader::TrackReaderImpl::NextEvent(cf::ScoreFollower::score_time_t & timestamp, sf::ScoreEventPtr & data)
{
	assert(current_ < count_);
	
	seconds_t seconds(sequence_.getEventTime(current_));
	timestamp = time::duration_cast<score_time_t>(seconds);

	seconds_t offSeconds(sequence_.getTimeOfMatchingKeyUp(current_));
	auto offTimestamp = time::duration_cast<score_time_t>(offSeconds);
	
	score_time_t noteLength = score_time_t::zero();
	if (offTimestamp > score_time_t::zero()) {
		noteLength = offTimestamp - timestamp;
	}

	auto eventPtr = sequence_.getEventPointer(current_);
	data = boost::make_shared<MidiEvent>(eventPtr->message, noteLength);

	++current_;
	return current_ < count_;
}