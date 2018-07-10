#include <windows.h>  
#include "resource.h" //引入资源文件  
#include "stdio.h" 
#include "main.h"
#include <Commdlg.h>
#include <string.h>
#include<Shlobj.h>  
#pragma comment(lib,"Shell32.lib") 
#include <fstream> 

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool GetSDKPath(std::wstring &path, const std::wstring &sbdir);
bool RegWritePca();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    //设计窗口类  
    WNDCLASS wndclass;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//设置窗口背景色为白色  
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hInstance = hInstance;
    wndclass.lpfnWndProc = WndProc;
    wndclass.lpszClassName = L"InstallBox";
    wndclass.lpszMenuName = NULL;
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    //注册窗口类  
    if (!RegisterClass(&wndclass))
    {
        MessageBox(0, L"注册窗口类失败，请重新注册！", L"提示", MB_OK | MB_ICONERROR);
        return -1;
    }
    //创建窗口  
    HWND hWnd;
    hWnd = CreateWindow(wndclass.lpszClassName, L"Certificate",
        WS_OVERLAPPEDWINDOW^WS_MAXIMIZEBOX^WS_THICKFRAME,
        50,50,400, 300,
        0, 0, hInstance, NULL);

    //显示窗口  
    ShowWindow(hWnd, nShowCmd);
    //更新窗口  
    UpdateWindow(hWnd);
    //消息循环  
    MSG * msg;
    msg = new MSG();
    while (GetMessage(msg, NULL, 0, 0))
    {
        TranslateMessage(msg);//消息转换  
        DispatchMessage(msg);//消息投递  
    }
    return msg->wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    HWND hwndButton;
    HWND FileWnd;
    HFONT hFont;
    //改变输出字体
    LOGFONT logfont;
    ZeroMemory(&logfont, sizeof(LOGFONT));
    logfont.lfCharSet = GB2312_CHARSET;
    logfont.lfHeight = -15;
    hFont = CreateFontIndirect(&logfont);
    //SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);

    //OPENFILENAME 结构体
    OPENFILENAME ofn = { 0 };
    TCHAR strFilename[MAX_PATH] = { 0 };//用于接收文件名  
    ofn.lStructSize = sizeof(OPENFILENAME);//结构体大小  
    ofn.hwndOwner = hWnd;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄  
    ofn.lpstrFilter = TEXT("选择文件*.dpvr\0*.dpvr\0\0");//设置过滤  
    ofn.nFilterIndex = 1;//过滤器索引  
    ofn.lpstrFile = strFilename;//接收返回的文件名，注意第一个字符需要为NULL  
    ofn.nMaxFile = sizeof(strFilename);//缓冲区长度  
    ofn.lpstrInitialDir = NULL;//初始目录为默认  
    ofn.lpstrTitle = TEXT("请选择一个文件");//使用系统默认标题留空即可  
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//文件、目录必须存在，隐藏只读选项  
    switch (uMsg)
    {
    case WM_CREATE:
        int scrWidth, scrHeight;
        RECT rect;
        scrWidth = GetSystemMetrics(SM_CXSCREEN);
        scrHeight = GetSystemMetrics(SM_CYSCREEN);
        GetWindowRect(hWnd, &rect);
        rect.left = (scrWidth - rect.right) / 2;
        rect.top = (scrHeight - rect.bottom) / 2;
        SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW);
        hwndButton = CreateWindow(L"Button", L"安装(Install)", 
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 
           130, 120, 150, 100, hWnd, (HMENU)10,
            (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
        return 0;
    case WM_COMMAND:
        if (LOWORD(wParam) == 10 && HIWORD(wParam) == BN_CLICKED)
        {
            if (GetOpenFileName(&ofn))
            {
                std::wstring path;
                std::wstring dir;
                if (!GetSDKPath(path, dir))
                {
                    MessageBox(hWnd, TEXT("安装目录不存在"), TEXT("失败"), MB_OK | MB_ICONEXCLAMATION);
                    return false;
                }
                path.append(L"\\config\\831F97750AF2CF664CC7F9B659997BCB.dpvr");
                if (CopyFile(strFilename,path.c_str(), FALSE))
                {
                    MessageBox(hWnd, L"安装成功", TEXT("成功"),MB_OK );
                }
                else
                {
                    MessageBox(hWnd, L"安装失败", TEXT("失败"), MB_OK | MB_ICONERROR);
                }
            }
            else
            {
                MessageBox(hWnd, TEXT("请选择一个文件"), TEXT("选择文件"), MB_OK | MB_ICONEXCLAMATION);
            }
        }
    case WM_CHAR:
        return 0;
    case WM_PAINT:     
        PAINTSTRUCT ps;
        hdc = BeginPaint(hWnd, &ps);
        SelectObject(hdc, hFont);
        GetClientRect(hWnd, &rect);//注意GetClientRect和GetWindowRect的区别   
        SetBkColor(hdc, RGB(255, 255, 255));//设置文字的矩形区域背景为黑色  
        SetTextColor(hdc, RGB(0,0,0 ));//设置文字颜色为白色
        TextOut(hdc, 20, 30, L"点击安装按钮，选择License",16);
        TextOut(hdc, 20, 50, L"(Press the Install button and select the license)", 49);
        EndPaint(hWnd, &ps);
        return 0;
    case WM_LBUTTONDOWN:
        return 0;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}
bool GetSDKPath(std::wstring &path,const std::wstring &sbdir)
{
    bool bExist = false;
    TCHAR *pathBin = new TCHAR[MAX_PATH + 1];
    std::wstring subdir;
    if (sbdir.compare(TEXT("")))
    {
        subdir.append(TEXT("\\"));
        subdir.append(sbdir);
    }


    if (!bExist){

        LONG lResult;
        DWORD len = MAX_PATH * sizeof(TCHAR);
        lResult = RegGetValue(HKEY_LOCAL_MACHINE, (L"SOFTWARE\\DPVR"), (L"DPVR_PLATFORM_DIR"), RRF_RT_ANY | 0x00010000 /*RRF_SUBKEY_WOW6464KEY*/, NULL, pathBin, &len);
        if (lResult == ERROR_SUCCESS)
        {
            wcscat_s(pathBin, MAX_PATH, subdir.c_str());
            WIN32_FIND_DATA  wfd;
            HANDLE hFind = FindFirstFile(pathBin, &wfd);
            if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                bExist = true;
            }
            FindClose(hFind);
        }
    }
    if (!bExist){
        DWORD ret_size = GetEnvironmentVariable(L"DPVR_PLATFORM_DIR", pathBin, MAX_PATH);
        if (ret_size != 0)
        {
            wcscat_s(pathBin, MAX_PATH, subdir.c_str());
            WIN32_FIND_DATA  wfd;
            HANDLE hFind = FindFirstFile(pathBin, &wfd);
            if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                bExist = true;
            }
            FindClose(hFind);
        }
    }

    if (bExist)
    {
        path.clear();
        path.append(pathBin);
    }
    delete[]pathBin;

    int iLength;
    char path_char[512] = { 0 };
    iLength = WideCharToMultiByte(CP_ACP, 0, path.c_str(), -1, NULL, 0, NULL, NULL);
    WideCharToMultiByte(CP_ACP, 0, path.c_str(), -1, path_char, iLength, NULL, NULL);
    //DPN_SDK_LOGI("GetSDKPath: %s", path_char);

    return bExist;
}

bool RegWritePca()
{
    HKEY hKey;
    if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Compatibility Assistant\\Persisted", 0, KEY_READ | KEY_WRITE, &hKey))
    {
        return false;
    }
    wchar_t szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);
    DWORD dwCode = 1;
    LONG lRet = RegSetValueEx(hKey, szPath, 0, REG_DWORD, (byte*)&dwCode, sizeof(DWORD));
    RegCloseKey(hKey);
    return lRet == ERROR_SUCCESS;
}