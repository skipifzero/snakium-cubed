#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "sfz/util/BlockingQueue.hpp"

#include <thread>
#include <chrono>
#include <vector>

TEST_CASE("Pushing and popping elements", "[sfz::BlockingQueue]")
{
	sfz::BlockingQueue<int> blockingQueue;
	SECTION("empty() returns correct state") {
		REQUIRE(blockingQueue.empty());
		blockingQueue.push(10);
		REQUIRE(!blockingQueue.empty());
	}
	SECTION("Empty queue returns empty unique_ptr (nullptr)") {
		REQUIRE(blockingQueue.tryPop() == nullptr);
	}
	SECTION("Pop blocks until element is available") {
		std::thread thr{ [&blockingQueue]() {
			REQUIRE(blockingQueue.empty());
			int res = blockingQueue.pop();
			REQUIRE(res == 10);
			REQUIRE(blockingQueue.empty());
		}};

		std::this_thread::sleep_for(std::chrono::milliseconds{250});
		REQUIRE(blockingQueue.empty());
		blockingQueue.push(10);
		thr.join();
	}
}