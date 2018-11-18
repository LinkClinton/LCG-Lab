#include "OccupancyHistogramTree.hpp"

#include "Helper.hpp"

#include <queue>

OccupancyHistogramNode::OccupancyHistogramNode()
{
	FrontOrder = 0;
	BackOrder = 0;
	Parent = nullptr;
	Type = OccupancyType::Unknown;

	memset(Chilren, 0, sizeof(Chilren));
	memset(OccupancyTypeCount, 0, sizeof(OccupancyTypeCount));
}

OccupancyHistogramNode::OccupancyHistogramNode(OccupancyHistogramNode * parent, SpaceOrder order) :
	OccupancyHistogramNode()
{
	AxiallyAlignedBoundingBox = Helper::divideAxiallyAlignedBoundingBox(parent->AxiallyAlignedBoundingBox, order);

	Parent = parent;
}

void OccupancyHistogramNode::update()
{
	if (isLeaf() == false) {

		memset(OccupancyTypeCount, 0, sizeof(OccupancyTypeCount));

		for (int type = 0; type < (int)OccupancyType::Count; type++) {
			for (int order = 0; order < (int)SpaceOrder::Count; order++) {
				OccupancyTypeCount[type] += getTypeCount(Chilren[order], (OccupancyType)type);
			}
		}
	}

	Type = getOccupancyType();
}

bool OccupancyHistogramNode::isLeaf()
{
	for (int i = 0; i < (int)SpaceOrder::Count; i++)
		if (Chilren[i] != nullptr) return false;

	return true;
}

auto OccupancyHistogramNode::getOccupancyType() -> OccupancyType
{
	int type = 0;
	int maxCount = 0;

	for (int i = 0; i < (int)OccupancyType::Count; i++) {
		if (maxCount <= OccupancyTypeCount[i]) {
			type = i;
			maxCount = OccupancyTypeCount[i];
		}
	}

	return (OccupancyType)type;
}

int OccupancyHistogramNode::getTypeCount(OccupancyHistogramNode * node, OccupancyType type)
{
	if (node == nullptr) return 0;

	return node->OccupancyTypeCount[(int)type];
}

auto OccupancyHistogramTree::getOccupancyHistogramNode(OccupancyHistogramNode * parent, SpaceOrder order) -> OccupancyHistogramNode *
{
	mNodeCount++;

	return new OccupancyHistogramNode(parent, order);
}

void OccupancyHistogramTree::insert(OccupancyHistogramNode * node, const glm::vec3 &position, OccupancyType type, int depth)
{
	if (depth >= mMaxDepth) {
		node->OccupancyTypeCount[(int)type]++;

		node->update();

		return;
	}

	auto order = Helper::getSpaceOrder(node->AxiallyAlignedBoundingBox, position);

	if (node->Chilren[(int)order] == nullptr) node->Chilren[(int)order] =
		getOccupancyHistogramNode(node, order);

	insert(node->Chilren[(int)order], position, type, depth + 1);

	node->update();
}

void OccupancyHistogramTree::setEyePosition(OccupancyHistogramNode * node, const glm::vec3 &eyePosition, int & travelTimes)
{
	node->FrontOrder = travelTimes++;

	std::vector<SpaceOrder> accessOrder(8);

	Helper::getAccessOrder(node->AxiallyAlignedBoundingBox, eyePosition, accessOrder);

	for (size_t i = 0; i < accessOrder.size(); i++) {
		if (node->Chilren[(int)accessOrder[i]] == nullptr) continue;

		setEyePosition(node->Chilren[(int)accessOrder[i]], eyePosition, travelTimes);
	}

	node->BackOrder = travelTimes++;
}

void OccupancyHistogramTree::setSize(const AxiallyAlignedBoundingBox &box)
{
	mRoot.AxiallyAlignedBoundingBox = box;
}

void OccupancyHistogramTree::setMaxDepth(int maxDepth)
{
	mMaxDepth = maxDepth;
}

void OccupancyHistogramTree::setEyePosition(const glm::vec3 & eyePosition)
{
	int travelTimes = 0;

	setEyePosition(&mRoot, eyePosition, travelTimes);
}

void OccupancyHistogramTree::insertNoEmpty(const glm::vec3 &position, OccupancyType type)
{
	insert(&mRoot, position, type, 1);
}

void OccupancyHistogramTree::getOccupancyGeometry(std::vector<OccupancyHistogramNodeCompareComponent>& geometry, bool sort)
{
	geometry.push_back(OccupancyHistogramNodeCompareComponent(&mRoot, true, mRoot.FrontOrder));
	geometry.push_back(OccupancyHistogramNodeCompareComponent(&mRoot, false, mRoot.BackOrder));

	std::queue<OccupancyHistogramNode*> queue;

	queue.push(&mRoot);

	while (queue.empty() == false) {
		auto node = queue.front(); queue.pop();

		for (int i = 0; i < (int)SpaceOrder::Count; i++) {
			if (node->Chilren[i] == nullptr) continue;

			if (node->Type != node->Chilren[i]->Type) 
			{
				geometry.push_back(OccupancyHistogramNodeCompareComponent(node->Chilren[i], true, node->Chilren[i]->FrontOrder));
				geometry.push_back(OccupancyHistogramNodeCompareComponent(node->Chilren[i], false, node->Chilren[i]->BackOrder));
			}

			queue.push(node->Chilren[i]);
		}
	}

	if (sort == false) return;

	std::sort(geometry.begin(), geometry.end(), OccupancyHistogramNodeCompareComponent::Compare);
}

