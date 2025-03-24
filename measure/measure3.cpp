#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <tuple>
#include <sstream>

int click = 0;
int term = 0;
cv::Point point0, point00, point1, point2, point2_2, point2_3, point3, point4, point5;
double a0, a1, a2, a3, a4; 
std::string coo;

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

//始点を指定し、p1からp2までの長さの線を引く。長方形用。
void draw_rectangle(cv::Mat& img, cv::Point p_1, cv::Point p1, cv::Point p2, cv::Point p3, cv::Point p4, cv::Scalar color, int width) {
    double dx_v = p2.x - p1.x;
    double dy_v = p2.y - p1.y;
    double dx_h = p4.x - p3.x;
    double dy_h = p4.y - p3.y;
    cv::Point p_2 = p_1;
    cv::Point p_3 = p_1;
    cv::Point p_4 = p_1;
    double distance = std::sqrt(dx_v*dx_v + dy_v*dy_v);

    if (distance > 0) {
        double scale = 1;
        p_2 = cv::Point(p_1.x + static_cast<int>(dx_v), p_1.y + static_cast<int>(dy_v));
        p_4 = cv::Point(p_1.x + static_cast<int>(dx_h), p_1.y + static_cast<int>(dy_h));
        p_3 = cv::Point(p_4.x + static_cast<int>(dx_v), p_4.y + static_cast<int>(dy_v));
        cv::line(img, p_1, p_2, color, width);
        cv::line(img, p_2, p_3, color, width);
        cv::line(img, p_3, p_4, color, width);
        cv::line(img, p_4, p_1, color, width);
    }
}


