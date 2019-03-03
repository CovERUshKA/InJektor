// InJekтоpNew.cpp : Определяет точку входа для приложения.
//

#include "stdafx.h"
#include "InJekтоpNew.h"

#define MAX_LOADSTRING 100

using namespace std;

// Глобальные переменные:
INPUT in;
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
#define IDC_INJECTINER_BUTTON 275
#define IDC_SELECT_PROCESS_BUTTON 276
#define IDC_SELECT_PROCESS_TEXT 277
#define IDC_SELECT_DLL_TEXT 279
#define IDC_SELECT_DLL_BUTTON 280
#define IDC_AUTOCLICKER_BUTTON 281
HWND hWndProcessName, hWndSuccessfulInject, hWndProcessList, hWndButton, hWndDLLName, hWndSelectDLLButton, hWndAutoclickerButton;
WNDCLASSEXW wc = { 0 };
DWORD aProcesses[1024], cbNeeded, cProcesses, pID;
HANDLE hProc, handle;
TCHAR szProcessName[MAX_PATH], szProcessPrint[MAX_PATH], buff[MAX_PATH], szModName[MAX_PATH];
char szFile[100], buf[50] = { 0 }, SzProcessNamech[MAX_PATH];
OPENFILENAME ofn;
HMODULE hLib, hMod;
LPVOID RemoteString, LoadLibAddy;
FILE* fp;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void				RegisterDialogClass(HWND);
void				select_dll(HWND hWnd);
void				select_process(HWND hWnd);
bool				Inject(const DWORD * pId, const char * dllName);
void				select_process(HWND hWnd);
DWORD				GetProcID(char * name);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Разместите код здесь.
	hInst = hInstance;

	// Инициализация глобальных строк
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_INJEKPNEW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_INJEKPNEW));

	MSG msg;

	// Цикл основного сообщения:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_INJEKPNEW));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 0);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDI_INJEKPNEW);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = NULL;

	return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX) &~(WS_THICKFRAME),
		CW_USEDEFAULT, 0, 500, 170, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	/*if (IsDlgButtonChecked(hWnd, IDC_AUTOCLICKER_BUTTON))
	{
		if (FindWindowA(NULL, "ROBLOX"))
		{
			SendMessageW(FindWindowA(NULL, "ROBLOX"), WM_LBUTTONDOWN, 0, MAKELPARAM(300, 300));
			SendMessageW(FindWindowA(NULL, "ROBLOX"), WM_LBUTTONUP, 0, MAKELPARAM(300, 300));
			PostMessageA(FindWindowA(NULL, "ROBLOX"), WM_LBUTTONDOWN, 0, MAKELPARAM(300, 300));
			PostMessageA(FindWindowA(NULL, "ROBLOX"), WM_LBUTTONUP, 0, MAKELPARAM(300, 300));
		} else {
			MessageBoxA(hWnd, "Window of ROBLOX not found", "Fail", MB_OK);
			CheckDlgButton(hWnd, IDC_AUTOCLICKER_BUTTON, BST_UNCHECKED);
		}
	}*/
	switch (message)
	{
	case WM_CREATE:
		RegisterDialogClass(hWnd);
		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDC_AUTOCLICKER_BUTTON:
		{
			BOOL checked = IsDlgButtonChecked(hWnd, IDC_AUTOCLICKER_BUTTON);
			if (checked) {
				CheckDlgButton(hWnd, IDC_AUTOCLICKER_BUTTON, BST_UNCHECKED);
			}
			else {
				CheckDlgButton(hWnd, IDC_AUTOCLICKER_BUTTON, BST_CHECKED);
			}

			/*HICON icon = (HICON)GetClassLongW(hWnd, GCL_HICON);
			HWND hCons = FindWindowA(NULL, "ROBLOX"); // I already opened windows calculator. you can use any other window but be sure to get its valid Handle
			if (!hCons)
			{
				MessageBoxA(0, "Window is not found!", 0, MB_OK | MB_ICONHAND);
			}
			else
			{
				SendMessageA(hCons, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)icon); // setting the icon
			}*/
		}
			break;
		case IDC_SELECT_DLL_BUTTON:
			select_dll(hWnd);
			break;
		case IDC_SELECT_PROCESS_BUTTON:
			select_process(hWnd);
			break;
		case IDC_INJECTINER_BUTTON:
		{
			char processid[MAX_PATH];
			char dllpath[MAX_PATH];
			GetWindowTextA(hWndProcessName, processid, MAX_PATH);
			DWORD pID = GetProcID(processid);
			if (!pID)
			{
				MessageBoxA(hWnd, "Process is not finded", "Error", MB_OK | MB_ICONERROR);
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			GetWindowTextA(hWndDLLName, dllpath, MAX_PATH);

			if (!Inject(&pID, (const char *)dllpath))
			{
				MessageBoxW(hWnd, L"Failed to inject", L"Status", MB_OK | MB_ICONERROR);
			}
			else
			{
				MessageBoxW(hWnd, L"Successful inject", L"Status", MB_OK | MB_ICONASTERISK);
			}
		}
		break;
		case IDM_ABOUT:
			DialogBoxW(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProcA(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Enter code here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "Информация".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void RegisterDialogClass(HWND hWnd)
{
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.hInstance = hInst;
	wc.hbrBackground = (HBRUSH)1;
	wc.lpszClassName = L"DialogClass";
	RegisterClassExW(&wc);

	CreateWindowA("STATIC", "Process to inject",
		WS_VISIBLE | WS_CHILD | ES_CENTER,
		500 / 2 - 80 - 5, 0, 160, 17, hWnd, (HMENU)1, NULL, NULL);

	hWndProcessName = CreateWindowA("EDIT", "",
		WS_VISIBLE | WS_BORDER | WS_CHILD | ES_CENTER | ES_AUTOHSCROLL,
		10, 25, 360, 17, hWnd, (HMENU)IDC_SELECT_PROCESS_TEXT, NULL, NULL);

	hWndProcessList = CreateWindowA("BUTTON", "Processes",
		WS_VISIBLE | WS_CHILD,
		500 - 95 - 30, 25, 95, 20, hWnd, (HMENU)IDC_SELECT_PROCESS_BUTTON, NULL, NULL);

	CreateWindowA("STATIC", "Select .dll to inject",
		WS_VISIBLE | WS_CHILD | ES_CENTER,
		500 / 2 - 80 - 5, 51, 160, 17, hWnd, (HMENU)1, NULL, NULL);

	hWndDLLName = CreateWindowA("EDIT", "",
		WS_VISIBLE | WS_BORDER | WS_CHILD | ES_CENTER | ES_AUTOHSCROLL,
		10, 75, 360, 17, hWnd, (HMENU)IDC_SELECT_DLL_TEXT, NULL, NULL);

	hWndSelectDLLButton = CreateWindowA("BUTTON", "Choose",
		WS_VISIBLE | WS_CHILD,
		500 - 95 - 30, 75, 95, 20, hWnd, (HMENU)IDC_SELECT_DLL_BUTTON, NULL, NULL);

	CreateWindowA("BUTTON", "Inject",
		WS_VISIBLE | WS_CHILD,
		5, 100, 475, 25, hWnd, (HMENU)IDC_INJECTINER_BUTTON, NULL, NULL);
}

void PrintProcessNameAndID(DWORD processID)
{
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

	// Get a handle to the process.

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);

	// Get the process name.

	if (NULL != hProcess)
	{
		HMODULE hMod;
		DWORD cbNeeded;

		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
			&cbNeeded))
		{
			GetModuleBaseName(hProcess, hMod, szProcessName,
				sizeof(szProcessName) / sizeof(TCHAR));
			SetConsoleTextAttribute(handle, FOREGROUND_GREEN | FOREGROUND_BLUE);
			_tprintf(TEXT("%s\n"), szProcessName);
			SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
	}

	// Release the handle to the process.

	CloseHandle(hProcess);
}

void select_process(HWND hWnd)
{
	AllocConsole();
	ZeroMemory(&fp, sizeof(FILE));
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	freopen_s(&fp, "CONOUT$", "w", stdout);

	cout.clear();
	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	setlocale(LC_ALL, "Russian");
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		return;
	}


	// Calculate how many process identifiers were returned.

	cProcesses = cbNeeded / sizeof(DWORD);

	// Print the name and process identifier for each process.

	for ( unsigned int i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
		{
			PrintProcessNameAndID(aProcesses[i]);
		}
	}
	FreeConsole();
}

void select_dll(HWND hWnd)
{

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = (LPWSTR)szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"DLL\0*.dll\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	GetOpenFileName(&ofn);

	if (ofn.lpstrFile)
	{
		SetWindowTextW(hWndDLLName, ofn.lpstrFile);
	}
	else
	{
		MessageBoxA(hWnd, "DLL isn't choosen", "Error", MB_OK);
	}
}

bool Inject(const DWORD *pId, const char * dllName)
{
	HANDLE h = OpenProcess(
		PROCESS_ALL_ACCESS,
		FALSE,
		*pId
	);
	if (h)
	{
		LPVOID LoadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
		LPVOID dereercomp = VirtualAllocEx(h, NULL, strlen(dllName), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		WriteProcessMemory(h, dereercomp, dllName, strlen(dllName), NULL);
		HANDLE asdc = CreateRemoteThread(h, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddr, dereercomp, 0, NULL);
		WaitForSingleObject(asdc, INFINITE);
		VirtualFreeEx(h, dereercomp, strlen(dllName), MEM_RELEASE);
		CloseHandle(asdc);
		CloseHandle(h);
		return true;
	}
	return false;
}

DWORD GetProcID( char * name )
{
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		return 0;
	}

	// Calculate how many process identifiers were returned.

	cProcesses = cbNeeded / sizeof(DWORD);

	// Print the name and process identifier for each process.

	for (unsigned int i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
		{
			*SzProcessNamech = NULL;

			// Get a handle to the process.

			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
				PROCESS_VM_READ,
				FALSE, aProcesses[i]);

			// Get the process name.

			if (NULL != hProcess)
			{
				hMod = NULL;
				cbNeeded = NULL;

				if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
					&cbNeeded))
				{
					if (GetModuleBaseNameA(hProcess, hMod, SzProcessNamech,
						sizeof(szProcessName) / sizeof(TCHAR)))
					{
						if ((string)SzProcessNamech == (string)name)
						{
							return aProcesses[i];
						}
					}
				}
			}
		}
	}
	return 0;
}