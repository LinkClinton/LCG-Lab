#pragma once

#include <WindowsFramework.hpp>

class RenderFramework : public WindowsFramework {
public:
	RenderFramework(const std::string name, int width, int height);
};