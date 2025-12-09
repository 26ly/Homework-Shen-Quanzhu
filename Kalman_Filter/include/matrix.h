#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
using std::vector;

//自定义矩阵类
class Matrix {
    vector<vector<double>> data;
    int rows,cols;
public:
    Matrix(int r,int c);
    Matrix multiply(const Matrix& other);
    Matrix add(const Matrix& other);
    Matrix transpose();
    Matrix inverse(); // Gauss-Jordan
    Matrix subtract(const Matrix& other);
    Matrix subtract(const Matrix& other)const;
    void set(int r,int c,double val);
    double get(int r,int c) const;
    int getRows() const;
    int getCols() const;
};

#endif