#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x400 //WM_MOUSEWHEEL�̂��߂�
#endif

#include <tchar.h>
#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <crtdbg.h>
#undef min
#undef max

#include "Mashiro/Mashiro.h"
#include "Mashiro/Window/Resource.h"
#include "Mashiro/Window/Window.h"
#include "Mashiro/Graphics/GraphicsManager.h"
#include "Mashiro/Input/InputManager.h"

using namespace std;
using namespace Mashiro;

//WindowCreator�����o�֐�
namespace Mashiro{

namespace{ //����

class Impl{
public:
	Impl() :
	mWidth( 800 ), 
	mHeight( 600 ), 
	mMinimized( false ),
	mActive( false ),
	mTitle( "�܂����A�v���P�[�V����" ),
	mCommandLine( "" ),
	mEndRequested( false ),
	mEnded( false ),
	mStarted( false ),
	mDragAndDropEnabled( false ),
	mMouseWheel( 0 ),
	mTimeBias( 0 ){
		//getTime()�p�o�C�A�X�l�v�Z
		unsigned time = timeGetTime();
		mTimeBias = 0xffffffff - time;
		mTimeBias -= 60000; //60�b�����B

		//�X���b�hID�擾
		mThreadId = GetCurrentThreadId();
	}
	~Impl(){
	}
	int mWidth;
	int mHeight;
	bool mMinimized;
	bool mActive;
	string mTitle;
	string mCommandLine;
	bool mEndRequested;
	bool mEnded;
	bool mStarted;
	bool mDragAndDropEnabled;
	int mMouseWheel;
	unsigned mTimeBias; //timeGetTime�ɂ͂�����Q�^
	unsigned mThreadId;
	HWND mWindowHandle;
	RECT mWindowRect;
};

Impl* gImpl = 0; //�B��C���X�^���X

}

Window::Window(){
}

const char* Window::title() const {
	return gImpl->mTitle.c_str();
}

int Window::titleLength() const {
	return static_cast< int >( gImpl->mTitle.size() );
}

int Window::height() const {
	return gImpl->mHeight;
}

int Window::width() const {
	return gImpl->mWidth;
}

bool Window::isEndRequested() const {
	return gImpl->mEndRequested;
}

void Window::requestEnd(){
	gImpl->mEndRequested = true;
}

void Window::end(){
	gImpl->mEnded = true;
}

unsigned Window::time() const {
	return timeGetTime() + gImpl->mTimeBias;
}

bool Window::isMinimized() const {
	return gImpl->mMinimized;
}

bool Window::isActive() const {
	return gImpl->mActive;
}

int Window::getAndResetMouseWheel(){
	int t = gImpl->mMouseWheel;
	gImpl->mMouseWheel = 0;
	return t;
}

bool Window::isMainThread() {
	return ( GetCurrentThreadId() == gImpl->mThreadId );
}

void Window::Configuration::setWidth( int w ){
	gImpl->mWidth = w;
}

void Window::Configuration::setHeight( int h ){
	gImpl->mHeight = h;
}

void Window::Configuration::setTitle( const char* title ){
	gImpl->mTitle = title;
}

const char* Window::commandLineString() const {
	return gImpl->mCommandLine.c_str();
}

}

#define MAX_LOADSTRING 100

// �O���[�o���ϐ�:
HINSTANCE hInst;								// ���݂̃C���^�[�t�F�C�X
TCHAR szTitle[MAX_LOADSTRING];					// �^�C�g�� �o�[�̃e�L�X�g
TCHAR szWindowClass[MAX_LOADSTRING];			// ���C�� �E�B���h�E �N���X��

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: �����ɃR�[�h��}�����Ă��������B
	MSG msg;
	HACCEL hAccelTable;

	HRESULT hr = ::CoInitializeEx( NULL, COINIT_MULTITHREADED );
	//�E�B���h�E��ԕێ��N���X����
	gImpl = NEW Impl();
	gImpl->mCommandLine = lpCmdLine;
	Window wc;
	Window::Configuration config;
	wc.configure( &config );

	memcpy(
		szTitle, 
		wc.title(), 
		wc.titleLength() ); 
	memcpy( szWindowClass, "Game Window Mashiro", sizeof("Game Window Mashiro"));

	MyRegisterClass( hInstance );

	// �A�v���P�[�V�����̏����������s���܂�:
	if ( !InitInstance ( hInstance, nCmdShow ) ){
		return FALSE;
	}

	hAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDC_WINDOWCREATOR ) );

	timeBeginPeriod( 1 );
	HWND windowHandle = gImpl->mWindowHandle;
	///�N���O�̗�O�͌p���s�\�Ȃ̂ŁA���b�Z�[�W�{�b�N�X���o���ďI��点��
	try{
		wc.start( windowHandle );
	}
	catch ( ... ){ //�N�����̗�O�͖����ł��Ȃ��B
		MessageBoxA( 
			windowHandle, 
			"�v���I�Ȗ�肪����A�N���ł��܂���B�\���󂠂�܂��񂪏I���������܂�", 
			"�v���I�G���[", 
			MB_OK | MB_ICONERROR );
		wc.requestEnd(); //�I���B
		wc.update(); //���ŏI������
		SAFE_DELETE( gImpl );
		timeEndPeriod( 1 );
		::CoUninitialize();
		return 0;
	}

	gImpl->mStarted = true; //���[�v�J�n�M��
	while ( true ){
		//���b�Z�[�W������Ȃ珈��
		if ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ){
			if ( GetMessage( &msg, NULL, 0, 0 ) ){
				if ( !TranslateAccelerator( msg.hwnd, hAccelTable, &msg ) ){
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
			}else{
				break; //WM_QUIT���܂����B���[�v�����܂��B
			}
		}
		//�I����Ă��window�j��
		if ( gImpl->mEnded ){ //�I����ĂȂ���΍X�V���[�v
			if ( windowHandle ){ //�����������2�񗈂邩������Ȃ��̂ŔO�̂���
				DestroyWindow( windowHandle );
				windowHandle = 0;
			}
		}else{	//���s�{��
			try{
				wc.update();
			}
			catch ( Mashiro::Exception e ){
				if ( e == Mashiro::EXCEPTION_EXIT ){ //�I���v����O�̏ꍇ�̂ݏI��
					MessageBoxA( 
						windowHandle, 
						"�v���I�Ȗ�肪����A�p���ł��܂���B�\���󂠂�܂��񂪏I���������܂�", 
						"�v���I�G���[", 
						MB_OK | MB_ICONERROR );
				}
			}
		}
	}
	SAFE_DELETE( gImpl );
	timeEndPeriod( 1 );
	::CoUninitialize();

	return (int) msg.wParam;
}



