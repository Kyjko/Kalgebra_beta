#ifndef _QMATRIX_H
#define _QMATRIX_H

#include "MatrixError.hpp"
#include <stdint.h>
#include <array>
#include <complex>
#include <ostream>

template<typename T> class QMatrix;

#define SQUARE

/*
	-------------

	
*/

template<typename U>
struct _row {
public:
	_row(const U* _a_, uint64_t k) {
		ALLOC_TRY(entries = new U[k]);
		memcpy(entries, _a_, SAFE_UINT(SAFE_UINT(k) * sizeof(U)));
	}
	~_row() {}

	U operator[](size_t idx) {
		DEREF_TRY(return entries[idx]);
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
	uint64_t GetN() const noexcept;
	uint64_t GetM() const noexcept;

	double OpNorm(const QMatrix<T>& a) const noexcept;
	double Det() const noexcept;
	
	uint64_t Rank() const noexcept;
	uint64_t Defect() const noexcept;
	
	bool IsLinearlyDep(const QMatrix<T>& a) const noexcept;
	bool IsSquare() const noexcept;

	SQUARE std::array<QMatrix<T>, 2> DecomposeLU() const;
	SQUARE std::array<QMatrix<T>, 3> DecomposeEigen(const QMatrix<T>& a) const;
	SQUARE std::array<QMatrix<T>, 3> DecomposeSingularValue(const QMatrix<T>& a) const;
	
	_row<T> operator[](uint64_t i) const;
	
	QMatrix<T>& operator=(const QMatrix<T>& other);
	QMatrix<T>& operator=(QMatrix<T>&& other) noexcept;

	template<typename T> friend std::ostream& operator<<(std::ostream& os, const QMatrix<T>& mat);
    SQUARE template<typename T> friend QMatrix<T> operator+(const QMatrix<T>& left, const QMatrix<T>& right);
    SQUARE template<typename T> friend QMatrix<T> operator-(const QMatrix<T>& left, const QMatrix<T>& right);
    template<typename T> friend QMatrix<T> operator*(const QMatrix<T>& left, const QMatrix<T>& right);
    template<typename T> friend QMatrix<T> operator+(const QMatrix<T>& a, T scalar);
    template<typename T> friend QMatrix<T> operator-(const QMatrix<T>& a, T scalar);
    template<typename T> friend QMatrix<T> operator*(const QMatrix<T>& a, T scalar);

private:
	T* data;
	uint64_t n, m;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
QMatrix<T>::~QMatrix() {
    n = static_cast<T>(0);
    m = static_cast<T>(0);
    delete[] data;
}

template<typename T>
QMatrix<T>::QMatrix(const T* entries, uint64_t n, uint64_t m) : n(n), m(m) {
    ALLOC_TRY(data = new T[SAFE_UINT(n) * SAFE_UINT(m)]);
    memcpy(data, entries, SAFE_UINT(SAFE_UINT(n) * SAFE_UINT(m) * sizeof(T)));
}

template<typename T>
QMatrix<T>::QMatrix(const QMatrix<T>& other) {
    if (n != other.n || m != other.m) {
        //delete[] data;
        ALLOC_TRY(data = new T[SAFE_UINT(other.GetN()) * SAFE_UINT(other.GetM())]);
    }
    n = other.n;
    m = other.m;

    memcpy(data, other.data, SAFE_UINT(SAFE_UINT(n) * SAFE_UINT(m) * sizeof(T)));
}

template<typename T>
QMatrix<T>::QMatrix(QMatrix<T>&& other) noexcept {
    if (n != other.n || m != other.m) {
        //delete[] data;
        ALLOC_TRY(data = new T[SAFE_UINT(other.n) * SAFE_UINT(other.m)]);
    }
    n = other.n;
    m = other.m;

    memcpy(data, other.data, SAFE_UINT(SAFE_UINT(n) * SAFE_UINT(m) * sizeof(T)));
    other.data = nullptr;
}

template<typename T>
QMatrix<T>& QMatrix<T>::operator=(const QMatrix<T>& other) {
    if (n != other.n || m != other.m) {
        delete[] data;
        ALLOC_TRY(data = new T[SAFE_UINT(other.n) * SAFE_UINT(other.m)]);
    }
    n = other.n;
    m = other.m;

    memcpy(data, other.data, SAFE_UINT(SAFE_UINT(n) * SAFE_UINT(m) * sizeof(T)));

    return *this;
}

template<typename T>
QMatrix<T>& QMatrix<T>::operator=(QMatrix<T>&& other) noexcept {
    if (n != other.n || m != other.m) {
        delete[] data;
        ALLOC_TRY(data = new T[SAFE_UINT(other.n) * SAFE_UINT(other.m)]);
    }
    n = other.n;
    m = other.m;

    memcpy(data, other.data, SAFE_UINT(SAFE_UINT(n) * SAFE_UINT(m) * sizeof(T)));
    other.data = nullptr;

    return *this;
}

template<typename T>
bool QMatrix<T>::IsSquare() const noexcept {
    return n == m;
}

template<typename T>
uint64_t QMatrix<T>::GetN() const noexcept {
    return SAFE_UINT(n);
}

template<typename T>
uint64_t QMatrix<T>::GetM() const noexcept {
    return SAFE_UINT(m);
}

template<typename T>
T QMatrix<T>::GetItem(uint64_t i, uint64_t j) const {
    T res = 0;
    DEREF_TRY(res = data[SAFE_UINT(SAFE_UINT(i) * SAFE_UINT(m) + SAFE_UINT(j))]);
    return res;
}

template<typename T>
_row<T> QMatrix<T>::operator[](uint64_t i) const {
    T* _tmp_dat = nullptr;
    ALLOC_TRY(_tmp_dat = new T[m]);
    memcpy(_tmp_dat, static_cast<T*>(data + i * SAFE_UINT(m)), SAFE_UINT(SAFE_UINT(m)*sizeof(T)));
  
    _row<T> r(_tmp_dat, m);
    _tmp_dat = nullptr;
    return r;
}

template<typename TT>
bool operator==(const QMatrix<TT>& a, const QMatrix<TT>& b) {
    if (a.GetN() != b.GetN() || a.GetM() != b.GetM()) {
        return false;
    }

    uint64_t n = a.GetN();
    uint64_t m = a.GetM();

    bool _flag = true;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            if (a[i][j] != b[i][j]) {
                _flag = false;
            }
        }
    }

