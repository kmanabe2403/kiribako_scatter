#include <opencv2/opencv.hpp>

int main()
{
    cv::Mat img = cv::imread("./image3.png"); // image.pngをimgに代入
    if (img.empty()) {
        std::cerr << "Error: Could not load image!" << std::endl;
        return -1;
    }

    // imgを白黒にしてimg_binに代入
    cv::Mat img_bin;
    cv::cvtColor(img, img_bin, cv::COLOR_BGR2GRAY);
    
    cv::imwrite("image3_test2.png", img_bin);
    cv::imshow("img_bin", img_bin); // imgの表示
    cv::waitKey(0); // キーが押されるまで待機

    return 0;
}
