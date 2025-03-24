#include <opencv2/opencv.hpp>

int main()
{
    // 画像を読み込む
    cv::Mat img = cv::imread("./test_image/image3.png");
    if (img.empty()) {
        std::cerr << "Error: Could not load image!" << std::endl;
        return -1;
    }

    // グレースケール変換
    cv::Mat img_gray;
    cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);

    // Canny エッジ検出
    cv::Mat img_canny;
    cv::Canny(img_gray, img_canny, 500.0, 700.0);

    // Hough変換で直線検出
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(img_canny, lines, 1, CV_PI / 180, 50, 50, 10);

    // 検出した直線を元の画像に描画
    for (size_t i = 0; i < lines.size(); i++) {
        cv::Vec4i l = lines[i];
        cv::line(img, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 2);
    }

    // 画像を表示
    cv::imwrite("./test_image/image3_test4.png", img);
    //cv::imwrite("image3_test4.png", img); プログラムがある場所でよければこれ
    cv::imshow("Canny Edge", img_canny);
    cv::imshow("Detected Lines", img);
    cv::waitKey(0);

    return 0;
}