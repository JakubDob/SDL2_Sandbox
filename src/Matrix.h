#pragma once
#include <iostream>
#include <type_traits>
#include <math.h>
#include "Lang.h"
#define PI 3.14159265

namespace math {
#if HAS_CONCEPTS
#include <concepts>
// remove_ref to turn Matrix&& into Matrix
template <typename MatrixType, size_t MAX_ROWS>
concept ColumnVector = requires {
    {
        std::remove_reference_t<MatrixType>::rowCount()
        } -> std::unsigned_integral;
    {
        std::remove_reference_t<MatrixType>::colCount()
        } -> std::unsigned_integral;
}
&&std::remove_reference_t<MatrixType>::rowCount() <=
        MAX_ROWS&& std::remove_reference_t<MatrixType>::colCount() ==
    1;
#endif
template <class T, size_t ROWS, size_t COLS>
class Matrix {
   public:
    constexpr static size_t rowCount() { return ROWS; };
    constexpr static size_t colCount() { return COLS; };
#if HAS_CONCEPTS
    // initialize matrix with column vectors of different sizes: eg.
    // Matrix<4,4>(Vec(2),Vec(4),Vec(3));
    template <ColumnVector<ROWS>... ColumnVectors>
    constexpr Matrix(ColumnVectors&&... vectors) {
        static_assert(sizeof...(vectors) <= COLS,
                      "Number of column vectors exceeds the matrix size");
        int c = 0;
        ((setColumnVector(c++, std::forward<ColumnVectors>(vectors))), ...);
    }
#else
    // template <typename std::enable_if_t<ROWS == 3 && COLS == 3, bool> = true>
    constexpr Matrix(Matrix<T, 2, 1> const& m1, Matrix<T, 2, 1> const& m2) {
        this->operator()(0, 0) = m1[0];
        this->operator()(1, 0) = m1[1];
        this->operator()(0, 1) = m2[0];
        this->operator()(1, 1) = m2[1];
        // this->operator()(2, 2) = 1;
    }
#endif
    // initialize square matrix to identity
    constexpr Matrix() {
        if (ROWS == COLS) {
            for (size_t i = 0; i < ROWS; ++i) {
                data[i + i * COLS] = 1;
            }
        }
    }

    // initializer list syntax: eg. Matrix<3,1>({1,2,3}), performs implicit
    // conversion from int to T: eg. [T=float]: {1,2.f}->{(float)1,2.f}
    constexpr Matrix(T const (&scalars)[ROWS * COLS]) {
        for (size_t i = 0; i < ROWS * COLS; ++i) {
            data[i] = scalars[i];
        }
    }

    // initialize vector from a smaller column vec and scalars: eg.
    // Vec4(Vec3, 1.0f); With implicit conversion
    template <size_t R1, size_t R2>
    constexpr Matrix(Matrix<T, R1, 1> const& v, T const (&scalars)[R2]) {
        static_assert(R1 + R2 == ROWS,
                      "Size of column vector plus number of scalars must match "
                      "the vector size");
        setColumnVector(0, v);
        for (auto i = 0; i < R2; ++i) {
            data[R1 + i] = scalars[i];
        }
    }

    // initialize vector from a smaller column vec and scalars: eg.
    // Vec4(Vec3, 1.0f); No implicit conversion allowed.
    template <
        size_t R, typename... Ts,
        typename = std::enable_if_t<std::conjunction_v<std::is_same<T, Ts>...>>>
    constexpr Matrix(Matrix<T, R, 1> const& v, Ts... rest) {
        static_assert(R + sizeof...(rest) == ROWS,
                      "Number of rows exceeds the vector size");
        setColumnVector(0, v);
        auto i = R;
        ((data[i++] = rest), ...);
    }
    // constuctor with scalars without braces: eg. Matrix(1,2,3,4). No implicit
    // conversion allowed.
    template <typename... Ts, typename = std::enable_if_t<
                                  std::conjunction_v<std::is_same<T, Ts>...>>>
    constexpr Matrix(T first, T second, Ts... rest) {
        static_assert(sizeof...(rest) == (ROWS * COLS) - 2,
                      "Number of scalars must match the size of the matrix");
        data[0] = first;
        data[1] = second;
        int i = 2;
        ((data[i++] = rest), ...);
    }

#if HAS_CONCEPTS
    template <ColumnVector<ROWS> V>
    void setColumnVector(int col, V const& v) {
        for (int i = 0; i < V::rowCount(); ++i) {
            this->operator()(i, col) = v[i];
        }
    }

