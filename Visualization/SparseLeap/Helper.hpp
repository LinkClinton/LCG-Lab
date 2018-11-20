#pragma once

#include <algorithm>
#include <fstream>
#include <vector>

#include <WindowsFramework.hpp>

#include "OccupancyHistogramTree.hpp"

struct InstanceData {
	glm::mat4x4 Transform;
	int Setting[4];
};

class Helper {
private:

	struct SpaceCompareComponent {
		float Distance;
		SpaceOrder SpaceOrderType;

		static bool Compare(const SpaceCompareComponent &first, const SpaceCompareComponent &second) {
			return first.Distance < second.Distance;
		}
	};

public:
	static AxiallyAlignedBoundingBox divideAxiallyAlignedBoundingBox(const AxiallyAlignedBoundingBox &box, SpaceOrder order) {
		float halfX = (box.Max.x + box.Min.x) * 0.5f;
		float halfY = (box.Max.y + box.Min.y) * 0.5f;
		float halfZ = (box.Max.z + box.Min.z) * 0.5f;

		switch (order)
		{
		case SpaceOrder::MinXMinYMinZ:
			return AxiallyAlignedBoundingBox(box.Min.x, box.Min.y, box.Min.z, halfX, halfY, halfZ);
		case SpaceOrder::MinXMinYMaxZ:
			return AxiallyAlignedBoundingBox(box.Min.x, box.Min.y, halfZ, halfX, halfY, box.Max.z);
		case SpaceOrder::MinXMaxYMinZ:
			return AxiallyAlignedBoundingBox(box.Min.x, halfY, box.Min.z, halfX, box.Max.y, halfZ);
		case SpaceOrder::MinXMaxYMaxZ:
			return AxiallyAlignedBoundingBox(box.Min.x, halfY, halfZ, halfX, box.Max.y, box.Max.z);
		case SpaceOrder::MaxXMinYMinZ:
			return AxiallyAlignedBoundingBox(halfX, box.Min.y, box.Min.z, box.Max.x, halfY, halfZ);
		case SpaceOrder::MaxXMinYMaxZ:
			return AxiallyAlignedBoundingBox(halfX, box.Min.y, halfZ, box.Max.x, halfY, box.Max.z);
		case SpaceOrder::MaxXMaxYMinZ:
			return AxiallyAlignedBoundingBox(halfX, halfY, box.Min.z, box.Max.x, box.Max.y, halfZ);
		case SpaceOrder::MaxXMaxYMaxZ:
			return AxiallyAlignedBoundingBox(halfX, halfY, halfZ, box.Max.x, box.Max.y, box.Max.z);
			break;
		}

		return AxiallyAlignedBoundingBox();
	}

	static float getDistance(const glm::vec3 point1, const glm::vec3 point2) {
		return sqrt(point1.x * point2.x + point1.y * point2.y + point1.z * point2.z);
	}

	static SpaceOrder getSpaceOrder(const AxiallyAlignedBoundingBox &box, const glm::vec3 &position) {
		int xOrder = 0;
		int yOrder = 0;
		int zOrder = 0;

		glm::vec3 half = (box.Max + box.Min) * 0.5f;

		if (position.x > half.x) xOrder = 1;
		if (position.y > half.y) yOrder = 1;
		if (position.z > half.z) zOrder = 1;

		int result = (zOrder << 2) + (yOrder << 1) + xOrder;

		return (SpaceOrder)result;
	}

	static SpaceOrder getSpaceOrder(const AxiallyAlignedBoundingBox &parent, const AxiallyAlignedBoundingBox &children) {
		int xOrder = 0;
		int yOrder = 0;
		int zOrder = 0;

		glm::vec3 half = (parent.Max + parent.Min) * 0.5f;

		if (children.Min.x >= half.x && children.Max.x > half.x) xOrder = 1;
		if (children.Min.y >= half.y && children.Max.y > half.y) yOrder = 1;
		if (children.Min.z >= half.z && children.Max.z > half.z) zOrder = 1;

		int result = (zOrder << 2) + (yOrder << 1) + xOrder;

		return (SpaceOrder)result;
	}

	static auto getSpaceCenter(const AxiallyAlignedBoundingBox &box, SpaceOrder order) -> const glm::vec3 {
		AxiallyAlignedBoundingBox spaceBox;

		glm::vec3 half = (box.Max + box.Min) * 0.5f;

		bool isMaxX = ((int(order) & 1) != 0);
		bool isMaxY = ((int(order) & 2) != 0);
		bool isMaxZ = ((int(order) & 4) != 0);

		spaceBox.Min.x = isMaxX ? half.x : box.Min.x;
		spaceBox.Min.y = isMaxY ? half.y : box.Min.y;
		spaceBox.Min.z = isMaxZ ? half.z : box.Min.z;

		spaceBox.Max.x = isMaxX ? box.Max.x : half.x;
		spaceBox.Max.y = isMaxY ? box.Max.y : half.y;
		spaceBox.Max.z = isMaxZ ? box.Max.z : half.z;

		return (spaceBox.Max + spaceBox.Min) * 0.5f;
	}

	static void getAccessOrder(const AxiallyAlignedBoundingBox &box, const glm::vec3 &eyePosition, std::vector<SpaceOrder> &accessOrder) {
		std::vector<SpaceCompareComponent> spaceCompareComponent(accessOrder.size());

		for (size_t i = 0; i < spaceCompareComponent.size(); i++) {
			spaceCompareComponent[i].SpaceOrderType = (SpaceOrder)i;
			spaceCompareComponent[i].Distance = glm::distance(eyePosition, getSpaceCenter(box, (SpaceOrder)i));
		}

		std::sort(spaceCompareComponent.begin(), spaceCompareComponent.end(), SpaceCompareComponent::Compare);

		for (size_t i = 0; i < spaceCompareComponent.size(); i++) 
			accessOrder[i] = spaceCompareComponent[i].SpaceOrderType;
	}

	static auto ReadFile(const std::string &fileName) -> std::vector<codebyte> {
		std::ifstream file(fileName, std::ios::binary | std::ios::ate);

		auto fileSize = (size_t)file.tellg();

		std::vector<codebyte> fileCode(fileSize);

		file.seekg(0, std::ios::beg);
		file.read((char*)&fileCode[0], fileSize);
		file.close();

		return fileCode;
	}
};