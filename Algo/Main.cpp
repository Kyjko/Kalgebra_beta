#include <iostream>
#include <map>
#include <cstdlib>

unsigned int fib(unsigned int n) {
	if (n < 0) return 0;
	if (n == 0 || n == 1) return 1;

	return fib(n - 1) + fib(n - 2);
}

auto memoize(auto fn) {
	return[done = std::map<unsigned int, unsigned int>{}, fn](auto n) mutable {
		if (auto it = done.find(n); it != done.end()) {
			return it->second;
		}
		auto const val = fn(n);
		done[n] = val;
		return val;
	};
}