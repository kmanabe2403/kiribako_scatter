#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>

cv::Mat img_original;  // 元画像（変更しない）
cv::Mat img_display;   // 表示用画像（切り取り・拡大して表示）

cv::Rect roi(0, 0, 100, 100); // 初期の表示範囲
double scale = 1.0; // 初期スケール


void update_display() {
    // ROI の範囲を制限する
    roi.x = std::max(0, std::min(roi.x, img_original.cols - roi.width));
    roi.y = std::max(0, std::min(roi.y, img_original.rows - roi.height));

    // ROIを切り抜く
    cv::Mat roi_img = img_original(roi);

    // 拡大して表示
    cv::resize(roi_img, img_display, cv::Size(roi.width * scale, roi.height * scale), 0, 0, cv::INTER_LINEAR);

    // 画像を表示
    cv::imshow("img_bin", img_display);
}

void handle_key(int key) {
    int step = 20;  // 移動量
    double scale_step = 1.1;  // 拡大縮小の倍率

    switch (key) {
        case 81:  // ← 左
            roi.x = std::max(0, roi.x - step);
            break;
        case 83:  // → 右
            roi.x = std::min(img_original.cols - roi.width, roi.x + step);
            break;
        case 82:  // ↑ 上
            roi.y = std::max(0, roi.y - step);
            break;
        case 84:  // ↓ 下
            roi.y = std::min(img_original.rows - roi.height, roi.y + step);
            break;
        case '+':  // 拡大
            {
                int new_width = std::min(static_cast<int>(roi.width * scale_step), img_original.cols);
                int new_height = std::min(static_cast<int>(roi.height * scale_step), img_original.rows);

                // ROIの中心を維持
                roi.x = std::max(0, roi.x - (new_width - roi.width) / 2);
                roi.y = std::max(0, roi.y - (new_height - roi.height) / 2);

                roi.width = new_width;
                roi.height = new_height;
            }
            break;
        case '-':  // 縮小
            {
                int new_width = std::max(static_cast<int>(roi.width / scale_step), 100);
                int new_height = std::max(static_cast<int>(roi.height / scale_step), 100);

                // ROIの中心を維持
                roi.x = std::min(img_original.cols - new_width, roi.x + (roi.width - new_width) / 2);
                roi.y = std::min(img_original.rows - new_height, roi.y + (roi.height - new_height) / 2);

                roi.width = new_width;
                roi.height = new_height;
            }
            break;
        default:
            std::cout << "Unknown key: " << key << std::endl;
            return;
    }

    // ROIの範囲を調整（画面からはみ出さないように）
    roi.x = std::max(0, std::min(roi.x, img_original.cols - roi.width));
    roi.y = std::max(0, std::min(roi.y, img_original.rows - roi.height));

    std::cout << "Updated ROI: x=" << roi.x << ", y=" << roi.y << ", width=" << roi.width << ", height=" << roi.height << std::endl;
    update_display();
}



void at_click(int event, int x, int y, int flags, void* param) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        // クリック位置を元画像の座標系に変換
        int orig_x = roi.x + x / scale;
        int orig_y = roi.y + y / scale;

        cv::circle(img_original, cv::Point(orig_x, orig_y), 5, cv::Scalar(0, 0, 255), -1);
        update_display();
    }
}

int main() {
    img_original = cv::imread("example2.png");
    if (img_original.empty()) {
        std::cerr << "Error: Could not load image!" << std::endl;
        return -1;
    }

    // 初期 ROI を画像全体に設定
    roi = cv::Rect(0, 0, img_original.cols, img_original.rows);

    cv::namedWindow("img_bin");
    cv::setMouseCallback("img_bin", at_click, nullptr);

    update_display();

    while (true) {
        int key = cv::waitKey(0);
        if (key == 27) break;  // ESC で終了
        handle_key(key);
    }

    return 0;
}

