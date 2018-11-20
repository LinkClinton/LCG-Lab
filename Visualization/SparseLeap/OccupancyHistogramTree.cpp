#include "OccupancyHistogramTree.hpp"

#include "Helper.hpp"

#include <queue>

#undef max

OccupancyHistogramNode::OccupancyHistogramNode()
{
	FrontOrder = 0;
	BackOrder = 0;
	Parent = nullptr;
	Type = OccupancyType::Unknown;

	memset(Children, 0, sizeof(Children));
	memset(OccupancyTypeCount, 0, sizeof(OccupancyTypeCount));
}

OccupancyHistogramNode::OccupancyHistogramNode(OccupancyHistogramNode * parent, SpaceOrder order, int depth) :
	OccupancyHistogramNode()
{
	AxiallyAlignedBoundingBox = Helper::divideAxiallyAlignedBoundingBox(parent->AxiallyAlignedBoundingBox, order);

	Parent = parent;
	Depth = depth;
}

void OccupancyHistogramNode::update()
{
	if (isLeaf() == false) {

		memset(OccupancyTypeCount, 0, sizeof(OccupancyTypeCount));

		for (int type = 0; type < (int)OccupancyType::Count; type++) {
			for (int order = 0; order < (int)SpaceOrder::Count; order++) {
				OccupancyTypeCount[type] += getTypeCount(Children[order], (OccupancyType)type);
			}
		}
	}

	Type = getOccupancyType();
}

