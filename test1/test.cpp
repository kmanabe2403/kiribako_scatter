#include <opencv2/opencv.hpp>

int main()
{
    cv::Mat img = cv::imread("./image3.png"); // image.pngをimgに代入.

    cv::imwrite("image3_test.png", img); // 画像の保存
    cv::imshow("img", img); // imgの表示.
    cv::waitKey(0); // キーが押されるまで待機.
    return 0;
}