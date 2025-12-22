#include "Visualization.h"
#include <iomanip>
#include <sstream>

void Visualizer::drawDetections(cv::Mat& frame,
                               const DetectionList& detections,
                               bool draw_mask) {
    for (const auto& det : detections) {
        // 绘制边界框
        cv::rectangle(frame, det.bbox, cv::Scalar(0, 255, 0), 2);
        
        // 绘制中心点
        cv::circle(frame, det.centroid, 3, cv::Scalar(0, 255, 255), -1);
        
        // 绘制置信度
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << det.confidence;
        cv::putText(frame, ss.str(),
                   cv::Point(det.bbox.x, det.bbox.y - 5),
                   cv::FONT_HERSHEY_SIMPLEX, 0.5,
                   cv::Scalar(0, 255, 0), 1);
        
        // 绘制分割掩码（如果可用）
        if (draw_mask && !det.mask.empty()) {
            cv::Mat colored_mask;
            det.mask.convertTo(colored_mask, CV_8UC1, 255);
            cv::applyColorMap(colored_mask, colored_mask, cv::COLORMAP_JET);
            
            // 叠加到原图
            cv::addWeighted(frame, 0.7, colored_mask, 0.3, 0, frame);
        }
    }
}

void Visualizer::drawTracks(cv::Mat& frame,
                           const TrackResultList& tracks,
                           const std::vector<cv::Point2f>& trajectory) {
    for (const auto& track : tracks) {
        cv::Scalar color = getColor(track.id);
        
        // 绘制当前位置
        cv::circle(frame, track.position, 5, color, -1);
        
        // 绘制预测位置
        cv::circle(frame, track.predicted_position, 5,
                  cv::Scalar(255, 0, 0), -1);
        
        // 绘制预测边界框
        cv::rectangle(frame, track.predicted_bbox,
                     cv::Scalar(255, 0, 0), 1);
        
        // 绘制ID和速度
        std::stringstream ss;
        ss << "ID:" << track.id
           << " V:(" << std::fixed << std::setprecision(1)
           << track.velocity.x << "," << track.velocity.y << ")";
        
        cv::putText(frame, ss.str(),
                   cv::Point(track.position.x + 10, track.position.y - 10),
                   cv::FONT_HERSHEY_SIMPLEX, 0.5, color, 1);
        
        // 绘制速度方向
        cv::Point2f velocity_end = track.position + track.velocity * 10;
        cv::arrowedLine(frame, track.position, velocity_end,
                       cv::Scalar(255, 255, 0), 2);
        
        // 绘制预测轨迹线
        cv::arrowedLine(frame, track.position, track.predicted_position,
                       cv::Scalar(0, 255, 255), 2, cv::LINE_AA, 0, 0.3);
    }
    
    // 绘制历史轨迹
    if (!trajectory.empty()) {
        drawTrajectory(frame, trajectory);
    }
}

void Visualizer::drawTrajectory(cv::Mat& frame,
                               const std::vector<cv::Point2f>& points,
                               const cv::Scalar& color) {
    for (size_t i = 1; i < points.size(); i++) {
        cv::line(frame, points[i-1], points[i], color, 2, cv::LINE_AA);
    }
}

void Visualizer::drawInfoPanel(cv::Mat& frame,
                              int frame_count,
                              double fps,
                              int num_tracks,
                              int num_detections) {
    // 创建信息面板背景
    cv::Rect panel_rect(10, 10, 300, 100);
    cv::Mat panel = frame(panel_rect).clone();
    cv::rectangle(frame, panel_rect, cv::Scalar(0, 0, 0), -1);
    cv::addWeighted(frame(panel_rect), 0.3, panel, 0.7, 0, frame(panel_rect));
    
    // 绘制边框
    cv::rectangle(frame, panel_rect, cv::Scalar(255, 255, 255), 1);
    
    // 绘制信息
    std::stringstream ss;
    ss << "Frame: " << frame_count << "\n"
       << "FPS: " << std::fixed << std::setprecision(1) << fps << "\n"
       << "Tracks: " << num_tracks << "\n"
       << "Detections: " << num_detections;
    
    int y_offset = 30;
    std::string line;
    std::istringstream iss(ss.str());
    
    while (std::getline(iss, line)) {
        cv::putText(frame, line,
                   cv::Point(20, y_offset),
                   cv::FONT_HERSHEY_SIMPLEX, 0.6,
                   cv::Scalar(255, 255, 255), 1);
        y_offset += 25;
    }
}

bool Visualizer::initVideoWriter(const std::string& filename,
                                cv::Size frame_size,
                                double fps) {
    video_writer_.open(filename,
                      cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
                      fps, frame_size);
    
    is_writer_initialized_ = video_writer_.isOpened();
    return is_writer_initialized_;
}

void Visualizer::writeFrame(const cv::Mat& frame) {
    if (is_writer_initialized_) {
        video_writer_ << frame;
    }
}

void Visualizer::releaseVideoWriter() {
    if (is_writer_initialized_) {
        video_writer_.release();
        is_writer_initialized_ = false;
    }
}

cv::Scalar Visualizer::getColor(int id) const {
    auto it = color_cache_.find(id);
    if (it != color_cache_.end()) {
        return it->second;
    }
    
    // 生成可区分的颜色
    int r = (id * 67) % 256;
    int g = (id * 43) % 256;
    int b = (id * 89) % 256;
    
    cv::Scalar color(b, g, r);
    color_cache_[id] = color;
    
    return color;
}