//
//  �֐�: MyRegisterClass()
//
//  �ړI: �E�B���h�E �N���X��o�^���܂��B
//
//  �R�����g:
//
//    ���̊֐�����юg�����́A'RegisterClassEx' �֐����ǉ����ꂽ
//    Windows 95 ���O�� Win32 �V�X�e���ƌ݊�������ꍇ�ɂ̂ݕK�v�ł��B
//    �A�v���P�[�V�������A�֘A�t����ꂽ
//    �������`���̏������A�C�R�����擾�ł���悤�ɂ���ɂ́A
//    ���̊֐����Ăяo���Ă��������B
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWCREATOR));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WINDOWCREATOR);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   �֐�: InitInstance(HINSTANCE, int)
//
//   �ړI: �C���X�^���X �n���h����ۑ����āA���C�� �E�B���h�E���쐬���܂��B
//
//   �R�����g:
//
//        ���̊֐��ŁA�O���[�o���ϐ��ŃC���X�^���X �n���h����ۑ����A
//        ���C�� �v���O���� �E�B���h�E���쐬����ѕ\�����܂��B
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂��B
//----
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = gImpl->mWidth;
	rect.bottom = gImpl->mHeight;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	rect.right -= rect.left;
	rect.bottom -= rect.top; //�����v�Z
	rect.left = 0;
	rect.top = 0;

//----

	HDC screen = GetDC( 0 );
	float dpiX = static_cast< FLOAT >( GetDeviceCaps( screen, LOGPIXELSX ) );
	float dpiY = static_cast< FLOAT >( GetDeviceCaps( screen, LOGPIXELSX ) );

	hWnd = CreateWindow( szWindowClass, szTitle, 
		WS_OVERLAPPEDWINDOW, 0, 0,
		static_cast< UINT >( rect.right * dpiX / 96.0f ),
		static_cast< UINT >( rect.bottom * dpiY / 96.0f ),
		NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   gImpl->mWindowHandle = hWnd;

   return TRUE;
}

//
//  �֐�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  �ړI:  ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
//
//  WM_COMMAND	- �A�v���P�[�V���� ���j���[�̏���
//  WM_PAINT	- ���C�� �E�B���h�E�̕`��
//  WM_DESTROY	- ���~���b�Z�[�W��\�����Ė߂�
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		break;
	case WM_CLOSE: //�I���v��
		gImpl->mEndRequested = true;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_ACTIVATE:
		if ( wParam & 0xffff0000 ){ //�ŏ�������Ă����false
			gImpl->mActive = false;
		}else{
			gImpl->mActive = ( ( wParam & 0xffff ) != 0 );
		}
		break;
	case WM_MOUSEWHEEL:
		gImpl->mMouseWheel = ( static_cast< short >( HIWORD( wParam ) ) > 0 ) ? 1 : -1;
		break;
	case WM_SYSCOMMAND:
		if( wParam == SC_MAXIMIZE ){
			//�Ȃɂ����Ȃ�
		} else if ( wParam == SC_MINIMIZE ){
			gImpl->mMinimized = true;
			gImpl->mActive = false;
			DefWindowProc(hWnd, message, wParam, lParam);
		}else if ( wParam == SC_RESTORE ){
			gImpl->mMinimized = false;
			gImpl->mActive = true;
			DefWindowProc(hWnd, message, wParam, lParam);
		}else{
			DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}