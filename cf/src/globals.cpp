#include "cf/globals.h"

#include <boost/make_shared.hpp>

#include "cf/TlsfAllocator.h"

namespace cf {

// Init statics
boost::atomic<int> Globals::refCount_(0);
boost::shared_ptr<ButlerThread> Globals::butler_;
boost::shared_ptr<FileLogger> Globals::logger_;
boost::shared_ptr<Globals::ErrorBuffer> Globals::errorBuffer_;

void Globals::Ref()
{
	if (refCount_.fetch_add(1) > 0) { return; }

	// Init pool allocator
	TlsfPool::Init(16 * 1024);

	butler_ = boost::make_shared<ButlerThread>(milliseconds_t(50));
	logger_ = boost::make_shared<FileLogger>("ScoreFollower.log", butler_);
	errorBuffer_ = boost::make_shared<ErrorBuffer>(16);

	LOG("Initialized!");
}

void Globals::Unref()
{
	if (refCount_.fetch_sub(1) > 1) { return; }

	logger_.reset();
	butler_.reset();
	errorBuffer_.reset();

	// Free allocator memory
	TlsfPool::Destroy();
}

} // namespace cf
