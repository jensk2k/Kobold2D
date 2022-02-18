#pragma once
#include "KTypes.h"
#include <vector>
#include <array>
#include <iostream>
#include <assert.h>

class Matrix
{
public:
	Matrix()
		: columns(0)
		, rows(0)
	{}

	Matrix(uint8 columns, uint8 rows)
		: columns(columns)
		, rows(rows)
	{
		data.resize(columns * rows);
	}

	Matrix(uint8 columns, uint8 rows, const std::vector<float>& initialData)
		: columns(columns)
		, rows(rows)
	{
		assert(initialData.size() == columns * rows);
		data.resize(columns * rows);
		for (int i = 0; i < initialData.size(); i++)
		{
			data[i] = initialData[i];
		}
	}

	Matrix(const Matrix& other)
		: columns(other.columns)
		, rows(other.rows)
	{
		data.resize(columns * rows);
		for (uint8 row = 0; row < other.rows; row++)
		{
			for (uint8 col = 0; col < other.columns; col++)
			{
				float val = other(col, row);
				this->operator()(col, row) = val;
			}
		}
	}

	uint8 Rows() const { return rows; }
	uint8 Columns() const { return columns; }

	const float& operator() (uint8 column, uint8 row) const
	{
		assert(column < columns);
		assert(row < rows);
		return data[row * columns + column];
	}

	float& operator() (uint8 column, uint8 row)
	{
		assert(column < columns);
		assert(row < rows);
		return data[row * columns + column];
	}

	//void Set(uint8 column, uint8 row, float value)
	//{
	//	assert(column < columns);
	//	assert(row < rows);
	//	// y* m_width + x

	//	data[row * columns + column] = value;
	//}

	//float Get(uint8 column, uint8 row) const
	//{
	//	assert(column < columns);
	//	assert(row < rows);

	//	return data[row * columns + column];
	//}

	static Matrix Add(const Matrix& m, float n)
	{
		Matrix result(m.columns, m.rows);
		for (uint8 row = 0; row < m.rows; row++)
		{
			for (uint8 col = 0; col < m.columns; col++)
			{
				float val = m(col, row);
				result(col, row) = val + n;
			}
		}
		return result;
	}

	static Matrix Add(const Matrix& m1, const Matrix& m2)
	{
		assert(m1.rows == m2.rows);
		assert(m1.columns == m2.columns);
		Matrix result(m1.columns, m1.rows);
		for (uint8 row = 0; row < m1.rows; row++)
		{
			for (uint8 col = 0; col < m1.columns; col++)
			{
				float m1val = m1(col, row);
				float m2val = m2(col, row);

				result(col, row) = m1val + m2val;
			}
		}
		return result;
	}

	static Matrix Subtract(const Matrix& m, float n)
	{
		Matrix result(m.columns, m.rows);
		for (uint8 row = 0; row < m.rows; row++)
		{
			for (uint8 col = 0; col < m.columns; col++)
			{
				float val = m(col, row);

				result(col, row) = val - n;
			}
		}
		return result;
	}

	static Matrix Subtract(const Matrix& m1, const Matrix& m2)
	{
		assert(m1.rows == m2.rows);
		assert(m1.columns == m2.columns);
		Matrix result(m1.columns, m1.rows);
		for (uint8 row = 0; row < m1.rows; row++)
		{
			for (uint8 col = 0; col < m1.columns; col++)
			{
				float m1val = m1(col, row);
				float m2val = m2(col, row);

				result(col, row) = m1val - m2val;
			}
		}
		return result;
	}

	static Matrix Scale(const Matrix& m, float scalar)
	{
		Matrix result(m.columns, m.rows);		
		for (uint8 row = 0; row < m.rows; row++)
		{
			for (uint8 col = 0; col < m.columns; col++)
			{
				float val = m(col, row);
				result(col, row) = val * scalar;
			}
		}
		return result;
	}

	static Matrix Product(const Matrix& m1, const Matrix& m2)
	{
		assert(m1.columns == m2.rows);
		Matrix result(m2.columns, m1.rows);

		for (int row = 0; row < result.rows; row++)
		{
			for (int col = 0; col < result.columns; col++)
			{
				float dotProduct = 0.f;
				for (int col1row2 = 0; col1row2 < m1.columns; col1row2++)
				{
					dotProduct += m1(col1row2, row) * m2(col, col1row2);
				}
				result(col, row) = dotProduct;
			}
		}

		return result;
	}

	// also known as element-wise product
	static Matrix HadamardProduct(const Matrix& m1, const Matrix& m2)
	{
		assert(m1.rows == m2.rows);
		assert(m1.columns == m2.columns);
		Matrix result(m1.columns, m2.rows);		
		for (uint8 row = 0; row < m1.rows; row++)
		{
			for (uint8 col = 0; col < m1.columns; col++)
			{
				float m1val = m1(col, row);
				float m2val = m2(col, row);

				result(col, row) = m1val * m2val;
			}
		}
		return result;
	}

	static Matrix Transpose(const Matrix& m)
	{
		Matrix result(m.rows, m.columns); 
		
		for (uint8 row = 0; row < m.rows; row++)
		{
			for (uint8 col = 0; col < m.columns; col++)
			{
				float val = m(col, row);
				result(row, col) = val;
			}
		}

		return result;
	}

private:
	uint8 columns;
	uint8 rows;
	std::vector<float> data;
};

inline std::ostream& operator<<(std::ostream& os, const Matrix& matrix)
{
	os << "(";
	for (int row = 0; row < matrix.Rows(); row++)
	{
		os << "[";
		for (int col = 0; col < matrix.Columns(); col++)
		{
			os << matrix(col, row);
			if (col < matrix.Columns() - 1)
				os << ",";
		}
		os << "]";
	}
	os << ")";

	return os;
}
