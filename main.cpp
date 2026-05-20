#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// 전역 변수 선언
Mat src, dst;
Point ptStart;          // 마우스 드래그 시작 좌표
bool isDrawing = false; // 마우스 드래그 상태 플래그

int sigma_val = 1;      // [조건] 표준편차 초기값은 반드시 1로 설정
const int max_val = 9;  // [조건] 트랙바 최대 범위 9

// 트랙바 콜백 함수 (값이 바뀔 때마다 호출되지만 여기서는 전역 변수 동기화용으로만 사용)
void on_trackbar(int pos, void*) {
    // 트랙바 슬라이더를 움직이면 sigma_val 변수 값이 자동으로 업데이트됩니다.
}

// 마우스 이벤트 처리 콜백 함수
void on_mouse(int event, int x, int y, int flags, void* userdata) {
    switch (event) {
    case EVENT_LBUTTONDOWN:
        ptStart = Point(x, y);
        isDrawing = true;
        break;

    case EVENT_MOUSEMOVE:
        if (isDrawing) {
            // 드래그 중일 때 실시간 사각형 가이드선 표시
            Mat temp = dst.clone();
            rectangle(temp, Rect(ptStart, Point(x, y)), Scalar(255), 1);
            imshow("src", temp);
        }
        break;

    case EVENT_LBUTTONUP:
        if (isDrawing) {
            isDrawing = false;

            // 드래그 영역 사각형 정의 및 예외 안전 처리
            Rect roi_rect(ptStart, Point(x, y));
            roi_rect &= Rect(0, 0, src.cols, src.rows);

            if (roi_rect.width > 0 && roi_rect.height > 0) {
                // 부분 참조 영역(ROI) 추출
                Mat roi = dst(roi_rect);

                // [핵심] 트랙바로 조절된 sigma_val 값을 표준편차로 사용하여 가우시안 블러 적용
                // 만약 트랙바 값이 0이면 블러링을 수행하지 않습니다.
                if (sigma_val > 0) {
                    GaussianBlur(roi, roi, Size(), (double)sigma_val);
                }

                // 최종 갱신된 화면 출력
                imshow("src", dst);
            }
        }
        break;
    }
}

int main(void)
{
    // 1. lenna 영상을 그레이스케일로 로드
    src = imread("lenna.bmp", IMREAD_GRAYSCALE);
    if (src.empty()) {
        cerr << "Image load failed!" << endl;
        return -1;
    }

    // 결과 누적용 도화지 행렬 복사
    dst = src.clone();

    // 윈도우 창 생성
    namedWindow("src");

    // 2. [조건] 트랙바 생성 (윈도우창 이름, 트랙바이름, 등록할 변수 주소, 최대값, 콜백함수)
    createTrackbar("sigma", "src", &sigma_val, max_val, on_trackbar);

    // 마우스 콜백 함수 등록
    setMouseCallback("src", on_mouse);

    // 초기 화면 출력
    imshow("src", dst);

    waitKey(0);
    destroyAllWindows();

    return 0;
}