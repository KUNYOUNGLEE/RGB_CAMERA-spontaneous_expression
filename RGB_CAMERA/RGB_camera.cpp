#include <windows.h>
#include <process.h>
#include <cv.h>
#include <highgui.h>
#include <time.h>

#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")// 콘솔창 띄우는 코드

#define WindowWidth  1920      // 윈도우 가로 크기
#define WindowHeight 1020      // 윈도우 세로 크기
#define CloseTimer 1003		//자동종료 타이머

// 윈도우의 위치 원점은 윈도우 좌상단의 끝을 원점으로 한다
// 윈도우 원점 x위치(현재 화면의 가로크기(SM_CXSCREEN)에서 윈도우의 가로크기를 뺀값의 절반값
#define WindowPosX ( GetSystemMetrics( SM_CXSCREEN ) - WindowWidth )  / 2
// 윈도우 원점 y위치(현재 화면의 가로크기(SM_CYSCREEN)에서 윈도우의 세로크기를 뺀값의 절반값
#define WindowPosY ( GetSystemMetrics( SM_CYSCREEN ) - WindowHeight ) / 2

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
DWORD  WINAPI ThreadFunction(LPVOID lpParam);
void MakeDirectory(char *full_path);
HANDLE hThread = NULL;
DWORD dwThreadParam = NULL;
DWORD dwThreadID = NULL;
HWND hWnd;

HINSTANCE g_hInst;
LPSTR lpszClass="RGB_camera 동영상";

TCHAR fileName[1024] = { 0 };
TCHAR S_Name[256] = { 0 };

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance
          ,LPSTR lpszCmdParam,int nCmdShow)
{
	//동영상 띄우는 윈도우 핸들및 클래스 선언
    MSG Message;
    WNDCLASS WndClass;
    g_hInst=hInstance;
    
    if(!hPrevInstance) {
        WndClass.cbClsExtra=0;
        WndClass.cbWndExtra=0;
        WndClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
        WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
        WndClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
        WndClass.hInstance=hInstance;
        WndClass.lpfnWndProc=(WNDPROC)WndProc;
        WndClass.lpszClassName=lpszClass;
        WndClass.lpszMenuName=NULL;
        WndClass.style=CS_HREDRAW | CS_VREDRAW;
        RegisterClass(&WndClass);
    }
	int i;
	printf("실험자의 이름을 입력하세요: ");
	scanf("%s", S_Name);

    hWnd=CreateWindowA(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		WindowPosX, WindowPosY, WindowWidth, WindowHeight,
          NULL,(HMENU)NULL,hInstance,NULL);

    ShowWindow(hWnd,nCmdShow);
    ///////////////////////////위의 코드까지가 동영상 관련 코드 선언부//////////////////////////
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	DWORD nExitCode = NULL;
	GetExitCodeThread(hThread, &nExitCode);
	TerminateThread(hThread, nExitCode);
	CloseHandle(hThread);

	return 0;
    //return Message.wParam;
}
 
// vfw.h를 인클루드하고 vfw32.lib를 추가해준다.
#include <vfw.h>
 
HWND hWndAVI=0;

// 아래 ""사이에 PLAY하려고 하는 wmv파일이름을 넣는다.
TCHAR *szAVIFilename;// = "D:\\감정유발영상(순방향).wmv";
 
