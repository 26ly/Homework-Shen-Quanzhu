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

    // 检查对应关系
    std::cout << "=== 点对应关系检查 ===" << std::endl;
    for (size_t i = 0; i < objectPoints.size(); ++i) {
        std::cout << "世界点" << i << ": (" << objectPoints[i].x << ", " 
                  << objectPoints[i].y << ", " << objectPoints[i].z 
                  << ") -> 图像点: (" << imagePoints[i].x << ", " << imagePoints[i].y << ")" << std::endl;
    }

    // 定义相机矩阵
    cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << 
        4479.131275940303, 0, 1973.930688628363,
        0, 4462.034355490826, 1641.077482676097,
        0, 0, 1);

    // 定义畸变系数
    cv::Mat distCoeffs = cv::Mat::zeros(4, 1, CV_64F);

    // 尝试不同的PnP算法
    std::cout << "\n=== 尝试不同PnP算法 ===" << std::endl;
    
    cv::Mat rvec, tvec;
    bool success;
    
    // 方法1: 迭代法
    success = cv::solvePnP(objectPoints, imagePoints, cameraMatrix, distCoeffs, 
                          rvec, tvec, false, cv::SOLVEPNP_ITERATIVE);
    std::cout << "ITERATIVE方法: " << (success ? "成功" : "失败") << std::endl;
    
    if (success) {
        std::vector<cv::Point2f> reprojected;
        cv::projectPoints(objectPoints, rvec, tvec, cameraMatrix, distCoeffs, reprojected);
        double error = 0;
        for (size_t i = 0; i < objectPoints.size(); ++i) {
            error += cv::norm(imagePoints[i] - reprojected[i]);
        }
        std::cout << "平均误差: " << error / objectPoints.size() << " 像素" << std::endl;
    }

    // 方法2: EPnP
    success = cv::solvePnP(objectPoints, imagePoints, cameraMatrix, distCoeffs, 
                          rvec, tvec, false, cv::SOLVEPNP_EPNP);
    std::cout << "EPnP方法: " << (success ? "成功" : "失败") << std::endl;
    
    if (success) {
        std::vector<cv::Point2f> reprojected;
        cv::projectPoints(objectPoints, rvec, tvec, cameraMatrix, distCoeffs, reprojected);
        double error = 0;
        for (size_t i = 0; i < objectPoints.size(); ++i) {
            error += cv::norm(imagePoints[i] - reprojected[i]);
        }
        std::cout << "平均误差: " << error / objectPoints.size() << " 像素" << std::endl;
        
        // 使用EPnP的结果继续计算
        cv::Mat rMatrix;
        cv::Rodrigues(rvec, rMatrix);
        
        cv::Mat rtMatrix;
        cv::hconcat(rMatrix, tvec, rtMatrix);
        cv::Mat matrix44 = cv::Mat::eye(4, 4, CV_64F);
        rtMatrix.copyTo(matrix44(cv::Rect(0, 0, 4, 3)));
        
        std::cout << "\n使用EPnP的4x4变换矩阵:\n" << matrix44 << std::endl;
        
        // 计算目标点
        cv::Point2f targetPixel(517, 455);
        double depth_s = 7.9;
        
        std::cout << "\n=== 计算目标点(" << targetPixel.x << ", " << targetPixel.y << ") ===" << std::endl;
        
        // 更稳定的计算方法
        cv::Mat pixelVector = (cv::Mat_<double>(3, 1) << targetPixel.x, targetPixel.y, 1);
        cv::Mat normalized = cameraMatrix.inv() * pixelVector; // 归一化坐标
        
        // 相机坐标系
        cv::Mat cameraPoint = normalized * depth_s;
        
        // 相机坐标系到世界坐标系: P_w = R^T * (P_c - t)
        cv::Mat worldPoint = rMatrix.t() * (cameraPoint - tvec);
        
        std::cout << "世界坐标: (" << worldPoint.at<double>(0) << ", " 
                  << worldPoint.at<double>(1) << ", " << worldPoint.at<double>(2) << ")" << std::endl;
        
        // 验证
        std::vector<cv::Point3f> testPoints = {cv::Point3f(
            worldPoint.at<double>(0), 
            worldPoint.at<double>(1), 
            worldPoint.at<double>(2))};
        std::vector<cv::Point2f> reprojectedTarget;
        cv::projectPoints(testPoints, rvec, tvec, cameraMatrix, distCoeffs, reprojectedTarget);
        
        std::cout << "重投影验证: (" << reprojectedTarget[0].x << ", " 
                  << reprojectedTarget[0].y << ")" << std::endl;
        std::cout << "重投影误差: " << cv::norm(targetPixel - reprojectedTarget[0]) << " 像素" << std::endl;
    }

    return 0;
}