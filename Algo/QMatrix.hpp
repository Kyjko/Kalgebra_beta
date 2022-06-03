#ifndef _QMATRIX_H
#define _QMATRIX_H

#include "MatrixError.hpp"
#include <stdint.h>
#include <array>
#include <complex>

#define SQUARE

/*
	This implementation allocates matrix data on the heap.
	Supposedly, there won't be too many allocations anyways, 
	therefore sacrificing the syscall-imposed time of mallocs
	lets us treating data dynamically without needing to
	specify predefined stack constraints on the data.

	The algorithms used to calculate matrix arithmetic operations
	such as matrix multiplication use the naive O(n^3) time complexity
	algorithm. In theory, there exist faster ways of multiplying two
	matrices, but the time difference will only be non-neglegible once we
	have very large data to work with. This implementation does not take that
	into consideration.

	
*/

template<typename U>
struct _row {
public:
	_row(U* _a_, uint64_t k) {
		ALLOC_TRY(entries = new U[k]);
		memcpy(entries, _a_, SAFE_UINT(SAFE_UINT(k) * sizeof(U)));
	}
	~_row() {
		delete[] entries;
	}

	U& operator[](const _row<U>& row, size_t idx) {
		return entries[idx];
	}

private:
	U* entries;
};

template<typename T>
class QMatrix {
public:
	QMatrix(const T* entries, uint64_t n, uint64_t m);
	~QMatrix();
	QMatrix(const QMatrix<T>& other);
	QMatrix(QMatrix<T>&& other) noexcept;
	T GetItem(uint64_t i, uint64_t j) const;
	T GetN() const noexcept;
	T GetM() const noexcept;

	double OpNorm(const QMatrix<T>& a) const noexcept;
	double Det(const QMatrix<T>& a) const noexcept;
	
	uint64_t Rank(const QMatrix<T>& a) const noexcept;
	uint64_t Defect(const QMatrix<T>& a) const noexcept;
	
	bool IsLinearlyDep(const QMatrix<T>& a) const noexcept;
	bool IsSquare(const QMatrix<T>& a) const noexcept;

	SQUARE std::array<QMatrix<T>, 2> DecomposeLU(const QMatrix<T>& a) const;
	SQUARE std::array<QMatrix<T>, 3> DecomposeEigen(const QMatrix<T>& a) const;
	SQUARE std::array<QMatrix<T>, 3> DecomposeSingularValue(const QMatrix<T>& a) const;
	
	_row<T>& operator[](uint64_t i) const;
	
	QMatrix<T>& operator=(const QMatrix<T>& other);
	QMatrix<T>& operator=(QMatrix<T>&& other) noexcept;

private:
	T* data;
	uint64_t n, m;
};


#endif