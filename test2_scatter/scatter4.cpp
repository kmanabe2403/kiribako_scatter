#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

// クリック回数をカウント
int click_count = 0;
cv::Point point1, point2;  // 2点を保存
cv::Mat img_original;  // ガイドラインを消すための元画像

// 指定した長さの直線を描く関数
cv::Point draw_limited_line(cv::Mat& img, cv::Point p1, cv::Point p2, int length) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    double distance = std::sqrt(dx * dx + dy * dy);

    cv::Point p_end = p1;
    if (distance > 0) {
        double scale = length / distance;
        p_end = cv::Point(p1.x + static_cast<int>(dx * scale), 
                          p1.y + static_cast<int>(dy * scale));

        cv::line(img, p1, p_end, cv::Scalar(255, 0, 0), 2); // 青い直線を描画
    }
    return p_end;
}

// 垂直な点線のガイドラインを描く関数
void draw_dashed_line(cv::Mat& img, cv::Point p, cv::Point p1, cv::Point p2, int length) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;

    double perp_x = -dy;
    double perp_y = dx;

    double perp_length = std::sqrt(perp_x * perp_x + perp_y * perp_y);
    if (perp_length > 0) {
        double scale = length / perp_length;
        cv::Point p_perp(p.x + static_cast<int>(perp_x * scale), 
                         p.y + static_cast<int>(perp_y * scale));

        // 点線を描画（5pxごとに線を引く）
        int num_dashes = 10;
        for (int i = 0; i < num_dashes; i += 2) {
            cv::Point start(p.x + static_cast<int>(perp_x * scale * i / num_dashes),
                            p.y + static_cast<int>(perp_y * scale * i / num_dashes));
            cv::Point end(p.x + static_cast<int>(perp_x * scale * (i + 1) / num_dashes),
                          p.y + static_cast<int>(perp_y * scale * (i + 1) / num_dashes));
            cv::line(img, start, end, cv::Scalar(0, 255, 0), 1);
        }
    }
}

// マウスクリックイベントのコールバック関数
void get_coordinates(int event, int x, int y, int flags, void* param) {
    cv::Mat* img = static_cast<cv::Mat*>(param); // 画像を取得
    
    if (event == cv::EVENT_LBUTTONDOWN) {  // 左クリック時
        // 次のクリックでガイドラインを消す（元の画像を復元）
        img_original.copyTo(*img);

        click_count++;
        std::cout << "Clicked at: (" << x << ", " << y << "), Number: " << click_count << std::endl;

        // クリック位置に赤い点を描画
        cv::circle(*img, cv::Point(x, y), 5, cv::Scalar(0, 0, 255), -1);

        // クリックした点の横に番号を表示
        std::string num_text = std::to_string(click_count);
        cv::putText(*img, num_text, cv::Point(x + 10, y - 10), 
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);

        // 2点目のクリックなら、直線 & ガイドラインを描画
        if (click_count % 2 == 1) {
            point1 = cv::Point(x, y);  // 1点目を記録
        } else {
            point2 = cv::Point(x, y);  // 2点目を記録

            // 50ピクセルの直線を描く
            cv::Point p_end = draw_limited_line(*img, point1, point2, 50);

            // 直線の終点から点線のガイドラインを引く
            draw_dashed_line(*img, p_end, point1, point2, 30);
        }

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

    // 元の画像をコピーして保存（ガイドライン消去用）
    img_original = img_color.clone();

    // 画像を表示
    cv::imshow("img_bin", img_color);

    // マウスクリックイベントを設定
    cv::setMouseCallback("img_bin", get_coordinates, &img_color);

    // キー入力を待ち、's' が押されたら画像を保存
    while (true) {
        int key = cv::waitKey(0);  // キー入力を待つ
        if (key == 's') {  // 's'キーが押されたら
            cv::imwrite("clicked_image.png", img_color);  // 画像を保存
            std::cout << "Image saved as clicked_image.png" << std::endl;
        } else if (key == 27) {  // ESCキーが押されたら終了
            break;
        }
    }

    return 0;
}
