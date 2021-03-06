#include "Injector.h"

#define IDC_PROCESS_NAME 275
#define IDC_SELECT_PROCESS_BUTTON 276
#define IDC_DLL_NAME 277
#define IDC_SELECT_DLL_BUTTON 278
#define IDC_INJECT_BUTTON 279

#define MAX_LOADSTRING 100

#define ErrorMessageBox(msg) MessageBoxA(NULL, msg, "Injector - Oops, Problem", MB_OK | MB_TOPMOST | MB_ICONERROR);
#define CustomMessageBox(title, msg) MessageBoxA(NULL, msg, "Injector - " title, MB_OK | MB_TOPMOST);

using namespace std;

// Global variables:
HINSTANCE hInst;                                // Current instance
WCHAR szTitle[MAX_LOADSTRING];                  // Header text of the main window
WCHAR szWindowClass[MAX_LOADSTRING];            // Class name of the main window

HWND hWndProcessName,                           // TextEdit Process name window
     hWndProcessList,                           // Button Create console window where displayed all current working processes
     hWndDLLName,                               // TextEdit Dll name window
     hWndSelectDLLButton;                       // Button Create window where user can select dll that he want to inject

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void				InitializeClientArea(HWND);
void				SelectDll(HWND hWnd);
void				SelectProcess(HWND hWnd);
BOOL				Inject(DWORD dwProcessID, const char * dllName);
DWORD               GetProcessIDByName(const wchar_t* name);
BOOL				CheckForDLL(DWORD processid, char *pathofDLL);

//
//  Function: wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
//
//  Purpose: Entry point of the process
//
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_INJEKPNEW, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Initialize application:
    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_INJEKPNEW));

    MSG msg;

    // Main message loop:
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
//  Function: MyRegisterClass()
//
//  Purpose: Register class of the window.
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
//   Function: InitInstance(HINSTANCE, int)
//
//   Purpose: Saves current instance in the global variable and creates main window
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Save current instance in the global variable

    // Create main window
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX) &~(WS_THICKFRAME),
        CW_USEDEFAULT, 0, 500, 170, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
        return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  Function: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Purpose: Handle messages of the main window.
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        InitializeClientArea(hWnd);
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        switch (wmId)
        {
        case IDC_SELECT_DLL_BUTTON:
            SelectDll(hWnd);
            break;
        case IDC_SELECT_PROCESS_BUTTON:
            SelectProcess(hWnd);
            break;
        case IDC_INJECT_BUTTON:
        {
            DWORD dwProcessID;

            wchar_t wchProcessName[MAX_PATH];
            ZeroMemory(wchProcessName, sizeof(wchProcessName));

            char dllpath[MAX_PATH];
            ZeroMemory(dllpath, sizeof(dllpath));

            try
            {
                // Get process name from process name window
                if (!GetWindowTextW(hWndProcessName, wchProcessName, MAX_PATH))
                    throw exception("Unable to get window text");

                // Get target process id by it's name
                dwProcessID = GetProcessIDByName(wchProcessName);
                if (!dwProcessID)
                    throw exception("Process is not finded");

                // Get dll path from dll name window
                if (!GetWindowTextA(hWndDLLName, dllpath, MAX_PATH))
                    throw exception("Unable to get window text");

                // Check if dll already in target process
                if (CheckForDLL(dwProcessID, dllpath))
                    throw exception("DLL already in process");

                // Inject dll into target process
                if (!Inject(dwProcessID, dllpath))
                    ErrorMessageBox("Failed to inject")
                else
                    CustomMessageBox("Status", "Successful inject");
            }
            catch (const std::exception& e)
            {
                // Display exception string
                ErrorMessageBox(e.what());
            }
        }
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
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

//
//   Function: InitializeClientArea(HWND)
//
//   Purpose: Creates buttons, text boxes, static text in the client area
//
void InitializeClientArea(HWND hWnd)
{
    // Create static text "Process to inject"
    CreateWindowA("STATIC", "Process to inject",
        WS_VISIBLE | WS_CHILD | ES_CENTER,
        500 / 2 - 80 - 5, 0, 160, 17, hWnd, (HMENU)1, NULL, NULL);

    // Create edit text box where user can write file name of the target process
    hWndProcessName = CreateWindowA("EDIT", "",
        WS_VISIBLE | WS_BORDER | WS_CHILD | ES_CENTER | ES_AUTOHSCROLL,
        10, 25, 360, 20, hWnd, (HMENU)IDC_PROCESS_NAME, NULL, NULL);

    // Create button with name "Processes" and when user press it, it calls SelectProcess function
    hWndProcessList = CreateWindowA("BUTTON", "Processes",
        WS_VISIBLE | WS_CHILD,
        500 - 95 - 30, 25, 95, 20, hWnd, (HMENU)IDC_SELECT_PROCESS_BUTTON, NULL, NULL);

    // Create static text "Select .dll to inject"
    CreateWindowA("STATIC", "Select .dll to inject",
        WS_VISIBLE | WS_CHILD | ES_CENTER,
        500 / 2 - 80 - 5, 51, 160, 17, hWnd, (HMENU)1, NULL, NULL);

    // Create edit text box where user can write dll path that he wants to inject
    hWndDLLName = CreateWindowA("EDIT", "",
        WS_VISIBLE | WS_BORDER | WS_CHILD | ES_CENTER | ES_AUTOHSCROLL,
        10, 75, 360, 20, hWnd, (HMENU)IDC_DLL_NAME, NULL, NULL);

    // Create button with name "Choose" and when user press it, it calls SelectDll function
    hWndSelectDLLButton = CreateWindowA("BUTTON", "Choose",
        WS_VISIBLE | WS_CHILD,
        500 - 95 - 30, 75, 95, 20, hWnd, (HMENU)IDC_SELECT_DLL_BUTTON, NULL, NULL);

    // Create button with name "Inject" and when user press it, it injects dll into target process
    CreateWindowA("BUTTON", "Inject",
        WS_VISIBLE | WS_CHILD,
        5, 100, 475, 25, hWnd, (HMENU)IDC_INJECT_BUTTON, NULL, NULL);
}

//
//   Function: SelectProcess(HWND)
//
//   Purpose: Creates console window where displays all names of processes
//
void SelectProcess(HWND hWnd)
{
    FILE* fp = 0;
    HANDLE hProcessSnap = INVALID_HANDLE_VALUE;

    PROCESSENTRY32 pe32;
    ZeroMemory(&pe32, sizeof(pe32));

    try
    {
        // Create console window
        if (!AllocConsole())
            throw exception("Unable to create console window");

        // Set input code page for the console
        if (!SetConsoleCP(1251))
            throw exception("Unable to set input code page for the console");

        // Set output code page for the console
        if (!SetConsoleOutputCP(1251))
            throw exception("Unable to set output code page for the console");

        // Get console write permission
        if (freopen_s(&fp, "CONOUT$", "w", stdout))
            throw exception("Unable get console write permission");

        // Set all locales for the console
        if (setlocale(LC_ALL, "") == nullptr)
            throw exception("Unable to set all locales for the console");

        // Take a snapshot of all processes in the system.
        hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hProcessSnap == INVALID_HANDLE_VALUE)
            throw exception("CreateToolhelp32Snapshot (of processes)");

        // Set the size of the structure before using it.
        pe32.dwSize = sizeof(PROCESSENTRY32);

        // Get information about the first process in snapshot
        if (!Process32First(hProcessSnap, &pe32))
            throw exception("Unable to get first process from the snapshot");

        // Print the names of all processes
        do {
            wcout << pe32.szExeFile << endl;
        } while (Process32Next(hProcessSnap, &pe32));
    }
    catch (const std::exception& e)
    {
        // Display exception string
        cout << e.what() << endl;
    }
    
    if (hProcessSnap != INVALID_HANDLE_VALUE) CloseHandle(hProcessSnap);

    // Free console, if not do this, when user close console window, main window will be closed too
    FreeConsole();

    return;
}

