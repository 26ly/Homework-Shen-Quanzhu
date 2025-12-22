#pragma once
#include "types.h"
#include <opencv2/opencv.hpp>

class Visualizer {
public:
    Visualizer() = default;
    
    // 绘制检测结果
    void drawDetections(cv::Mat& frame, 
                       const DetectionList& detections,
                       bool draw_mask = false);
    
    // 绘制追踪结果
    void drawTracks(cv::Mat& frame, 
                   const TrackResultList& tracks,
                   const std::vector<cv::Point2f>& trajectory = {});
    
    // 绘制预测轨迹
    void drawTrajectory(cv::Mat& frame,
                       const std::vector<cv::Point2f>& points,
                       const cv::Scalar& color = cv::Scalar(255, 255, 0));
    
    // 绘制信息面板
    void drawInfoPanel(cv::Mat& frame,
                      int frame_count,
                      double fps,
                      int num_tracks,
                      int num_detections);
    
    // 保存视频
    bool initVideoWriter(const std::string& filename,
                        cv::Size frame_size,
                        double fps = 30.0);
    
    void writeFrame(const cv::Mat& frame);
    void releaseVideoWriter();

private:
    cv::VideoWriter video_writer_;
    bool is_writer_initialized_ = false;
    
    // 颜色表（为不同ID分配不同颜色）
    cv::Scalar getColor(int id) const;
    mutable std::unordered_map<int, cv::Scalar> color_cache_;
};