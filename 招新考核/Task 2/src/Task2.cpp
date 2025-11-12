#include <opencv2/opencv.hpp>
#include <bits/stdc++.h>
using namespace std;
using namespace cv;

class ImageProcessor{
private:
    Mat image;
    bool isLoaded;

    void checkImageLoaded() const {
        if (!isLoaded||image.empty()) {
            cout<<"未加载成功"<<endl;
        }
    }

public:
    ImageProcessor(const string& imagePath):isLoaded(false){
        loadImage(imagePath);
    }

    // 加载图像
    void loadImage(const string& imagePath) {
        image=imread(imagePath);
        if (image.empty()){
            cout<<"无法加载图像，可能是路径问题"<<endl;
        }
        isLoaded=true;
    }

    // 获取图像尺寸
    Size getImageSize()const{
        checkImageLoaded();
        return image.size();
    }

    // 获取图像宽度
    int getWidth()const{
        checkImageLoaded();
        return image.cols;
    }

    // 获取图像高度
    int getHeight()const{
        checkImageLoaded();
        return image.rows;
    }

    // 获取通道数
    int getChannels()const{
        checkImageLoaded();
        return image.channels();
    }

    // 获取原始像素数据
    Mat& getPixelData(){
        checkImageLoaded();
        return image;
    }

    // RGB转灰度图
    Mat convertToGrayscale(){
        checkImageLoaded();
        Mat grayImage;
        if (image.channels()==3){
            cvtColor(image,grayImage,COLOR_BGR2GRAY);
        }
        else if (image.channels()==4) {
            cvtColor(image,grayImage,COLOR_BGRA2GRAY);
        }
        else {
            grayImage=image.clone();
        }
        return grayImage;
    }

    // 均值模糊去噪
    Mat applyMeanBlur(int kernelSize=5){
        checkImageLoaded();
        Mat blurredImage;
        blur(image,blurredImage,Size(kernelSize, kernelSize));
        return blurredImage;
    }

    // 基于亮度阈值分割
    Mat Threshold1(){
        checkImageLoaded();
        Mat thresholdImage;
        threshold(image, thresholdImage, 180, 255, THRESH_BINARY);
        return thresholdImage;
    }

    Mat Threshold2(){
        checkImageLoaded();
        Mat hsv;
        cvtColor(image,hsv,COLOR_BGR2HSV);

        // 定义蓝色范围
        Scalar lower_blue(100, 70, 50);
        Scalar upper_blue(130, 255, 255);

        // 定义红色范围
        Scalar lower_red1(0, 30, 50);
        Scalar upper_red1(10, 255, 255);
        Scalar lower_red2(160, 30, 50);
        Scalar upper_red2(180, 255, 255);

        // 根据颜色范围创建掩码
        Mat mask_blue,mask_red1,mask_red2;
        inRange(hsv, lower_blue, upper_blue, mask_blue);
        inRange(hsv, lower_red1, upper_red1, mask_red1);
        inRange(hsv, lower_red2, upper_red2, mask_red2);


        // 合并掩码
        Mat mask,mask_red;
        bitwise_or(mask_red1, mask_red2, mask_red);
        bitwise_or(mask_blue, mask_red, mask);

        // 应用掩码
        Mat resultImage;
        bitwise_and(image, image, resultImage, mask);

        return resultImage;
    }

    // 显示图像
    void showImage(const string& windowName) const{
        checkImageLoaded();
        imshow(windowName, image);
        waitKey(0);
    }

    // 保存图像
    void saveImage(const string& outputPath) const{
        checkImageLoaded();
        if (!imwrite(outputPath,image)){
            cout<<"储存失败"<<endl;
        }
    }

    // 检查图像是否成功加载
    bool isImageLoaded() const{
        return isLoaded&&!image.empty();
    }
};

int main() {
    ImageProcessor processor("../image/hero.png");
    processor.showImage("Window");
    Mat res1=processor.Threshold1();
    Mat res2=processor.Threshold2();
    imshow("1",res1);
    imshow("2",res2);
    waitKey(0);
    return 0;
}