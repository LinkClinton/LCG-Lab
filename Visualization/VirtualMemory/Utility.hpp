#pragma once

template<typename T>
struct Vector3 {
	T X, Y, Z;

	Vector3(T x, T y, T z) :
		X(x), Y(y), Z(z) {}

	template<typename T = int>
	Vector3(T x = 0, T y = 0, T z = 0) :
		X(x), Y(y), Z(z) {}
};

typedef Vector3<int> Size;
typedef Vector3<int> VirtualAddress;
typedef unsigned char byte;