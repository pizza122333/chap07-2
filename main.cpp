#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// 전역 변수 선언 (마우스 콜백 함수에서 접근하기 위함)
Mat src, dst;
Point ptStart;   // 마우스를 처음 누른 시작 좌표
bool isDrawing = false; // 마우스가 드래그 중인지 체크하는 플래그

// 마우스 이벤트 처리 콜백 함수
void on_mouse(int event, int x, int y, int flags, void* userdata) {
    switch (event) {
    case EVENT_LBUTTONDOWN: // 1. 마우스 왼쪽 버튼을 눌렀을 때
        ptStart = Point(x, y);
        isDrawing = true;
        break;

    case EVENT_MOUSEMOVE: // 2. 마우스 왼쪽 버튼을 누른 채 움직일 때 (드래그 효과 시각화)
        if (isDrawing) {
            // 원본 이미지를 복사하여 매번 새로운 드래그 가이드 사각형을 그림
            Mat temp = dst.clone();
            rectangle(temp, Rect(ptStart, Point(x, y)), Scalar(255), 1);
            imshow("src", temp);
        }
        break;

    case EVENT_LBUTTONUP: // 3. 마우스 왼쪽 버튼에서 손을 뗐을 때 (필터 적용 핵심)
        if (isDrawing) {
            isDrawing = false;

            // 시작점과 끝점을 이용해 사각형(Rect) 영역 정의
            Rect roi_rect(ptStart, Point(x, y));

            // [예외 처리] 마우스를 반대로 드래그하거나 비정상적인 크기일 때 검사
            // 영상 범위를 벗어나지 않도록 안전하게 잘라줍니다.
            roi_rect &= Rect(0, 0, src.cols, src.rows);

            // 사각형 영역의 크기가 유효할 때만 연산 수행
            if (roi_rect.width > 0 && roi_rect.height > 0) {
                // dst 이미지에서 마우스로 드래그한 부분 행렬(ROI)만 참조 추출
                Mat roi = dst(roi_rect);

                // 추출한 ROI 영역에만 가우시안 블러링 수행 (조건: 표준편차 5)
                // 자기 자신(roi)에 연산 결과를 다시 덮어씌우면 dst 원본 영상에 그대로 반영됩니다.
                GaussianBlur(roi, roi, Size(), 5.0);

                // 업데이트된 결과 최종 출력
                imshow("src", dst);
            }
        }
        break;
    }
}

int main(void)
{
    // [조건] lenna 영상을 그레이스케일(흑백)로 변환하여 로드
    src = imread("lenna.bmp", IMREAD_GRAYSCALE);
    if (src.empty()) {
        cerr << "Image load failed!" << endl;
        return -1;
    }

    // 마우스 드래그 결과를 계속 누적해서 업데이트할 도화지 행렬 복사
    dst = src.clone();

    // 윈도우 창 생성 및 마우스 콜백 함수 등록
    namedWindow("src");
    setMouseCallback("src", on_mouse);

    // 초기 이미지 화면에 표시
    imshow("src", dst);

    // 키 입력 전까지 대기 (Esc나 아무 키나 누르면 종료)
    waitKey(0);
    destroyAllWindows();

    return 0;
}