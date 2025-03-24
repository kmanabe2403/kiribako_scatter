#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <tuple>
#include <sstream>
#include <functional>

int click = 0;
int term = 0;
cv::Point point1, point2, point3, point4, point5;
cv::Point point2_1, point2_2, point2_3, point2_4; //point2_4がpoint2_endに変わる
cv::Point point4_1, point4_2, point4_3; //point4はこれに変わる
double a0, a1, a2, a3, a4; 
double psi1, psi2, psi3;
double psi_rms, psi_sigma, theta_0;
std::string coo;
std::ostringstream psi1_text, psi2_text, psi3_text, theta_text, psi_rms_text, psi_sigma_text, theta_0_text;
std::ostringstream beta_text, pb_ele_mc_text, bpc_text;
double cm = 53.69; //ピクセル

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

//ある長さの線を引く(distance+length)　+4分割のところに点を打つ
std::tuple<cv::Point, cv::Point, cv::Point, cv::Point, double> draw_line3(cv::Mat& img, cv::Point p1, cv::Point p2, int length, cv::Scalar color, int width) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    double distance = std::sqrt(dx*dx + dy*dy);
    cv::Point p_1 = p1;
    cv::Point p_2 = p1;
    cv::Point p_3 = p1;
    cv::Point p_4 = p1;

    if (distance > 0) {
        double scale_1 = 1+length/distance/4*1;
        double scale_2 = 1+length/distance/4*2;
        double scale_3 = 1+length/distance/4*3;
        double scale_4 = 1+length/distance/4*4;
        p_1 = cv::Point(p1.x + static_cast<int>(dx * scale_1), p1.y + static_cast<int>(dy * scale_1));
        p_2 = cv::Point(p1.x + static_cast<int>(dx * scale_2), p1.y + static_cast<int>(dy * scale_2));
        p_3 = cv::Point(p1.x + static_cast<int>(dx * scale_3), p1.y + static_cast<int>(dy * scale_3));
        p_4 = cv::Point(p1.x + static_cast<int>(dx * scale_4), p1.y + static_cast<int>(dy * scale_4));
        cv::line(img, p1, p_4, color, width);
    }
    double angle = std::atan2(-dy, dx);
    return std::make_tuple(p_1,p_2,p_3,p_4, angle * 180.0 / M_PI);
}

//変数の取り出し方
//std::tie(p_med, p_end) = draw_line3(...)

//ある長さの線を引く(distance+lengthぶん)
double draw_line4(cv::Mat& img, cv::Point p1, cv::Point p2, int length, cv::Scalar color, int width){
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    double distance = std::sqrt(dx*dx + dy*dy);
    cv::Point p_end = p1;

    if (distance > 0) {
        double scale = 1 + length/distance;
        p_end = cv::Point(p1.x + static_cast<int>(dx * scale), p1.y + static_cast<int>(dy * scale));
        cv::line(img, p1, p_end, color, width);
    }

    double angle = std::atan2(-dy, dx);
    return angle * 180.0 / M_PI;
}


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



//////////////////////////////////////////////////////////////////////////
//ニュートン法の関数
double NewtonRaphson(std::function<double(double)> func, std::function<double(double)> dfunc, double initial_guess, double tol = 1e-6, int max_iter = 100) {
    double x = initial_guess;
    for (int i = 0; i < max_iter; i++) {
        double fx = func(x);
        double dfx = dfunc(x);
        if (std::fabs(dfx) < 1e-8) { // ゼロ割防止
            std::cerr << "Derivative too small, stopping iteration" << std::endl;
            return x;
        }
        double x_new = x - fx / dfx;
        if (std::fabs(x_new - x) < tol) return x_new;
        x = x_new;
    }
    std::cerr << "Newton-Raphson method did not converge" << std::endl;
    return x;
}