    return _flag;

}

template<typename TT>
bool operator<(const QMatrix<TT>& a, const QMatrix<TT> b) {
    return a.OpNorm() < b.OpNorm();
}

template<typename TT>
bool operator>(const QMatrix<TT>& a, const QMatrix<TT> b) {
    return a.OpNorm() > b.OpNorm();
}

template<typename T>
SQUARE
double QMatrix<T>::Det() const noexcept {
    if (!IsSquare()) {
        merror("Cannot calculate determinant of non-square matrix!", E_MAT_INVALID_DIMENSION);
        return static_cast<T>(0);
    }

    double detU = 1, detL = 1;
    std::array<QMatrix<T>, 2> decomp = this->DecomposeLU();
    for (size_t i = 0; i < GetN(); ++i) {
        DEREF_TRY(detL *= decomp[0][i][i]);
        DEREF_TRY(detU *= decomp[1][i][i]);
    }

    return detL * detU;

}

template<typename T>
SQUARE
std::array<QMatrix<T>, 2> QMatrix<T>::DecomposeLU() const {
    
    auto a = *this;
    if (!a.IsSquare()) {
        merror("Cannot apply LU-decomposition to non-square matrix!", E_MAT_INVALID_DIMENSION);
    }

    uint64_t n = a.GetN();

    T* _tmp_nums = nullptr;
    ALLOC_TRY(_tmp_nums = new T[SAFE_UINT(n * n)]);
    memset(_tmp_nums, 0, SAFE_UINT(sizeof(T) * SAFE_UINT(n * n)));

    QMatrix<T> l(_tmp_nums, n, n);
    QMatrix<T> u(_tmp_nums, n, n);

    _tmp_nums = nullptr;

    size_t i = 0, j = 0, k = 0;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (j < i)
                l.data[j * n + i] = 0;
            else {
                l.data[j * n + i] = a[j][i];
                for (k = 0; k < i; k++) {
                    l.data[i * n + j] = l[j][i] - l[j][k] * u[k][i];
                }
            }
        }

        for (j = 0; j < n; j++) {
            if (j < i)
                u.data[i * n + j] = 0;
            else if (j == i)
                u.data[i * n + j] = 1;
            else {
                u.data[i * n + j] = a[i][j] / l[i][i];
                for (k = 0; k < i; k++) {
                    u.data[i * n + j] = u[i][j] - ((l[i][k] * u[k][j]) / l[i][i]);
                }
            }
        }
    }

    return std::array<QMatrix<T>, 2> {l, u};
}

