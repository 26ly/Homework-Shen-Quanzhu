#include <vector>
#include <iostream>
#include <cmath>
#include <random>
#include <opencv2/opencv.hpp>
#include "../include/matrix.h" 
using namespace std;
using namespace cv;

Matrix x(4,1); // 状态向量 [x,y,vx,vy]
Matrix P(4,4); // 协方差矩阵
Matrix F(4,4); // 状态转移矩阵
Matrix Q(4,4); // 过程噪声协方差矩阵
Matrix H(2,4); // 观测矩阵
Matrix R(2,2); // 观测噪声协方差矩阵
Matrix I(4,4); // 单位矩阵

void initKalmanFilter(double dt,double process_noise,double measurement_noise){
    // 状态转移矩阵 F
    // [1   0  dt    0]
    // [0   1   0   dt]
    // [0   0   1    0]
    // [0   0   0    1]
    for (int i=0;i<4;i++){
        F.set(i,i,1.0);
    }
    F.set(0,2,dt);
    F.set(1,3,dt);
    
    // 观测矩阵 H - 只能观测位置
    // [1 0 0 0]
    // [0 1 0 0]
    H.set(0,0,1.0);
    H.set(1,1,1.0);
    
    // 过程噪声协方差 Q
    for (int i=0;i<4;i++){
        Q.set(i,i,process_noise);
    }
    
    // 测量噪声协方差 R
    for (int i=0;i<2;i++){
        R.set(i,i,measurement_noise);
    }
    
    // 单位矩阵 I
    for (int i=0;i<4;i++){
        I.set(i,i,1.0);
    }
    
    // 初始状态 - 从原点开始
    x.set(0,0,0.0);  // x
    x.set(1,0,1.0);  // y (从(0,1)开始，即圆的上点)
    x.set(2,0,-1.0); // vx (向左运动)
    x.set(3,0,0.0);  // vy
    
    // 初始协方差 - 较大的不确定性
    for (int i=0;i<4;i++){
        P.set(i,i,1.0);
    }
}

// 预测步骤
void predict() {
    x=F.multiply(x);
    P=F.multiply(P).multiply(F.transpose()).add(Q);
}

// 更新步骤
void update(const Matrix& z) {
    Matrix y=z.subtract(H.multiply(x)); //测量残差
    Matrix S=H.multiply(P).multiply(H.transpose()).add(R); //残差协方差
    Matrix K=P.multiply(H.transpose()).multiply(S.inverse()); //卡尔曼增益
    x=x.add(K.multiply(y)); //更新状态估计
    P=I.subtract(K.multiply(H)).multiply(P); //更新协方差矩阵
}



// 生成圆周运动轨迹
void generateCircularTrajectory(int num_points,double radius, double period,vector<Matrix>& true_states,vector<Matrix>& measurements){
    true_states.clear();
    measurements.clear();
    
    // 随机数生成器
    default_random_engine generator;
    normal_distribution<double> noise_dist(0.0,2.0);
    normal_distribution<double> noise(0.0,0.1);
    
    double dt=period/num_points;
    double angular_velocity=2.0*M_PI/period;
    
    for (int i=0;i<num_points;i++){
        double t=i*dt;
        double angle=angular_velocity*t;
        
        // 真实状态 [x,y,vx,vy]^T
        double true_x=radius*cos(angle);
        double true_y=radius*sin(angle);
        double true_vx=-radius*angular_velocity*sin(angle)+noise(generator);
        double true_vy=radius*angular_velocity*cos(angle)+noise(generator);
        
        Matrix true_state(4,1);
        true_state.set(0,0,true_x);
        true_state.set(1,0,true_y);
        true_state.set(2,0,true_vx);
        true_state.set(3,0,true_vy);
        true_states.push_back(true_state);
        
        // 添加噪声的测量值 [x,y]^T
        double measured_x=true_x+noise_dist(generator);
        double measured_y=true_y+noise_dist(generator);
        
        Matrix measurement(2, 1);
        measurement.set(0,0,measured_x);
        measurement.set(1,0,measured_y);
        measurements.push_back(measurement);
    }
}

// 计算RMSE误差
double calculateRMSE(const vector<Point2f>& points1,const vector<Point2f>& points2) {
    double sum=0.0;
    int n=min(points1.size(),points2.size());
    for (int i=0;i<n;i++){
        double dx=points1[i].x-points2[i].x;
        double dy=points1[i].y-points2[i].y;
        sum+=dx*dx+dy*dy;
    }
    return sqrt(sum/n);
}

