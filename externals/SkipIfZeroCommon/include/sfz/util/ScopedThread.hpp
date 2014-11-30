#pragma once
#ifndef SFZ_UTIL_SCOPED_THREAD_HPP
#define SFZ_UTIL_SCOPED_THREAD_HPP

#include <thread>
#include <cassert>

namespace sfz {

/**
 * @brief A simple wrapper class for std::thread that joins the thread in the destructor.
 * 
 * @author Peter Hillerström <peter@hstroem.se>
 */
class ScopedThread final {
public:

	/**
	 * @brief Constructs a ScopedThread given an std::thread that is joinable.
	 * The specified std::thread is moved into this ScopedThread which then assumes ownership of
	 * the thread.
	 * @assert thread is joinable
	 * @param t the std::thread to transfer to this ScopedThread
	 */
	explicit ScopedThread(std::thread t);

	// No default constructor.
	ScopedThread() = delete;

	// No copy constructor.
	ScopedThread(const ScopedThread&) = delete;
	
	~ScopedThread();

	/**
	 * @brief Returns the id of this thread.
	 * Returns the id given by the internal std::thread.
	 * @return id of this thread
	 */
	std::thread::id getID() const noexcept;

	// No assignment.
	ScopedThread& operator= (const ScopedThread&) = delete;
	
private:
	std::thread mThread;
};

} // namespace sfz

#endif