#include "Detector.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iostream>

Detector::Detector() {
    // 构造函数
}

bool Detector::init(const std::string& model_path,
                   const std::string& config_path,
                   const std::string& classes_path,
                   bool use_gpu) {
    try {
        // 加载模型
        if (config_path.empty()) {
            // ONNX模型
            net_ = cv::dnn::readNet(model_path);
        } else {
            // TensorFlow/PyTorch等模型
            net_ = cv::dnn::readNet(model_path, config_path);
        }
        
        if (net_.empty()) {
            std::cerr << "Failed to load model: " << model_path << std::endl;
            return false;
        }
        
        // 设置后端（CPU/GPU）
        if (use_gpu) {
            net_.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
            net_.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
        } else {
            net_.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
            net_.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        }
        
        // 加载类别名称
        if (!classes_path.empty()) {
            std::ifstream file(classes_path);
            std::string line;
            while (std::getline(file, line)) {
                class_names_.push_back(line);
            }
            
            // 查找奶龙类别
            // for (size_t i = 0; i < class_names_.size(); i++) {
            //     if (class_names_[i].find("dragon") != std::string::npos ||
            //         class_names_[i].find("nai_long") != std::string::npos ||
            //         class_names_[i].find("奶龙") != std::string::npos) {
            //         target_class_id_ = static_cast<int>(i);
            //         break;
            //     }
            // }

            target_class_id_ = -1; 

        }
        
        // 使用固定的YOLO输入尺寸
        // YOLOv5/v8通常使用640x640
        input_size_ = cv::Size(640, 640);
        
        std::cout << "Detector initialized with input size: " 
                  << input_size_.width << "x" << input_size_.height << std::endl;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize detector: " << e.what() << std::endl;
        return false;
    }
}

DetectionList Detector::detect(const cv::Mat& frame) {
    DetectionList detections;
    
    try {
        // 预处理
        cv::Mat blob = preprocess(frame);
        
        // 设置输入
        net_.setInput(blob);
        
        // 获取输出层名称
        std::vector<std::string> output_names = net_.getUnconnectedOutLayersNames();
        
        // 前向传播
        std::vector<cv::Mat> outputs;
        net_.forward(outputs, output_names);
        
        // 后处理
        detections = postprocess(outputs, frame.size());
        
    } catch (const std::exception& e) {
        std::cerr << "Detection failed: " << e.what() << std::endl;
    }
    
    return detections;
}

cv::Mat Detector::preprocess(const cv::Mat& frame) {
    cv::Mat blob;
    
    // YOLO格式的预处理
    // 注意：blobFromImage会自动进行尺寸调整、归一化和通道交换
    cv::dnn::blobFromImage(frame, blob, 
                          1.0/255.0,           // 缩放因子
                          input_size_,         // 目标大小
                          cv::Scalar(0, 0, 0), // 均值
                          true,                // 交换RB通道
                          false,               // 不裁剪
                          CV_32F);             // 输出类型
    
    return blob;
}

DetectionList Detector::postprocess(
    const std::vector<cv::Mat>& outputs,
    const cv::Size& original_size) {
    
    DetectionList detections;
    
    if (outputs.empty()) return detections;
    
    const cv::Mat& output = outputs[0];
    
    // 获取输出维度信息
    int dimensions = output.size[1];  // 每行的列数
    int rows = output.size[2];        // 行数
    
    std::cout << "Output dimensions: " << dimensions 
              << ", rows: " << rows << std::endl;
    
    // 解析YOLO输出
    std::vector<cv::Rect> boxes;
    std::vector<float> scores;
    std::vector<int> class_ids;
    
    // 遍历所有检测
    for (int i = 0; i < rows; i++) {
        // 获取当前行的指针
        const float* data = output.ptr<float>(0, 0, i);
        
        // 获取边界框坐标（已经归一化到0-1）
        float center_x = data[0];
        float center_y = data[1];
        float width = data[2];
        float height = data[3];
        
        // 转换为像素坐标
        float x1 = (center_x - width / 2) * original_size.width;
        float y1 = (center_y - height / 2) * original_size.height;
        float w = width * original_size.width;
        float h = height * original_size.height;
        
        // 确保坐标在图像范围内
        if (x1 < 0) x1 = 0;
        if (y1 < 0) y1 = 0;
        if (x1 + w > original_size.width) w = original_size.width - x1;
        if (y1 + h > original_size.height) h = original_size.height - y1;
        
        // 获取类别置信度
        float max_confidence = 0;
        int class_id = -1;
        
        // YOLOv8: 数据格式为 [x, y, w, h, cls1_conf, cls2_conf, ...]
        // 从第5个元素开始是类别置信度
        for (int j = 4; j < dimensions; j++) {
            if (data[j] > max_confidence) {
                max_confidence = data[j];
                class_id = j - 4;  // 类别索引从0开始
            }
        }
        
        // 过滤低置信度检测
        if (max_confidence < conf_threshold_) continue;
        
        // 如果指定了目标类别，只保留该类别
        if (target_class_id_ >= 0 && class_id != target_class_id_) continue;
        
        // 如果边界框有效，添加到列表
        if (w > 0 && h > 0) {
            boxes.emplace_back(x1, y1, w, h);
            scores.push_back(max_confidence);
            class_ids.push_back(class_id);
        }
    }
    
    // 应用NMS
    std::vector<int> indices = nms(boxes, scores);
    
    // 创建检测结果
    for (int idx : indices) {
        Detection det;
        det.bbox = boxes[idx];
        det.centroid = cv::Point2f(
            boxes[idx].x + boxes[idx].width / 2,
            boxes[idx].y + boxes[idx].height / 2
        );
        det.confidence = scores[idx];
        
        detections.push_back(det);
        
        // 调试输出
        if (detections.size() == 1) {
            std::cout << "First detection - Confidence: " << det.confidence 
                      << ", BBox: " << det.bbox << std::endl;
        }
    }
    
    return detections;
}

std::vector<int> Detector::nms(const std::vector<cv::Rect>& boxes,
                              const std::vector<float>& scores) const {
    std::vector<int> indices;
    
    if (boxes.empty()) return indices;
    
    // 按置信度排序
    std::vector<int> idxs(boxes.size());
    std::iota(idxs.begin(), idxs.end(), 0);
    
    std::sort(idxs.begin(), idxs.end(),
        [&scores](int i, int j) { return scores[i] > scores[j]; });
    
    while (!idxs.empty()) {
        int best = idxs.front();
        indices.push_back(best);
        
        std::vector<int> rest;
        for (size_t i = 1; i < idxs.size(); i++) {
            float iou = calculateIoU(boxes[best], boxes[idxs[i]]);
            if (iou < nms_threshold_) {
                rest.push_back(idxs[i]);
            }
        }
        
        idxs = std::move(rest);
    }
    
    return indices;
}

float Detector::calculateIoU(const cv::Rect& a, const cv::Rect& b) const {
    int x1 = std::max(a.x, b.x);
    int y1 = std::max(a.y, b.y);
    int x2 = std::min(a.x + a.width, b.x + b.width);
    int y2 = std::min(a.y + a.height, b.y + b.height);
    
    if (x2 <= x1 || y2 <= y1) {
        return 0.0f;
    }
    
    float intersection = (x2 - x1) * (y2 - y1);
    float area_a = a.width * a.height;
    float area_b = b.width * b.height;
    
    return intersection / (area_a + area_b - intersection);
}