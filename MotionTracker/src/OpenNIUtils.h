#pragma once

#include <string>
#include <iostream>

#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/utility.hpp>

#include <XnCppWrapper.h>

namespace cf {
namespace MotionTracker {

#define CheckXnStatus(obj, status, desc) obj.CheckStatus(status, desc, __FILE__, __LINE__)

class OpenNIUtils : public boost::noncopyable
{
public:
	OpenNIUtils() {}

	bool CheckStatus(XnStatus status, char const * taskDescription, char const * file, int line);

	void ResetErrors() { errorsOccurred_ = false; }
	bool ErrorsOccurred() { return errorsOccurred_; }

private:
	bool errorsOccurred_;
};

} // namespace MotionTacker
} // namespace cf