//
//   Function: SelectDll(HWND)
//
//   Purpose: Creates window where user can select dll that he wants to inject and set dll path to the dll name window
//
void SelectDll(HWND hWnd)
{
    // Buffer for dll path
    wchar_t szFile[MAX_PATH];
    ZeroMemory(szFile, sizeof(szFile));

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    // Initialize structure
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"DLL\0*.dll\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    // Create window to let user choose dll for inject
    if (!GetOpenFileNameW(&ofn))
        return;

    // Change name of the window to user choosed dll path
    if (!SetWindowTextW(hWndDLLName, ofn.lpstrFile))
        return;

    return;
}

//
//   Function: Inject(DWORD, const char*)
//
//   Purpose: Inject dll into target process
//
BOOL Inject(DWORD dwProcessID, const char *chDLLName)
{
    BOOL bRet = FALSE, bStatus = FALSE;
    LPVOID lpLoadLibrary = nullptr;
    LPVOID lpAlloc = nullptr;
    HANDLE hProcess = 0, hThread = 0;
    HMODULE hModule = 0;

    try
    {
        // Get a handle to the target process to inject
        hProcess = OpenProcess(
            PROCESS_ALL_ACCESS,
            FALSE,
            dwProcessID
        );
        if (hProcess == INVALID_HANDLE_VALUE)
            throw exception("Failed to open target process");

        // Get a handle to the kernel32.dll module
        hModule = GetModuleHandleA("kernel32.dll");
        if (!hModule)
            throw exception("Failed to get kernel32.dll module handle");

        // Get address of LoadLibraryA function in kernel32.dll module
        lpLoadLibrary = (LPVOID)GetProcAddress(hModule, "LoadLibraryA");
        if (!lpLoadLibrary)
            throw exception("Failed to get address of LoadLibraryA");
        
        // Allocate memory in target process to contain full dll path
        lpAlloc = VirtualAllocEx(hProcess, NULL, strlen(chDLLName), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!lpAlloc)
            throw exception("Unable to allocate memory in target process");

        // Write dll path to the allocated memory of target process
        bStatus = WriteProcessMemory(hProcess, lpAlloc, chDLLName, strlen(chDLLName), NULL);
        if (!bStatus)
            throw exception("Failed to write full dll path to the allocated memory at the target process");

        // Create thread in target process, that begins at LoadLibraryA function and passes as an argument pointer to the allocated memory that contains full dll path to inject
        hThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)lpLoadLibrary, lpAlloc, 0, NULL);
        if (!hThread)
            throw exception("Failed to create remote thread at target process");

        // Wait for the thread to end
        WaitForSingleObject(hThread, INFINITE);

        bRet = TRUE;
    }
    catch (const std::exception& e)
    {
        // Display exception string
        ErrorMessageBox(e.what());
    }

    // Safely release everything
    if (hThread) CloseHandle(hThread);
    if (hProcess && lpAlloc)
        VirtualFreeEx(hProcess, lpAlloc, strlen(chDLLName), MEM_RELEASE);
    if (hProcess) CloseHandle(hProcess);

    return bRet;
}

