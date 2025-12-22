#include "MultiTracker.h"
#include <algorithm>
#include <limits>
#include <iostream>

// 匈牙利算法实现
namespace {
    class HungarianAlgorithm {
    public:
        double solve(const cv::Mat& cost_matrix,
                     std::vector<int>& assignment) {
            int n = cost_matrix.rows;
            int m = cost_matrix.cols;
            
            // 确保方阵
            int size = std::max(n, m);
            cv::Mat cost(size, size, CV_64F, cv::Scalar(0));
            
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    cost.at<double>(i, j) = cost_matrix.at<float>(i, j);
                }
            }
            
            assignment.assign(size, -1);
            
            // 简单贪心匹配
            return solveMinCost(cost, assignment, n, m);
        }
        
    private:
        double solveMinCost(const cv::Mat& cost,
                           std::vector<int>& assignment,
                           int n, int m) {
            std::vector<bool> row_covered(n, false);
            std::vector<bool> col_covered(m, false);
            assignment.assign(n, -1);
            
            double total_cost = 0.0;
            
            // 简单贪心匹配
            for (int i = 0; i < n; i++) {
                if (row_covered[i]) continue;
                
                double min_cost = std::numeric_limits<double>::max();
                int best_j = -1;
                
                for (int j = 0; j < m; j++) {
                    if (col_covered[j]) continue;
                    
                    double current_cost = cost.at<double>(i, j);
                    if (current_cost < min_cost) {
                        min_cost = current_cost;
                        best_j = j;
                    }
                }
                
                if (best_j != -1) {
                    assignment[i] = best_j;
                    row_covered[i] = true;
                    col_covered[best_j] = true;
                    total_cost += min_cost;
                }
            }
            
            return total_cost;
        }
    };
}

MultiTracker::MultiTracker(int max_age, int min_hits)
    : max_age_(max_age), min_hits_(min_hits) {}

TrackResultList MultiTracker::update(const DetectionList& detections) {
    // 1. 预测所有追踪器的下一帧位置
    for (auto& kv : trackers_) {
        kv.second->predict();
    }
    
    // 2. 数据关联
    auto match_result = hungarianMatching(detections);
    
    // 3. 更新匹配的追踪器
    for (const auto& match : match_result.matches) {
        int det_idx = match.first;
        int trk_id = match.second;
        
        auto it = trackers_.find(trk_id);
        if (it != trackers_.end()) {
            it->second->update(detections[det_idx].bbox);
        }
    }
    
    // 4. 为未匹配的检测创建新追踪器
    for (int det_idx : match_result.unmatched_detections) {
        createNewTracker(detections[det_idx]);
    }
    
    // 5. 删除丢失的追踪器
    removeLostTrackers();
    
    // 6. 收集结果
    TrackResultList results;
    for (const auto& kv : trackers_) {
        if (kv.second->getHits() >= min_hits_) {
            results.push_back(kv.second->getState());
        }
    }
    
    return results;
}

MultiTracker::MatchResult MultiTracker::hungarianMatching(
    const DetectionList& detections) {
    
    MatchResult result;
    
    if (trackers_.empty()) {
        for (size_t i = 0; i < detections.size(); i++) {
            result.unmatched_detections.push_back(i);
        }
        return result;
    }
    
    // 计算成本矩阵（IoU距离）
    cv::Mat cost_matrix = computeCostMatrix(detections);
    
    // 使用匈牙利算法
    HungarianAlgorithm hungarian;
    std::vector<int> assignment;
    hungarian.solve(cost_matrix, assignment);
    
    // 将tracker_的键转换为向量以便索引
    std::vector<int> tracker_ids;
    for (const auto& kv : trackers_) {
        tracker_ids.push_back(kv.first);
    }
    
    // 处理匹配结果
    for (size_t i = 0; i < assignment.size(); i++) {
        if (i >= detections.size()) break;
        
        if (assignment[i] != -1 && static_cast<size_t>(assignment[i]) < tracker_ids.size()) {
            // 检查IoU是否超过阈值
            float iou = 1.0f - cost_matrix.at<float>(i, assignment[i]); // 成本转回IoU
            if (iou > matching_threshold_) {
                int tracker_id = tracker_ids[assignment[i]];
                result.matches.emplace_back(i, tracker_id);
            } else {
                result.unmatched_detections.push_back(i);
                result.unmatched_trackers.push_back(tracker_ids[assignment[i]]);
            }
        } else {
            result.unmatched_detections.push_back(i);
        }
    }
    
    // 找出未匹配的追踪器
    std::vector<int> all_tracker_ids;
    for (const auto& kv : trackers_) {
        all_tracker_ids.push_back(kv.first);
    }
    
    for (int tracker_id : all_tracker_ids) {
        bool matched = false;
        for (const auto& match : result.matches) {
            if (match.second == tracker_id) {
                matched = true;
                break;
            }
        }
        
        if (!matched) {
            result.unmatched_trackers.push_back(tracker_id);
        }
    }
    
    return result;
}

cv::Mat MultiTracker::computeCostMatrix(const DetectionList& detections) const {
    size_t n = detections.size();
    size_t m = trackers_.size();
    
    cv::Mat cost_matrix(n, m, CV_32F, cv::Scalar(0));
    
    // 将tracker转换为向量以便索引
    std::vector<std::pair<int, std::shared_ptr<KalmanTracker>>> tracker_list;
    for (const auto& kv : trackers_) {
        tracker_list.emplace_back(kv.first, kv.second);
    }
    
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < m; j++) {
            auto track_result = tracker_list[j].second->getState();
            cv::Rect pred_bbox = track_result.predicted_bbox;
            
            // 计算IoU
            float iou = calculateIoU(detections[i].bbox, pred_bbox);
            cost_matrix.at<float>(i, j) = 1.0f - iou; // 转换为成本
        }
    }
    
    return cost_matrix;
}

void MultiTracker::createNewTracker(const Detection& detection) {
    auto tracker = std::make_shared<KalmanTracker>(next_id_++, detection.bbox);
    trackers_[tracker->getID()] = tracker;
}

void MultiTracker::removeLostTrackers() {
    std::vector<int> to_remove;
    
    for (const auto& kv : trackers_) {
        if (kv.second->getTimeSinceUpdate() > max_age_) {
            to_remove.push_back(kv.first);
        }
    }
    
    for (int id : to_remove) {
        trackers_.erase(id);
    }
}

float MultiTracker::calculateIoU(const cv::Rect& a, const cv::Rect& b) const {
    int x1 = std::max(a.x, b.x);
    int y1 = std::max(a.y, b.y);
    int x2 = std::min(a.x + a.width, b.x + b.width);
    int y2 = std::min(a.y + a.height, b.y + b.height);
    
    if (x2 <= x1 || y2 <= y1) {
        return 0.0f;
    }
    
    float intersection = static_cast<float>((x2 - x1) * (y2 - y1));
    float area_a = static_cast<float>(a.width * a.height);
    float area_b = static_cast<float>(b.width * b.height);
    
    return intersection / (area_a + area_b - intersection);
}