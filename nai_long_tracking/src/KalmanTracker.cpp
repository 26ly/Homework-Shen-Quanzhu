#include "KalmanTracker.h"
#include <iostream>

KalmanTracker::KalmanTracker(int track_id, const cv::Rect& initial_bbox)
    : track_id_(track_id), hits_(1), age_(0), time_since_update_(0) {
    initKalmanFilter(initial_bbox);
}

void KalmanTracker::initKalmanFilter(const cv::Rect& bbox) {
    kf_.init(STATE_DIM, MEAS_DIM, 0);
    
    // 状态转移矩阵（匀速模型）
    cv::setIdentity(kf_.transitionMatrix);
    kf_.transitionMatrix.at<float>(0, 2) = 1.0f;
    kf_.transitionMatrix.at<float>(1, 3) = 1.0f;
    kf_.transitionMatrix.at<float>(2, 4) = 0.0f;
    kf_.transitionMatrix.at<float>(3, 5) = 0.0f;
    
    // 测量矩阵
    kf_.measurementMatrix = cv::Mat::zeros(MEAS_DIM, STATE_DIM, CV_32F);
    kf_.measurementMatrix.at<float>(0, 0) = 1.0f;
    kf_.measurementMatrix.at<float>(1, 1) = 1.0f;
    kf_.measurementMatrix.at<float>(2, 4) = 1.0f;
    kf_.measurementMatrix.at<float>(3, 5) = 1.0f;
    
    // 过程噪声协方差
    cv::setIdentity(kf_.processNoiseCov, cv::Scalar::all(1e-2));
    
    // 测量噪声协方差
    cv::setIdentity(kf_.measurementNoiseCov, cv::Scalar::all(1e-1));
    
    // 后验误差协方差
    cv::setIdentity(kf_.errorCovPost, cv::Scalar::all(1));
    
    // 初始化状态
    float center_x = bbox.x + bbox.width / 2.0f;
    float center_y = bbox.y + bbox.height / 2.0f;
    
    kf_.statePost.at<float>(0) = center_x;
    kf_.statePost.at<float>(1) = center_y;
    kf_.statePost.at<float>(2) = 0.0f;  // vx
    kf_.statePost.at<float>(3) = 0.0f;  // vy
    kf_.statePost.at<float>(4) = bbox.width;
    kf_.statePost.at<float>(5) = bbox.height;
}

cv::Mat KalmanTracker::predict() {
    cv::Mat prediction = kf_.predict();
    age_++;
    time_since_update_++;
    return prediction;
}

void KalmanTracker::update(const cv::Rect& bbox) {
    cv::Mat measurement = bboxToMeasurement(bbox);
    kf_.correct(measurement);
    time_since_update_ = 0;
    hits_++;
}

TrackResult KalmanTracker::getState(){
    const cv::Mat& state = kf_.statePost;
    
    TrackResult result;
    result.id = track_id_;
    result.position = cv::Point2f(state.at<float>(0), state.at<float>(1));
    result.velocity = cv::Point2f(state.at<float>(2), state.at<float>(3));
    result.predicted_position = getPredictedPosition();
    result.predicted_bbox = stateToBbox(state);
    result.age = age_;
    result.hits = hits_;
    
    return result;
}

cv::Point2f KalmanTracker::getPredictedPosition(){
    cv::Mat prediction = kf_.predict();
    return cv::Point2f(prediction.at<float>(0), prediction.at<float>(1));
}

cv::Rect KalmanTracker::stateToBbox(const cv::Mat& state) const {
    float center_x = state.at<float>(0);
    float center_y = state.at<float>(1);
    float width = state.at<float>(4);
    float height = state.at<float>(5);
    
    return cv::Rect(
        static_cast<int>(center_x - width / 2),
        static_cast<int>(center_y - height / 2),
        static_cast<int>(width),
        static_cast<int>(height)
    );
}

cv::Mat KalmanTracker::bboxToMeasurement(const cv::Rect& bbox) const {
    cv::Mat measurement = cv::Mat::zeros(MEAS_DIM, 1, CV_32F);
    
    float center_x = bbox.x + bbox.width / 2.0f;
    float center_y = bbox.y + bbox.height / 2.0f;
    
    measurement.at<float>(0) = center_x;
    measurement.at<float>(1) = center_y;
    measurement.at<float>(2) = bbox.width;
    measurement.at<float>(3) = bbox.height;
    
    return measurement;
}