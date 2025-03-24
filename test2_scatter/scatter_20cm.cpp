#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <tuple>
#include <sstream>

int click = 0;
int term = 0;
cv::Point point1, point2, point3, point4, point5;
cv::Point point2_1, point2_2, point2_3, point2_4; //point2_4がpoint2_endに変わる
cv::Point point4_1, point4_2, point4_3; //point4はこれに変わる
double a0, a1, a2, a3, a4; 
double psi1, psi2, psi3;
double psi_rms, psi_sigma;
std::string coo;
std::ostringstream distance_text, psi2_text, psi3_text, theta_text, psi_rms_text, psi_sigma_text;

//ある長さの線を引く(distanceぶん)
double draw_line(cv::Mat& img, cv::Point p1, cv::Point p2, cv::Scalar color, int width){
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    double distance = std::sqrt(dx*dx + dy*dy);
    cv::Point p_end = p1;

    if (distance > 0) {
        double scale = 1;
        p_end = cv::Point(p1.x + static_cast<int>(dx * scale), p1.y + static_cast<int>(dy * scale));
        cv::line(img, p1, p_end, color, width);
    }

    return distance;
}


void at_click(int event, int x, int y, int flags, void* param) {
    if (event == cv::EVENT_LBUTTONDOWN) {  // 左クリック時
        click++;
        cv::Mat* img = static_cast<cv::Mat*>(param);
        int x_length = 400;

        if (click == 1) {
            point1 = cv::Point(x,y);
            cv::circle(*img, cv::Point(x, y), 4, cv::Scalar(0, 0, 255), -1);
            cv::putText(*img, "1", cv::Point(x + 5, y - 5), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,0,255), 1.5);
            coo =  "1: (" + std::to_string(x) + ", " + std::to_string(y) + ")";   
            cv::putText(*img, coo, cv::Point(20, 50+term*80), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 1.5);
            std::cout <<  "1, " << x << ", " << y << std::endl;


        } else if (click == 2) {
            point2 = cv::Point(x,y);
            cv::circle(*img, cv::Point(x, y), 4, cv::Scalar(0, 0, 255), -1);
            cv::putText(*img, "2", cv::Point(x + 5, y - 5), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,0,255), 1.5);
            coo =  "2: (" + std::to_string(x) + ", " + std::to_string(y) + ")";   
            cv::putText(*img, coo, cv::Point(20, 80+term*80), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 1.5);
            std::cout << "2, " << x << ", " << y << std::endl;

            distance_text.str("");  // 文字列の内容をリセット
            distance_text.clear();  // 状態フラグをリセット
            double distance = draw_line(*img, point1, point2, cv::Scalar(0, 255, 0), 1);
            distance_text << "20cm= " << std::fixed << std::setprecision(2) << distance;
            cv::putText(*img, distance_text.str(), cv::Point(point1.x + 20, point1.y - 20), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,255,0), 1);
            cv::putText(*img, distance_text.str(), cv::Point(300, 50+term*80), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,0,255), 1);

        } else {
            //////////////////////////////////////////ここは、1個目とだいたい同じ
            click = 1;
            term++; 
            point1 = cv::Point(x,y);
            cv::circle(*img, cv::Point(x, y), 4, cv::Scalar(0, 0, 255), -1);
            cv::putText(*img, "1", cv::Point(x + 5, y - 5), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,0,255), 1.5);
            coo =  "1: (" + std::to_string(x) + ", " + std::to_string(y) + ")";   
            cv::putText(*img, coo, cv::Point(20, 50+term*80), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 1.5);
            std::cout <<  "1, " << x << ", " << y << std::endl;
        }

        //画像の更新、これがないと線などが反映されない
        cv::imshow("img_bin", *img);
    }
}

int main() {
    // 画像を読み込む
    std::string name = "muon5";

    std::ostringstream name_in, name_out;
    name_in << "./png/" << name << ".png";
    name_out << "./png_test/" << name << "_scatter_20cm.png";


    cv::Mat img = cv::imread(name_in.str()); 
    if (img.empty()) {
        std::cerr << "Error: Could not load image!" << std::endl;
        return -1;
    }


    cv::Mat img_bin;
    cv::cvtColor(img, img_bin, cv::COLOR_BGR2GRAY);
    cv::Mat img_color;
    cv::cvtColor(img_bin, img_color, cv::COLOR_GRAY2BGR);
    cv::imshow("img_bin", img_color);


    cv::setMouseCallback("img_bin", at_click, &img_color);
    while (true) {
        int key = cv::waitKey(0);  // キー入力を待つ
        if (key == 's') {  // 's'キーが押されたら
            cv::imwrite(name_out.str(), img_color);  // 画像を保存
        } else if (key == 27) {  // ESCキーが押されたら終了
            break;
        }
    }

    return 0;
}