bool OccupancyHistogramNode::isLeaf()
{
	for (int i = 0; i < (int)SpaceOrder::Count; i++)
		if (Children[i] != nullptr) return false;

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

auto OccupancyHistogramTree::getOccupancyHistogramNode(OccupancyHistogramNode * parent, SpaceOrder order, int depth) -> OccupancyHistogramNode *
{
	mNodeCount++;

	return new OccupancyHistogramNode(parent, order, depth);
}

auto OccupancyHistogramTree::getVirtualNode(VirtualNode * parent, OccupancyHistogramNode * target) -> VirtualNode *
{
	return new VirtualNode(parent, target);
}

auto OccupancyHistogramTree::getLowestCommonAncestor(OccupancyHistogramNode * nodeu, OccupancyHistogramNode * nodev) -> OccupancyHistogramNode *
{
	//up the node u
	if (nodeu->Depth < nodev->Depth) std::swap(nodeu, nodev);

	while (nodeu->Depth > nodev->Depth) nodeu = nodeu->Parent;

	while (nodeu != nodev) nodeu = nodeu->Parent, nodev = nodev->Parent;

	assert(nodeu != nullptr);

	return nodeu;
}

void OccupancyHistogramTree::insert(OccupancyHistogramNode * node, const glm::vec3 &position, OccupancyType type, int depth)
{
	if (depth >= mMaxDepth) {
		assert(node->OccupancyTypeCount[0] == 0);
		assert(node->OccupancyTypeCount[1] == 0);
		assert(node->OccupancyTypeCount[2] == 0);

		node->OccupancyTypeCount[(int)type]++;

		node->update();

		return;
	}

	auto order = Helper::getSpaceOrder(node->AxiallyAlignedBoundingBox, position);

	if (node->Children[(int)order] == nullptr) node->Children[(int)order] =
		getOccupancyHistogramNode(node, order, depth + 1);

	insert(node->Children[(int)order], position, type, depth + 1);

	node->update();

	//delete node
	int maxOccupancyTypeCount = std::max(node->OccupancyTypeCount[0], 
		std::max(node->OccupancyTypeCount[1], node->OccupancyTypeCount[2]));

	int target = (int)std::pow(8, (mMaxDepth - depth));

	if (maxOccupancyTypeCount == target) {
		for (int i = 0; i < (int)SpaceOrder::Count; i++) {
			Utility::Delete(node->Children[i]);
		}
	}
}

void OccupancyHistogramTree::setEyePosition(OccupancyHistogramNode * node, const glm::vec3 &eyePosition, int & travelTimes)
{
	node->FrontOrder = travelTimes++;

	std::vector<SpaceOrder> accessOrder(8);

	Helper::getAccessOrder(node->AxiallyAlignedBoundingBox, eyePosition, accessOrder);

	for (size_t i = 0; i < accessOrder.size(); i++) {
		if (node->Children[(int)accessOrder[i]] == nullptr) continue;

		setEyePosition(node->Children[(int)accessOrder[i]], eyePosition, travelTimes);
	}

	node->BackOrder = travelTimes++;
}

void OccupancyHistogramTree::buildVirtualTree(OccupancyHistogramNode * node, VirtualNode * virtualNode)
{
	for (int i = 0; i < (int)SpaceOrder::Count; i++) {
		if (node->Children[i] == nullptr) continue;

		if (node->Type != node->Children[i]->Type) {
			SpaceOrder order = Helper::getSpaceOrder(virtualNode->Target->AxiallyAlignedBoundingBox,
				node->Children[i]->AxiallyAlignedBoundingBox);

			if (virtualNode->Children[(int)order] == nullptr) {
				virtualNode->Children[(int)order] = getVirtualNode(virtualNode, node->Children[i]);

				buildVirtualTree(node->Children[i], virtualNode->Children[(int)order]);
			}
			else {
				auto oldChildren = virtualNode->Children[(int)order];
				auto lcaNode = getLowestCommonAncestor(oldChildren->Target, node->Children[i]);

				SpaceOrder order0 = Helper::getSpaceOrder(lcaNode->AxiallyAlignedBoundingBox,
					oldChildren->Target->AxiallyAlignedBoundingBox);
				SpaceOrder order1 = Helper::getSpaceOrder(lcaNode->AxiallyAlignedBoundingBox,
					node->Children[i]->AxiallyAlignedBoundingBox);

				auto newNode = getVirtualNode(virtualNode, lcaNode);

				virtualNode->Children[(int)order] = newNode;

				newNode->Children[(int)order0] = getVirtualNode(newNode, oldChildren->Target);
				newNode->Children[(int)order1] = getVirtualNode(newNode, node->Children[i]);

				memcpy(newNode->Children[(int)order0]->Children, oldChildren->Children, sizeof(oldChildren->Children));

				delete oldChildren;

				buildVirtualTree(node->Children[i], newNode->Children[(int)order1]);
			}
		}
		else buildVirtualTree(node->Children[i], virtualNode);
	}
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
	mVirtualRoot.Target = &mRoot;

	buildVirtualTree(&mRoot, &mVirtualRoot);

	geometry.push_back(OccupancyHistogramNodeCompareComponent(&mRoot, true, mRoot.FrontOrder));
	geometry.push_back(OccupancyHistogramNodeCompareComponent(&mRoot, false, mRoot.BackOrder));

	std::queue<VirtualNode*> queue;

	queue.push(&mVirtualRoot);

	while (queue.empty() == false) {
		auto node = queue.front(); queue.pop();

		for (int i = 0; i < (int)SpaceOrder::Count; i++) {
			if (node->Children[i] == nullptr) continue;

			geometry.push_back(OccupancyHistogramNodeCompareComponent(node->Children[i]->Target, true, node->Children[i]->Target->FrontOrder));
			geometry.push_back(OccupancyHistogramNodeCompareComponent(node->Children[i]->Target, false, node->Children[i]->Target->BackOrder));
			
			queue.push(node->Children[i]);
		}
	}

	if (sort == false) return;

	std::sort(geometry.begin(), geometry.end(), OccupancyHistogramNodeCompareComponent::Compare);
}

VirtualNode::VirtualNode()
{
	Parent = nullptr;
	Target = nullptr;

	memset(Children, 0, sizeof(Children));
}

VirtualNode::VirtualNode(VirtualNode * parent, OccupancyHistogramNode * target)
{
	assert(parent != nullptr);
	assert(target != nullptr);

	Parent = parent;
	Target = target;

	memset(Children, 0, sizeof(Children));
}


