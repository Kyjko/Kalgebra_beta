#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <ostream>

#define STACK_TRESHOLD 256
#define FLAG
#define ONLYSQUARE

template<typename T> class Matrix;

template<typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& m);

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
		if (n * m <= STACK_TRESHOLD) {
			// allocate on stack
			memcpy(stack_data, entries, n * m * sizeof(T));
		}
		else {
			// allocate on heap
			is_heap = true;
			heap_data = new T[n*m];
			memcpy(heap_data, entries, n * m * sizeof(T));
		}
	}

	Matrix(const Matrix& other) : n(other.n), m(other.m), is_heap(other.is_heap) {
		if (is_heap) {
			heap_data = new T[n * m];
			memcpy(heap_data, other.heap_data, n * m * sizeof(T));
			memset(stack_data, 0, sizeof(stack_data));
		}
		else {
			memcpy(stack_data, other.stack_data, n * m * sizeof(T));
			//delete[] heap_data;
		}
	}

	Matrix(Matrix&& other) noexcept : n(std::move(other.n)), m(std::move(other.m)),
		is_heap(std::move(other.is_heap)) {
		
		if (is_heap) {
			heap_data = other.heap_data;
			other.heap_data = nullptr;

		}
		else {
			memcpy(stack_data, other.stack_data, n * m * sizeof(T));
			other.stack_data = nullptr;
		}
	}

	Matrix& operator=(const Matrix<T>& other) {
		if (this != &other) {
			n = other.n;
			m = other.m;
			if (is_heap) {
				delete[] heap_data;
				heap_data = new T[n * m];
				memcpy(heap_data, other.heap_data, n * m * sizeof(T));
			}
			else {
				
				memcpy(stack_data, other.stack_data, n * m * sizeof(T));
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
					heap_data[i * m + j] += other.heap_data[i * m + j];
				}
			}
		}
		else if(!is_heap && !other.is_heap) {
			for (size_t i = 0; i < n; ++i) {
				for (size_t j = 0; j < m; ++j) {
					stack_data[i * m + j] += other.stack_data[i * m + j];
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
					heap_data[i * m + j] -= other.heap_data[i * m + j];
				}
			}
		}
		else if (!is_heap && !other.is_heap) {
			for (size_t i = 0; i < n; ++i) {
				for (size_t j = 0; j < m; ++j) {
					stack_data[i * m + j] -= other.stack_data[i * m + j];
				}
			}
		}

		return *this;
	}

	ONLYSQUARE
	constexpr Matrix& operator*=(const Matrix<T>& other) {

		if (is_heap && other.is_heap) {
			T *_tmp_heap_data = new T[n * m];
			

			for (size_t i = 0; i < n; ++i) {
				for (size_t j = 0; j < m; ++j) {
					T part_sum = 0;
					for (size_t k = 0; k = m; ++k) {
						part_sum += heap_data[i * m + k] * other.heap_data[k * m + j];
					}
					_tmp_heap_data[i * m + j] = part_sum;
				}
			}

			memcpy(heap_data, _tmp_heap_data, n * m * sizeof(T));
		}
		else if (!is_heap && !other.is_heap) {
			T _tmp_stack_data[STACK_TRESHOLD];

			for (size_t i = 0; i < n; ++i) {
				for (size_t j = 0; j < m; ++j) {
					T part_sum = 0;
					for (size_t k = 0; k < m; ++k) {
						part_sum += stack_data[i * m + k] * other.stack_data[k * m + j];
					}
					_tmp_stack_data[i * m + j] = part_sum;
				}
			}

			memcpy(stack_data, _tmp_stack_data, n * m * sizeof(T));
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
				memcpy(stack_data, other.stack_data, n * m * sizeof(T));
				memset(other.stack_data, 0, n * m * sizeof(T));
			}
		}

		return *this;
	}

	~Matrix() {
		if (is_heap) {
			delete[] heap_data;
		}
		else {
			memset(stack_data, 0, n * m * sizeof(T));
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


int main(void) {
	
	int nums[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9};

	Matrix<int> A(nums, 3, 3);

	Matrix<int> B(nums, 3, 3);
	A *= B;

	std::cout << A;

	Matrix<int> C = A;
	std::cout << C << "\n" << A;

	std::cout << "----------------------------\n";

	int nums2[] = {5, 6, 3, 6, 5, 7, 8, 9, 1};
	int nums3[] = { 50, 60, 30, 60, 50, 70, 80, 90, 10 };
	Matrix<int> M1(nums2, 3, 3);
	Matrix<int> M2(nums3, 3, 3);
	Matrix<int> M3(nums2, 3, 3);
	M1 -= M2;
	M3 = A + M1;
	M3 *= M1;

	std::cout << M1 << M2 << M3;

}