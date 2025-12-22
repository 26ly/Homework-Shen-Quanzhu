#pragma once
#include "types.h"
#include <opencv2/dnn.hpp>

class Detector {
public:
    Detector();
    
    // 初始化检测器（使用OpenCV DNN）
    bool init(const std::string& model_path, 
              const std::string& config_path = "",
              const std::string& classes_path = "",
              bool use_gpu = false);
    
    // 检测单帧图像
    DetectionList detect(const cv::Mat& frame);
    
    // 设置参数
    void setConfidenceThreshold(float conf_thresh) { 
        conf_threshold_ = conf_thresh; 
    }
    
    void setNMSThreshold(float nms_thresh) { 
        nms_threshold_ = nms_thresh; 
    }

private:
    // 预处理
    cv::Mat preprocess(const cv::Mat& frame);
    
    // 后处理
    DetectionList postprocess(
        const std::vector<cv::Mat>& outputs,
        const cv::Size& original_size);
    
    // 计算IoU
    float calculateIoU(const cv::Rect& a, const cv::Rect& b) const;
    
    // 非极大值抑制
    std::vector<int> nms(const std::vector<cv::Rect>& boxes,
                        const std::vector<float>& scores) const;
    
    // OpenCV DNN网络
    cv::dnn::Net net_;
    
    // 参数
    float conf_threshold_ = 0.5f;
    float nms_threshold_ = 0.45f;
    
    // 模型输入大小
    cv::Size input_size_ = cv::Size(640, 640);
    
    // 类别信息
    std::vector<std::string> class_names_;
    int target_class_id_ = -1; // 奶龙的类别ID
};