#include <opencv2/opencv.hpp> //画像・動画処理のライブラリ
#include <iostream> //標準入出力
#include <fstream> //ファイル入出力
#include <unordered_map> //設定ファイルのキー、値の格納
#include <string>
#include <thread> //並列処理用(このコードでは使用していない)
#include <atomic>
//#include "readConfig.cpp" //外部ファイルの設定の読み込み
//#include "timer.cpp"
#include <algorithm> //なんか足りなかったので追加


int DEBUG=1;

#define CHECK if(DEBUG==1)printf("LINE = %d\n", __LINE__)

void printImage(cv::Mat& img){
	  // 行数
  std::cout << "rows:" << img.rows <<std::endl;
  // 列数
  std::cout << "cols:" << img.cols << std::endl;
  // 次元数
  std::cout << "dims:" << img.dims << std::endl;
  // サイズ（2次元の場合）
  std::cout << "size[]:" << img.size().width << "," << img.size().height << std::endl;
  // ビット深度ID
  std::cout << "depth (ID):" << img.depth() << "(=" << CV_64F << ")" << std::endl;
  // チャンネル数
  std::cout << "channels:" << img.channels() << std::endl;
  // （複数チャンネルから成る）1要素のサイズ [バイト単位]
  std::cout << "elemSize:" << img.elemSize() << "[byte]" << std::endl;
  // 1要素内の1チャンネル分のサイズ [バイト単位]
  std::cout << "elemSize1 (elemSize/channels):" << img.elemSize1() << "[byte]" << std::endl;
  // 要素の総数
  std::cout << "total:" << img.total() << std::endl;
  // ステップ数 [バイト単位]
  std::cout << "step:" << img.step << "[byte]" << std::endl;
  // 1ステップ内のチャンネル総数
  std::cout << "step1 (step/elemSize1):" << img.step1()  << std::endl;
  // データは連続か？
  std::cout << "isContinuous:" << (img.isContinuous()?"true":"false") << std::endl;
  // 部分行列か？
  std::cout << "isSubmatrix:" << (img.isSubmatrix()?"true":"false") << std::endl;
  // データは空か？
  std::cout << "empty:" << (img.empty()?"true":"false") << std::endl;
}


