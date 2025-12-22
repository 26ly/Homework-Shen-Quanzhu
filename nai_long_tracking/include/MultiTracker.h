#pragma once
#include "types.h"
#include "KalmanTracker.h"
#include <memory>
#include <unordered_map>

class MultiTracker {
public:
    MultiTracker(int max_age = 30, int min_hits = 3);
    
    // 更新所有追踪器
    TrackResultList update(const DetectionList& detections);
    
    // 获取所有活跃的追踪器
    std::vector<std::shared_ptr<KalmanTracker>> getTrackers() const;
    
    // 设置匹配阈值
    void setMatchingThreshold(float threshold) { 
        matching_threshold_ = threshold; 
    }
    
    // 清空所有追踪器
    void clear();

private:
    // 数据关联（匈牙利算法）
    struct MatchResult {
        std::vector<std::pair<int, int>> matches;  // (detection_idx, tracker_idx)
        std::vector<int> unmatched_detections;
        std::vector<int> unmatched_trackers;
    };
    
    MatchResult hungarianMatching(const DetectionList& detections);
    
    // 计算成本矩阵（IoU距离）
    cv::Mat computeCostMatrix(const DetectionList& detections) const;
    
    // 创建新追踪器
    void createNewTracker(const Detection& detection);
    
    // 删除丢失的追踪器
    void removeLostTrackers();
    
    std::unordered_map<int, std::shared_ptr<KalmanTracker>> trackers_;
    int next_id_ = 0;
    
    // 参数
    int max_age_;
    int min_hits_;
    float matching_threshold_ = 0.3f;
    
    // 匈牙利算法辅助函数
    double lapCost(const cv::Mat& cost_matrix,
                  std::vector<int>& rowsol,
                  std::vector<int>& colsol) const;

    float calculateIoU(const cv::Rect& a, const cv::Rect& b) const;
};