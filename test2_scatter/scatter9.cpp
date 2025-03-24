#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <tuple>
#include <sstream>

int click = 0;
cv::Point point0, point00, point1, point2, point2_2, point2_3, point3, point4, point5;
double a0, a1, a2, a3, a4; 

//ある長さの線を引く(distance関係なし)
double draw_line(cv::Mat& img, cv::Point p1, cv::Point p2, int length, cv::Scalar color, int width) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    double distance = std::sqrt(dx*dx + dy*dy);
    cv::Point p_end = p1;

    if (distance > 0) {
        double scale = length / distance;
        p_end = cv::Point(p1.x + static_cast<int>(dx * scale), p1.y + static_cast<int>(dy * scale));
        cv::line(img, p1, p_end, color, width);
    }

    double angle = std::atan2(-dy, dx);
    return angle * 180.0 / M_PI;
}

//ある長さの線を引く(distanceぶん)
double draw_line2(cv::Mat& img, cv::Point p1, cv::Point p2, cv::Scalar color, int width){
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    double distance = std::sqrt(dx*dx + dy*dy);
    cv::Point p_end = p1;

    if (distance > 0) {
        double scale = 1;
        p_end = cv::Point(p1.x + static_cast<int>(dx * scale), p1.y + static_cast<int>(dy * scale));
        cv::line(img, p1, p_end, color, width);
    }

    double angle = std::atan2(-dy, dx);
    return angle * 180.0 / M_PI;
}

//ある長さの線を引く(distance+length)　+50, 100のところに点を打つ
std::tuple<cv::Point, cv::Point, double> draw_line3(cv::Mat& img, cv::Point p1, cv::Point p2, int length, cv::Scalar color, int width) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    double distance = std::sqrt(dx*dx + dy*dy);
    cv::Point p_med = p1;
    cv::Point p_end = p1;

    if (distance > 0) {
        double scale = 1 + length/distance;
        double scale_med = 1+length/distance/2;
        p_med = cv::Point(p1.x + static_cast<int>(dx * scale_med), p1.y + static_cast<int>(dy * scale_med));
        p_end = cv::Point(p1.x + static_cast<int>(dx * scale), p1.y + static_cast<int>(dy * scale));
        cv::line(img, p1, p_end, color, width);
    }
    double angle = std::atan2(-dy, dx);
    return std::make_tuple(p_med, p_end, angle * 180.0 / M_PI);
}

//変数の取り出し方
//std::tie(p_med, p_end) = draw_line3(...)


//ある長さの垂直な線を引く(distance関係なし)
double draw_subline(cv::Mat& img, cv::Point p_int, cv::Point p1, cv::Point p2, int length, cv::Scalar color, int width) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    double distance = std::sqrt(dx*dx + dy*dy);
    double dx_ver = -dy;
    double dy_ver = dx;
    cv::Point p_end = p_int;

    if (distance > 0) {
        double scale = length / distance;
        p_end = cv::Point(p_int.x + static_cast<int>(dx_ver * scale), p_int.y + static_cast<int>(dy_ver * scale));
        cv::line(img, p_int, p_end, color, width);
    }
    double angle = std::atan2(-dy_ver, dx_ver);
    return angle * 180.0 / M_PI;
}