    template <ColumnVector<ROWS> V>
    void setColumnVector(int col, V&& v) {
        for (size_t i = 0; i < std::remove_reference_t<V>::rowCount(); ++i) {
            this->operator()(i, col) = v[i];
        }
    }
#else
    template <size_t R, std::enable_if_t<R <= ROWS, bool> = true>
    void setColumnVector(int col, Matrix<T, R, 1> const& v) {
        for (int i = 0; i < R; ++i) {
            this->operator()(i, col) = v[i];
        }
    }
    // content of v is copied anyway, is && useful here?
    template <size_t R, std::enable_if_t<R <= ROWS, bool> = true>
    void setColumnVector(int col, Matrix<T, R, 1>&& v) {
        for (int i = 0; i < R; ++i) {
            this->operator()(i, col) = v[i];
        }
    }
#endif
    Matrix& operator=(Matrix<T, ROWS, COLS> const& o) {
        for (size_t i = 0; i < ROWS * COLS; ++i) {
            data[i] = o.data[i];
        }
        return *this;
    }
    T const& operator()(size_t row, size_t col) const {
        return data[col + COLS * row];
    }
    T& operator()(size_t row, size_t col) { return data[col + COLS * row]; }
    T const& operator[](int i) const { return data[i]; }
    T& operator[](int i) { return data[i]; }
    void print() const {
        std::cout << "---------\n";
        for (size_t i = 0; i < ROWS * COLS; ++i) {
            std::cout << data[i];
            ((i + 1) % COLS == 0) ? std::cout << "\n" : std::cout << " ";
        }
        std::cout << "---------\n";
    }

    Matrix<T, ROWS, COLS>& operator+=(Matrix<T, ROWS, COLS> const& m) {
        for (size_t i = 0; i < ROWS * COLS; ++i) {
            data[i] += m[i];
        }
        return *this;
    }

    Matrix<T, ROWS, COLS>& operator-=(Matrix<T, ROWS, COLS> const& m) {
        for (size_t i = 0; i < ROWS * COLS; ++i) {
            data[i] -= m[i];
        }
        return *this;
    }

    auto operator*=(T scalar) {
        for (size_t i = 0; i < ROWS * COLS; ++i) {
            data[i] = data[i] * scalar;
        }
        return *this;
    }

    auto operator/=(T scalar) {
        for (int i = 0; i < ROWS * COLS; ++i) {
            data[i] = data[i] / scalar;
        }
        return *this;
    }

    auto operator/=(Matrix<T, ROWS, COLS> const& m) {
        for (int i = 0; i < ROWS * COLS; ++i) {
            data[i] /= m[i];
        }
        return *this;
    }

    Matrix<T, ROWS, 1> getColumnVector(int column) const {
        Matrix<T, ROWS, 1> result;
        for (int i = 0; i < ROWS; ++i) {
            result(i, 0) = data(i, column);
        }
        return result;
    }
    T magnitude() const {
        static_assert(COLS == 1,
                      "Can only measure magnitude of column vectors");
        return sqrt(dot(*this, *this));
    }
    Matrix<T, ROWS, 1>& normalize() {
        T len = magnitude();
        if (len > 0) {
            for (int i = 0; i < ROWS; ++i) {
                data[i] /= len;
            }
        }
        return *this;
    }
    void clear() { memset(data, 0, COLS * ROWS); }
    T const* getData() const { return data; }

