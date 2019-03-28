#pragma once

#include <glm/glm.hpp>

#include <unordered_map>
#include <fstream>

template<typename T>
struct Vector3 {
	T X, Y, Z;

	Vector3(T x, T y, T z) :
		X(x), Y(y), Z(z) {}

	Vector3(T xyz) : Vector3(xyz, xyz, xyz) {}

	template<typename T = int>
	Vector3(T x = (int)0, T y = (int)0, T z = (int)0) :
		X(x), Y(y), Z(z) {}

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

	VirtualLink(const VirtualAddress &address = VirtualAddress(),
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

	UInt4(Vector3<int> value) :
		X(value.X), Y(value.Y), Z(value.Z), W(0) {}

	template<typename T>
	static auto fromVector(const std::vector<T> value) -> std::vector<UInt4> {
		std::vector<UInt4> result;

		for (auto x : value) result.push_back(UInt4(x));

		return result;
	}
};

class Helper {
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

	static auto readFile(const std::string &fileName) -> std::vector<byte> {
		std::ifstream file(fileName, std::ios::binary | std::ios::ate);

		auto fileSize = (size_t)file.tellg();

		std::vector<byte> fileCode(fileSize);

		file.seekg(0, std::ios::beg);
		file.read((char*)&fileCode[0], fileSize);
		file.close();

		return fileCode;
	}
};