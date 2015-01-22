#include "geometry.h"
#include <cassert>

template<> template<> Vec3f::Vec3(const Vec3i& other)
	: x(static_cast<float>(other.x)), y(static_cast<float>(other.y)), z(static_cast<float>(other.z)) { }

template<> template<> Vec3i::Vec3(const Vec3f& other)
	: x(static_cast<int>(other.x + 0.5f)), y(static_cast<int>(other.y + 0.5f)), z(static_cast<int>(other.z + 0.5f)) { }

Matrix::Matrix(int rows, int cols)
	: _m(std::vector<std::vector<float> >(rows, std::vector<float>(cols, 0.0f))), _rows(rows), _cols(cols) { }

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

std::vector<float>& Matrix::operator[](const int i) {
	assert(i >= 0 && i < _rows);
	return _m[i];
}

Matrix Matrix::operator*(const Matrix& other) {
	assert(_cols == other._rows);
	Matrix result(_rows, other._cols);
	for (int i = 0; i<_rows; i++) {
		for (int j = 0; j < other._cols; j++) {
			result._m[i][j] = 0.0f;
			for (int k = 0; k < _cols; k++) {
				result._m[i][j] += _m[i][k] * other._m[k][j];
			}
		}
	}
	return result;
}

Matrix Matrix::transpose() {
	Matrix result(_cols, _rows);
	for (int i = 0; i < _rows; i++) {
		for (int j = 0; j < _cols; j++) {
			result[j][i] = _m[i][j];
		}
	}
	return result;
}

Matrix Matrix::inverse() {
	assert(rows == cols);
	Matrix result(_rows, _cols * 2);
	for (int i = 0; i < _rows; i++) {
		for (int j = 0; j < _cols; j++) {
			result[i][j] = _m[i][j];
		}
	}
	for (int i = 0; i < _rows; i++) {
		result[i][i + _cols] = 1;
	}

	for (int i = 0; i < _rows - 1; i++) {
		for (int j = result._cols - 1; j >= 0; j--) {
			result[i][j] /= result[i][i];
		}
		for (int k = i + 1; k < _rows; k++) {
			float coeff = result[k][i];
			for (int j = 0; j < result._cols; j++) {
				result[k][j] -= result[i][j] * coeff;
			}
		}
	}

	for (int j = result._cols - 1; j >= _rows - 1; j--) {
		result[_rows - 1][j] /= result[_rows - 1][_rows - 1];
	}

	for (int i = _rows - 1; i > 0; i--) {
		for (int k = i - 1; k >= 0; k--) {
			float coeff = result[k][i];
			for (int j = 0; j < result._cols; j++) {
				result[k][j] -= result[i][j] * coeff;
			}
		}
	}

	Matrix truncate(_rows, _cols);
	for (int i = 0; i < _rows; i++) {
		for (int j = 0; j < _cols; j++) {
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