   private:
    T data[COLS * ROWS]{};
};

template <typename T, size_t ROWS_A, size_t COLS_A, size_t COLS_B>
auto operator*(Matrix<T, ROWS_A, COLS_A> const& m1,
               Matrix<T, COLS_A, COLS_B> const& m2) {
    Matrix<T, ROWS_A, COLS_B> ret;
    for (int d = 0; d < ROWS_A; d++) {
        for (int c = 0; c < COLS_B; c++) {
            T total = 0;
            for (int r = 0; r < COLS_A; r++) {
                total += m1(d, r) * m2(r, c);
            }
            ret(d, c) = total;
        }
    }
    return ret;
}
template <typename T, size_t ROWS_A, size_t COLS_A, size_t ROWS_B,
          size_t COLS_B>
auto multiplyVerbose(Matrix<T, ROWS_A, COLS_A> const& m1,
                     Matrix<T, ROWS_B, COLS_B> const& m2) {
    static_assert(COLS_A == ROWS_B,
                  "Columns of matrix A must equal rows of matrix B");
    Matrix<T, ROWS_A, COLS_B> ret;

    for (size_t d = 0; d < ROWS_A; d++) {
        for (size_t c = 0; c < COLS_B; c++) {
            T total = 0;
            for (size_t r = 0; r < ROWS_B; r++) {
                std::cout << m1(d, r) << "*" << m2(r, c);
                (r + 1 == ROWS_B) ? std::cout << " " : std::cout << "+";
                total += m1(d, r) * m2(r, c);
            }
            ret(d, c) = total;
        }
        std::cout << "\n";
    }
    return ret;
}

template <typename T, size_t ROWS_A, size_t COLS_A>
auto operator*(Matrix<T, ROWS_A, COLS_A> const& m1, T scalar) {
    Matrix<T, ROWS_A, COLS_A> ret;
    for (size_t i = 0; i < ROWS_A * COLS_A; ++i) {
        ret[i] = m1[i] * scalar;
    }
    return ret;
}
template <typename T, size_t ROWS_A, size_t COLS_A>
auto operator*(T scalar, Matrix<T, ROWS_A, COLS_A> const& m1) {
    Matrix<T, ROWS_A, COLS_A> ret;
    for (size_t i = 0; i < ROWS_A * COLS_A; ++i) {
        ret[i] = scalar * m1[i];
    }
    return ret;
}

template <typename T, size_t ROWS, size_t COLS>
auto operator+(Matrix<T, ROWS, COLS> const& m1,
               Matrix<T, ROWS, COLS> const& m2) {
    Matrix<T, ROWS, COLS> ret;
    for (size_t i = 0; i < ROWS * COLS; ++i) {
        ret[i] = m1[i] + m2[i];
    }
    return ret;
}
template <typename T, size_t ROWS, size_t COLS>
auto operator-(Matrix<T, ROWS, COLS> const& m1,
               Matrix<T, ROWS, COLS> const& m2) {
    Matrix<T, ROWS, COLS> ret;
    for (size_t i = 0; i < ROWS * COLS; ++i) {
        ret[i] = m1[i] - m2[i];
    }
    return ret;
}
template <typename T, size_t ROWS, size_t COLS>
auto operator/(Matrix<T, ROWS, COLS> const& m1, T scalar) {
    Matrix<T, ROWS, COLS> ret;
    for (size_t i = 0; i < ROWS * COLS; ++i) {
        ret[i] = m1[i] / scalar;
    }
    return ret;
}
template <typename T, size_t ROWS_A, size_t COLS_A, size_t ROWS_B,
          size_t COLS_B>
bool operator==(Matrix<T, ROWS_A, COLS_A> const& m1,
                Matrix<T, ROWS_B, COLS_B> const& m2) {
    if (COLS_A != COLS_B || ROWS_A != ROWS_B) {
        return false;
    }
    Matrix<T, ROWS_A, COLS_A> ret;
    for (size_t i = 0; i < ROWS_A * COLS_A; ++i) {
        if (m1[i] != m2[i]) {
            return false;
        }
    }
    return true;
}

template <typename T, size_t ROWS_A, size_t COLS_A, size_t ROWS_B,
          size_t COLS_B>
bool operator!=(Matrix<T, ROWS_A, COLS_A> const& m1,
                Matrix<T, ROWS_B, COLS_B> const& m2) {
    if (COLS_A != COLS_B || ROWS_A != ROWS_B) {
        return true;
    }
    Matrix<T, ROWS_A, COLS_A> ret;
    for (size_t i = 0; i < ROWS_A * COLS_A; ++i) {
        if (m1[i] != m2[i]) {
            return true;
        }
    }
    return false;
}
template <typename T, size_t ROWS, size_t COLS>
auto operator-(Matrix<T, ROWS, COLS> const& m) {
    Matrix<T, ROWS, COLS> ret;
    for (size_t i = 0; i < ROWS * COLS; ++i) {
        ret[i] = -m[i];
    }
    return ret;
}

template <typename T, size_t N>
auto& transpose(Matrix<T, N, N>& m) {
    for (size_t i = 0; i < N; ++i) {
        int x = i;
        while (x-- > 0) {
            std::swap(m(x, i), m(i, x));
        }
    }
    return m;
}
template <typename T, size_t N>
auto transpose(Matrix<T, N, N>&& m) {
    for (size_t i = 0; i < N; ++i) {
        int x = i;
        while (x-- > 0) {
            std::swap(m(x, i), m(i, x));
        }
    }
    return m;
}
template <typename T, size_t ROWS, size_t COLS>
auto dot(Matrix<T, ROWS, COLS> const& m1, Matrix<T, ROWS, COLS> const& m2) {
    T total = 0;
    for (size_t i = 0; i < ROWS; ++i) {
        total += m1[i] * m2[i];
    }
    return total;
}
template <typename T, size_t ROWS_A, size_t ROWS_B>
auto dot2D(Matrix<T, ROWS_A, 1> const& m1, Matrix<T, ROWS_B, 1> const& m2) {
    static_assert(
        ROWS_A >= 2 && ROWS_B >= 2,
        "Vectors must have at least two rows for the dot2D operation");
    T total = 0;
    for (size_t i = 0; i < 2; ++i) {
        total += m1[i] * m2[i];
    }
    return total;
}
template <typename T, size_t ROWS, size_t COLS>
auto hadamard(Matrix<T, ROWS, COLS> const& m1,
              Matrix<T, ROWS, COLS> const& m2) {
    Matrix<T, ROWS, COLS> ret;
    for (size_t i = 0; i < ROWS * COLS; ++i) {
        ret[i] = m1[i] * m2[i];
    }
    return ret;
}
template <typename T, size_t ROWS>
auto distance(Matrix<T, ROWS, 1> const& m1, Matrix<T, ROWS, 1> const& m2) {
    T total = 0;
    for (size_t i = 0; i < ROWS; ++i) {
        total += (m2[i] - m1[i]) * (m2[i] - m1[i]);
    }
    return sqrt(total);
}

template <typename T, size_t ROWS>
auto distanceSquared(Matrix<T, ROWS, 1> const& m1,
                     Matrix<T, ROWS, 1> const& m2) {
    T total = 0;
    for (size_t i = 0; i < ROWS; ++i) {
        total += (m2[i] - m1[i]) * (m2[i] - m1[i]);
    }
    return total;
}

template <typename T>
auto getRotationMatrix(T degreeAngle) {
    auto rad = degreeAngle * PI / 180.0;
    return Matrix<T, 4, 4>(
        {static_cast<T>(cosf(rad)), static_cast<T>(-sinf(rad)), 0, 0,
         static_cast<T>(sinf(rad)), static_cast<T>(cosf(rad)), 0, 0, 0, 0, 1, 0,
         0, 0, 0, 1});
}
template <typename T>
auto get2x2RotationMatrix2D(T degreeAngle) {
    auto rad = degreeAngle * PI / 180.0;
    return Matrix<T, 2, 2>({
        static_cast<T>(cosf(rad)),
        static_cast<T>(-sinf(rad)),
        static_cast<T>(sinf(rad)),
        static_cast<T>(cosf(rad)),
    });
}

template <typename T>
auto getScalingMatrix(T scalar) {
    Matrix<T, 4, 4> m;
    m(0, 0) = scalar;
    m(1, 1) = scalar;
    m(2, 2) = 1;
    m(3, 3) = 1;
    return m;
}

template <typename T>
auto getTranslationMatrix(Matrix<T, 2, 1> const& v) {
    return Matrix<T, 4, 4>(
        {1, 0, 0, v[0], 0, 1, 0, v[1], 0, 0, 1, 0, 0, 0, 0, 1});
}

template <typename T, size_t ROWS, size_t COLS>
void negateX(Matrix<T, ROWS, COLS>& m) {
    for (int i = 0; i < ROWS; ++i) {
        m(i, 0) = -m(i, 0);
    }
}
}  // namespace math