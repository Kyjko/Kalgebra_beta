#ifndef _MATRIX_H
#define _MATRIX_H

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <ostream>

#include "MatrixError.hpp"

#define STACK_TRESHOLD 256
#define FLAG
#define ONLYSQUARE

template<typename T> class Matrix;
template<typename T> class Vector;

template<typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& m);

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vector<T>& v);

template <typename T>
class Matrix {
private:
	// dimensions
	uint32_t n, m;
	T stack_data[STACK_TRESHOLD];
	T* heap_data;
	FLAG bool is_heap;
public:
	Matrix(T* entries, uint32_t n, uint32_t m) :
		heap_data(nullptr), n(n), m(m), is_heap(false)
	{
		// decide between allocating on stack or heap
		// if total number of elements exceeds a predefined treshold, allocate on heap, otherwise on stack
		if (SAFE_UINT(n * m) <= STACK_TRESHOLD) {
			// allocate on stack
			memcpy(stack_data, entries, SAFE_UINT(n * m * sizeof(T)));
		}
		else {
			// allocate on heap
			is_heap = true;
			heap_data = new T[SAFE_UINT(n * m)];
			memcpy(heap_data, entries, SAFE_UINT(n * m * sizeof(T)));
		}
	}

	Matrix(const Matrix& other) : n(other.n), m(other.m), is_heap(other.is_heap) {
		if (is_heap) {
			ALLOC_TRY(heap_data = new T[SAFE_UINT(n * m)]);
			memcpy(heap_data, other.heap_data, SAFE_UINT(n * m * sizeof(T)));
			memset(stack_data, 0, sizeof(stack_data));
		}
		else {
			memcpy(stack_data, other.stack_data, SAFE_UINT(n * m * sizeof(T)));
		}
	}

	Matrix(Matrix&& other) noexcept : n(std::move(other.n)), m(std::move(other.m)),
		is_heap(std::move(other.is_heap)) {

		if (is_heap) {
			heap_data = other.heap_data;
			other.heap_data = nullptr;

		}
		else {
			memcpy(stack_data, other.stack_data, SAFE_UINT(n * m * sizeof(T)));
			other.stack_data = nullptr;
		}
	}

	Matrix& operator=(const Matrix<T>& other) {
		if (this != &other) {
			n = other.n;
			m = other.m;
			if (is_heap) {
				delete[] heap_data;
				heap_data = new T[SAFE_UINT(n * m)];
				memcpy(heap_data, other.heap_data, SAFE_UINT(n * m * sizeof(T)));
			}
			else {

				memcpy(stack_data, other.stack_data, SAFE_UINT(n * m * sizeof(T)));
			}
		}

		return *this;
	}

	constexpr Matrix& operator+=(const Matrix<T>& other) {
		if (n != other.n || m != other.m) {
			return *this;
		}
		if (is_heap && other.is_heap) {
			for (size_t i = 0; i < n; ++i) {
				for (size_t j = 0; j < m; ++j) {
					DEREF_TRY(heap_data[SAFE_UINT(i * m + j)] += other.heap_data[SAFE_UINT(i * m + j)]);
				}
			}
		}
		else if (!is_heap && !other.is_heap) {
			for (size_t i = 0; i < n; ++i) {
				for (size_t j = 0; j < m; ++j) {
					DEREF_TRY(stack_data[SAFE_UINT(i * m + j)] += other.stack_data[SAFE_UINT(i * m + j)]);
				}
			}
		}

		return *this;
	}

	constexpr Matrix& operator-=(const Matrix<T>& other) {
		if (n != other.n || m != other.m) {
			return *this;
		}
		if (is_heap && other.is_heap) {
			for (size_t i = 0; i < n; ++i) {
				for (size_t j = 0; j < m; ++j) {
					DEREF_TRY(heap_data[SAFE_UINT(i * m + j)] -= other.heap_data[SAFE_UINT(i * m + j)]);
				}
			}
		}
		else if (!is_heap && !other.is_heap) {
			for (size_t i = 0; i < n; ++i) {
				for (size_t j = 0; j < m; ++j) {
					DEREF_TRY(stack_data[SAFE_UINT(i * m + j)] -= other.stack_data[SAFE_UINT(i * m + j)]);
				}
			}
		}

		return *this;
	}

	ONLYSQUARE
		constexpr Matrix& operator*=(const Matrix<T>& other) {

		if (is_heap && other.is_heap) {
			T* _tmp_heap_data = nullptr;
			ALLOC_TRY(_tmp_heap_data = new T[SAFE_UINT(n * m)]);


			for (size_t i = 0; i < n; ++i) {
				for (size_t j = 0; j < m; ++j) {
					T part_sum = 0;
					for (size_t k = 0; k = m; ++k) {
						DEREF_TRY(part_sum += heap_data[SAFE_UINT(i * m + k)] * other.heap_data[SAFE_UINT(k * m + j)]);
					}
					G_TRY(_tmp_heap_data[SAFE_UINT(i * m + j)] = part_sum);
				}
			}

			memcpy(heap_data, _tmp_heap_data, SAFE_UINT(n * m * sizeof(T)));
		}
		else if (!is_heap && !other.is_heap) {
			T _tmp_stack_data[STACK_TRESHOLD];

			for (size_t i = 0; i < n; ++i) {
				for (size_t j = 0; j < m; ++j) {
					T part_sum = 0;
					for (size_t k = 0; k < m; ++k) {
						DEREF_TRY(part_sum += stack_data[SAFE_UINT(i * m + k)] * other.stack_data[SAFE_UINT(k * m + j)]);
					}
					G_TRY(_tmp_stack_data[SAFE_UINT(i * m + j)] = part_sum);
				}
			}

			memcpy(stack_data, _tmp_stack_data, SAFE_UINT(n * m * sizeof(T)));
		}

		return *this;

	}

