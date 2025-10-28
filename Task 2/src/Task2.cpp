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
            cout<<"δ���سɹ�"<<endl;
        }
    }

public:
    ImageProcessor(const string& imagePath):isLoaded(false){
        loadImage(imagePath);
    }

    // ����ͼ��
    void loadImage(const string& imagePath) {
        image=imread(imagePath);
        if (image.empty()){
            cout<<"�޷�����ͼ�񣬿�����·������"<<endl;
        }
        isLoaded=true;
    }

    // ��ȡͼ��ߴ�
    Size getImageSize()const{
        checkImageLoaded();
        return image.size();
    }

    // ��ȡͼ����
    int getWidth()const{
        checkImageLoaded();
        return image.cols;
    }

    // ��ȡͼ��߶�
    int getHeight()const{
        checkImageLoaded();
        return image.rows;
    }

    // ��ȡͨ����
    int getChannels()const{
        checkImageLoaded();
        return image.channels();
    }

    // ��ȡԭʼ��������
    Mat& getPixelData(){
        checkImageLoaded();
        return image;
    }

    // RGBת�Ҷ�ͼ
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

    // ��ֵģ��ȥ��
    Mat applyMeanBlur(int kernelSize=5){
        checkImageLoaded();
        Mat blurredImage;
        blur(image,blurredImage,Size(kernelSize, kernelSize));
        return blurredImage;
    }

    // ����������ֵ�ָ�
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

        // ������ɫ��Χ
        Scalar lower_blue(100, 70, 50);
        Scalar upper_blue(130, 255, 255);

        // �����ɫ��Χ
        Scalar lower_red1(0, 30, 50);
        Scalar upper_red1(10, 255, 255);
        Scalar lower_red2(160, 30, 50);
        Scalar upper_red2(180, 255, 255);

        // ������ɫ��Χ��������
        Mat mask_blue,mask_red1,mask_red2;
        inRange(hsv, lower_blue, upper_blue, mask_blue);
        inRange(hsv, lower_red1, upper_red1, mask_red1);
        inRange(hsv, lower_red2, upper_red2, mask_red2);


        // �ϲ�����
        Mat mask,mask_red;
        bitwise_or(mask_red1, mask_red2, mask_red);
        bitwise_or(mask_blue, mask_red, mask);

        // Ӧ������
        Mat resultImage;
        bitwise_and(image, image, resultImage, mask);

        return resultImage;
    }

    // ��ʾͼ��
    void showImage(const string& windowName) const{
        checkImageLoaded();
        imshow(windowName, image);
        waitKey(0);
    }

    // ����ͼ��
    void saveImage(const string& outputPath) const{
        checkImageLoaded();
        if (!imwrite(outputPath,image)){
            cout<<"����ʧ��"<<endl;
        }
    }

    // ���ͼ���Ƿ�ɹ�����
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