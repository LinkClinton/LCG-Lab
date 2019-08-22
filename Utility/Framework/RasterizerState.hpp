#pragma once

#include "FillMode.hpp"
#include "CullMode.hpp"

class RasterizerState {
protected:
	FillMode mFillMode;
	CullMode mCullMode;

	RasterizerState() : 
		mFillMode(FillMode::Solid), mCullMode(CullMode::Back) {}

public:
	virtual void setFillMode(FillMode fillMode) = 0;

	virtual void setCullMode(CullMode cullMode) = 0;

	virtual void enableDepth(bool enable) = 0;

	virtual auto getFillMode() -> FillMode const {
		return mFillMode;
	}

	virtual auto getCullMode() -> CullMode const {
		return mCullMode;
	}
};