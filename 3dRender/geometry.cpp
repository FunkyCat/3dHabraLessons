#include "geometry.h"
#include <cassert>

template<> template<> Vec3f::Vec3(const Vec3i& other)
	: x(static_cast<float>(other.x)), y(static_cast<float>(other.y)), z(static_cast<float>(other.z)) { }

template<> template<> Vec3i::Vec3(const Vec3f& other)
	: x(static_cast<int>(other.x + 0.5f)), y(static_cast<int>(other.y + 0.5f)), z(static_cast<int>(other.z + 0.5f)) { }

template<> Vec3f::Vec3(const Matrix& matrix)
	: x(matrix[0][0] / matrix[3][0]), y(matrix[1][0] / matrix[3][0]), z(matrix[2][0] / matrix[3][0]) { }

template<> Vec3i::Vec3(const Matrix& matrix)
{
	(*this) = Vec3f(matrix);
}

Matrix::Matrix(size_t rows, size_t cols)
	: _m(std::vector<std::vector<float> >(rows, std::vector<float>(cols, 0.0f))), _rows(rows), _cols(cols) { }

Matrix::Matrix(const Vec3f vector) : _m(std::vector< std::vector< float > >(4, std::vector<float>(1))),
	_cols(1), _rows(4)
{
	_m[0][0] = vector.x;
	_m[1][0] = vector.y;
	_m[2][0] = vector.z;
	_m[3][0] = 1.0f;
}

int Matrix::rowCount() {
	return _rows;
}

int Matrix::colCount() {
	return _cols;
}

Matrix Matrix::identity(int dimensions) {
	Matrix E(dimensions, dimensions);
	for (int i = 0; i < dimensions; i++) {
		for (int j = 0; j < dimensions; j++) {
			E[i][j] = (i == j ? 1.0f : 0.0f);
		}
	}
	return E;
}

const std::vector<float>& Matrix::operator[](const size_t idx) const {
	assert(idx < _rows);
	return _m[idx];
}

std::vector<float>& Matrix::operator[](const size_t idx) {
	assert(idx < _rows);
	return _m[idx];
}

Matrix Matrix::operator*(const Matrix& other) {
	assert(_cols == other._rows);
	Matrix result(_rows, other._cols);
	for (size_t i = 0; i<_rows; i++) {
		for (size_t j = 0; j < other._cols; j++) {
			result._m[i][j] = 0.0f;
			for (size_t k = 0; k < _cols; k++) {
				result._m[i][j] += _m[i][k] * other._m[k][j];
			}
		}
	}
	return result;
}

Matrix Matrix::transpose() {
	Matrix result(_cols, _rows);
	for (size_t i = 0; i < _rows; i++) {
		for (size_t j = 0; j < _cols; j++) {
			result[j][i] = _m[i][j];
		}
	}
	return result;
}

Matrix Matrix::inverse() {
	assert(_rows == _cols);
	Matrix result(_rows, _cols * 2);
	for (size_t i = 0; i < _rows; i++) {
		for (size_t j = 0; j < _cols; j++) {
			result[i][j] = _m[i][j];
		}
	}
	for (size_t i = 0; i < _rows; i++) {
		result[i][i + _cols] = 1;
	}

	for (size_t i = 0; i < _rows - 1; i++) {
		for (size_t j = result._cols - 1; j >= 0; j--) {
			result[i][j] /= result[i][i];
		}
		for (size_t k = i + 1; k < _rows; k++) {
			float coeff = result[k][i];
			for (size_t j = 0; j < result._cols; j++) {
				result[k][j] -= result[i][j] * coeff;
			}
		}
	}

	for (size_t j = result._cols - 1; j >= _rows - 1; j--) {
		result[_rows - 1][j] /= result[_rows - 1][_rows - 1];
	}

	for (size_t i = _rows - 1; i > 0; i--) {
		for (size_t k = i - 1; k >= 0; k--) {
			float coeff = result[k][i];
			for (size_t j = 0; j < result._cols; j++) {
				result[k][j] -= result[i][j] * coeff;
			}
		}
	}

	Matrix truncate(_rows, _cols);
	for (size_t i = 0; i < _rows; i++) {
		for (size_t j = 0; j < _cols; j++) {
			truncate[i][j] = result[i][j + _cols];
		}
	}
	return truncate;
}

std::ostream& operator<<(std::ostream& s, Matrix& m) {
	for (int i = 0; i < m.rowCount(); i++)  {
		for (int j = 0; j<m.colCount(); j++) {
			s << m[i][j];
			if (j < m.colCount() - 1) s << "\t";
		}
		s << "\n";
	}
	return s;
}