// f1(beta) の関数
double f1(double beta, double x, double theta, double mc2) {
    const double z = 1.0;
    const double X0 = 30050.0;
    return 13.6 / mc2 * std::sqrt(1 - beta * beta) / (beta * beta) * z * std::sqrt(x / X0) *(1 + 0.038 * std::log(x * z * z / (X0 * beta * beta))) - theta;
}

// f1(beta) の微分
double df1(double beta, double x, double mc2) {
    const double z = 1.0;
    const double X0 = 30050.0;    
    double term1 = -13.6 / mc2 * z * std::sqrt(x / X0);
    double term2 = (1 + 0.038 * std::log(x * z * z / (X0 * beta * beta)));
    double term3 = -2 * beta * std::sqrt(1 - beta * beta) / (beta * beta * beta);
    double term4 = -0.076 / beta;

    return term1 * (term2 * term3 + term4);
}

// f2(bpc) の関数
double f2(double bpc, double x, double theta) {
    const double z = 1.0;
    const double X0 = 30050.0;
    double beta = 1.0;
    return 13.6 / bpc * z * std::sqrt(x / X0) * (1.0 + 0.038 * std::log(x * z * z / (X0 * beta * beta))) - theta;
}


// f2(bpc) の微分
double df2(double bpc, double x) {
    double beta = 1.0;
    const double X0 = 30050.0;
    const double z = 1.0;
    return -13.6 / (bpc * bpc) * z * std::sqrt(x / X0) * (1.0 + 0.038 * std::log(x * z * z / (X0 * beta * beta)));
}

/////////////////////////////////////////////////////////////////////////////