cv::Mat addPaddingToWidth(const cv::Mat& img, int targetWidth) {
    if (img.cols >= targetWidth) {
        return img;
    }

    int paddingWidth = targetWidth - img.cols;
    cv::Mat paddedImg;
    cv::copyMakeBorder(img, paddedImg, 0, 0, 0, paddingWidth, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    return paddedImg;
}

// Function to count pixels along a detected line
int countPixelsOnLine(const cv::Mat& image, float rho, float theta) {
    // Calculate line equation parameters
    double a = cos(theta), b = sin(theta);
    double x0 = a * rho, y0 = b * rho;

    // Image dimensions
    int width = image.cols;
    int height = image.rows;

    // Initialize pixel count
    int pixelCount = 0;

    // Traverse along the line and count pixels
    for (int x = 0; x < width; ++x) {
        int y = static_cast<int>(-(a / b) * x + y0 / b);
        if (y >= 0 && y < height) {
            if (image.at<uchar>(y, x) != 0) {
                pixelCount++;
            }
        }
    }

    return pixelCount;
}

int main(void)
{   
    // Read config
    // printf("read config\n");
    // auto config = readConfig("config.ini");
    // int xmin = getConfigValue(config, "xmin", 100);
    // int ymin = getConfigValue(config, "ymin", 100);
    // int cols = getConfigValue(config, "cols", 1000);
    // int rows = getConfigValue(config, "rows", 1000);
    // int threshold = getConfigValue(config, "threshold", 100);

    // read config関連の削除
    // 設定ファイルを使わず、値を直接定義
    int xmin = 100;
    int ymin = 100;
    int cols = 1000;
    int rows = 1000;
    int threshold = 100;


    // 動画ファイルのパスの文字列を格納するオブジェクトを宣言する
    std::string filepath = "kiribakoalpha.mp4";
	// 動画ファイルを取り込むためのオブジェクトを宣言する

    //capが未定義だったのでここで宣言
    cv::VideoCapture cap;
	cv::VideoCapture video;
	// 動画ファイルを開く
	video.open(filepath);
	if (video.isOpened() == false) {
		// 動画ファイルが開けなかったときは終了する
		return 0;
	}
	cv::VideoWriter writer; // 動画ファイルを書き出すためのオブジェクトを宣言する
    
	int    width, height, fourcc; // 作成する動画ファイルの設定
	fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v'); // ビデオフォーマットの指定( ISO MPEG-4 / .mp4)
	double fps;
	width  = (int)video.get(cv::CAP_PROP_FRAME_WIDTH);	// フレーム横幅を取得
	height = (int)video.get(cv::CAP_PROP_FRAME_HEIGHT);	// フレーム縦幅を取得
	fps    = video.get(cv::CAP_PROP_FPS);				// フレームレートを取得

//	writer.open("CloneVideo.mp4", fourcc, fps, cv::Size(width, height));
	writer.open("CloneVideo.mp4", fourcc, fps, cv::Size(cols/4, rows));
    cv::Mat img;// 画像を格納するオブジェクトを宣言する
	
	// とりあえず動画の縦横幅をプリント
	video >> img;
	printImage(img);

    std::cout << "Image size: " << img.cols << "x" << img.rows << std::endl;
    std::cout << "ROI before adjustment: " << xmin << "," << ymin << "," << cols << "," << rows << std::endl;

    // ROI の範囲を超えないように修正
    if (xmin < 0) xmin = 0;
    if (ymin < 0) ymin = 0;
    if (xmin + cols > img.cols) cols = img.cols - xmin;
    if (ymin + rows > img.rows) rows = img.rows - ymin;

    // 修正後のROIを表示
    std::cout << "ROI after adjustment: " << xmin << "," << ymin << "," << cols << "," << rows << std::endl;

    // 画像の一部を切り出し
    cv::Mat org_img(img, cv::Rect(xmin, ymin, cols, rows));

    
    printf("xmin %d, ymin %d, cols %d, rows %d, threshold %d\n", xmin, ymin, cols, rows, threshold);
    // timer.cpp関連の削除
    // HighResolutionTimer timer;
    // HighResolutionTimer timestamp;
    // timestamp.start();

    cv::Mat bg_img;
	
    int ifr = -1;
	while (1) {
        ifr++;
        
        // timer.cpp関連の削除
		//timer.start();
        while (cap.read(img)); // バッファをクリア
		video >> img; //videoからimageへ1フレームを取り込む
		if (img.empty() == true) break; // 画像が読み込めなかったとき、無限ループを抜ける
		cv::Mat org_img(img, cv::Rect(xmin, ymin, cols, rows)) ; // 画像の一部を切り出し。
		
		cv::resize(org_img, org_img, cv::Size(), 0.25, 0.25, cv::INTER_AREA); // サイズを小さくしてみる。
		
        cv::Mat src_img, dst_img, gray_img, work_img, src_img_4;
        
        org_img.copyTo(src_img);
        // BG image
        if(bg_img.empty()) {
            // If no BG image, just copy the first frame.
            printf("Initial copy to bg_img\n");
            src_img.copyTo(bg_img);
        }
        cv::addWeighted(bg_img, 0.9, src_img, 0.1, 0.0, bg_img);


        // Subtract background
        src_img = src_img - bg_img;

        // 負の値を0にクリップ
        cv::threshold(src_img, src_img, 0, 255, cv::THRESH_TOZERO);

		cv::cvtColor(src_img, gray_img, cv::COLOR_BGR2GRAY); // Grayスケールにする。
		cv::threshold(gray_img, dst_img, 50, 255, cv::THRESH_BINARY); // ２値化してみてる。

/*
        // カーネルを生成（長方形のカーネルを使用する例）
        int kernelSize = 3; // 3x3のカーネルを使用する例
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize, kernelSize));
        // 収縮処理を適用する
        cv::Mat erodedImage;
        cv::erode(dst_img, dst_img, kernel);
*/		
		// いろいろ画像処理試してみたけどやめた。
		// cv::medianBlur(gray_img, gray_img, 3);
        
        //cv::Canny(gray_img, dst_img, 50, 200, 3);
		//cv::Laplacian(src_img, dst_img, -1, 3);
		
//
		  // （古典的）Hough変換
		std::vector<cv::Vec2f> lines;
		// 入力画像，出力，距離分解能，角度分解能，閾値，*,*
		cv::HoughLines(dst_img, lines, 1, CV_PI/360, 30, 0, 0); // 閾値を変えたりするとぐちゃぐちゃになる
			
		cv::cvtColor(dst_img, dst_img, cv::COLOR_GRAY2BGR); // dst_imgに線を重ねるためにGrayスケール画像からカラー画像に変換しなおしている。
		// 線の描画
		std::vector<cv::Vec2f>::iterator it = lines.begin();
		FILE *fp = fopen("lines.txt","at");
        for(; it!=lines.end(); ++it) {
			float rho = (*it)[0], theta = (*it)[1];
			cv::Point pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a*rho, y0 = b*rho;
			pt1.x = cv::saturate_cast<int>(x0 + 1000*(-b));
			pt1.y = cv::saturate_cast<int>(y0 + 1000*(a));
			pt2.x = cv::saturate_cast<int>(x0 - 1000*(-b));
			pt2.y = cv::saturate_cast<int>(y0 - 1000*(a));
            int cnt = countPixelsOnLine(dst_img, rho, theta);
            if(cnt<2) continue;
			cv::line(dst_img, pt1, pt2, cv::Scalar(0,0,255), 3, 16);
            // timer.cpp関連の削除
            // fprintf(fp, "%6d %7.2lf %6.2lf %8.3lf %5d %5d %5d %5d %5d\n", ifr, timestamp.elapsed(), rho, theta, pt1.x, pt1.y, pt2.x, pt2.y, cnt);
            //fprintf(fp, "%6d %7.2lf %6.2lf %8.3lf %5d %5d %5d %5d %5d\n", ifr, rho, theta, pt1.x, pt1.y, pt2.x, pt2.y, cnt);
            // 型宣言してなくて何かが起こってた？？
            //fprintf(fp, "%6d %7.2f %6.2f %8.3f %5d %5d %5d %5d %5d\n", ifr, (double)rho, (double)theta, (double)pt1.x, pt1.y, pt2.x, pt2.y, cnt);
            //fprintf(fp, "%6d %7.2f %6.2f %8.3f %5d %5d %5d %5d %5d\n", ifr, (double)rho, (double)theta, (double)pt1.x, (int)pt1.y, (int)pt2.x, (int)pt2.y, (int)cnt);
            //fprintf(fp, "%6d %7.2f %6.2f %8.3f %5d %5d %5d %5d %5d\n", ifr, (double)rho, (double)theta, (double)pt1.x, pt1.y, pt2.x, pt2.y, (int)cnt);
            fprintf(fp, "%6d %7.2f %6.2f %8.3d %5d %5d %5d %5d\n",ifr, rho, theta, pt1.x, pt1.y, pt2.x, pt2.y, cnt);
            
        }
        fclose(fp);
		
		// 画像を合体させる。


        // maxにstd::がついてなくてなんか変なことになってた？？
        int commonWidth = std::max(src_img.cols, src_img_4.cols);
//        if (img1.cols != commonWidth) cv::resize(img1, img1, cv::Size(commonWidth, img1.rows * commonWidth / img1.cols));



        cv::putText(org_img, "Original", cv::Point(5, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
		cv::putText(bg_img, "Background", cv::Point(5, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
		cv::putText(src_img, "After BG subtraction", cv::Point(5, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
		cv::putText(dst_img, "Binary and lines", cv::Point(5, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
		
		//cv::Mat src_img_4_2 = addPaddingToWidth(src_img_4, commonWidth);
        //cv::Mat bg_img_2 = addPaddingToWidth(bg_img, commonWidth);
		cv::Mat conv_img,conv_img2;
//		cv::vconcat(src_img, src_img_4_2,conv_img);
        cv::vconcat(org_img, bg_img,conv_img);
		cv::vconcat(conv_img, src_img,conv_img);
		cv::vconcat(conv_img, dst_img, conv_img);
		

		cv::imshow("showing", conv_img); // ウィンドウに動画を表示する
		
		writer << conv_img;  // 画像 image を動画ファイルへ書き出す
		if (cv::waitKey(1) == 'q') break; //qを押すと終了

        // timer.cpp関連の削除
        //printf("time %lf sec\n", timer.elapsed());
        //printf("col raw = %d %d\n", conv_img.cols, conv_img.rows);
	}
	return 0;
}



