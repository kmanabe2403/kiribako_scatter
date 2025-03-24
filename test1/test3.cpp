#include <opencv2/opencv.hpp>

int main()
{
    cv::Mat img = cv::imread("./image3.png"); // image.pngをimgに代入

    // imgを白黒にしてimg_binに代入
    cv::Mat img_gray;
    cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);

    cv::Mat img_canny;
    cv::Canny(img_gray, img_canny, 100, 200);

    cv::imwrite("image3_test3.png", img_canny);
    cv::imshow("Canny Edge Detection", img_canny); // imgの表示
    cv::waitKey(0); // キーが押されるまで待機

    return 0;
}

//cv::Canny(img, img_canny, 500.0, 700.0);