void at_click(int event, int x, int y, int flags, void* param) {
    if (event == cv::EVENT_LBUTTONDOWN) {  // 左クリック時
        click++;
        cv::Mat* img = static_cast<cv::Mat*>(param);
        int x_length = 400;
        double x_cm = x_length/cm;

        if (click == 1) {
            point1 = cv::Point(x,y);
            cv::circle(*img, cv::Point(x, y), 4, cv::Scalar(0, 0, 255), -1);
            //ガイドライン2
            cv::circle(*img, cv::Point(x, y), x_length/10, cv::Scalar(255, 0, 0), 1); //幅20の円を書く
            cv::putText(*img, "1", cv::Point(x + 5, y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0,0,255), 1.5);
            coo =  "1: (" + std::to_string(x) + ", " + std::to_string(y) + ")";   
            cv::putText(*img, coo, cv::Point(20, 50+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.5);
            std::cout <<  "1, " << x << ", " << y << std::endl;



        } else if (click == 2) {
            //＊step1
            point2 = cv::Point(x,y);
            cv::circle(*img, cv::Point(x, y), 4, cv::Scalar(0, 0, 255), -1);
            cv::putText(*img, "2", cv::Point(x + 5, y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0,0,255), 1.5);
            coo =  "2: (" + std::to_string(x) + ", " + std::to_string(y) + ")";   
            cv::putText(*img, coo, cv::Point(20, 65+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.5);
            std::cout << "2, " << x << ", " << y << std::endl;

            //ガイドライン2を消す
            cv::circle(*img, cv::Point(point1.x, point1.y), x_length/10, cv::Scalar(100, 100, 100), 1); //幅20の円を書く
            

            //＊step2
            std::tie(point2_1, point2_2, point2_3, point2_4, a0) = draw_line3(*img, point1, point2, x_length, cv::Scalar(100, 100, 255), 1);
            cv::circle(*img, point2_1, 4, cv::Scalar(0,0,255), -1);
            cv::circle(*img, point2_2, 4, cv::Scalar(0,0,255), -1);
            cv::circle(*img, point2_3, 4, cv::Scalar(0,0,255), -1);
            cv::circle(*img, point2_4, 4, cv::Scalar(0,0,255), -1);
            cv::putText(*img, "2_1", cv::Point(point2_1.x + 5, point2_1.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0,0,255), 1.5);
            cv::putText(*img, "2_2", cv::Point(point2_2.x + 5, point2_2.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0,0,255), 1.5);
            cv::putText(*img, "2_3", cv::Point(point2_3.x + 5, point2_3.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0,0,255), 1.5);
            cv::putText(*img, "2_4", cv::Point(point2_4.x + 5, point2_4.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0,0,255), 1.5);
            coo =  "2_1: (" + std::to_string(point2_1.x) + ", " + std::to_string(point2_1.y) + ")";   
            cv::putText(*img, coo, cv::Point(20, 80+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.5);
            coo =  "2_2: (" + std::to_string(point2_2.x) + ", " + std::to_string(point2_2.y) + ")";   
            cv::putText(*img, coo, cv::Point(20, 95+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.5);
            coo =  "2_3: (" + std::to_string(point2_3.x) + ", " + std::to_string(point2_3.y) + ")";   
            cv::putText(*img, coo, cv::Point(20, 110+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.5);
            coo =  "2_4: (" + std::to_string(point2_4.x) + ", " + std::to_string(point2_4.y) + ")";   
            cv::putText(*img, coo, cv::Point(20, 125+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.5);
            std::cout << "2_1" <<  ", " << point2_1.x << ", " << point2_1.y << std::endl;
            std::cout << "2_2" <<  ", " << point2_2.x << ", " << point2_2.y << std::endl;
            std::cout << "2_3" <<  ", " << point2_3.x << ", " << point2_3.y << std::endl;
            std::cout << "2_4" <<  ", " << point2_4.x << ", " << point2_4.y << std::endl;


            //＊step3
            draw_subline(*img, point2_4, point1, point2, 20, cv::Scalar(0, 255, 0), 1); //key 1の向き
            draw_subline(*img, point2_4, point1, point2, -20, cv::Scalar(255, 0, 0), 1); //key 2の向き
            cv::putText(*img, "x  " + std::to_string(x_length), cv::Point(point2.x + 50, point2.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0,255,0), 1);
            cv::putText(*img, "x  " + std::to_string(x_length), cv::Point(230, 50+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1);

            cv::putText(*img, "x cm " + std::to_string(x_cm), cv::Point(230, 65+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1);
        

            //＊step4
            cv::imshow("img_bin", *img);
            int key = cv::waitKey(0);  // キー入力を待つ、ガイドラインの色を消す
            draw_subline(*img, point2_4, point1, point2, 20, cv::Scalar(100,100,100), 1); //key 1の向き
            draw_subline(*img, point2_4, point1, point2, -20, cv::Scalar(100,100,100), 1); //key 2の向き
            if (key == '1') {  // 1が押されたら、point1->point2に対して右側に線を引く
                draw_subline(*img, point2_1, point1, point2, x_length, cv::Scalar(100, 100, 100), 1);
                draw_subline(*img, point2_2, point1, point2, x_length, cv::Scalar(100, 100, 100), 1);
                draw_subline(*img, point2_3, point1, point2, x_length, cv::Scalar(100, 100, 100), 1);
                draw_subline(*img, point2_4, point1, point2, x_length, cv::Scalar(100, 100, 100), 1);
                draw_subline(*img, point2_1, point1, point2, -x_length/10, cv::Scalar(100, 100, 100), 1);
                draw_subline(*img, point2_2, point1, point2, -x_length/10, cv::Scalar(100, 100, 100), 1);
                draw_subline(*img, point2_3, point1, point2, -x_length/10, cv::Scalar(100, 100, 100), 1);
                draw_subline(*img, point2_4, point1, point2, -x_length/10, cv::Scalar(100, 100, 100), 1);
            } else if (key == '2') {  // 2が押されたら、point1->point2に対して左側に線を引く
                draw_subline(*img, point2_1, point1, point2, -x_length, cv::Scalar(100, 100, 100), 1);
                draw_subline(*img, point2_2, point1, point2, -x_length, cv::Scalar(100, 100, 100), 1);
                draw_subline(*img, point2_3, point1, point2, -x_length, cv::Scalar(100, 100, 100), 1);
                draw_subline(*img, point2_4, point1, point2, -x_length, cv::Scalar(100, 100, 100), 1);
                draw_subline(*img, point2_1, point1, point2, x_length/10, cv::Scalar(100, 100, 100), 1);
                draw_subline(*img, point2_2, point1, point2, x_length/10, cv::Scalar(100, 100, 100), 1);
                draw_subline(*img, point2_3, point1, point2, x_length/10, cv::Scalar(100, 100, 100), 1);
                draw_subline(*img, point2_4, point1, point2, x_length/10, cv::Scalar(100, 100, 100), 1);
            }

            //ガイドライン3
            cv::circle(*img, point2_4, 4, cv::Scalar(255,0,0), -1);
            draw_subline(*img, point2_4, point1, point2, 10, cv::Scalar(255, 0, 0), 1); //key 1の向き
            draw_subline(*img, point2_4, point1, point2, -10, cv::Scalar(255, 0, 0), 1); //key 2の向き


            
            

        } else if (click == 3) {
            point3 = cv::Point(x,y);
            cv::circle(*img, cv::Point(x, y), 4, cv::Scalar(0, 0, 255), -1);
            cv::putText(*img, "3", cv::Point(x + 5, y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0,0,255), 1.5);
            coo =  "3: (" + std::to_string(x) + ", " + std::to_string(y) + ")";   
            cv::putText(*img, coo, cv::Point(20, 140+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.5);
            std::cout << "3, " << x << ", " << y << std::endl;



            draw_line2(*img, point2_4, point3, cv::Scalar(100, 100, 255), 1);


            //ガイドライン3を消す
            cv::circle(*img, point2_4, 4, cv::Scalar(0,0,255), -1);
            draw_subline(*img, point2_4, point1, point2, 10, cv::Scalar(100,100,100), 1); //key 1の向き
            draw_subline(*img, point2_4, point1, point2, -10, cv::Scalar(100,100,100), 1); //key 2の向き
            //ガイドライン4を書く
            cv::circle(*img, point2_1, 4, cv::Scalar(255,0,0), -1);
            draw_subline(*img, point2_1, point1, point2, 10, cv::Scalar(255,0,0), 1); //key 1の向き
            draw_subline(*img, point2_1, point1, point2, -10, cv::Scalar(255,0,0), 1); //key 2の向き



        } else if (click == 4) {
            //＊step1
            point4_1 = cv::Point(x,y);
            cv::circle(*img, cv::Point(x, y), 4, cv::Scalar(0, 0, 255), -1);
            coo =  "4_1: (" + std::to_string(x) + ", " + std::to_string(y) + ")";   
            cv::putText(*img, coo, cv::Point(20, 155+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.5);
            cv::putText(*img, "4_1", cv::Point(x + 5, y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0,0,255), 1.5);
            std::cout <<  "4_1, " << x << ", " << y << std::endl;


            //＊step2
            draw_line2(*img, point2_1, point4_1, cv::Scalar(100, 100, 100), 1);
            a2 = draw_line2(*img, point4_1, point3, cv::Scalar(100, 100, 255), 1);
            a3 = draw_line4(*img, point2, point4_1, x_length/10, cv::Scalar(100, 100, 255), 1);
            psi1 = a2-a3;

            std::cout << a2 << std::endl;
            std::cout << a3 << std::endl;
            std::cout << "psi1: " << a2-a3 << std::endl;


            psi1_text.str("");  // 文字列の内容をリセット
            psi1_text.clear();  // 状態フラグをリセット
            psi1_text << "psi1 " << std::fixed << std::setprecision(2) << (a2 - a3);
            cv::putText(*img, psi1_text.str(), cv::Point(point4_1.x + 20, point4_1.y + 50), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0,255,0), 1);
            cv::putText(*img, psi1_text.str(), cv::Point(230, 80+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1);


            //ガイドライン4を消す
            cv::circle(*img, point2_1, 4, cv::Scalar(0,0,255), -1);
            draw_subline(*img, point2_1, point1, point2, 10, cv::Scalar(100,100,100), 1); //key 1の向き
            draw_subline(*img, point2_1, point1, point2, -10, cv::Scalar(100,100,100), 1); //key 2の向き
            //ガイドライン5を書く
            cv::circle(*img, point2_2, 4, cv::Scalar(255,0,0), -1);
            draw_subline(*img, point2_2, point1, point2, 10, cv::Scalar(255,0,0), 1); //key 1の向き
            draw_subline(*img, point2_2, point1, point2, -10, cv::Scalar(255,0,0), 1); //key 2の向き



        } else if (click == 5){
            point4_2 = cv::Point(x,y);
            point4_2 = cv::Point(x,y);
            cv::circle(*img, cv::Point(x, y), 4, cv::Scalar(0, 0, 255), -1);
            coo =  "4_2: (" + std::to_string(x) + ", " + std::to_string(y) + ")";   
            cv::putText(*img, coo, cv::Point(20, 170+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.5);
            cv::putText(*img, "4_2", cv::Point(x + 5, y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0,0,255), 1.5);
            std::cout <<  "4_2, " << x << ", " << y << std::endl;
            //＊step2
            draw_line2(*img, point2_2, point4_2, cv::Scalar(100, 100, 100), 1);
            a2 = draw_line2(*img, point4_2, point3, cv::Scalar(100, 100, 255), 1);
            a3 = draw_line4(*img, point2, point4_2, x_length/10, cv::Scalar(100, 100, 255), 1);
            psi2 = a2-a3;
            std::cout << a2 << std::endl;
            std::cout << a3 << std::endl;
            std::cout << "psi2: " << a2-a3 << std::endl;
            psi2_text.str("");  // 文字列の内容をリセット
            psi2_text.clear();  // 状態フラグをリセット
            psi2_text << "psi2 " << std::fixed << std::setprecision(2) << (a2 - a3);
            cv::putText(*img, psi2_text.str(), cv::Point(point4_2.x + 20, point4_2.y + 50), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0,255,0), 1);
            cv::putText(*img, psi2_text.str(), cv::Point(230, 95+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1);


            //ガイドライン5を消す
            cv::circle(*img, point2_2, 4, cv::Scalar(0,0,255), -1);
            draw_subline(*img, point2_2, point1, point2, 10, cv::Scalar(100,100,100), 1); //key 1の向き
            draw_subline(*img, point2_2, point1, point2, -10, cv::Scalar(100,100,100), 1); //key 2の向き
            //ガイドライン6を書く
            cv::circle(*img, point2_3, 4, cv::Scalar(255,0,0), -1);
            draw_subline(*img, point2_3, point1, point2, 10, cv::Scalar(255,0,0), 1); //key 1の向き
            draw_subline(*img, point2_3, point1, point2, -10, cv::Scalar(255,0,0), 1); //key 2の向き


        } else if (click == 6){
            cv::Mat* img = static_cast<cv::Mat*>(param);
            //＊step1
            point4_3 = cv::Point(x,y);
            cv::circle(*img, cv::Point(x, y), 4, cv::Scalar(0, 0, 255), -1);
            coo =  "4_3: (" + std::to_string(x) + ", " + std::to_string(y) + ")";   
            cv::putText(*img, coo, cv::Point(20, 185+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.5);
            cv::putText(*img, "4_3", cv::Point(x + 5, y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0,0,255), 1.5);
            std::cout <<  "4_3, " << x << ", " << y << std::endl;
            //＊step2
            draw_line2(*img, point2_3, point4_3, cv::Scalar(100, 100, 100), 1);
            a2 = draw_line2(*img, point4_3, point3, cv::Scalar(100, 100, 255), 1);
            a3 = draw_line4(*img, point2, point4_3, x_length/10, cv::Scalar(100, 100, 255), 1);
            psi3 = a2-a3;
            std::cout << a2 << std::endl;
            std::cout << a3 << std::endl;
            std::cout << "psi3: " << a2-a3 << std::endl;
            psi3_text.str("");  // 文字列の内容をリセット
            psi3_text.clear();  // 状態フラグをリセット
            psi3_text << "psi3 " << std::fixed << std::setprecision(2) << (a2 - a3);
            cv::putText(*img, psi3_text.str(), cv::Point(point4_3.x + 20, point4_3.y + 50), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0,255,0), 1);
            cv::putText(*img, psi3_text.str(), cv::Point(230, 110+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1);

            //＊step5
            draw_line2(*img, point2, point3, cv::Scalar(100, 100, 100), 1);
        

            //ガイドライン6を消す
            cv::circle(*img, point2_3, 4, cv::Scalar(0,0,255), -1);
            draw_subline(*img, point2_3, point1, point2, 10, cv::Scalar(100,100,100), 1); //key 1の向き
            draw_subline(*img, point2_3, point1, point2, -10, cv::Scalar(100,100,100), 1); //key 2の向き
            //ガイドライン7を書く
            cv::circle(*img, cv::Point(point3.x, point3.y), x_length/10, cv::Scalar(255, 0, 0), 1); //幅20の円を書く

        } else if (click == 7) {
            //＊step1
            point5 = cv::Point(x,y);
            cv::circle(*img, cv::Point(x, y), 4, cv::Scalar(0, 0, 255), -1);
            cv::putText(*img, "5", cv::Point(x + 5, y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0,0,255), 1.5);
            coo =  "5: (" + std::to_string(x) + ", " + std::to_string(y) + ")";   
            cv::putText(*img, coo, cv::Point(20, 200+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1.5);
            std::cout << 5 <<  ", " << x << ", " << y << std::endl;
            //ガイドライン7を消す
            cv::circle(*img, cv::Point(point3.x, point3.y), x_length/10, cv::Scalar(100, 100, 100), 1);


            //＊step2
            a4 = draw_line2(*img, point3, point5, cv::Scalar(100, 100, 255), 1);
            double theta = a1-a4;
            theta_text.str("");  // 文字列の内容をリセット
            theta_text.clear();  // 状態フラグをリセット
            theta_text << "theta " << std::fixed << std::setprecision(2) << (a1 - a4);
            cv::putText(*img, theta_text.str(), cv::Point(point3.x + 20, point3.y + 50), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0,255,0), 1);
            cv::putText(*img, theta_text.str(), cv::Point(230, 125+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1);
            std::cout << a1 << std::endl;
            std::cout << a4 << std::endl;
            std::cout << "theta: " << a1-a4 << std::endl;

            //＊step3
            draw_subline(*img, point3, point3, point2_4, 20, cv::Scalar(0, 255, 0), 1); //key 1の向き
            draw_subline(*img, point3, point3, point2_4, -20, cv::Scalar(255, 0, 0), 1); //key 2の向き

            //＊step4
            cv::imshow("img_bin", *img);
            int key = cv::waitKey(0);  // キー入力を待つ、ガイドラインの色を消す
            draw_subline(*img, point3, point3, point2_4, 20, cv::Scalar(100,100,100), 1); //key 1の向き
            draw_subline(*img, point3, point3, point2_4, -20, cv::Scalar(100,100,100), 1); //key 2の向き
            if (key == '1') {  // 1が押されたら、point3->point2_4に対して右側に線を引く
                a1 = draw_subline(*img, point3, point3, point2_4, x_length/10, cv::Scalar(100, 100, 255), 1);
            } else if (key == '2') {  // 2が押されたら、point3->point2_4に対して左側に線を引く
                a1 = draw_subline(*img, point3, point3, point2_4, -x_length/10, cv::Scalar(100, 100, 255), 1);
            }



            //＊step5
            draw_rectangle(*img, point2, point2, point2_4, point2_4, point3, cv::Scalar(150,150,150), 1);

            //＊step6
            psi_rms = std::sqrt((psi1 * psi1 + psi2 * psi2 + psi3 * psi3) / 3.0);
            psi_rms_text.str("");  // 文字列の内容をリセット
            psi_rms_text.clear();  // 状態フラグをリセット
            psi_rms_text << "psi_rms " << std::fixed << std::setprecision(2) << psi_rms;
            cv::putText(*img, psi_rms_text.str(), cv::Point(230, 155+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1);
            std::cout << "psi_rms: " << psi_rms << std::endl;

            psi_sigma = std::sqrt(std::sqrt(((psi_rms*psi_rms - psi1*psi1)*(psi_rms*psi_rms - psi1*psi1) + (psi_rms*psi_rms - psi2*psi2)*(psi_rms*psi_rms - psi2*psi2) + (psi_rms*psi_rms - psi3*psi3)*(psi_rms*psi_rms - psi3*psi3))/3)); 
            psi_sigma_text.str("");  // 文字列の内容をリセット
            psi_sigma_text.clear();  // 状態フラグをリセット
            psi_sigma_text << "psi_sigma " << std::fixed << std::setprecision(2) << psi_sigma;
            cv::putText(*img, psi_sigma_text.str(), cv::Point(230, 170+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1);
            std::cout << "psi_sigma: " << psi_sigma << std::endl;

            theta_0 = std::sqrt(3.0) * psi_rms;
            theta_0_text.str("");  // 文字列の内容をリセット
            theta_0_text.clear();  // 状態フラグをリセット
            theta_0_text << "theta_0 " << std::fixed << std::setprecision(2) << theta_0;
            cv::putText(*img, theta_0_text.str(), cv::Point(230, 185+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1);
            std::cout << "theta_0: " << theta_0 << std::endl;



            //＊step7
            //theta_0とthetaとx_cmから、beta, beta gamma, (bpc)を求める

            //今はelectronの場合を考えてる。muonにしたければmc2を変える
            double initial_guess = 0.1;
            beta_text.str("");  // 文字列の内容をリセット
            beta_text.clear();  // 状態フラグをリセット
            pb_ele_mc_text.str("");  // 文字列の内容をリセット
            pb_ele_mc_text.clear();  // 状態フラグをリセット
            bpc_text.str("");  // 文字列の内容をリセット
            bpc_text.clear();  // 状態フラグをリセット

            double beta_ans = NewtonRaphson([&](double beta) { return f1(beta, x_cm, theta*M_PI/180, 0.511); },[&](double beta) { return df1(beta, x_cm, 0.511); },initial_guess);
            beta_text << "beta " << std::fixed << std::setprecision(2) << beta_ans;
            cv::putText(*img, beta_text.str(), cv::Point(230, 215+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1);
            std::cout << "beta: " << beta_ans << std::endl;
            double pb_ele_mc = beta_ans / std::sqrt(1 - beta_ans * beta_ans);
            pb_ele_mc_text << "p/mc " << std::fixed << std::setprecision(2) << pb_ele_mc;
            cv::putText(*img, pb_ele_mc_text.str(), cv::Point(230, 230+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1);
            std::cout << "p/mc: " << pb_ele_mc << std::endl;
            double bpc_ans = NewtonRaphson([&](double bpc) { return f2(bpc, x_cm, theta*M_PI/180); },[&](double bpc) { return df2(bpc, x_cm); },initial_guess);
            bpc_text << "bpc " << std::fixed << std::setprecision(2) << bpc_ans;
            cv::putText(*img, bpc_text.str(), cv::Point(230, 245+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1);
            std::cout << "bpc: " << bpc_ans << std::endl;



            beta_text.str("");  // 文字列の内容をリセット
            beta_text.clear();  // 状態フラグをリセット
            pb_ele_mc_text.str("");  // 文字列の内容をリセット
            pb_ele_mc_text.clear();  // 状態フラグをリセット
            bpc_text.str("");  // 文字列の内容をリセット
            bpc_text.clear();  // 状態フラグをリセット


            beta_ans = NewtonRaphson([&](double beta) { return f1(beta, x_cm, theta_0*M_PI/180, 0.511); },[&](double beta) { return df1(beta, x_cm, 0.511); },initial_guess);
            beta_text << "beta_0 " << std::fixed << std::setprecision(2) << beta_ans;
            cv::putText(*img, beta_text.str(), cv::Point(230, 275+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1);
            std::cout << "beta_0: " << beta_ans << std::endl;
            pb_ele_mc = beta_ans / std::sqrt(1 - beta_ans * beta_ans);
            pb_ele_mc_text << "p/mc_0 " << std::fixed << std::setprecision(2) << pb_ele_mc;
            cv::putText(*img, pb_ele_mc_text.str(), cv::Point(230, 290+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1);
            std::cout << "p/mc_0: " << pb_ele_mc << std::endl;
            bpc_ans = NewtonRaphson([&](double bpc) { return f2(bpc, x_cm, theta_0*M_PI/180); },[&](double bpc) { return df2(bpc, x_cm); },initial_guess);
            bpc_text << "bpc_0 " << std::fixed << std::setprecision(2) << bpc_ans;
            cv::putText(*img, bpc_text.str(), cv::Point(230, 305+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,0,255), 1);
            std::cout << "bpc_0: " << bpc_ans << std::endl;

        } else {
            click = 1;
            term++; 
            point1 = cv::Point(x,y);
            cv::circle(*img, cv::Point(x, y), 4, cv::Scalar(0, 0, 255), -1);
            cv::circle(*img, cv::Point(x, y), x_length/10, cv::Scalar(255, 0, 0), 1); //幅20の円を書く
            cv::putText(*img, "1", cv::Point(x + 5, y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0,0,255), 1.5);
            coo =  "1: (" + std::to_string(x) + ", " + std::to_string(y) + ")";   
            cv::putText(*img, coo, cv::Point(20, 50+term*300), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 0, 255), 1.5);
            std::cout << 1 <<  ", " << x << ", " << y << std::endl;   
        }

        //画像の更新、これがないと線などが反映されない
        cv::imshow("img_bin", *img);
    }
}

int main() {
    // 画像を読み込む
    std::string name = "example2";

    std::ostringstream name_in, name_out;
    name_in << "./" << name << ".png";
    name_out << "./png_test/" << name << "_scatter16.png";


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



    cv::circle(img_color, cv::Point(img.cols - 600, 100), 4, cv::Scalar(0, 0, 255), -1);
    cv::putText(img_color, "0", cv::Point(img.cols - 600, 100 - 10), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0, 0, 255), 1.5);
    cv::circle(img_color, cv::Point(img.cols - 550, 100), 4, cv::Scalar(0, 0, 255), -1);
    cv::putText(img_color, "1: 50", cv::Point(img.cols - 550, 100 - 10), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0, 0, 255), 1.5);
    cv::circle(img_color, cv::Point(img.cols - 500, 100), 4, cv::Scalar(0, 0, 255), -1);
    cv::putText(img_color, "2: 100", cv::Point(img.cols - 500, 100 - 10), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0, 0, 255), 1.5);
    cv::circle(img_color, cv::Point(img.cols - 400, 100), 4, cv::Scalar(0, 0, 255), -1);
    cv::putText(img_color, "3: 200", cv::Point(img.cols - 400, 100 - 10), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0, 0, 255), 1.5);
    cv::circle(img_color, cv::Point(img.cols - 200, 100), 4, cv::Scalar(0, 0, 255), -1);
    cv::putText(img_color, "4: 400", cv::Point(img.cols - 200, 100 - 10), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0, 0, 255), 1.5);
    cv::line(img_color, cv::Point(img.cols - 600, 100), cv::Point(img.cols - 100, 100), cv::Scalar(0, 0, 255), 2);
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