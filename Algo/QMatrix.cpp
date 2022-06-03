#include "QMatrix.hpp"

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
		delete[] data;
		ALLOC_TRY(data = new T[SAFE_UINT(other.n) * SAFE_UINT(other.m)]);
	}
	n = other.n;
	m = other.m;

	memcpy(data, other.data, SAFE_UINT(SAFE_UINT(n) * SAFE_UINT(m)*sizeof(T)));
}

template<typename T>
QMatrix<T>::QMatrix(QMatrix<T>&& other) noexcept {
    if (n != other.n || m != other.m) {
        delete[] data;
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
}

template<typename T>
T QMatrix<T>::GetN() const noexcept {
    return SAFE_UINT(n);
}

template<typename T>
T QMatrix<T>::GetM() const noexcept {
    return SAFE_UINT(m);
}

template<typename T>
T QMatrix<T>::GetItem(uint64_t i, uint64_t j) const {
    T res = 0;
    DEREF_TRY(res = data[SAFE_UINT(SAFE_UINT(i) * SAFE_UINT(m) + SAFE_UINT(j))]);
    return res;
}

template<typename T>
_row<T>& QMatrix<T>::operator[](uint64_t i) const {
    ALLOC_TRY(T * _tmp_dat = new T[m]);
    memcpy(_tmp_dat, SAFE_UINT(data + SAFE_UINT(i * m)), SAFE_UINT(m));
    _row<T> r = _row(_tmp_dat, m);
    _tmp_dat = nullptr;
    return r;
}

template<typename TT>
bool operator==(const QMatrix<TT>& a, const QMatrix<TT>& b) {
    if (a.GetN() != b.GetN() || a.GetM() != b.GetM()) {
        return false;
    }
    bool _flag = true;
    for (size_t i = 0; i < a.GetN(); ++i) {
        for (size_t j = 0; j < a.GetM(); ++j) {
            if (a.GetItem(i, j) != b.GetItem(i, j)) {
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
double QMatrix<T>::Det(const QMatrix<T>& a) const noexcept {
    if (!a.IsSquare()) {
        merror("Cannot calculate determinant of non-square matrix!", E_MAT_INVALID_DIMENSION);
        return static_cast<T>(0);
    }
    
    double detU = 1, detL = 1;
    std::array<QMatrix<T>, 2> decomp = a.DecomposeLU();
    for (size_t i = 0; i < a.GetN(); ++i) {
        detL *= decomp[0][i][i];
        detU *= decomp[1][i][i];
    }

    return detL * detU;

}

template<typename T>
SQUARE
std::array<QMatrix<T>, 2> QMatrix<T>::DecomposeLU(const QMatrix<T>& a) const {
    if (!a.IsSquare()) {
        merror("Cannot apply LU-decomposition to non-square matrix!", E_MAT_INVALID_DIMENSION);
    }
    
    auto n = a.GetN();

    T* _tmp_nums = nullptr;
    ALLOC_TRY(_tmp_nums = new T[SAFE_UINT(n)]);
    memset(_tmp_nums, 0, SAFE_UINT(sizeof(T) * SAFE_UINT(n)));

    QMatrix<T> l(_tmp_nums, n, n);
    QMatrix<T> u(_tmp_nums, n, n);

    _tmp_nums = nullptr;

    size_t i = 0, j = 0, k = 0;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (j < i)
                l.data[j*n + i] = 0;
            else {
                l.data[j][i] = a[j][i];
                for (k = 0; k < i; k++) {
                    l.data[j*n + i] = l[j][i] - l[j][k] * u[k][i];
                }
            }
        }
        for (j = 0; j < n; j++) {
            if (j < i)
                u.data[i*n + j] = 0;
            else if (j == i)
                u.data[i*n + j] = 1;
            else {
                u.data[i*n + j] = a[i][j] / l[i][i];
                for (k = 0; k < i; k++) {
                    u.data[i*n + j] = u[i][j] - ((l[i][k] * u[k][j]) / l[i][i]);
                }
            }
        }
    }

    return std::array<QMatrix<T>, 2> {l, u};
}

template<typename T>
QMatrix<T> operator+(const QMatrix<T>& left, const QMatrix<T>& right) {
    if (a.GetN() != b.GetN() || a.GetM() != b.GetM()) {
        merror("Cannot add two matrices with different dimensions!", E_MAT_INVALID_DIMENSION);
        return left;
    }

    QMatrix<T> res = left;
    for (size_t i = 0; i < left.GetN(); ++i) {
        for (size_t j = 0; j < left.GetM(); ++j) {
            DEREF_TRY(res.data[i * left.GetM() + j] = left[i][j] + right[i][j]);
        }
    }

    return res;
}

template<typename T>
QMatrix<T> operator-(const QMatrix<T>& left, const QMatrix<T>& right) {
    if (a.GetN() != b.GetN() || a.GetM() != b.GetM()) {
        merror("Cannot add two matrices with different dimensions!", E_MAT_INVALID_DIMENSION);
        return left;
    }

    QMatrix<T> res = left;
    for (size_t i = 0; i < left.GetN(); ++i) {
        for (size_t j = 0; j < left.GetM(); ++j) {
            DEREF_TRY(res.data[i * left.GetM() + j] = left[i][j] - right[i][j]);
        }
    }

    return res;
}

template<typename T>
QMatrix<T> operator*(const QMatrix<T>& left, const QMatrix<T>& right) {

}

template<typename T>
QMatrix<T> operator+(const QMatrix<T>& a, T scalar) {
    QMatrix<T> res = a;
    for (size_t i = 0; i < a.GetN(); ++i) {
        for (size_t j = 0; j < a.GetM(); ++j) {
            DEREF_TRY(res.data[i * a.GetM() + j] += scalar);
        }
    }

    return res;
}

template<typename T>
QMatrix<T> operator-(const QMatrix<T>& a, T scalar) {
    QMatrix<T> res = a;
    for (size_t i = 0; i < a.GetN(); ++i) {
        for (size_t j = 0; j < a.GetM(); ++j) {
            DEREF_TRY(res.data[i * a.GetM() + j] -= scalar);
        }
    }

    return res;
}

template<typename T>
QMatrix<T> operator*(const QMatrix<T>& a, T scalar) {
    QMatrix<T> res = a;
    for (size_t i = 0; i < a.GetN(); ++i) {
        for (size_t j = 0; j < a.GetM(); ++j) {
            DEREF_TRY(res.data[i * a.GetM() + j] *= scalar);
        }
    }

    return res;
}