#include<iostream>
#include<vector>
#include<cmath>

using std::cin;

using std::cout;
using std::endl;
using std::vector;

struct matrix{
    vector<vector<double>> a;
    int length,width;
};
matrix n,m;

void scan(matrix &x){  //输入坐标
    x.a.resize(3);
    for(int i=0;i<3;i++)
        x.a[i].resize(1);
    x.a[2][0]=1;
    x.length=3;
    x.width=1;
    cin>>x.a[0][0]>>x.a[1][0];
}

void print(matrix &x){  //输出
    printf("%.2lf %.2lf",x.a[0][0],x.a[1][0]);
    cout<<endl;
}

matrix matrix_multiplication(matrix x,matrix y){  //矩阵乘法
    matrix result;
    if(x.width==y.length){
        result.a.resize(x.length);
        for(int i=0;i<x.length;i++){
            result.a[i].resize(y.width);
        }
        result.length=x.length;
        result.width=y.width;
        for(int i=0;i<x.length;i++){
            for(int j=0;j<y.width;j++){
                result.a[i][j]=0;
                for(int k=0;k<x.width;k++){
                    // for(int l=0;l<y.length;l++)
                        result.a[i][j]+=x.a[i][k]*y.a[k][j];
                }
            }
        }
        return result;
    }
}

matrix rotation_matrix(double c,double x,double y){
    matrix result;
    result.length=3;
    result.width=3;
    result.a.resize(3);
    for(int i=0;i<3;i++)
        result.a[i].resize(3);
    result.a[0][0]=cos(c);
    result.a[0][1]=-sin(c);
    result.a[0][2]=x;
    result.a[1][0]=sin(c);
    result.a[1][1]=cos(c);
    result.a[1][2]=y;
    result.a[2][0]=0;
    result.a[2][1]=0;
    result.a[2][2]=1;
    return result;
}

int main(){
    scan(n);  //二维原向量
    double c,x,y;  //旋转角和平移向量
    cin>>c>>x>>y;
    matrix b;
    b=rotation_matrix(c,x,y);
    n=matrix_multiplication(b,n);
    print(n);
    return 0;
}