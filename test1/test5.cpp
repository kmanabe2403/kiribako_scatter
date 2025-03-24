#include <opencv2/opencv.hpp>
#include <iostream>

// マウスクリックイベントのコールバック関数
void get_coordinates(int event, int x, int y, int flags, void* param) {
    if (event == cv::EVENT_LBUTTONDOWN) {  // 左クリック時
        std::cout << "Clicked at: (" << x << ", " << y << ")" << std::endl;
        cv::Mat* img = static_cast<cv::Mat*>(param); // 画像を取得
        
        // クリックした座標に赤い点を描画
        cv::circle(*img, cv::Point(x, y), 5, cv::Scalar(0, 0, 255), -1);
        cv::imshow("img_bin", *img);
    }
}

int main() {
    cv::Mat img = cv::imread("./image3.png"); // 画像を読み込む
    if (img.empty()) {
        std::cerr << "Error: Could not load image!" << std::endl;
        return -1;
    }

    // グレースケール変換
    cv::Mat img_bin;
    cv::cvtColor(img, img_bin, cv::COLOR_BGR2GRAY);

    // 画像を保存
    cv::imwrite("./test_imae/image3_test5.png", img_bin);
    
    // 画像を表示
    cv::imshow("img_bin", img_bin);

    // マウスクリックイベントを設定
    cv::setMouseCallback("img_bin", get_coordinates, &img_bin);

    // キーが押されるまで待機
    cv::waitKey(0);

    return 0;
}