//
//   Function: GetProcessIDByName(const wchar_t*)
//
//   Purpose: Get process ID by it's name
//
DWORD GetProcessIDByName(const wchar_t* name)
{
    DWORD dwProcessID = 0;
    HANDLE hSnapshot = INVALID_HANDLE_VALUE;

    PROCESSENTRY32 peProcessEntry;
    ZeroMemory(&peProcessEntry, sizeof(peProcessEntry));

    peProcessEntry.dwSize = sizeof(PROCESSENTRY32);

    try
    {
        // Create snapshot of all processes
        hSnapshot = CreateToolhelp32Snapshot(
            TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE)
            throw exception("Unable to create snapshot of all processes");

        // Get information about the first process in snapshot
        if (!Process32First(hSnapshot, &peProcessEntry))
            throw exception("Unable to get first process from the snapshot");

        do {
            if (wcscmp(name, peProcessEntry.szExeFile) == NULL)
            {
                dwProcessID = peProcessEntry.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &peProcessEntry));
    }
    catch (const std::exception& e)
    {
        // Display exception string
        ErrorMessageBox(e.what());
    }

    // Close snapshot of all processes
    CloseHandle(hSnapshot);

    return dwProcessID;
}

//
//   Function: CheckForDLL(DWORD, char*)
//
//   Purpose: Check if dll already in target process
//
BOOL CheckForDLL(DWORD dwProcessID, char *chFullPath)
{
    BOOL bRet = FALSE;
    DWORD cbNeeded;
    HANDLE hProcess = INVALID_HANDLE_VALUE;

    // Inject DLL Title
    char chIDLLTitle[MAX_PATH];
    ZeroMemory(chIDLLTitle, MAX_PATH);

    // Process name
    char chProcessName[MAX_PATH];
    ZeroMemory(chProcessName, MAX_PATH);

    // Buffer for target process module names
    char chDLLName[MAX_PATH];
    ZeroMemory(chDLLName, MAX_PATH);

    HMODULE hMod[1024];

    try
    {
        // Get a handle to the process.
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
            PROCESS_VM_READ,
            FALSE, dwProcessID);
        if (hProcess == INVALID_HANDLE_VALUE)
            throw exception("Unable to open target process");

        // Get process name from the process name window
        if (!GetWindowTextA(hWndProcessName, chProcessName, MAX_PATH))
            throw exception("Unable to get window text from process name window");

        // Get file title from full dll path
        if (GetFileTitleA(chFullPath, chIDLLTitle, MAX_PATH))
            throw exception("Unable to get file title from full dll path");

        // Get all module handles from the process to the buffer
        if (!K32EnumProcessModules(hProcess, hMod, sizeof(hMod),
            &cbNeeded) != NULL)
            throw exception("Unable to enum target process modules");

        // Enum all target process modules
        for (int i = 0; i < (sizeof(hMod) / sizeof(HMODULE)); i++)
        {
            // Get name of the module
            if (!GetModuleBaseNameA(hProcess, hMod[i], chDLLName, MAX_PATH) != NULL)
                continue;

            // Be sure that name of the dll is not a name of the process exe file and equals to our dll to inject
            if (strcmp(chDLLName, chProcessName) != NULL
                && strcmp(chDLLName, chIDLLTitle) == NULL)
                    bRet = TRUE;
        }
    }
    catch (const std::exception& e)
    {
        // Display exception string
        ErrorMessageBox(e.what());
    }

    // Release the handle to the process.
    CloseHandle(hProcess);

    return bRet;
}