//영상 재생하는 메인 메시지루프
LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
    HDC          hdc;
    PAINTSTRUCT  ps;
    RECT         rt;
	TCHAR msg[256] = "촬영을 시작할려면 마우스 왼쪽 버튼(순방향 영상) 또는 오른쪽 버튼(역방향 영상)을 클릭하세요";
	int one_thread = 0;
	switch(iMessage)
    {
    case WM_LBUTTONDOWN:
		//동영상및 촬영을 시작하는 메세지 코드.
		if (one_thread == 0) {
			szAVIFilename = "D:\\감정유발영상(순방향).wmv";
			hThread = CreateThread(NULL, 0, ThreadFunction, &dwThreadParam, 0, &dwThreadID);
			one_thread++;
		}
		if (hThread == 0) return 0;
        
		//자동종료 타이머 구현
		SetTimer(hWnd, CloseTimer, 887000, NULL);//887초 14분 47초 지나면 종료
		//SetTimer(hWnd, CloseTimer, 1000, NULL);//테스트용 1초후 자동종료

		if (hWndAVI)
        {
            MCIWndClose(hWndAVI);
            MCIWndDestroy(hWndAVI);
            hWndAVI=0;
        }

		hWndAVI = MCIWndCreate(hWnd, g_hInst,
			MCIWNDF_NOTIFYANSI | MCIWNDF_NOMENU |
			MCIWNDF_NOTIFYALL | MCIWNDF_NOPLAYBAR,
			szAVIFilename);

		if (hWndAVI)
		{
			GetClientRect(hWnd, &rt);
			SetWindowPos(hWndAVI, NULL, 0, 0, rt.right, rt.bottom, SWP_NOZORDER | SWP_NOMOVE);
			MCIWndPlay(hWndAVI);
		}
        return 0;

	case WM_RBUTTONDOWN:
		//동영상및 촬영을 시작하는 메세지 코드.
		if (one_thread == 0) {
			szAVIFilename = "D:\\감정유발영상(역방향).wmv";
			hThread = CreateThread(NULL, 0, ThreadFunction, &dwThreadParam, 0, &dwThreadID);
			one_thread++;
		}
		if (hThread == 0) return 0;

		//자동종료 타이머 구현
		SetTimer(hWnd, CloseTimer, 887000, NULL);//887초 14분 47초 지나면 종료

		if (hWndAVI)
		{
			MCIWndClose(hWndAVI);
			MCIWndDestroy(hWndAVI);
			hWndAVI = 0;
		}

		hWndAVI = MCIWndCreate(hWnd, g_hInst,
			MCIWNDF_NOTIFYANSI | MCIWNDF_NOMENU |
			MCIWNDF_NOTIFYALL | MCIWNDF_NOPLAYBAR,
			szAVIFilename);

		if (hWndAVI)
		{
			GetClientRect(hWnd, &rt);
			SetWindowPos(hWndAVI, NULL, 0, 0, rt.right, rt.bottom, SWP_NOZORDER | SWP_NOMOVE);
			MCIWndPlay(hWndAVI);
		}
		return 0;
	
	case WM_TIMER:
		KillTimer(hWnd, CloseTimer);
		SendMessage(hWnd, WM_CLOSE, NULL, NULL);

		return 0;
 
    case WM_SIZE:
        if (wParam==SIZE_MAXIMIZED || wParam==SIZE_MAXSHOW || wParam==SIZE_RESTORED)
        {
            SetWindowPos(hWndAVI, NULL, 0,0,LOWORD(lParam),HIWORD(lParam),SWP_NOZORDER|SWP_NOMOVE);
        }
        return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, WindowWidth/2-300, WindowHeight/2-50, msg, strlen(msg));
		EndPaint(hWnd, &ps);
		return 0;
 
    case WM_DESTROY:
		printf("\n\n***********************************촬영이 종료되었습니다. 현재창을 닫아주세요***************************************\n");
        PostQuitMessage(0);
        return 0;
    }
    return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

//카메라 영상 촬영 스레드 함수 cvCreateVideoWriter로 저장경로받아서 영상 저장함
DWORD  WINAPI ThreadFunction(LPVOID lpParam)
{
	IplImage *img;
	CvCapture *capture;
	CvVideoWriter *writer;
	
	int key;

	cvNamedWindow("RGB_camera", CV_WINDOW_AUTOSIZE); //촬영영상
	capture = cvCaptureFromCAM(0);
	img = cvQueryFrame(capture);
	//영상 저장할 파일경로 설정
	sprintf(fileName, "C:\\Record\\%s_affective\\RGB_Avi\\", S_Name);
	MakeDirectory(fileName);

	struct tm *t;
	time_t timer;
	TCHAR      buf[80];
	timer = time(NULL);    // 현재 시각을 초 단위로 얻기
	t = localtime(&timer); // 초 단위의 시간을 분리하여 구조체에 넣기
	sprintf(buf, "%02d_%02d_%02d_%02d_%02d%02d_",
		t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
		t->tm_hour, t->tm_min, t->tm_sec
	);

	//파일경로+파일명
	sprintf(buf, "%s%s", buf, S_Name);
	sprintf(fileName, "%s%s.avi", fileName, buf);

	int fps = 30;
	int width = img->width;
	int height = img->height;
	writer = cvCreateVideoWriter(fileName, CV_FOURCC('D', 'I', 'V', 'X'), fps, cvSize(width, height), 1);

	while (1)
	{
		img = cvQueryFrame(capture);

		cvShowImage("RGB_camera", img);//촬영영상 보여줌
		cvWriteFrame(writer, img);

		key = cvWaitKey(30);
		if (key == 27) break;//ESC key
	}

	return 0;
}

//영상을 저장할 경로명 문자열을 받아서 상위 디렉토리부터 하위 디렉토리순으로 하나씩 디렉토리를 생성하는 함수
void MakeDirectory(char *full_path)
{
	char temp[256], *sp;
	strcpy(temp, full_path); // 경로문자열을 복사
	sp = temp; // 포인터를 문자열 처음으로

	while ((sp = strchr(sp, '\\'))) { // 디렉토리 구분자를 찾았으면
		if (sp > temp && *(sp - 1) != ':') { // 루트디렉토리가 아니면
			*sp = '\0'; // 잠시 문자열 끝으로 설정
			CreateDirectory(temp, NULL);
			// 디렉토리를 만들고 (존재하지 않을 때)
			*sp = '\\'; // 문자열을 원래대로 복귀
		}
		sp++; // 포인터를 다음 문자로 이동
	}
}