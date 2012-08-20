#pragma once

#include <string>
#include <vector>

#include "Data/Track.h"

namespace cf {
namespace Data {

typedef std::vector<Track> TrackList;

struct Score
{
	std::string name;
	std::string midiFile;
	std::string instrumentFile;
	std::string beatPatternFile;
	TrackList tracks;
};

} // namespace Data
} // namespace cf
