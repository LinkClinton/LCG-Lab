#pragma once

#include <cstddef>
#include <unordered_map>

template<typename T>
struct Vector3 {
	T X, Y, Z;

	Vector3(T x, T y, T z) :
		X(x), Y(y), Z(z) {}

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
	Mapped = 0,
	UnMapped = 1,
	Empty = 2
};

struct VirtualLink {
	VirtualAddress Address;
	PageState State;

	VirtualLink(const VirtualAddress &address = VirtualAddress(),
		PageState state = PageState::UnMapped) : Address(address), State(state) {}
};

class Utility {
public:
	template<typename T>
	static auto multiple(const Vector3<T> &left, const Vector3<T> &right) -> Vector3<T> {
		return Vector3<T>(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
	}

	template<typename T>
	static auto multiple(const Vector3<T> &left, const glm::vec3 &right) -> Vector3<T> {
		return Vector3<T>(
			int(left.X * right.x),
			int(left.Y * right.y),
			int(left.Z * right.z));
	}
};