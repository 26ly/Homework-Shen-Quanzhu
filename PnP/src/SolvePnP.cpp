#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // 定义物体点
    std::vector<cv::Point3f> objectPoints;
    objectPoints.push_back(cv::Point3f(17.45877,-4.22194, 0.4));
    objectPoints.push_back(cv::Point3f(17.85, -9.45271, 0.4));
    objectPoints.push_back(cv::Point3f(24.33395, -3.95052, 0.73188));
    objectPoints.push_back(cv::Point3f(18.91038, -10.8, 0.8));
    objectPoints.push_back(cv::Point3f(9.535, -6.06409, 0));
    objectPoints.push_back(cv::Point3f(9.535, -8.93591, 0));
    objectPoints.push_back(cv::Point3f(14.17481, -10, 0.47836));

    // 定义图像点
    std::vector<cv::Point2f> imagePoints;
    imagePoints.push_back(cv::Point2f(468, 320));
    imagePoints.push_back(cv::Point2f(692, 316));
    imagePoints.push_back(cv::Point2f(504, 280));
    imagePoints.push_back(cv::Point2f(755, 296));
    imagePoints.push_back(cv::Point2f(468, 406));
    imagePoints.push_back(cv::Point2f(659, 412));
    imagePoints.push_back(cv::Point2f(264, 340));

    // 定义相机矩阵
    cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << 
        4479.131275940303, 0, 1973.930688628363,
        0, 4462.034355490826, 1641.077482676097,
        0, 0, 1);

    // 定义畸变系数
    cv::Mat distCoeffs = cv::Mat::zeros(4, 1, CV_64F);

    cv::Mat rvec, tvec;
    cv::solvePnP(objectPoints, imagePoints, cameraMatrix, distCoeffs, rvec, tvec);

    std::cout << "旋转向量:\n" << rvec << std::endl;
    std::cout << "平移向量:\n" << tvec << std::endl;

    // 将旋转向量转换为旋转矩阵
    cv::Mat rMatrix;
    cv::Rodrigues(rvec, rMatrix);
    std::cout << "旋转矩阵:\n" << rMatrix << std::endl;

    // 组合旋转和平移矩阵
    cv::Mat rtMatrix;
    cv::hconcat(rMatrix, tvec, rtMatrix);

    // 创建4x4变换矩阵
    cv::Mat matrix44 = cv::Mat::eye(4, 4, CV_64F);
    rtMatrix.copyTo(matrix44(cv::Rect(0, 0, 4, 3)));
    
    std::cout << "4x4变换矩阵:\n" << matrix44 << std::endl;

    return 0;
}