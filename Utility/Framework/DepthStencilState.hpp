#pragma once

#include "ComparisonMode.hpp"

class DepthStencilState {
protected:
	bool mDepthEnable;
	
	ComparisonMode mComparisonMode;

	DepthStencilState() :
		mDepthEnable(true), mComparisonMode(ComparisonMode::Less) {}

public:
	virtual void setDepthEnable(bool enable) = 0;

	virtual void setComparisonMode(ComparisonMode comparisonMode) = 0;

	auto getDepthEnable() -> bool {
		return mDepthEnable;
	}

	auto getComparisonMode() -> ComparisonMode {
		return mComparisonMode;
	}
};