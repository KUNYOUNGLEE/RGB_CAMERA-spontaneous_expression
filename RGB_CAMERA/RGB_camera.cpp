#include <windows.h>
#include <process.h>
#include <cv.h>
#include <highgui.h>
#include <time.h>

#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")// �ܼ�â ���� �ڵ�

#define WindowWidth  1920      // ������ ���� ũ��
#define WindowHeight 1020      // ������ ���� ũ��
#define CloseTimer 1003		//�ڵ����� Ÿ�̸�

// �������� ��ġ ������ ������ �»���� ���� �������� �Ѵ�
// ������ ���� x��ġ(���� ȭ���� ����ũ��(SM_CXSCREEN)���� �������� ����ũ�⸦ ������ ���ݰ�
#define WindowPosX ( GetSystemMetrics( SM_CXSCREEN ) - WindowWidth )  / 2
// ������ ���� y��ġ(���� ȭ���� ����ũ��(SM_CYSCREEN)���� �������� ����ũ�⸦ ������ ���ݰ�
#define WindowPosY ( GetSystemMetrics( SM_CYSCREEN ) - WindowHeight ) / 2

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
DWORD  WINAPI ThreadFunction(LPVOID lpParam);
void MakeDirectory(char *full_path);
HANDLE hThread = NULL;
DWORD dwThreadParam = NULL;
DWORD dwThreadID = NULL;
HWND hWnd;

HINSTANCE g_hInst;
LPSTR lpszClass="RGB_camera ������";

TCHAR fileName[1024] = { 0 };
TCHAR S_Name[256] = { 0 };

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance
          ,LPSTR lpszCmdParam,int nCmdShow)
{
	//������ ���� ������ �ڵ�� Ŭ���� ����
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
	printf("�������� �̸��� �Է��ϼ���: ");
	scanf("%s", S_Name);

    hWnd=CreateWindowA(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		WindowPosX, WindowPosY, WindowWidth, WindowHeight,
          NULL,(HMENU)NULL,hInstance,NULL);

    ShowWindow(hWnd,nCmdShow);
    ///////////////////////////���� �ڵ������ ������ ���� �ڵ� �����//////////////////////////
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
 
// vfw.h�� ��Ŭ����ϰ� vfw32.lib�� �߰����ش�.
#include <vfw.h>
 
HWND hWndAVI=0;

// �Ʒ� ""���̿� PLAY�Ϸ��� �ϴ� wmv�����̸��� �ִ´�.
TCHAR *szAVIFilename;// = "D:\\�������߿���(������).wmv";
 
//���� ����ϴ� ���� �޽�������
LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
    HDC          hdc;
    PAINTSTRUCT  ps;
    RECT         rt;
	TCHAR msg[256] = "�Կ��� �����ҷ��� ���콺 ���� ��ư(������ ����) �Ǵ� ������ ��ư(������ ����)�� Ŭ���ϼ���";
	int one_thread = 0;
	switch(iMessage)
    {
    case WM_LBUTTONDOWN:
		//������� �Կ��� �����ϴ� �޼��� �ڵ�.
		if (one_thread == 0) {
			szAVIFilename = "D:\\�������߿���(������).wmv";
			hThread = CreateThread(NULL, 0, ThreadFunction, &dwThreadParam, 0, &dwThreadID);
			one_thread++;
		}
		if (hThread == 0) return 0;
        
		//�ڵ����� Ÿ�̸� ����
		SetTimer(hWnd, CloseTimer, 887000, NULL);//887�� 14�� 47�� ������ ����
		//SetTimer(hWnd, CloseTimer, 1000, NULL);//�׽�Ʈ�� 1���� �ڵ�����

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
		//������� �Կ��� �����ϴ� �޼��� �ڵ�.
		if (one_thread == 0) {
			szAVIFilename = "D:\\�������߿���(������).wmv";
			hThread = CreateThread(NULL, 0, ThreadFunction, &dwThreadParam, 0, &dwThreadID);
			one_thread++;
		}
		if (hThread == 0) return 0;

		//�ڵ����� Ÿ�̸� ����
		SetTimer(hWnd, CloseTimer, 887000, NULL);//887�� 14�� 47�� ������ ����

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
		printf("\n\n***********************************�Կ��� ����Ǿ����ϴ�. ����â�� �ݾ��ּ���***************************************\n");
        PostQuitMessage(0);
        return 0;
    }
    return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

//ī�޶� ���� �Կ� ������ �Լ� cvCreateVideoWriter�� �����ι޾Ƽ� ���� ������
DWORD  WINAPI ThreadFunction(LPVOID lpParam)
{
	IplImage *img;
	CvCapture *capture;
	CvVideoWriter *writer;
	
	int key;

	cvNamedWindow("RGB_camera", CV_WINDOW_AUTOSIZE); //�Կ�����
	capture = cvCaptureFromCAM(0);
	img = cvQueryFrame(capture);
	//���� ������ ���ϰ�� ����
	sprintf(fileName, "C:\\Record\\%s_affective\\RGB_Avi\\", S_Name);
	MakeDirectory(fileName);

	struct tm *t;
	time_t timer;
	TCHAR      buf[80];
	timer = time(NULL);    // ���� �ð��� �� ������ ���
	t = localtime(&timer); // �� ������ �ð��� �и��Ͽ� ����ü�� �ֱ�
	sprintf(buf, "%02d_%02d_%02d_%02d_%02d%02d_",
		t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
		t->tm_hour, t->tm_min, t->tm_sec
	);

	//���ϰ��+���ϸ�
	sprintf(buf, "%s%s", buf, S_Name);
	sprintf(fileName, "%s%s.avi", fileName, buf);

	int fps = 30;
	int width = img->width;
	int height = img->height;
	writer = cvCreateVideoWriter(fileName, CV_FOURCC('D', 'I', 'V', 'X'), fps, cvSize(width, height), 1);

	while (1)
	{
		img = cvQueryFrame(capture);

		cvShowImage("RGB_camera", img);//�Կ����� ������
		cvWriteFrame(writer, img);

		key = cvWaitKey(30);
		if (key == 27) break;//ESC key
	}

	return 0;
}

//������ ������ ��θ� ���ڿ��� �޾Ƽ� ���� ���丮���� ���� ���丮������ �ϳ��� ���丮�� �����ϴ� �Լ�
void MakeDirectory(char *full_path)
{
	char temp[256], *sp;
	strcpy(temp, full_path); // ��ι��ڿ��� ����
	sp = temp; // �����͸� ���ڿ� ó������

	while ((sp = strchr(sp, '\\'))) { // ���丮 �����ڸ� ã������
		if (sp > temp && *(sp - 1) != ':') { // ��Ʈ���丮�� �ƴϸ�
			*sp = '\0'; // ��� ���ڿ� ������ ����
			CreateDirectory(temp, NULL);
			// ���丮�� ����� (�������� ���� ��)
			*sp = '\\'; // ���ڿ��� ������� ����
		}
		sp++; // �����͸� ���� ���ڷ� �̵�
	}
}