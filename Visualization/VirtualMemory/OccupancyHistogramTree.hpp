#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include <glm\glm.hpp>

/**
 * @brief occupancy type
 */
enum class OccupancyType : int {
	Empty = 0,
	NoEmpty = 1,
	Unknown = 2,
	Count = 3
};

/* 
 * @brief we can divide the box into eight part, we use this way to define them. 
 * MinX = 0, MaxX = 1, MinY = 0, MaxY = 2, MinZ = 0, MaxZ = 4
 */
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

/**
 * @brief face
 */
enum class Face : int {
	Front,
	Back
};

/**
 * @brief Axially Aligned Bounding Box(Min and Max)
 */
struct AxiallyAlignedBoundingBox {
	glm::vec3 Min; //Min bounding
	glm::vec3 Max; //Max bounding

	AxiallyAlignedBoundingBox(
		float minX = 0, float minY = 0, float minZ = 0,
		float maxX = 0, float maxY = 0, float maxZ = 0) :
		Min(minX, minY, minZ),
		Max(maxX, maxY, maxZ) {}

	AxiallyAlignedBoundingBox(glm::vec3 min, glm::vec3 max) :
		Min(min), Max(max) {}
};

/**
 * @brief OccupancyHistogramTree Node
 */
struct OccupancyHistogramNode {
	int FrontOrder; //front face order
	int BackOrder; //back face order
	int Depth; //the depth
	
	int OccupancyTypeCount[(int)OccupancyType::Count]; //type count

	OccupancyType Type;

	OccupancyHistogramNode* Parent;

	OccupancyHistogramNode* Children[(int)SpaceOrder::Count];
	
	AxiallyAlignedBoundingBox AxiallyAlignedBoundingBox; //define the area that node contained

	OccupancyHistogramNode();

	OccupancyHistogramNode(OccupancyHistogramNode* parent, SpaceOrder order, int depth);

	/**
	 * @brief update information
	 */
	void update(); 

	/*
	 * @brief Is it leaf
	 */
	bool isLeaf();

	/**
	 * @brief get node type
	 */
	auto getOccupancyType() -> OccupancyType;

	static int getTypeCount(OccupancyHistogramNode* node, OccupancyType type);
};

/**
 * @brief virtual tree node(for optimization)
 */
struct VirtualNode {
	int FrontOrder; //front face order
	int BackOrder; //back face order

	VirtualNode* Parent; //parent in virtual tree

	VirtualNode* Children[(int)SpaceOrder::Count]; //chilren in virtual tree

	OccupancyHistogramNode* Target; //node in occupancy hisyogram node

	VirtualNode();

	VirtualNode(VirtualNode* parent, OccupancyHistogramNode* target);
};

/**
 * @brief component for sorting
 */
struct OccupancyHistogramNodeCompareComponent {
	OccupancyHistogramNode* Node;

	bool IsFrontFace; //is front face

	int CompareValue; //key value

	OccupancyHistogramNodeCompareComponent(OccupancyHistogramNode* node = nullptr, bool isFrontFace = true,
		int compareValue = 0) :
		Node(node), IsFrontFace(isFrontFace), CompareValue(compareValue) {}

	static bool Compare(const OccupancyHistogramNodeCompareComponent &first, const OccupancyHistogramNodeCompareComponent &second) {
		return first.CompareValue < second.CompareValue;
	}
};

/**
 * @brief Occupancy Histogram Tree
 */
class OccupancyHistogramTree {
private:
	OccupancyHistogramNode mRoot; //tree root
	
	int mNodeCount;
	int mMaxDepth; //tree's max depth

	/**
	 * @brief get a new node, we can replace it by using our allocator
	 */
	auto getOccupancyHistogramNode(OccupancyHistogramNode* parent, SpaceOrder order, int depth) -> OccupancyHistogramNode*;

	/**
	 * @brief get the lowest common ancestor from node u and node v
	 */
	auto getLowestCommonAncestor(OccupancyHistogramNode* nodeu, OccupancyHistogramNode* nodev) -> OccupancyHistogramNode*;

	/**
	 * @brief insert a empty or no-empty node at position(input)
	 */
	void insert(OccupancyHistogramNode* node, const glm::vec3 &position, OccupancyType type, int depth);

	void update(OccupancyHistogramNode* node, const glm::vec3& position, OccupancyType type, int depth);

	/**
	 * @brief query a node's type(unknown means is not existed)
	 */
	auto query(OccupancyHistogramNode* node, const glm::vec3& position, int depth) -> OccupancyType;

	/**
	 * @brief set the eye position
	 */
	void setEyePosition(OccupancyHistogramNode* node, const glm::vec3 &eyePosition, int &travelTimes);

public:
	OccupancyHistogramTree() :
		mMaxDepth(0), mNodeCount(0), mRoot() {
		mRoot.Depth = 1;
	}

	/**
	 * @brief set the size 
	 */
	void setSize(const AxiallyAlignedBoundingBox &box);

	/**
	 * @brief set the max depth
	 */
	void setMaxDepth(int maxDepth);

	/**
	 * @brief set the eye position
	 */
	void setEyePosition(const glm::vec3 &eyePosition);

	/**
	 * @brief insert empty or no-empty at position
	 */
	void insertNoEmpty(const glm::vec3 &position, OccupancyType type);

	void updateBlock(const glm::vec3& position, OccupancyType type);

	auto queryNodeType(const glm::vec3& position) -> OccupancyType;

	auto maxDepth() -> int;

	/**
	 * @brief get occupancy geometry, some aabb with right order
	 */
	void getOccupancyGeometry(std::vector<OccupancyHistogramNodeCompareComponent> &geometry, bool sort = true);
	
};