	Matrix& operator=(Matrix<T>&& other) noexcept {
		if (this != &other) {
			n = other.n;
			m = other.m;
			if (is_heap) {
				delete[] heap_data;
				heap_data = other.heap_data;
				other.heap_data = nullptr;

			}
			else {
				memcpy(stack_data, other.stack_data, SAFE_UINT(n * m * sizeof(T)));
				memset(other.stack_data, 0, SAFE_UINT(n * m * sizeof(T)));
			}
		}

		return *this;
	}

	Vector<T> operator*(const Vector<T>& vec) {
		if (vec.n != m) {
			return vec;
		}
		T ent[STACK_TRESHOLD];
		memset(ent, 0, SAFE_UINT(vec.n * sizeof(T)));

		Vector<T> res(ent, vec.n);


		for (size_t i = 0; i < vec.n; ++i) {
			T partres = 0;
			for (size_t j = 0; j < m; ++j) {
				if (is_heap) {
					DEREF_TRY(partres += vec.stack_data[j] * heap_data[SAFE_UINT(i * m + j)]);
				}
				else {
					DEREF_TRY(partres += vec.stack_data[j] * stack_data[SAFE_UINT(i * m + j)]);
				}
				G_TRY(res.stack_data[i] = partres);
			}
		}

		return res;

	}

	~Matrix() {
		if (is_heap) {
			delete[] heap_data;
		}
		else {
			memset(stack_data, 0, SAFE_UINT(n * m * sizeof(T)));
		}
	}

	friend std::ostream& operator<<<T>(std::ostream& os, const Matrix<T>& m);

	T det() {

		return 0;
	}

};


template<typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& m) {
	os << m.n << "x" << m.m << " matrix" << "\n";
	for (size_t i = 0; i < m.n; ++i) {
		for (size_t j = 0; j < m.m; ++j) {
			if (m.is_heap) {
				os << m.heap_data[i * m.m + j] << " ";
			}
			else {
				os << m.stack_data[i * m.m + j] << " ";
			}
		}
		os << "\n";
	}

	return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vector<T>& v) {
	os << v.n << "x" << "1" << " Vector" << "\n";
	for (size_t i = 0; i < v.n; ++i) {
		os << v.stack_data[i];
		os << "\n";
	}

	return os;
}

template<typename T>
constexpr Matrix<T> operator*(Matrix<T>& a, const Matrix<T>& b) {
	return a *= b;
}

template<typename T>
constexpr Matrix<T> operator+(Matrix<T>& a, const Matrix<T>& b) {
	return a += b;
}

template<typename T>
constexpr Matrix<T> operator-(Matrix<T>& a, const Matrix<T>& b) {
	return a -= b;
}

template<typename T>
class Vector {
	friend class Matrix<T>;
private:
	T stack_data[STACK_TRESHOLD];
	uint32_t n;

public:
	Vector(T* entries, uint32_t _n) : n(_n) {
		memcpy(stack_data, entries, n * sizeof(T));
	}
	~Vector() {
		memset(stack_data, 0, sizeof(T) * n);
	}

	Vector(const Vector& other) : n(other.n) {
		memcpy(stack_data, other.stack_data, n * sizeof(T));
	}

	Vector(Vector&& other) noexcept : n(other.n) {
		memcpy(stack_data, other.stack_data, n * sizeof(T));
		//other.stack_data = nullptr;
	}

	Vector& operator=(const Vector<T>& other) {
		if (this != &other) {
			n = other.n;
			memcpy(stack_data, other.stack_data, n * sizeof(T));
		}

		return *this;
	}

	Vector& operator=(Vector<T>&& other) noexcept {
		if (this != &other) {
			memcpy(stack_data, other.stack_data, n * sizeof(T));
			memset(other.stack_data, 0, n * sizeof(T));
		}

		return *this;
	}

	constexpr Vector& operator+=(const Vector& other) {
		if (n != other.n) {
			return *this;
		}
		for (size_t i = 0; i < n; ++i) {
			stack_data[i] += other.stack_data[i];
		}

		return *this;
	}
	constexpr Vector& operator-=(const Vector& other) {
		if (n != other.n) {
			return *this;
		}
		for (size_t i = 0; i < n; ++i) {
			stack_data[i] -= other.stack_data[i];
		}

		return *this;
	}
	constexpr Vector& operator*=(const Vector& other) {
		if (n != other.n) {
			return *this;
		}
		for (size_t i = 0; i < n; ++i) {
			stack_data[i] *= other.stack_data[i];
		}

		return *this;
	}
	constexpr Vector& operator/=(const Vector& other) {
		if (n != other.n) {
			return *this;
		}
		for (size_t i = 0; i < n; ++i) {
			if (other.stack_data[i] == 0) {
				return *this;
			}
			stack_data[i] /= other.stack_data[i];
		}

		return *this;
	}

	T inner_prod(const Vector<T>& a, const Vector<T>& b) {
		if (a.n != b.n) {
			return 0;
		}
		T res = 0;
		for (size_t i = 0; i < a.n; ++i) {
			res += a[i] * b[i];
		}
	}

	friend std::ostream& operator<<<T>(std::ostream& os, const Vector<T>& v);
};

template<typename T>
constexpr Vector<T> operator*(Vector<T>& a, const Vector<T>& b) {
	return a *= b;
}
template<typename T>
constexpr Vector<T> operator+(Vector<T>& a, const Vector<T>& b) {
	return a += b;
}
template<typename T>
constexpr Vector<T> operator-(Vector<T>& a, const Vector<T>& b) {
	return a -= b;
}
template<typename T>
constexpr Vector<T> operator/(Vector<T>& a, const Vector<T>& b) {
	return a /= b;
}

#endif