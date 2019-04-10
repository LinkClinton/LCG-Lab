#pragma once

#include <glm/glm.hpp>

#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <vector>

#include <WindowsFramework.hpp>

#include "OccupancyHistogramTree.hpp"

#undef min

template<typename T>
struct Vector3 {
	T X, Y, Z;

	Vector3() :
		X(0), Y(0), Z(0) {}

	Vector3(T x, T y, T z) :
		X(x), Y(y), Z(z) {}

	Vector3(T xyz) : Vector3(xyz, xyz, xyz) {}

	bool operator == (const Vector3 &vector)const {
		return (X == vector.X) && (Y == vector.Y) && (Z == vector.Z);
	}

	struct HashFunction {

		std::size_t operator()(const Vector3 &key) const {
			return ((std::hash<int>()(key.X)
				^ (std::hash<int>()(key.Y) << 1)) >> 1)
				^ (std::hash<int>()(key.Z) << 1);
		}

	};
};

typedef Vector3<int> Size;
typedef Vector3<int> VirtualAddress;
typedef unsigned char byte;

enum class PageState {
	Mapped = 1,
	UnMapped = 0,
	Empty = 2
};

struct VirtualLink {
	VirtualAddress Address;
	VirtualAddress FromAddress;
	PageState State;

	explicit VirtualLink(const VirtualAddress &address = VirtualAddress(),
		const VirtualAddress &fromAddress = VirtualAddress(),
		PageState state = PageState::UnMapped) : Address(address), FromAddress(fromAddress), State(state) {}
};

struct MatrixStructure {
	glm::mat4 WorldTransform;
	glm::mat4 CameraTransform;
	glm::mat4 ProjectTransform;
	glm::mat4 RenderConfig;

	MatrixStructure() : WorldTransform(1),
		CameraTransform(1), ProjectTransform(1), RenderConfig(0) {}
};

struct UInt4 {
	unsigned int X, Y, Z, W;

	UInt4(unsigned int x, unsigned int y, unsigned int z, unsigned int w) :
		X(x), Y(y), Z(z), W(w) {}

	UInt4(unsigned int value = 0) :
		X(value), Y(value), Z(value), W(value) {}

	UInt4(const Vector3<int> &value) :
		X(value.X), Y(value.Y), Z(value.Z), W(0) {}

	template<typename T>
	static auto fromVector(const std::vector<T> value) -> std::vector<UInt4> {
		std::vector<UInt4> result;

		for (auto x : value) result.push_back(UInt4(x));

		return result;
	}
};

class Helper {
private:
	/**
	 * @brief component about sort the access order
	 */
	struct SpaceCompareComponent {
		float Distance;
		SpaceOrder SpaceOrderType;

		static bool compare(const SpaceCompareComponent& first, const SpaceCompareComponent& second) {
			return first.Distance < second.Distance;
		}
	};
public:
	template<typename T>
	static auto multiple(const Vector3<T> &left, const Vector3<T> &right) -> Vector3<T> {
		return Vector3<T>(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
	}

	template<typename T>
	static auto multiple(const Vector3<T> &left, const glm::vec3 &right) -> Vector3<T> {
		return Vector3<T>(
			T(left.X * right.x),
			T(left.Y * right.y),
			T(left.Z * right.z));
	}

	template<typename T>
	static auto div(const Vector3<T> &left, const Vector3<T> &right) -> Vector3<T> {
		return Vector3<T>(
			left.X / right.X,
			left.Y / right.Y,
			left.Z / right.Z);
	}

	template<typename T>
	static auto divToFloat(const Vector3<T>& left, const Vector3<T>& right) -> glm::vec3 {
		return glm::vec3(
			float(left.X) / right.X,
			float(left.Y) / right.Y,
			float(left.Z) / right.Z);
	}

	template<typename T>
	static auto add(const Vector3<T> &left, const Vector3<T> &right) -> Vector3<T> {
		return Vector3<T>(
			left.X + right.X,
			left.Y + right.Y,
			left.Z + right.Z);
	}

	template<typename T>
	static auto min(const T &x, const T &y) -> T {
		return x < y ? x : y;
	}

	/**
	 * @brief make sub-AxiallyAlignedBoundingBox by space order
	 */
	static AxiallyAlignedBoundingBox divideAxiallyAlignedBoundingBox(const AxiallyAlignedBoundingBox& box, SpaceOrder order) {
		const float halfX = (box.Max.x + box.Min.x) * 0.5f;
		const float halfY = (box.Max.y + box.Min.y) * 0.5f;
		const float halfZ = (box.Max.z + box.Min.z) * 0.5f;

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
		default:
			throw std::runtime_error("not support order.");
		}
	}

