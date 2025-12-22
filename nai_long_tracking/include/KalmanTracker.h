#pragma once
#include "types.h"
#include <opencv2/opencv.hpp>

class KalmanTracker {
public:
    KalmanTracker(int track_id, const cv::Rect& initial_bbox);
    
    // 预测下一帧状态
    cv::Mat predict();
    
    // 用观测值更新
    void update(const cv::Rect& bbox);
    
    // 获取当前状态
    TrackResult getState();
    
    // 获取预测的下一个位置
    cv::Point2f getPredictedPosition();
    
    // 获取追踪器ID
    int getID() const { return track_id_; }
    
    // 获取最后更新时间
    int getTimeSinceUpdate() const { return time_since_update_; }
    
    // 增加未匹配次数
    void markMissed() { time_since_update_++; }
    
    // 获取命中次数
    int getHits() const { return hits_; }

private:
    void initKalmanFilter(const cv::Rect& bbox);
    cv::Rect stateToBbox(const cv::Mat& state) const;
    cv::Mat bboxToMeasurement(const cv::Rect& bbox) const;
    
    cv::KalmanFilter kf_;
    int track_id_;
    int hits_;
    int age_;
    int time_since_update_;
    
    // 状态维度: [x, y, vx, vy, w, h]
    static const int STATE_DIM = 6;
    static const int MEAS_DIM = 4;
};