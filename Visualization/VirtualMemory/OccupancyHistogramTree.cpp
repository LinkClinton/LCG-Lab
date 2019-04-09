#include "OccupancyHistogramTree.hpp"

#include "Helper.hpp"

#include <queue>

#undef max

OccupancyHistogramNode::OccupancyHistogramNode()
{
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

		//clear value
		memset(OccupancyTypeCount, 0, sizeof(OccupancyTypeCount));

		//update value
		for (int type = 0; type < (int)OccupancyType::Count; type++) {
			for (int order = 0; order < (int)SpaceOrder::Count; order++) {
				OccupancyTypeCount[type] += getTypeCount(Children[order], (OccupancyType)type);
			}
		}
	}

	//update type
	Type = getOccupancyType();
}

bool OccupancyHistogramNode::isLeaf()
{
	//if no children, it is leaf
	for (int i = 0; i < (int)SpaceOrder::Count; i++)
		if (Children[i] != nullptr) return false;

	return true;
}

auto OccupancyHistogramNode::getOccupancyType() -> OccupancyType
{
	int type = 0;
	int maxCount = 0;

	//find max type count
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

auto OccupancyHistogramTree::getLowestCommonAncestor(OccupancyHistogramNode * nodeu, OccupancyHistogramNode * nodev) -> OccupancyHistogramNode *
{
	//up the node u
	if (nodeu->Depth < nodev->Depth) std::swap(nodeu, nodev);

	//up to same depth
	while (nodeu->Depth > nodev->Depth) nodeu = nodeu->Parent;

	//find lca
	//because the depth is not big(O(1)), so we do not need to use other algorithm
	while (nodeu != nodev) nodeu = nodeu->Parent, nodev = nodev->Parent;

	assert(nodeu != nullptr);

	return nodeu;
}

void OccupancyHistogramTree::insert(OccupancyHistogramNode * node, const glm::vec3 &position, OccupancyType type, int depth)
{
	//if get the end
	if (depth >= mMaxDepth) {
		//only one(debug)
		assert(node->OccupancyTypeCount[0] == 0);
		assert(node->OccupancyTypeCount[1] == 0);
		assert(node->OccupancyTypeCount[2] == 0);

		node->OccupancyTypeCount[(int)type]++;

		node->update();

		return;
	}

	//get order
	auto order = Helper::getSpaceOrder(node->AxiallyAlignedBoundingBox, position);

	//create new node
	if (node->Children[(int)order] == nullptr) node->Children[(int)order] =
		getOccupancyHistogramNode(node, order, depth + 1);

	insert(node->Children[(int)order], position, type, depth + 1);

	node->update();

	//delete node for optimization
	//we can delete the node that all nodes are same
	int maxOccupancyTypeCount = std::max(node->OccupancyTypeCount[0], 
		std::max(node->OccupancyTypeCount[1], node->OccupancyTypeCount[2]));

	int target = (int)std::pow(8, (mMaxDepth - depth));

	//free memory
	//becarefull if we use other allocator
	if (maxOccupancyTypeCount == target) {
		for (int i = 0; i < (int)SpaceOrder::Count; i++) {
			Utility::Delete(node->Children[i]);
		}
	}
}

void OccupancyHistogramTree::update(OccupancyHistogramNode* node, const glm::vec3& position, OccupancyType type, int depth)
{
	//if get the end
	if (depth >= mMaxDepth) {
		std::memset(node->OccupancyTypeCount, 0, sizeof(node->OccupancyTypeCount));

		node->OccupancyTypeCount[(int)type]++;

		node->update();

		return;
	}

	//get order
	auto order = Helper::getSpaceOrder(node->AxiallyAlignedBoundingBox, position);

	//create new node
	if (node->Children[(int)order] == nullptr) node->Children[(int)order] =
		getOccupancyHistogramNode(node, order, depth + 1);

	update(node->Children[(int)order], position, type, depth + 1);

	node->update();

	//delete node for optimization
	//we can delete the node that all nodes are same
	int maxOccupancyTypeCount = std::max(node->OccupancyTypeCount[0],
		std::max(node->OccupancyTypeCount[1], node->OccupancyTypeCount[2]));

	int target = (int)std::pow(8, (mMaxDepth - depth));

	//free memory
	//becarefull if we use other allocator
	if (maxOccupancyTypeCount == target) {
		for (int i = 0; i < (int)SpaceOrder::Count; i++) {
			Utility::Delete(node->Children[i]);
		}
	}
}

auto OccupancyHistogramTree::query(OccupancyHistogramNode* node, const glm::vec3& position, int depth) -> OccupancyType
{
	//do not have the node, we return unkown
	if (node == nullptr) return OccupancyType::Unknown;

	int maxOccupancyTypeCount = std::max(node->OccupancyTypeCount[0],
		std::max(node->OccupancyTypeCount[1], node->OccupancyTypeCount[2]));

	int target = (int)std::pow(8, (mMaxDepth - depth));

	//get leaf or the sub tree's type are same
	if (node->isLeaf() == true || maxOccupancyTypeCount == target) return node->getOccupancyType();

	//get space order
	auto order = Helper::getSpaceOrder(node->AxiallyAlignedBoundingBox, position);

	return query(node->Children[(int)order], position, depth + 1);
}

void OccupancyHistogramTree::setEyePosition(OccupancyHistogramNode* node, const glm::vec3 &eyePosition, int & travelTimes)
{
	//dfs
	node->FrontOrder = travelTimes++;

	std::vector<SpaceOrder> accessOrder(8);

	//get access order
	Helper::getAccessOrder(node->AxiallyAlignedBoundingBox, eyePosition, accessOrder);

	for (size_t i = 0; i < accessOrder.size(); i++) {
		if (node->Children[(int)accessOrder[i]] == nullptr) continue;

		setEyePosition(node->Children[(int)accessOrder[i]], eyePosition, travelTimes);
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

void OccupancyHistogramTree::updateBlock(const glm::vec3& position, OccupancyType type)
{
	update(&mRoot, position, type, 1);
}

auto OccupancyHistogramTree::queryNodeType(const glm::vec3& position) -> OccupancyType
{
	return query(&mRoot, position, 1);
}

auto OccupancyHistogramTree::maxDepth() -> int
{
	return mMaxDepth;
}

void OccupancyHistogramTree::getOccupancyGeometry(std::vector<OccupancyHistogramNodeCompareComponent>& geometry, bool sort)
{
	geometry.push_back(OccupancyHistogramNodeCompareComponent(&mRoot, true, mRoot.FrontOrder));
	geometry.push_back(OccupancyHistogramNodeCompareComponent(&mRoot, false, mRoot.BackOrder));

	std::queue<OccupancyHistogramNode*> queue;

	queue.push(&mRoot);

	//bfs
	while (queue.empty() == false) {
		auto node = queue.front(); queue.pop();

		for (int i = 0; i < (int)SpaceOrder::Count; i++) {
			if (node->Children[i] == nullptr) continue;
			
			//cull the lca node
			if (node->Children[i]->Parent != nullptr &&
				node->Children[i]->Parent->Type != node->Children[i]->Type) {
				geometry.push_back(OccupancyHistogramNodeCompareComponent(node->Children[i], true, node->Children[i]->FrontOrder));
				geometry.push_back(OccupancyHistogramNodeCompareComponent(node->Children[i], false, node->Children[i]->BackOrder));
			}
			
			queue.push(node->Children[i]);
		}
	}

	if (sort == false) return;

	//sort
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

	FrontOrder = 0;
	BackOrder = 0;

	memset(Children, 0, sizeof(Children));
}