void at_click(int event, int x, int y, int flags, void* param) {
    if (event == cv::EVENT_LBUTTONDOWN) {  // 左クリック時
        click++;
        std::cout << click <<  ", " << x << ", " << y << std::endl;
        cv::Mat* img = static_cast<cv::Mat*>(param);
        
        
        //ここを色々変えることで、線の出方を指定できそう！！
        //1回目　点打つだけでいい　」ok
        //2回目　1を支点にdistance + 200だけ線を引く 」ok
        //　　　　+ 100, 200に点を打つ　」ok
        //　　　　+ 100支点に垂直な線を200だけ引く　」◉向き調整
        //　　　　+ 200支点に垂直な線を200だけ引く　」◉向き調整
        //3回目　100を支点にdistanceだけ線を引く 　」ok
        //　　　　+ 2を支点にdistanceだけ線を引く　」ok
        //　　　　+ 3を支点に垂直な線を20だけ引く　」◉向き調整
        //4回目　50を支点にdistanceだけ線を引く　」ok
        //　　　　+ 3を支点にdistanceだけ線を引く　」ok
        //　　　　+ 2を支点に200だけ線を引く　」ok
        //5回目　3を支点にdistanceだけ線を引く　」ok

        int x_length = 400;

        if (click == 1) {
            point1 = cv::Point(x,y);
            cv::circle(*img, cv::Point(x, y), 4, cv::Scalar(0, 0, 255), -1);
            coo =  "1: (" + std::to_string(x) + ", " + std::to_string(y) + ")";  // 右上隅の位置 (幅 - 120, 高さ 20)に座標を書く
            cv::putText(*img, coo, cv::Point(20, 20+term*120+30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.5);
            cv::circle(*img, cv::Point(x, y), x_length/10, cv::Scalar(255, 0, 0), 1); //幅20の円を書く
            cv::putText(*img, std::to_string(click), cv::Point(x + 5, y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1.5);
            cv::imshow("img_bin", *img);


        } else if (click == 2) {
            point2 = cv::Point(x,y);
            cv::circle(*img, cv::Point(x, y), 4, cv::Scalar(0, 0, 255), -1);
            coo =  "2: (" + std::to_string(x) + ", " + std::to_string(y) + ")";  // 右上隅の位置 (幅 - 120, 高さ 20)に座標を書く
            cv::putText(*img, coo, cv::Point(20, 35+term*120+30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.5);
            cv::putText(*img, std::to_string(click), cv::Point(x + 5, y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1.5);

            std::tie(point2_2, point2_3, a0) = draw_line3(*img, point1, point2, x_length, cv::Scalar(100, 100, 255), 1);
            cv::circle(*img, point2_2, 4, cv::Scalar(0,0,255), -1);
            cv::circle(*img, point2_3, 4, cv::Scalar(0,0,255), -1);
            coo =  "2_2: (" + std::to_string(point2_2.x) + ", " + std::to_string(point2_2.y) + ")";  // 右上隅の位置 (幅 - 120, 高さ 20)に座標を書く
            cv::putText(*img, coo, cv::Point(20, 50+term*120+30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.5);
            coo =  "2_3: (" + std::to_string(point2_3.x) + ", " + std::to_string(point2_3.y) + ")";  // 右上隅の位置 (幅 - 120, 高さ 20)に座標を書く
            cv::putText(*img, coo, cv::Point(20, 65+term*120+30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.5);
            cv::putText(*img, "2_2", cv::Point(point2_2.x + 5, point2_2.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1.5);
            cv::putText(*img, "2_3", cv::Point(point2_3.x + 5, point2_3.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1.5);
            std::cout << "2_2" <<  ", " << point2_2.x << ", " << point2_2.y << std::endl;
            std::cout << "2_3" <<  ", " << point2_3.x << ", " << point2_3.y << std::endl;

            draw_subline(*img, point2_3, point1, point2, 20, cv::Scalar(0, 255, 0), 1); //key 1の向き
            draw_subline(*img, point2_3, point1, point2, -20, cv::Scalar(255, 0, 0), 1); //key 2の向き

            cv::putText(*img, "x  " + std::to_string(x_length), cv::Point(point2.x + 50, point2.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,255,0), 1);

            cv::imshow("img_bin", *img);



            int key = cv::waitKey(0);  // キー入力を待つ、ガイドラインの色を消す
            draw_subline(*img, point2_3, point1, point2, 20, cv::Scalar(100,100,100), 1); //key 1の向き
            draw_subline(*img, point2_3, point1, point2, -20, cv::Scalar(100,100,100), 1); //key 2の向き
            if (key == '1') {  // 1が押されたら、point1->point2に対して右側に線を引く
                draw_subline(*img, point2_2, point1, point2, x_length, cv::Scalar(100, 100, 100), 1);
                draw_subline(*img, point2_3, point1, point2, x_length, cv::Scalar(100, 100, 100), 1);
            } else if (key == '2') {  // 2が押されたら、point1->point2に対して左側に線を引く
                draw_subline(*img, point2_2, point1, point2, -x_length, cv::Scalar(100, 100, 100), 1);
                draw_subline(*img, point2_3, point1, point2, -x_length, cv::Scalar(100, 100, 100), 1);
            }




            
            
            

        } else if (click == 3) {
            point3 = cv::Point(x,y);
            cv::circle(*img, cv::Point(x, y), 4, cv::Scalar(0, 0, 255), -1);
            cv::circle(*img, cv::Point(x, y), x_length/10, cv::Scalar(255, 0, 0), 1); //幅20の円を書く
            coo =  "3: (" + std::to_string(x) + ", " + std::to_string(y) + ")";  // 右上隅の位置 (幅 - 120, 高さ 20)に座標を書く
            cv::putText(*img, coo, cv::Point(20, 80+term*120+30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.5);
            cv::putText(*img, std::to_string(click), cv::Point(x + 5, y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1.5);
            draw_line2(*img, point2_3, point3, cv::Scalar(100, 100, 255), 1);


        } else if (click == 4) {
            point4 = cv::Point(x,y);
            cv::circle(*img, cv::Point(x, y), 4, cv::Scalar(0, 0, 255), -1);
            coo =  "4: (" + std::to_string(x) + ", " + std::to_string(y) + ")";  // 右上隅の位置 (幅 - 120, 高さ 20)に座標を書く
            cv::putText(*img, coo, cv::Point(20, 95+term*120+30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.5);
            cv::putText(*img, std::to_string(click), cv::Point(x + 5, y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1.5);
            draw_line2(*img, point2_2, point4, cv::Scalar(100, 100, 100), 1);
            a2 = draw_line2(*img, point4, point3, cv::Scalar(100, 100, 255), 1);
            a3 = draw_line(*img, point2, point4, x_length, cv::Scalar(100, 100, 255), 1);

            //これをpoin4のあとにする
            draw_line2(*img, point2, point3, cv::Scalar(100, 100, 100), 1);



            
        } else if (click == 5) {
            point5 = cv::Point(x,y);
            cv::circle(*img, cv::Point(x, y), 4, cv::Scalar(0, 0, 255), -1);
            coo =  "5: (" + std::to_string(x) + ", " + std::to_string(y) + ")";  // 右上隅の位置 (幅 - 120, 高さ 20)に座標を書く
            cv::putText(*img, coo, cv::Point(20, 110+term*120+30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.5);
            cv::putText(*img, std::to_string(click), cv::Point(x + 5, y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1.5);
            a4 = draw_line2(*img, point3, point5, cv::Scalar(100, 100, 255), 1);
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

            cv::putText(*img, "x  " + std::to_string(x_length), cv::Point(200, 20+term*120+30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1);
            cv::putText(*img, theta_text.str(), cv::Point(200, 35+term*120+30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1);
            cv::putText(*img, psi_text.str(), cv::Point(200, 50+term*120+30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1);

            //注目した区画を長方形で囲む
            draw_rectangle(*img, point2, point2, point2_3, point2_3, point3, cv::Scalar(255,0,0), 1);


            //このへんをpoint5の後にする
            draw_subline(*img, point3, point3, point2_3, 20, cv::Scalar(0, 255, 0), 1); //key 1の向き
            draw_subline(*img, point3, point3, point2_3, -20, cv::Scalar(255, 0, 0), 1); //key 2の向き
            cv::imshow("img_bin", *img);
            int key = cv::waitKey(0);  // キー入力を待つ、ガイドラインの色を消す
            draw_subline(*img, point3, point3, point2_3, 20, cv::Scalar(100,100,100), 1); //key 1の向き
            draw_subline(*img, point3, point3, point2_3, -20, cv::Scalar(100,100,100), 1); //key 2の向き
            if (key == '1') {  // 1が押されたら、point3->point2_3に対して右側に線を引く
                a1 = draw_subline(*img, point3, point3, point2_3, x_length/10, cv::Scalar(100, 100, 255), 1);
            } else if (key == '2') {  // 2が押されたら、point3->point2_3に対して左側に線を引く
                a1 = draw_subline(*img, point3, point3, point2_3, -x_length/10, cv::Scalar(100, 100, 255), 1);
            }



        } else {
            click = 1;
            term++;
            point1 = cv::Point(x,y);
            coo =  "1: (" + std::to_string(x) + ", " + std::to_string(y) + ")";  // 右上隅の位置 (幅 - 120, 高さ 20)に座標を書く
            cv::putText(*img, coo, cv::Point(20, 20+term*120+30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.5);
            cv::circle(*img, cv::Point(x, y), 4, cv::Scalar(0, 0, 255), -1);
            cv::circle(*img, cv::Point(x, y), x_length/10, cv::Scalar(255, 0, 0), 1); //幅20の円を書く
            cv::putText(*img, std::to_string(click), cv::Point(x + 5, y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1.5);
       
       
        }


        //点を打つ
        //これを、適宜上で入れた方がいい
        // cv::circle(*img, cv::Point(x, y), 4, cv::Scalar(0, 0, 255), -1);
        // cv::putText(*img, std::to_string(click), cv::Point(x + 5, y - 5), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,0,255), 1.5);

        //画像の更新、これがないと線などが反映されない
        cv::imshow("img_bin", *img);
    }
}

int main() {
    // 画像を読み込む
    std::string name = "muon5";

    std::ostringstream name_in, name_out;
    name_in << "./png/" << name << ".png";
    name_out << "./png_measure3/" << name << "_measure3.png";


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