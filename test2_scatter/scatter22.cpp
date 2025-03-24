#include <opencv2/opencv.hpp>
#include <iostream>

cv::Mat img_original, img_display;
cv::Rect roi;
double scale = 1.0;  // 拡大率
int step = 50;  // 矢印キーで移動するピクセル数

void update_display() {
    // ROI の範囲をチェック
    roi.x = std::max(0, std::min(roi.x, img_original.cols - roi.width));
    roi.y = std::max(0, std::min(roi.y, img_original.rows - roi.height));

    // ROI の範囲で画像を切り抜く
    cv::Mat img_roi = img_original(roi);

    // 表示用にリサイズ
    cv::resize(img_roi, img_display, cv::Size(img_original.cols, img_original.rows));

    // 画像を表示
    cv::imshow("Zoom Window", img_display);
}

void handle_key(int key) {
    switch (key) {
        case 81:  // ← 左
            roi.x -= step;
            break;
        case 83:  // → 右
            roi.x += step;
            break;
        case 82:  // ↑ 上
            roi.y -= step;
            break;
        case 84:  // ↓ 下
            roi.y += step;
            break;
        case '+':  // 拡大
            if (scale < 5.0) {
                scale *= 1.2;
                roi.width = std::max(100, static_cast<int>(img_original.cols / scale));
                roi.height = std::max(100, static_cast<int>(img_original.rows / scale));
            }
            break;
        case '-':  // 縮小
            if (scale > 0.2) {
                scale /= 1.2;
                roi.width = std::min(img_original.cols, static_cast<int>(img_original.cols / scale));
                roi.height = std::min(img_original.rows, static_cast<int>(img_original.rows / scale));
            }
            break;
        case 27:  // ESCキーで終了
            std::cout << "Exit program." << std::endl;
            exit(0);
    }

    update_display();
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <image path>" << std::endl;
        return -1;
    }

    img_original = cv::imread(argv[1]);
    if (img_original.empty()) {
        std::cerr << "Error: Could not load image!" << std::endl;
        return -1;
    }

    // ROI の初期設定
    roi = cv::Rect(0, 0, img_original.cols, img_original.rows);

    cv::namedWindow("Zoom Window");
    update_display();

    while (true) {
        int key = cv::waitKey(0);
        handle_key(key);
    }

    return 0;
}
