#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

std::vector<cv::Point> points;

void mouseCallback(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        points.push_back(cv::Point(x, y));
        std::cout << "点击坐标: (" << x << ", " << y << ")" << std::endl;
    }
    else if (event == cv::EVENT_RBUTTONDOWN) {
        if (!points.empty()) {
            points.pop_back();
            std::cout << "删除上一个坐标" << std::endl;
        }
    }
    else if (event == cv::EVENT_MOUSEMOVE) {
        if (flags & cv::EVENT_FLAG_LBUTTON) {
            std::cout << "拖动坐标: (" << x << ", " << y << ")" << std::endl;
        }
    }
}

int main() {
    // 读取图像或创建空白图像
    cv::Mat image = cv::imread("../image/stage.png");
    // cv::Mat image = cv::Mat::ones(480, 640, CV_8UC3) * 255;
    
    if (image.empty()) {
        std::cout << "无法加载图像，创建空白图像" << std::endl;
        
        // 创建空白图像作为替代
        image = cv::Mat::ones(800, 1200, CV_8UC3) * 255;
        
        // 在空白图像上添加一些文字说明
        cv::putText(image, "Original image not found - Using blank canvas", 
                   cv::Point(50, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, 
                   cv::Scalar(0, 0, 0), 2);
        cv::putText(image, "Left click: add point, Right click: remove point", 
                   cv::Point(50, 150), cv::FONT_HERSHEY_SIMPLEX, 0.7, 
                   cv::Scalar(0, 0, 0), 2);
        cv::putText(image, "ESC: exit, c: clear all, p: print points", 
                   cv::Point(50, 200), cv::FONT_HERSHEY_SIMPLEX, 0.7, 
                   cv::Scalar(0, 0, 0), 2);
    }else{
        std::cout << "图像加载成功，尺寸: " << image.cols << " x " << image.rows << std::endl;
    }
    
    cv::namedWindow("Image");
    cv::setMouseCallback("Image", mouseCallback);
    
    std::cout << "使用方法:" << std::endl;
    std::cout << "- 左键点击: 记录坐标" << std::endl;
    std::cout << "- 右键点击: 删除上一个坐标" << std::endl;
    std::cout << "- 按ESC键退出" << std::endl;
    std::cout << "- 按'c'键清空所有点" << std::endl;
    std::cout << "- 按'p'键打印所有坐标" << std::endl;
    
    while (true) {
        cv::Mat displayImage = image.clone();
        
        // 绘制所有点
        for (size_t i = 0; i < points.size(); ++i) {
            cv::circle(displayImage, points[i], 5, cv::Scalar(0, 0, 255), -1);
            std::string text = std::to_string(i) + ":(" + 
                             std::to_string(points[i].x) + "," + 
                             std::to_string(points[i].y) + ")";
            cv::putText(displayImage, text, 
                       cv::Point(points[i].x + 10, points[i].y - 10),
                       cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 1);
        }
        
        cv::imshow("Image", displayImage);
        
        int key = cv::waitKey(1);
        if (key == 27) { // ESC
            break;
        }
        else if (key == 'c') {
            points.clear();
            std::cout << "清空所有坐标" << std::endl;
        }
        else if (key == 'p') {
            std::cout << "所有坐标点:" << std::endl;
            for (size_t i = 0; i < points.size(); ++i) {
                std::cout << "点" << i << ": (" << points[i].x << ", " << points[i].y << ")" << std::endl;
            }
        }
    }
    
    // 最终输出
    std::cout << "\n最终获取的坐标点:" << std::endl;
    for (size_t i = 0; i < points.size(); ++i) {
        std::cout << "点" << i << ": (" << points[i].x << ", " << points[i].y << ")" << std::endl;
    }
    
    return 0;
}