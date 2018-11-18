#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include <glm\glm.hpp>

enum class OccupancyType : int {
	Empty = 0,
	NoEmpty = 1,
	Unknown = 2,
	Count = 3
};

// MinX = 0, MaxX = 1, MinY = 0, MaxY = 2, MinZ = 0, MaxZ = 4
enum class SpaceOrder : int {
	MinXMinYMinZ = 0,
	MaxXMinYMinZ = 1,
	MinXMaxYMinZ = 2,
	MaxXMaxYMinZ = 3,
	MinXMinYMaxZ = 4,
	MaxXMinYMaxZ = 5,
	MinXMaxYMaxZ = 6,
	MaxXMaxYMaxZ = 7,
	Count = 8
};

enum class Face : int {
	Front,
	Back
};

struct AxiallyAlignedBoundingBox {
	glm::vec3 Min;
	glm::vec3 Max;

	AxiallyAlignedBoundingBox(
		float minX = 0, float minY = 0, float minZ = 0,
		float maxX = 0, float maxY = 0, float maxZ = 0) :
		Min(minX, minY, minZ),
		Max(maxX, maxY, maxZ) {}

	AxiallyAlignedBoundingBox(glm::vec3 min, glm::vec3 max) :
		Min(min), Max(max) {}
};

struct OccupancyHistogramNode {
	int FrontOrder;
	int BackOrder;
	
	int OccupancyTypeCount[(int)OccupancyType::Count];

	OccupancyType Type;

	OccupancyHistogramNode* Parent;

	OccupancyHistogramNode* Chilren[(int)SpaceOrder::Count];
	
	AxiallyAlignedBoundingBox AxiallyAlignedBoundingBox;

	OccupancyHistogramNode();

	OccupancyHistogramNode(OccupancyHistogramNode* parent, SpaceOrder order);

	void update();

	bool isLeaf();

	auto getOccupancyType() -> OccupancyType;

	static int getTypeCount(OccupancyHistogramNode* node, OccupancyType type);
};

struct OccupancyHistogramNodeCompareComponent {
	OccupancyHistogramNode* Node;

	bool IsFrontFace;

	int CompareValue;

	OccupancyHistogramNodeCompareComponent(OccupancyHistogramNode* node = nullptr, bool isFrontFace = true,
		int compareValue = 0) :
		Node(node), IsFrontFace(isFrontFace), CompareValue(compareValue) {}

	static bool Compare(const OccupancyHistogramNodeCompareComponent &first, const OccupancyHistogramNodeCompareComponent &second) {
		return first.CompareValue < second.CompareValue;
	}
};

class OccupancyHistogramTree {
private:
	OccupancyHistogramNode mRoot;

	int mNodeCount;
	int mMaxDepth;

	auto getOccupancyHistogramNode(OccupancyHistogramNode* parent, SpaceOrder order) -> OccupancyHistogramNode*;

	void insert(OccupancyHistogramNode* node, const glm::vec3 &position, OccupancyType type ,int depth);

	void setEyePosition(OccupancyHistogramNode* node, const glm::vec3 &eyePosition, int &travelTimes);
public:
	OccupancyHistogramTree() :
		mMaxDepth(0), mNodeCount(0), mRoot() {}

	void setSize(const AxiallyAlignedBoundingBox &box);

	void setMaxDepth(int maxDepth);

	void setEyePosition(const glm::vec3 &eyePosition);

	void insertNoEmpty(const glm::vec3 &position, OccupancyType type);

	void getOccupancyGeometry(std::vector<OccupancyHistogramNodeCompareComponent> &geometry, bool sort = true);
	
};