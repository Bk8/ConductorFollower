#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

template<typename TData> class TrackReader;
class ScoreEventHandle;

typedef boost::shared_ptr<TrackReader<ScoreEventHandle> > TrackReaderPtr;
typedef boost::shared_ptr<TrackReader<tempo_t> > TempoReaderPtr;

// Abstraction of reading a score (e.g. midi)
class ScoreReader : public boost::noncopyable
{
public:
	virtual ~ScoreReader()  {}

	virtual int TrackCount() const = 0;

	// The events referenced by the ScoreEventHandles have to be valid
	// as long as the ScoreReader is valid
	virtual TrackReaderPtr Track(int index) = 0;

	virtual TempoReaderPtr TempoTrack() = 0;
	
};

} // namespace ScoreFollower
} // namespace cf
