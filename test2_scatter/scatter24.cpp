#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <tuple>
#include <sstream>
#include <functional>
#include <vector>

//いろいろ定義
cv::Mat img_original, img_display;
float scale = 1.0f;
cv::Point2f offset(0, 0);
const float scale_factor = 1.1f;  // 拡大率
const int move_step = 20;         // 移動量
const float min_scale = 1.0f;     // 最小拡大率
const float max_scale = 3.0f;     // 最大拡大率

//座標変換
cv::Point2f transform_point(int x, int y) {
    return cv::Point2f((x + offset.x) / scale, (y + offset.y) / scale);
}

//画面の表示
void update_display() {
    cv::Mat img_resized;
    cv::resize(img_original, img_resized, cv::Size(), scale, scale, cv::INTER_LINEAR);
    cv::Mat M = (cv::Mat_<double>(2, 3) << 1, 0, -offset.x, 0, 1, -offset.y);
    cv::warpAffine(img_resized, img_display, M, img_original.size());
    cv::imshow("Zoom Window", img_display);
}

//画面の拡大・縮小、移動のキー設定
void on_key(int key) {
    switch (key) {
        case 59:  // '+' で拡大
            if (scale * scale_factor <= max_scale) scale *= scale_factor;
            break;
        case 45:  // '-' で縮小
            if (scale / scale_factor >= min_scale) scale /= scale_factor;
            break;
        case 82:  // ↑
            if (offset.y - move_step >= 0) offset.y -= move_step;
            break;
        case 84:  // ↓
            if (offset.y + move_step <= img_original.rows - img_original.rows / scale)
                offset.y += move_step;
            break;
        case 81:  // ←
            if (offset.x - move_step >= 0) offset.x -= move_step;
            break;
        case 83:  // →
            if (offset.x + move_step <= img_original.cols - img_original.cols / scale)
                offset.x += move_step;
            break;
        case 27:  // ESC で終了
            exit(0);
    }
    update_display();
}

//座標を変換してクリック時の操作
void at_click(int event, int x, int y, int flags, void* param) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        cv::Point2f img_point = transform_point(x, y);
        cv::circle(img_original, img_point, 4, cv::Scalar(0, 0, 255), -1);
        std::cout << img_point.x << ", " << img_point.y << std::endl;
        update_display(); //これにしないと、imshowでは全部表示されちゃうからだめ？
    }
}

int main() {
    img_original = cv::imread("example2.png");

    if (img_original.empty()) {
        std::cerr << "Error: Could not load image!" << std::endl;
        return -1;
    }

    cv::namedWindow("Zoom Window");
    cv::setMouseCallback("Zoom Window", at_click);
    update_display();
    
    while (true) {
        int key = cv::waitKey(0);
        on_key(key);
    }
    return 0;
}
