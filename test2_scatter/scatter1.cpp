#include <opencv2/opencv.hpp>
#include <iostream>

// クリック回数をカウント
int click_count = 0;

// マウスクリックイベントのコールバック関数
void get_coordinates(int event, int x, int y, int flags, void* param) {
    if (event == cv::EVENT_LBUTTONDOWN) {  // 左クリック時
        click_count++;  // カウントを増やす
        std::cout <<click_count <<", " << x << ", " << y << std::endl;

        cv::Mat* img = static_cast<cv::Mat*>(param); // 画像を取得
        
        // クリックした座標に赤い点を描画 (BGR)
        cv::circle(*img, cv::Point(x, y), 5, cv::Scalar(0, 0, 255), -1);

        // クリックした点の右上に番号を表示
        std::string num_text = std::to_string(click_count);
        cv::putText(*img, num_text, cv::Point(x + 10, y - 10), 
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);

        // 画像を更新
        cv::imshow("img_bin", *img);
    }
}

int main() {
    // 画像を読み込む
    cv::Mat img = cv::imread("./png/beta1.png"); 
    if (img.empty()) {
        std::cerr << "Error: Could not load image!" << std::endl;
        return -1;
    }

    // グレースケール変換
    cv::Mat img_bin;
    cv::cvtColor(img, img_bin, cv::COLOR_BGR2GRAY);

    // グレースケールを3チャンネルのBGR画像に変換
    cv::Mat img_color;
    cv::cvtColor(img_bin, img_color, cv::COLOR_GRAY2BGR);

    // 画像を表示
    cv::imshow("img_bin", img_color);

    // マウスクリックイベントを設定
    cv::setMouseCallback("img_bin", get_coordinates, &img_color);

    // キー入力を待ち、's' が押されたら画像を保存
    while (true) {
        int key = cv::waitKey(0);  // キー入力を待つ
        if (key == 's') {  // 's'キーが押されたら
            cv::imwrite("./png_scatter/20cm.png", img_color);  // 画像を保存
            std::cout << "Image saved as clicked_image.png" << std::endl;
        } else if (key == 27) {  // ESCキーが押されたら終了
            break;
        }
    }

    return 0;
}
