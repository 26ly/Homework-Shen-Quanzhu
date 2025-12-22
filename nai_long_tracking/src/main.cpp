#include "Detector.h"
#include "MultiTracker.h"
#include "Visualization.h"
#include <chrono>
#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
    // 参数配置
    std::string video_path = "../data/test.mp4";
    std::string model_path = "../models/yolov5s.onnx";
    std::string output_path = "output/tracking_result.mp4";
    std::string classes_path = "../models/classes.txt";
    
    // 解析命令行参数
    if (argc > 1) video_path = argv[1];
    if (argc > 2) model_path = argv[2];
    if (argc > 3) output_path = argv[3];
    
    // 打开视频
    cv::VideoCapture cap(video_path);
    if (!cap.isOpened()) {
        std::cerr << "Error: Cannot open video file: " << video_path << std::endl;
        return -1;
    }
    
    // 获取视频信息
    int frame_width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(cv::CAP_PROP_FPS);
    int total_frames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
    
    std::cout << "Video Info:" << std::endl;
    std::cout << "  Resolution: " << frame_width << "x" << frame_height << std::endl;
    std::cout << "  FPS: " << fps << std::endl;
    std::cout << "  Total Frames: " << total_frames << std::endl;
    
    // 初始化检测器
    Detector detector;
    if (!detector.init(model_path, classes_path)) {
        std::cerr << "Failed to initialize detector" << std::endl;
        return -1;
    }
    
    // 初始化多目标追踪器
    MultiTracker tracker(30, 3);  // max_age=30, min_hits=3
    
    // 初始化可视化器
    Visualizer visualizer;
    visualizer.initVideoWriter(output_path,
                              cv::Size(frame_width, frame_height),
                              fps);
    
    // 性能统计
    int frame_count = 0;
    double total_time = 0.0;
    std::vector<double> frame_times;
    
    // 轨迹记录（用于绘制历史轨迹）
    std::unordered_map<int, std::vector<cv::Point2f>> trajectories;
    
    // 主循环
    cv::Mat frame;
    while (cap.read(frame)) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        frame_count++;
        
        // 目标检测
        DetectionList detections = detector.detect(frame);
        
        // 多目标追踪
        TrackResultList tracks = tracker.update(detections);
        
        // 更新轨迹记录
        for (const auto& track : tracks) {
            trajectories[track.id].push_back(track.position);
            
            // 限制轨迹长度
            if (trajectories[track.id].size() > 50) {
                trajectories[track.id].erase(trajectories[track.id].begin());
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        double frame_time = std::chrono::duration<double>(end_time - start_time).count();
        frame_times.push_back(frame_time);
        total_time += frame_time;
        
        // 计算当前FPS
        double current_fps = 1.0 / frame_time;
        
        // 可视化
        cv::Mat display_frame = frame.clone();
        
        // 绘制检测结果
        visualizer.drawDetections(display_frame, detections);
        
        // 绘制追踪结果和轨迹
        for (const auto& track : tracks) {
            auto it = trajectories.find(track.id);
            if (it != trajectories.end()) {
                visualizer.drawTracks(display_frame, {track}, it->second);
            } else {
                visualizer.drawTracks(display_frame, {track});
            }
        }
        
        // 绘制信息面板
        visualizer.drawInfoPanel(display_frame, frame_count,
                                current_fps, tracks.size(),
                                detections.size());
        
        // 显示帧率
        std::stringstream fps_ss;
        fps_ss << "FPS: " << std::fixed << std::setprecision(1) << current_fps;
        cv::putText(display_frame, fps_ss.str(),
                   cv::Point(frame_width - 150, 30),
                   cv::FONT_HERSHEY_SIMPLEX, 0.7,
                   cv::Scalar(0, 255, 0), 2);
        
        // 保存视频
        visualizer.writeFrame(display_frame);
        
        // 显示结果（可选）
        cv::imshow("Nai Long Tracking", display_frame);
        
        // 进度显示
        if (frame_count % 10 == 0) {
            std::cout << "\rProcessed: " << frame_count << "/" << total_frames
                      << " (" << (frame_count * 100 / total_frames) << "%)"
                      << " | FPS: " << current_fps
                      << " | Tracks: " << tracks.size()
                      << std::flush;
        }
        
        // 按键退出
        char key = static_cast<char>(cv::waitKey(1));
        if (key == 27 || key == 'q') {  // ESC或Q
            break;
        }
        
        // 限制最大处理帧数（用于调试）
        #ifdef DEBUG
        if (frame_count > 1000) break;
        #endif
    }
    
    // 性能报告
    std::cout << "\n\n=== Performance Report ===" << std::endl;
    std::cout << "Total frames processed: " << frame_count << std::endl;
    std::cout << "Total time: " << total_time << " seconds" << std::endl;
    std::cout << "Average FPS: " << frame_count / total_time << std::endl;
    
    // 计算百分位数
    std::sort(frame_times.begin(), frame_times.end());
    double p50 = frame_times[frame_times.size() / 2];
    double p95 = frame_times[static_cast<int>(frame_times.size() * 0.95)];
    
    std::cout << "Frame time P50: " << p50 * 1000 << " ms" << std::endl;
    std::cout << "Frame time P95: " << p95 * 1000 << " ms" << std::endl;
    
    // 保存性能数据
    std::ofstream perf_file("performance.csv");
    perf_file << "frame,time_ms\n";
    for (size_t i = 0; i < frame_times.size(); i++) {
        perf_file << i << "," << frame_times[i] * 1000 << "\n";
    }
    perf_file.close();
    
    // 清理
    cap.release();
    visualizer.releaseVideoWriter();
    cv::destroyAllWindows();
    
    std::cout << "\nTracking completed. Output saved to: " << output_path << std::endl;
    
    return 0;
}