void at_click(int event, int x, int y, int flags, void* param) {
    if (event == cv::EVENT_LBUTTONDOWN) {  // 左クリック時
        click++;
        std::cout << click <<  ", " << x << ", " << y << std::endl;
        cv::Mat* img = static_cast<cv::Mat*>(param);
        
        
        //ここを色々変えることで、線の出方を指定できそう！！
        //1回目　点打つだけでいい
        //2回目　1を支点にdistance + 200だけ線を引く + 100, 200に点を打つ + 100, 200を支点に垂直な線を200だけ引く
        //3回目　100を支点にdistanceだけ線を引く + 2を支点にdistanceだけ線を引く　+ 3を支点に垂直な線を20だけ引く
        //4回目　50を支点にdistanceだけ線を引く　+　3を支点にdistanceだけ線を引く　+　2を支点に200だけ線を引く
        //5回目　3を支点にdistanceだけ線を引く
        if (click == 1) {
            point1 = cv::Point(x,y);
        } else if (click == 2) {
            point2 = cv::Point(x,y);
            std::tie(point2_2, point2_3, a0) = draw_line3(*img, point1, point2, 200, cv::Scalar(100, 100, 255), 3);
            cv::circle(*img, point2_2, 4, cv::Scalar(0,0,255), -1);
            cv::circle(*img, point2_3, 4, cv::Scalar(0,0,255), -1);
            draw_subline(*img, point2_2, point1, point2, 200, cv::Scalar(100, 100, 100), 1);
            draw_subline(*img, point2_3, point1, point2, 200, cv::Scalar(100, 100, 100), 1);
            std::cout << "2_2" <<  ", " << point2_2.x << ", " << point2_2.y << std::endl;
            std::cout << "2_3" <<  ", " << point2_3.x << ", " << point2_3.y << std::endl;
        } else if (click == 3) {
            point3 = cv::Point(x,y);
            draw_line2(*img, point2_3, point3, cv::Scalar(100, 100, 255), 3);
            draw_line2(*img, point2, point3, cv::Scalar(100, 100, 100), 1);
            a1 = draw_subline(*img, point3, point3, point2_3, 100, cv::Scalar(100, 100, 255), 3);
        } else if (click == 4) {
            point4 = cv::Point(x,y);
            draw_line2(*img, point2_2, point4, cv::Scalar(100, 100, 100), 1);
            a2 = draw_line2(*img, point4, point3, cv::Scalar(100, 100, 255), 3);
            a3 = draw_line(*img, point2, point4, 200, cv::Scalar(100, 100, 255), 3);
        } else if (click == 5) {
            point5 = cv::Point(x,y);
            a4 = draw_line2(*img, point3, point5, cv::Scalar(100, 100, 255), 3);
            std::cout << a1 << std::endl;
            std::cout << a2 << std::endl;
            std::cout << a3 << std::endl;
            std::cout << a4 << std::endl;
            std::cout << "theta: " << a1-a4 << std::endl;
            std::cout << "psi: " << a2-a3 << std::endl;

            
            //小数点以下の桁数を指定して、文字列を作って画像に表示する
            std::ostringstream theta_text, psi_text;
            theta_text << "theta " << std::fixed << std::setprecision(2) << (a1 - a4);
            psi_text << "psi " << std::fixed << std::setprecision(2) << (a2 - a3);
            cv::putText(*img, theta_text.str(), cv::Point(point3.x + 20, point3.y + 50), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,255,0), 1);
            cv::putText(*img, psi_text.str(), cv::Point(point4.x + 20, point4.y + 50), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,255,0), 1);
        } else {
            click = 1;
            point1 = cv::Point(x,y);
        }


        //点を打つ
        cv::circle(*img, cv::Point(x, y), 4, cv::Scalar(0, 0, 255), -1);
        std::string num = std::to_string(click);
        cv::putText(*img, num, cv::Point(x + 5, y - 5), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,0,255), 1.5);


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


    cv::Mat img_bin;
    cv::cvtColor(img, img_bin, cv::COLOR_BGR2GRAY);
    cv::Mat img_color;
    cv::cvtColor(img_bin, img_color, cv::COLOR_GRAY2BGR);
    cv::imshow("img_bin", img_color);


    cv::setMouseCallback("img_bin", at_click, &img_color);
    while (true) {
        int key = cv::waitKey(0);  // キー入力を待つ
        if (key == 's') {  // 's'キーが押されたら
            cv::imwrite("beta1_scatter9.png", img_color);  // 画像を保存
            std::cout << "Image saved as clicked_image.png" << std::endl;
        } else if (key == 27) {  // ESCキーが押されたら終了
            break;
        }
    }

    return 0;
}