#include "../include/matrix.h"
#include <vector>
using std::vector;


Matrix::Matrix(int r,int c):rows(r),cols(c){
        data.resize(r,vector<double> (c,0.0));
    }
Matrix Matrix::multiply(const Matrix& other){
    Matrix result(rows,other.cols);
    for (int i=0;i<rows;i++){
        for (int j=0;j<other.cols;j++){
            for (int k=0;k<cols;k++){
                result.data[i][j]+=data[i][k]*other.data[k][j];
            }
        }
    }
    return result; 
}
Matrix Matrix::add(const Matrix& other){
    Matrix result(rows,cols);
    for (int i=0;i<rows;i++){
        for (int j=0;j<cols;j++){
            result.data[i][j]=data[i][j]+other.data[i][j];
        }
    }
    return result;
}
Matrix Matrix::transpose(){
    Matrix result(cols,rows);
    for (int i=0;i<rows;i++){
        for (int j=0;j<cols;j++){
            result.data[j][i]=data[i][j];
        }
    }
    return result;
}
Matrix Matrix::inverse(){
    int n=rows;
    Matrix result(n,2*n);
    for (int i=0;i<n;i++){
        for (int j=0;j<n;j++){
            result.data[i][j]=data[i][j];
        }
        result.data[i][i+n]=1.0;
    }
    for (int i=0;i<n;i++){
        double diag=result.data[i][i];
        for (int j=0;j<2*n;j++){
            result.data[i][j]/=diag;
        }
        for (int k=0;k<n;k++){
            if (k!=i){
                double factor=result.data[k][i];
                for (int j=0;j<2*n;j++){
                    result.data[k][j]-=factor*result.data[i][j];
                }
            }
        }
    }
    Matrix inv(n,n);
    for (int i=0;i<n;i++){
        for (int j=0;j<n;j++){
            inv.data[i][j]=result.data[i][j+n];
        }
    }
    return inv;
}
Matrix Matrix::subtract(const Matrix& other){
    Matrix result(rows,cols);
    for (int i=0;i<rows;i++){
        for (int j=0;j<cols;j++){
            result.data[i][j]=data[i][j]-other.data[i][j];
        }
    }
    return result;
}
Matrix Matrix::subtract(const Matrix& other)const{
    Matrix result(rows,cols);
    for (int i=0;i<rows;i++){
        for (int j=0;j<cols;j++){
            result.data[i][j]=data[i][j]-other.data[i][j];
        }
    }
    return result;
}
void Matrix::set(int r,int c,double val){
    data[r][c]=val;
}
double Matrix::get(int r,int c) const{
    return data[r][c];
}
int Matrix::getRows() const{
    return rows;
}
int Matrix::getCols() const{
    return cols;
}