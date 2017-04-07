#include <cstddef>
#include <cstdlib>
#include <string>
#include <iostream>
#include <chrono>
#include <functional>
#include <vector>
#include <typeinfo>
#include <string>
#include <ctime>
#include <algorithm>

#include "HashMap.h"
#include "TreeMap.h"

template <typename Collection>
class Tests {
private:
	int repeat_count;
	std::chrono::high_resolution_clock::time_point begin, end;
	std::vector<std::pair<std::string, std::function<void()>>> tests;
	std::vector<int> indexes;

	void start()
	{
		begin = std::chrono::high_resolution_clock::now();
	}

	void finish()
	{
		end = std::chrono::high_resolution_clock::now();
	}

	double delta() const
	{
		return std::chrono::duration<double, std::milli>(end - begin).count();
	}

public:
	Tests(int n)
		: repeat_count(n)
		, tests({
			std::make_pair("inserting into empty map", [this]()->void
			{
				Collection collection;
				this->start();
				for (int i = 0; i < this->repeat_count; ++i) {
					collection[this->indexes[i]] = "test";
				}
				this->finish();
			}),
			std::make_pair("removing from non-empty map", [this]()->void
			{
				Collection collection;
				for (int i = 0; i < this->repeat_count; ++i) {
					collection[this->indexes[i]] = "test";
				}
				this->start();
				for (int i = 0; i < this->repeat_count; ++i) {
					collection.remove(i);
				}
				this->finish();
			}),
			std::make_pair("searching for element with given key (n/2 elements)", [this]()->void
			{
				Collection collection;
				for (int i = 0; i < this->repeat_count/2; ++i) {
					collection[this->indexes[i]] = "test";
				}
				this->start();
				for (int i = 0; i < this->repeat_count; ++i) {
					(void)collection.find(i);
				}
				this->finish();
			}),
			std::make_pair("searching for element with given key (n elements)", [this]()->void
			{
				Collection collection;
				for (int i = 0; i < this->repeat_count; ++i) {
					collection[this->indexes[i]] = "test";
				}
				this->start();
				for (int i = 0; i < this->repeat_count; ++i) {
					(void)collection.find(i);
				}
				this->finish();
			}),
			std::make_pair("iterating through map", [this]()->void
			{
				Collection collection;
				for (int i = 0; i < this->repeat_count; ++i) {
					collection[this->indexes[i]] = "test";
				}
				this->start();
				for (auto it = collection.begin(); it != collection.end(); ++it) {
					(void)it;
				}
				this->finish();
			}),

		})
	{
		for (int i = 0; i < repeat_count; ++i) {
			indexes.push_back(i);
		}
		std::random_shuffle(indexes.begin(), indexes.end());
	}

	void runTests()
	{
		std::cout << "=== Running " << typeid(Collection).name() << " tests ===\n";
		for (const auto& test : tests) {
			std::cout << test.first << "...";
			test.second();
			std::cout << " -> "<< delta() << "ms\n";
		}
		std::cout << std::endl;
	}
};

int main(int argc, char** argv)
{
	const int repeat_count = argc > 1 ? std::atoll(argv[1]) : 100000;
	Tests<aisdi::HashMap<int, std::string>> hashmap_tests(repeat_count);
	Tests<aisdi::TreeMap<int, std::string>> treemap_tests(repeat_count);
	hashmap_tests.runTests();
	treemap_tests.runTests();
	return 0;
}
