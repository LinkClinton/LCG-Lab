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