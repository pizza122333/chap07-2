#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(void)
{
    // 1. 영상 로드
    Mat src = imread("rose.bmp", IMREAD_GRAYSCALE);
    if (src.empty()) {
        cerr << "Image load failed!" << endl;
        return -1;
    }

    // 테스트할 마스크 사이즈 배열
    int sizes[] = { 3, 10, 30 };
    Mat dst;

    // OpenCV 시간 측정 클래스 선언
    TickMeter tm;

    for (int ksize : sizes) {
        // 이전 루프의 측정 기록 초기화
        tm.reset();

        // 시간 측정 시작
        tm.start();

        // 평균값 필터(blur) 수행
        blur(src, dst, Size(ksize, ksize));

        // 시간 측정 종료
        tm.stop();

        // 결과 출력 (밀리초 단위)
        cout << "Mask Size [" << ksize << "x" << ksize << "] Execution Time: "
            << tm.getTimeMilli() << " ms" << endl;

        // 화면 시각화용 글자 추가
        String desc = format("Mean: %dx%d (%.2f ms)", ksize, ksize, tm.getTimeMilli());
        putText(dst, desc, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255), 1, LINE_AA);

        imshow("dst", dst);
        waitKey(0);
    }

    return 0;
}