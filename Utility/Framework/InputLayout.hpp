#pragma once

#include <string>
#include <vector>

struct InputLayoutElement {
	int Size;
	int Index;
	std::string Tag;

	InputLayoutElement(const std::string &tag, int size, int index = 0): 
		Tag(tag), Size(size), Index(index) {}
};

class InputLayout {
protected:
	std::vector<InputLayoutElement> mElements;

	InputLayout() = default;

	InputLayout(const std::vector<InputLayoutElement> &elements) :
		mElements(elements) {}
public:
	virtual void addElement(const InputLayoutElement &element) = 0;


};