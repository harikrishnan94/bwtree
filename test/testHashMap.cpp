#include "indexes/hashtable/concurrent_map.h"
#include "sha512.h"
#include "testConcurrentMapUtils.h"

#include <absl/hash/hash.h>
#include <catch.hpp>

#include <limits>
#include <random>
#include <string>
#include <unordered_map>

TEST_CASE("HashMapBasic", "[hashtable]")
{
	indexes::hashtable::concurrent_map<int, int, absl::Hash<uint64_t>> map;

	int num_keys = 1000 * 1000;

	std::uniform_int_distribution<int> udist{ 1, num_keys };
	std::random_device r;
	std::seed_seq seed{ r(), r(), r(), r(), r(), r(), r(), r() };
	std::mt19937 rnd(seed);

	std::unordered_map<int, int> key_values;

	for (int i = 0; i < num_keys; i++)
	{
		int key = udist(rnd);

		map.Upsert(key, i);
		key_values[key] = i;
	}

	REQUIRE(map.size() == key_values.size());

	for (const auto &kv : key_values)
	{
		REQUIRE(*map.Delete(kv.first) == kv.second);
		REQUIRE(map.Insert(kv.first, kv.second) == true);
	}

	REQUIRE(map.size() == key_values.size());

	for (const auto &kv : key_values)
	{
		REQUIRE(*map.Delete(kv.first) == kv.second);
	}
}

TEST_CASE("HashMapString", "[hashtable]")
{
	indexes::hashtable::concurrent_map<std::string, int, absl::Hash<std::string>> map;

	int num_keys = 1000000;

	std::uniform_int_distribution<int> udist;
	std::random_device r;
	std::seed_seq seed{ r(), r(), r(), r(), r(), r(), r(), r() };
	std::mt19937 rnd(seed);

	std::map<std::string, int> key_values;

	for (int i = 0; i < num_keys; i++)
	{
		std::string key = sha512(std::to_string(udist(rnd)));

		map.Upsert(key, i);
		key_values[key] = i;
	}

	REQUIRE(map.size() == key_values.size());

	for (const auto &kv : key_values)
	{
		REQUIRE(*map.Delete(kv.first) == kv.second);
		REQUIRE(map.Insert(kv.first, kv.second) == true);
	}

	REQUIRE(map.size() == key_values.size());

	for (const auto &kv : key_values)
	{
		REQUIRE(*map.Delete(kv.first) == kv.second);
	}
}

TEST_CASE("HashMapConcurrentMapMixed", "[hashtable]")
{
	MixedMapTest<indexes::hashtable::concurrent_map<int, int, absl::Hash<int>>>();
}

TEST_CASE("HashMapConcurrentMapConcurrencyRandom", "[hashtable]")
{
	ConcurrentMapTest<indexes::hashtable::concurrent_map<uint64_t, uint64_t, absl::Hash<uint64_t>>>(
	    ConcurrentMapTestWorkload::WL_RANDOM);
}

TEST_CASE("HashMapConcurrentMapConcurrencyContented", "[hashtable]")
{
	ConcurrentMapTest<indexes::hashtable::concurrent_map<uint64_t, uint64_t, absl::Hash<uint64_t>>>(
	    ConcurrentMapTestWorkload::WL_CONTENTED);
}
