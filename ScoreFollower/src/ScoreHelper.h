#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/utility.hpp>

#include "cf/EventBuffer.h"
#include "Data/InstrumentParser.h"
#include "Data/Score.h"
#include "ScoreFollower/types.h"
#include "ScoreFollower/ScoreEvent.h"

#include "InstrumentPatchSwitcher.h"

namespace cf {
namespace ScoreFollower {

class ScoreReader;
class TimeHelper;

class ScoreHelper : public boost::noncopyable
{
	typedef EventBuffer<ScoreEventPtr, score_time_t, std::vector> TrackBuffer;

public:
	ScoreHelper(boost::shared_ptr<TimeHelper> timeHelper);

	void LearnScore(boost::shared_ptr<ScoreReader> scoreReader);
	void LearnInstruments(Data::InstrumentMap const & instruments, Data::TrackList const & tracks);

	void SetVelocityFromMotion(double velocity) { velocity_ = velocity; }

	void GetTrackEventsForBlock(unsigned track, Follower::BlockBuffer & events);

private:
	void CopyEventToBuffer(score_time_t const & time, ScoreEventPtr data,
		Follower::BlockBuffer & events, InstrumentPatchSwitcher & patchSwitcher) const;

	double NewVelocityAt(double oldVelocity, score_time_t const & time) const;

private:
	boost::shared_ptr<TimeHelper> timeHelper_;
	
	// Keep reference to scoreReader, so that the events are valid
	boost::shared_ptr<ScoreReader> scoreReader_;

	boost::ptr_vector<TrackBuffer> trackBuffers_;
	boost::ptr_vector<InstrumentPatchSwitcher> trackInstruments_;

	double velocity_;
};

} // namespace ScoreFollower
} // namespace cf