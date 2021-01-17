// InJekтоpNew.cpp : Определяет точку входа для приложения.
//

#include "stdafx.h"
#include "Injector.h"

#define IDC_INJECT_BUTTON 275
#define IDC_SELECT_PROCESS_BUTTON 276
#define IDC_SELECT_PROCESS_TEXT 277
#define IDC_SELECT_DLL_TEXT 279
#define IDC_SELECT_DLL_BUTTON 280
#define IDC_AUTOCLICKER_BUTTON 281

#define MAX_LOADSTRING 100
#define ErrorMessageBox(msg) MessageBoxA(NULL, msg, "Injector - Oops, Problem", MB_OK | MB_TOPMOST | MB_ICONERROR);
#define CustomMessageBox(title, msg) MessageBoxA(NULL, msg, "Injector - " title, MB_OK | MB_TOPMOST);

using namespace std;

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HWND hWndProcessName, hWndProcessList, hWndDLLName, hWndSelectDLLButton;
WNDCLASSEXW wc = { 0 };
char szFile[100];
OPENFILENAME ofn;
FILE* fp;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void				RegisterDialogClass(HWND);
void				SelectDll(HWND hWnd);
void				SelectProcess(HWND hWnd);
BOOL				Inject(DWORD dwProcessID, const char * dllName);
DWORD               GetProcessIDByName(const wchar_t* name);
BOOL				CheckForDLL(DWORD *processid, char *pathofDLL);

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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCEW(IDI_INJEKPNEW));
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
		}
			break;
		case IDC_SELECT_DLL_BUTTON:
			SelectDll(hWnd);
			break;
		case IDC_SELECT_PROCESS_BUTTON:
			SelectProcess(hWnd);
			break;
		case IDC_INJECT_BUTTON:
		{
			wchar_t wchProcessName[MAX_PATH];
			ZeroMemory(wchProcessName, sizeof(wchProcessName));

			char dllpath[MAX_PATH];
			ZeroMemory(dllpath, sizeof(dllpath));

			GetWindowTextW(hWndProcessName, wchProcessName, sizeof(wchProcessName));

			DWORD dwProcessID = GetProcessIDByName(wchProcessName);
			if (!dwProcessID)
			{
				ErrorMessageBox("Process is not finded");
				return DefWindowProcW(hWnd, message, wParam, lParam);
			}
			GetWindowTextA(hWndDLLName, dllpath, MAX_PATH);

			if (CheckForDLL(&dwProcessID, dllpath))
			{
				ErrorMessageBox("DLL already in process");
				return DefWindowProc(hWnd, message, wParam, lParam);
			}

			if (!Inject(dwProcessID, (const char *)dllpath))
			{
				ErrorMessageBox("Failed to inject");
			}
			else
			{
				CustomMessageBox("Status", "Successful inject");
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
		10, 25, 360, 20, hWnd, (HMENU)IDC_SELECT_PROCESS_TEXT, NULL, NULL);

	hWndProcessList = CreateWindowA("BUTTON", "Processes",
		WS_VISIBLE | WS_CHILD,
		500 - 95 - 30, 25, 95, 20, hWnd, (HMENU)IDC_SELECT_PROCESS_BUTTON, NULL, NULL);

	CreateWindowA("STATIC", "Select .dll to inject",
		WS_VISIBLE | WS_CHILD | ES_CENTER,
		500 / 2 - 80 - 5, 51, 160, 17, hWnd, (HMENU)1, NULL, NULL);

	hWndDLLName = CreateWindowA("EDIT", "",
		WS_VISIBLE | WS_BORDER | WS_CHILD | ES_CENTER | ES_AUTOHSCROLL,
		10, 75, 360, 20, hWnd, (HMENU)IDC_SELECT_DLL_TEXT, NULL, NULL);

	hWndSelectDLLButton = CreateWindowA("BUTTON", "Choose",
		WS_VISIBLE | WS_CHILD,
		500 - 95 - 30, 75, 95, 20, hWnd, (HMENU)IDC_SELECT_DLL_BUTTON, NULL, NULL);

	CreateWindowA("BUTTON", "Inject",
		WS_VISIBLE | WS_CHILD,
		5, 100, 475, 25, hWnd, (HMENU)IDC_INJECT_BUTTON, NULL, NULL);
}