// 可视化轨迹
void visualize(const vector<Point2f>& true_traj,const vector<Point2f>& measured_traj,const vector<Point2f>& filtered_traj,const string& window_name){
    
    // 创建图像
    Mat img(600,600,CV_8UC3,Scalar(255,255,255));

    // 坐标变换参数
    double scale=20; // 缩放因子
    Point2f center(300, 300); // 图像中心
    
    // 绘制真实轨迹(蓝色)
    for (size_t i = 1; i < true_traj.size(); i++) {
        Point2f p1 = true_traj[i-1];
        Point2f p2 = true_traj[i];
        Point pt1(p1.x * scale + center.x, -p1.y * scale + center.y); // 注意：y轴方向翻转
        Point pt2(p2.x * scale + center.x, -p2.y * scale + center.y);
        line(img, pt1, pt2, Scalar(255, 0, 0), 2);
    }
    
    // 绘制测量点(绿色)
    for (size_t i = 0; i < measured_traj.size(); i++) {
        Point2f p = measured_traj[i];
        Point pt(p.x * scale + center.x, -p.y * scale + center.y);
        circle(img, pt, 3, Scalar(0, 255, 0), -1);
    }
    
    // 绘制滤波轨迹(红色)
    for (size_t i = 1; i < filtered_traj.size(); i++) {
        Point2f p1 = filtered_traj[i-1];
        Point2f p2 = filtered_traj[i];
        Point pt1(p1.x * scale + center.x, -p1.y * scale + center.y);
        Point pt2(p2.x * scale + center.x, -p2.y * scale + center.y);
        line(img, pt1, pt2, Scalar(0, 0, 255), 2);
    }
    
    // 绘制坐标轴
    line(img, Point(center.x - 250, center.y), Point(center.x + 250, center.y), Scalar(0, 0, 0), 1);
    line(img, Point(center.x, center.y - 250), Point(center.x, center.y + 250), Scalar(0, 0, 0), 1);
    
    // 绘制图例
    putText(img, "True Trajectory", Point(20, 30), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 0, 0), 2);
    putText(img, "Measurements", Point(20, 60), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 0), 2);
    putText(img, "Kalman Filter", Point(20, 90), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2);
    
    // 显示图像
    imshow(window_name, img);
}

int main() {

    int num_points=200; // 轨迹点数
    double radius=10.0; // 圆半径
    double period=20.0; // 周期（秒
    double dt=period/num_points; // 时间步长
    
    vector<Matrix> true_states;
    vector<Matrix> measurements;
    generateCircularTrajectory(num_points,radius,period,true_states,measurements);

    vector<Point2f> true_traj,measured_traj,filtered_traj;
    
    for (int i=0;i<num_points;i++){

        // 真实轨迹
        true_traj.push_back(Point2f(true_states[i].get(0,0),true_states[i].get(1,0)));
        
        // 测量轨迹
        measured_traj.push_back(Point2f(measurements[i].get(0,0),measurements[i].get(1,0)));

    }

    vector<pair<double, double>> param_sets = {
        {1e-5, 1e-1},  // 小过程噪声，大测量噪声
        {1e-1, 1e-5},  // 大过程噪声，小测量噪声
        {1e-3, 1e-3}   // 平衡参数
    };
    
    vector<string> param_names = {
        "Small Q, Large R",
        "Large Q, Small R",
        "Balanced Q and R"
    };

    for (int param_idx = 0; param_idx < param_sets.size(); param_idx++) {
        // 重置滤波器和轨迹
        initKalmanFilter(dt,param_sets[param_idx].first,param_sets[param_idx].second);
        filtered_traj.clear();
        
        // 运行卡尔曼滤波
        for (int i=0;i<num_points;i++){
            // 预测
            predict();
            
            // 更新
            update(measurements[i]);
            
            // 保存滤波后的位置
            filtered_traj.push_back(Point2f(x.get(0, 0), x.get(1, 0)));
        }

        // 计算误差
        double measurement_error=calculateRMSE(true_traj,measured_traj);
        double filter_error=calculateRMSE(true_traj,filtered_traj);
        
        cout << "=== Parameter Set " << param_idx + 1 << " ===" << endl;
        cout << "Description: " << param_names[param_idx] << endl;
        cout << "Q = " << param_sets[param_idx].first << ", R = " << param_sets[param_idx].second << endl;
        cout << "Measurement RMSE: " << measurement_error << endl;
        cout << "Filtered RMSE: " << filter_error << endl;
        cout << "Improvement: " << (measurement_error - filter_error) / measurement_error * 100 << "%" << endl;
        cout << endl;
        
        // 可视化
        string window_name = "Kalman Filter: " + param_names[param_idx];
        namedWindow(window_name, WINDOW_AUTOSIZE);
        visualize(true_traj, measured_traj, filtered_traj, window_name);
    }

    // 等待按键
    cout << "Press any key to exit..." << endl;
    waitKey(0);
    
    
    return 0;
}