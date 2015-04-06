#include "sfz/util/ScopedThread.hpp"

namespace sfz {

ScopedThread::ScopedThread(std::thread t)
:
	mThread{std::move(t)}
{
	sfz_assert_debug(mThread.joinable());
}

ScopedThread::~ScopedThread()
{
	mThread.join();
}

std::thread::id ScopedThread::getID() const
{
	return mThread.get_id();
}

} // namespace sfz