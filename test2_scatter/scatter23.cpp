#include <opencv2/opencv.hpp>
#include <iostream>

cv::Mat img_original, img_display;
float scale = 1.0f;
cv::Point2f offset(0, 0);
const float scale_factor = 1.1f;  // 拡大率
const int move_step = 20;         // 移動量
const float min_scale = 1.0f;     // 最小拡大率
const float max_scale = 3.0f;     // 最大拡大率

// 画像を更新して表示する
void update_display() {
    // 画像を拡大縮小
    cv::Mat img_resized;
    cv::resize(img_original, img_resized, cv::Size(), scale, scale, cv::INTER_LINEAR);

    // 画像サイズ取得
    int img_w = img_resized.cols;
    int img_h = img_resized.rows;
    int win_w = img_original.cols;
    int win_h = img_original.rows;

    // **オフセットの制限**
    offset.x = std::min(std::max(offset.x, 0.0f), (float)(img_w - win_w));
    offset.y = std::min(std::max(offset.y, 0.0f), (float)(img_h - win_h));

    // 平行移動行列を作成
    cv::Mat M = (cv::Mat_<double>(2, 3) << 1, 0, -offset.x, 0, 1, -offset.y);

    // 移動を適用
    cv::warpAffine(img_resized, img_display, M, img_original.size());

    // 画像を表示
    cv::imshow("Zoom Window", img_display);
}

// キーボードイベント処理
void on_key(int key) {
    std::cout << "Pressed key: " << key << std::endl;

    switch (key) {
        case 59:  // 拡大
            if (scale * scale_factor <= max_scale) {
                scale *= scale_factor;
            }
            break;
        case 45:  // 縮小
            if (scale / scale_factor >= min_scale) {
                scale /= scale_factor;
            }
            break;
        case 82:  // ↑ 上に移動 → yを減少 (上方向)
            offset.y -= move_step;
            break;
        case 84:  // ↓ 下に移動 → yを増加 (下方向)
            offset.y += move_step;
            break;
        case 81:  // ← 左に移動 → xを減少 (左方向)
            offset.x -= move_step;
            break;
        case 83:  // → 右に移動 → xを増加 (右方向)
            offset.x += move_step;
            break;
        case 27:  // ESCキーで終了
            exit(0);
    }

    update_display();
}

int main() {
    // 画像を読み込む
    img_original = cv::imread("example2.png");
    if (img_original.empty()) {
        std::cerr << "Error: Could not load image!" << std::endl;
        return -1;
    }

    cv::namedWindow("Zoom Window");
    update_display();

    while (true) {
        int key = cv::waitKey(0);
        on_key(key);
    }

    return 0;
}