template<typename T>
SQUARE
QMatrix<T> operator+(const QMatrix<T>& left, const QMatrix<T>& right) {
    if (left.GetN() != right.GetN() || left.GetM() != right.GetM()) {
        merror("Cannot add two matrices with different dimensions!", E_MAT_INVALID_DIMENSION);
        return left;
    }

    uint64_t n = left.GetN();
    uint64_t m = left.GetM();

    QMatrix<T> res = left;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            DEREF_TRY(res.data[i * m + j] = left[i][j] + right[i][j]);
        }
    }

    return res;
}

template<typename T>
SQUARE
QMatrix<T> operator-(const QMatrix<T>& left, const QMatrix<T>& right) {
    if (left.GetN() != right.GetN() || left.GetM() != right.GetM()) {
        merror("Cannot subtract two matrices with different dimensions!", E_MAT_INVALID_DIMENSION);
        return left;
    }

    uint64_t n = left.GetN();
    uint64_t m = left.GetM();

    QMatrix<T> res = left;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            DEREF_TRY(res.data[i * m + j] = left[i][j] - right[i][j]);
        }
    }

    return res;
}

template<typename T>
QMatrix<T> operator*(const QMatrix<T>& left, const QMatrix<T>& right) {
    if (left.GetN() != right.GetN() || left.GetM() != right.GetM()) {
        merror("Cannot multiply two matrices with invalid dimensions!", E_MAT_INVALID_DIMENSION);
        return left;
    }

    uint64_t n = left.GetN();
    uint64_t m = right.GetM();
    uint64_t p = left.GetM();

    T* _tmp_nums = nullptr;
    ALLOC_TRY(_tmp_nums = new T[SAFE_UINT(n * m)]);
    memset(_tmp_nums, 0, SAFE_UINT(sizeof(T) * SAFE_UINT(n * m)));

    QMatrix<T> res(_tmp_nums, n, m);
    _tmp_nums = nullptr;

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            T part_sum = 0;
            for (size_t k = 0; k < p; ++k) {
                //DEREF_TRY(part_sum += left[i][k] * right[k][j]);
                part_sum += left.data[i * m + j] * right.data[k * m + j];
            }
            res.data[SAFE_UINT(i * m + j)] = part_sum;
        }
    }

    return res;
}

template<typename T>
QMatrix<T> operator+(const QMatrix<T>& a, T scalar) {
    QMatrix<T> res = a;

    uint64_t n = a.GetN();
    uint64_t m = a.GetM();

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            DEREF_TRY(res.data[i * m + j] += scalar);
        }
    }

    return res;
}

template<typename T>
QMatrix<T> operator-(const QMatrix<T>& a, T scalar) {
    QMatrix<T> res = a;

    uint64_t n = a.GetN();
    uint64_t m = a.GetM();

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            DEREF_TRY(res.data[i * m + j] -= scalar);
        }
    }

    return res;
}

template<typename T>
QMatrix<T> operator*(const QMatrix<T>& a, T scalar) {
    QMatrix<T> res = a;

    uint64_t n = a.GetN();
    uint64_t m = a.GetM();

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            DEREF_TRY(res.data[i * m + j] *= scalar);
        }
    }

    return res;
}

template<typename T>
SQUARE
QMatrix<T> I(uint64_t n) {
    T* _tmp_nums = nullptr;
    ALLOC_TRY(_tmp_nums = new T[SAFE_UINT(n * n)]);
    memset(_tmp_nums, 0, SAFE_UINT(sizeof(T) * SAFE_UINT(n * n)));



    QMatrix<T> res(_tmp_nums, n, n);
    for (size_t i = 0; i < n; ++i) {
        res.data[SAFE_UINT(i * n + i)] = static_cast<T>(1);
    }
    _tmp_nums = nullptr;

    return res;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const QMatrix<T>& mat) {
    uint64_t n = mat.GetN();
    uint64_t m = mat.GetM();
    os << n << " x " << m << " matrix\n";
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            os << mat[i][j] << " ";
        }
        os << "\n";
    }

    return os;
}

#endif