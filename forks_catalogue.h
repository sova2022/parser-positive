#pragma once

#include <string>
#include <unordered_map>

#include "domain.h" 

class ForksCatalogue {
public:
	using Forks = typename std::unordered_map<int, Fork>;

	ForksCatalogue() {}

private:
	Forks forks_;
};