	/**
	 * @brief get distance from p1 to p2
	 */
	static float getDistance(const glm::vec3 point1, const glm::vec3 point2) {
		return sqrt(point1.x * point2.x + point1.y * point2.y + point1.z * point2.z);
	}

	/**
	 * @brief get the space order(which space order that the position is)
	 */
	static SpaceOrder getSpaceOrder(const AxiallyAlignedBoundingBox & box, const glm::vec3 & position) {
		auto xOrder = 0;
		auto yOrder = 0;
		auto zOrder = 0;

		const auto half = (box.Max + box.Min) * 0.5f;

		if (position.x > half.x) xOrder = 1;
		if (position.y > half.y) yOrder = 1;
		if (position.z > half.z) zOrder = 1;

		//combine the sub propetry
		auto result = (zOrder << 2) + (yOrder << 1) + xOrder;

		return SpaceOrder(result);
	}

	/**
	 * @brief get the space order(aabb version)
	 */
	static SpaceOrder getSpaceOrder(const AxiallyAlignedBoundingBox & parent, const AxiallyAlignedBoundingBox & children) {
		auto xOrder = 0;
		auto yOrder = 0;
		auto zOrder = 0;

		const auto half = (parent.Max + parent.Min) * 0.5f;

		if (children.Min.x >= half.x && children.Max.x > half.x) xOrder = 1;
		if (children.Min.y >= half.y && children.Max.y > half.y) yOrder = 1;
		if (children.Min.z >= half.z && children.Max.z > half.z) zOrder = 1;

		auto result = (zOrder << 2) + (yOrder << 1) + xOrder;

		return SpaceOrder(result);
	}

	/**
	 * @brief does parent contain the children
	 */
	static bool isContain(const AxiallyAlignedBoundingBox & parent, const AxiallyAlignedBoundingBox & children) {
		if (children.Min.x < parent.Min.x) return false;
		if (children.Min.y < parent.Min.y) return false;
		if (children.Min.z < parent.Min.z) return false;

		if (children.Max.x > parent.Max.x) return false;
		if (children.Max.y > parent.Max.y) return false;
		if (children.Max.z > parent.Max.z) return false;

		return true;
	}

	/**
	 * @brief get the space center
	 */
	static auto getSpaceCenter(const AxiallyAlignedBoundingBox & box, SpaceOrder order) -> const glm::vec3 {
		AxiallyAlignedBoundingBox spaceBox;

		const auto half = (box.Max + box.Min) * 0.5f;

		const auto isMaxX = ((int(order) & 1) != 0);
		const auto isMaxY = ((int(order) & 2) != 0);
		const auto isMaxZ = ((int(order) & 4) != 0);

		spaceBox.Min.x = isMaxX ? half.x : box.Min.x;
		spaceBox.Min.y = isMaxY ? half.y : box.Min.y;
		spaceBox.Min.z = isMaxZ ? half.z : box.Min.z;

		spaceBox.Max.x = isMaxX ? box.Max.x : half.x;
		spaceBox.Max.y = isMaxY ? box.Max.y : half.y;
		spaceBox.Max.z = isMaxZ ? box.Max.z : half.z;

		return (spaceBox.Max + spaceBox.Min) * 0.5f;
	}

	/**
	 * @brief get the access order by eye position.
	 */
	static void getAccessOrder(const AxiallyAlignedBoundingBox & box, const glm::vec3 & eyePosition, std::vector<SpaceOrder> & accessOrder) {
		std::vector<SpaceCompareComponent> spaceCompareComponent(accessOrder.size());

		//we use the distance from box's center to eye position
		//because some special propetry, we can do this. (need to prove)
		for (size_t i = 0; i < spaceCompareComponent.size(); i++) {
			spaceCompareComponent[i].SpaceOrderType = SpaceOrder(i);
			spaceCompareComponent[i].Distance = glm::distance(eyePosition, getSpaceCenter(box, SpaceOrder(i)));
		}

		std::sort(spaceCompareComponent.begin(), spaceCompareComponent.end(), SpaceCompareComponent::compare);

		for (size_t i = 0; i < spaceCompareComponent.size(); i++)
			accessOrder[i] = spaceCompareComponent[i].SpaceOrderType;
	}

	static auto readFile(const std::string &fileName) -> std::vector<byte> {
		std::ifstream file(fileName, std::ios::binary | std::ios::ate);

		const auto fileSize = size_t(file.tellg());

		std::vector<byte> fileCode(fileSize);

		file.seekg(0, std::ios::beg);
		file.read(reinterpret_cast<char*>(&fileCode[0]), fileSize);
		file.close();

		return fileCode;
	}
};