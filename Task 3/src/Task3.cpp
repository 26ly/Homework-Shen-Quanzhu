#include<bits/stdc++.h>
using namespace std;
const double dt=0.01;  //德尔塔t=10ms
const int steps=100;  //步数（按1s来的)

struct Point{
    double x;
    double y;
};

int main(){
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<> noise2(0.0, 0.5);
    normal_distribution<> noise3(0.0, 0.1);
    
    Point true_pos={0.0,0.0};
    Point true_vel={2.0,3.0};
    vector<Point> true_positions;
    vector<Point> obs_positions;  //观测噪声的模拟
    
    for (int i=0;i<steps;i++) {
        true_positions.push_back(true_pos);
        obs_positions.push_back({true_pos.x+noise2(gen),true_pos.y+noise2(gen)});  //加观测噪声
        true_vel.x+=noise3(gen);
        true_vel.y+=noise3(gen);  //加过程噪声
        true_pos.x+=true_vel.x*dt;
        true_pos.y+=true_vel.y*dt;  //计算位置
    }
    
    for (int i=0;i<steps;i++) {
        cout<<i*dt <<" "<<obs_positions[i].x<<" "<<obs_positions[i].y<<endl;  //按照时间，x，y的顺序输出
    }
    return 0;
}