void SelectProcess(HWND hWnd)
{
	AllocConsole();
	ZeroMemory(&fp, sizeof(FILE));
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	freopen_s(&fp, "CONOUT$", "w", stdout);

	cout.clear();
	setlocale(LC_ALL, "Russian");

	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		cout << "CreateToolhelp32Snapshot (of processes)";
		FreeConsole();
		return;
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32))
	{
		cout << "Process32First"; // show cause of failure
		CloseHandle(hProcessSnap);          // clean the snapshot object
		FreeConsole();
		return;
	}

	do
	{
		_tprintf(L"\n%s", pe32.szExeFile);

	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	FreeConsole();
}

void SelectDll(HWND hWnd)
{
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = (LPWSTR)szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"DLL\0*.dll\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	GetOpenFileNameW(&ofn);

	if (ofn.lpstrFile)
		SetWindowTextW(hWndDLLName, ofn.lpstrFile);
	else
		ErrorMessageBox("DLL isn't choosen");
}

BOOL Inject(DWORD dwProcessID, const char *chDLLName)
{
	BOOL bRet = FALSE, bStatus;
	LPVOID lpLoadLibrary = nullptr;
	LPVOID lpAlloc = nullptr;
	HANDLE h = 0, hThread = 0;

	CHAR chError[MAX_PATH]; // MAX_PATH is used for system paths, it's still big enough
	ZeroMemory(chError, MAX_PATH);

	h = OpenProcess(
		PROCESS_ALL_ACCESS,
		FALSE,
		dwProcessID
	);
	if (!h)
	{
		ErrorMessageBox("Failed to open process process");
		goto end;
	}

	lpLoadLibrary = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	if (lpLoadLibrary == NULL)
	{
		ErrorMessageBox("Failed to create remote process");
		goto end;
	}

	lpAlloc = VirtualAllocEx(h, NULL, strlen(chDLLName), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (lpAlloc == NULL)
	{
		ErrorMessageBox("Allocation of memory failed");
		goto end;
	}

	bStatus = WriteProcessMemory(h, lpAlloc, chDLLName, strlen(chDLLName), NULL);
	if (bStatus == NULL)
	{
		ErrorMessageBox("Failed to write memory to the process");
		goto end;
	}

	hThread = CreateRemoteThread(h, NULL, NULL, (LPTHREAD_START_ROUTINE)lpLoadLibrary, lpAlloc, 0, NULL);
	if (hThread == NULL)
	{
		ErrorMessageBox("Failed to create remote process");
		goto end;
	}

	WaitForSingleObject(hThread, INFINITE);

	bRet = TRUE;
end:
	if (!bRet)
	{
		wsprintfA(chError, "Error: %lu", GetLastError());
		ErrorMessageBox(chError);
	}

	if (h
		&& lpAlloc)
	{
		VirtualFreeEx(h, lpAlloc, strlen(chDLLName), MEM_RELEASE);
	}

	if (hThread) CloseHandle(hThread);
	if (h) CloseHandle(h);

	return bRet;
}

DWORD GetProcessIDByName(const wchar_t* name)
{
	PROCESSENTRY32 peProcessEntry;
	ZeroMemory(&peProcessEntry, sizeof(peProcessEntry));

	HANDLE CONST hSnapshot = CreateToolhelp32Snapshot(
		TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	peProcessEntry.dwSize = sizeof(PROCESSENTRY32);
	Process32First(hSnapshot, &peProcessEntry);
	do {
		if (wcscmp(name, peProcessEntry.szExeFile) == NULL)
		{
			return peProcessEntry.th32ProcessID;
		}
	} while (Process32Next(hSnapshot, &peProcessEntry));

	CloseHandle(hSnapshot);
	return NULL;
}

BOOL CheckForDLL(DWORD *processid, char *pathofDLL)
{
	// Get a handle to the process.

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, *processid);

	// Get the process name.

	if (NULL != hProcess)
	{
		HMODULE hMod[1024];
		DWORD cbNeeded;
		char nameofDLL[MAX_PATH];
		char currentnameofdll[MAX_PATH];
		char processname[MAX_PATH];

		GetWindowTextA(hWndProcessName, processname, MAX_PATH);
		GetFileTitleA(pathofDLL, currentnameofdll, MAX_PATH);

		if (K32EnumProcessModules(hProcess, hMod, sizeof(hMod),
			&cbNeeded) != NULL)
		{
			for (int i = 0; i < (sizeof(hMod) / sizeof(HMODULE)); i++)
			{
				if (K32GetModuleBaseNameA(hProcess, hMod[i], nameofDLL, MAX_PATH) != NULL)
				{
					if ((string)nameofDLL != (string)processname)
					{
						if ((string)nameofDLL == (string)currentnameofdll)
						{
							return TRUE;
						}
					}
				}
			}
		}
	}

	// Release the handle to the process.

	CloseHandle(hProcess);
	return FALSE;
}
