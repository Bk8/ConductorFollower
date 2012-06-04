#pragma once

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

template<typename TData> class TrackReader;

// Abstraction of reading a score (e.g. midi)
template<typename TData>
class ScoreReader
{
public:
	virtual ~ScoreReader()  {}

	virtual int TrackCount() const = 0;
	virtual TrackReader<TData> * Track(int index) = 0;

	virtual TrackReader<tempo_t> * TempoTrack() = 0;
	
};

} // namespace ScoreFollower
} // namespace cf
