#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <memory>

struct Detection {
    cv::Rect bbox;          // 边界框
    cv::Point2f centroid;   // 中心点
    cv::Mat mask;           // 分割掩码（可选）
    float confidence;       // 置信度
    std::vector<cv::Point2f> keypoints; // 关键点（可选）
};

struct TrackResult {
    int id;                 // 追踪ID
    cv::Point2f position;   // 当前位置
    cv::Point2f velocity;   // 速度
    cv::Point2f predicted_position; // 预测位置
    cv::Rect predicted_bbox; // 预测边界框
    int age;               // 存在时间
    int hits;              // 成功追踪次数
};

using DetectionList = std::vector<Detection>;
using TrackResultList = std::vector<TrackResult>;