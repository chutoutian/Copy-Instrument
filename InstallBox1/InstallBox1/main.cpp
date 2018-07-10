#include <windows.h>  
#include "resource.h" //������Դ�ļ�  
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
    //��ƴ�����  
    WNDCLASS wndclass;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//���ô��ڱ���ɫΪ��ɫ  
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hInstance = hInstance;
    wndclass.lpfnWndProc = WndProc;
    wndclass.lpszClassName = L"InstallBox";
    wndclass.lpszMenuName = NULL;
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    //ע�ᴰ����  
    if (!RegisterClass(&wndclass))
    {
        MessageBox(0, L"ע�ᴰ����ʧ�ܣ�������ע�ᣡ", L"��ʾ", MB_OK | MB_ICONERROR);
        return -1;
    }
    //��������  
    HWND hWnd;
    hWnd = CreateWindow(wndclass.lpszClassName, L"Certificate",
        WS_OVERLAPPEDWINDOW^WS_MAXIMIZEBOX^WS_THICKFRAME,
        50,50,400, 300,
        0, 0, hInstance, NULL);

    //��ʾ����  
    ShowWindow(hWnd, nShowCmd);
    //���´���  
    UpdateWindow(hWnd);
    //��Ϣѭ��  
    MSG * msg;
    msg = new MSG();
    while (GetMessage(msg, NULL, 0, 0))
    {
        TranslateMessage(msg);//��Ϣת��  
        DispatchMessage(msg);//��ϢͶ��  
    }
    return msg->wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    HWND hwndButton;
    HWND FileWnd;
    HFONT hFont;
    //�ı��������
    LOGFONT logfont;
    ZeroMemory(&logfont, sizeof(LOGFONT));
    logfont.lfCharSet = GB2312_CHARSET;
    logfont.lfHeight = -15;
    hFont = CreateFontIndirect(&logfont);
    //SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);

    //OPENFILENAME �ṹ��
    OPENFILENAME ofn = { 0 };
    TCHAR strFilename[MAX_PATH] = { 0 };//���ڽ����ļ���  
    ofn.lStructSize = sizeof(OPENFILENAME);//�ṹ���С  
    ofn.hwndOwner = hWnd;//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������  
    ofn.lpstrFilter = TEXT("ѡ���ļ�*.dpvr\0*.dpvr\0\0");//���ù���  
    ofn.nFilterIndex = 1;//����������  
    ofn.lpstrFile = strFilename;//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL  
    ofn.nMaxFile = sizeof(strFilename);//����������  
    ofn.lpstrInitialDir = NULL;//��ʼĿ¼ΪĬ��  
    ofn.lpstrTitle = TEXT("��ѡ��һ���ļ�");//ʹ��ϵͳĬ�ϱ������ռ���  
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//�ļ���Ŀ¼������ڣ�����ֻ��ѡ��  
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
        hwndButton = CreateWindow(L"Button", L"��װ(Install)", 
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
                    MessageBox(hWnd, TEXT("��װĿ¼������"), TEXT("ʧ��"), MB_OK | MB_ICONEXCLAMATION);
                    return false;
                }
                path.append(L"\\config\\831F97750AF2CF664CC7F9B659997BCB.dpvr");
                if (CopyFile(strFilename,path.c_str(), FALSE))
                {
                    MessageBox(hWnd, L"��װ�ɹ�", TEXT("�ɹ�"),MB_OK );
                }
                else
                {
                    MessageBox(hWnd, L"��װʧ��", TEXT("ʧ��"), MB_OK | MB_ICONERROR);
                }
            }
            else
            {
                MessageBox(hWnd, TEXT("��ѡ��һ���ļ�"), TEXT("ѡ���ļ�"), MB_OK | MB_ICONEXCLAMATION);
            }
        }
    case WM_CHAR:
        return 0;
    case WM_PAINT:     
        PAINTSTRUCT ps;
        hdc = BeginPaint(hWnd, &ps);
        SelectObject(hdc, hFont);
        GetClientRect(hWnd, &rect);//ע��GetClientRect��GetWindowRect������   
        SetBkColor(hdc, RGB(255, 255, 255));//�������ֵľ������򱳾�Ϊ��ɫ  
        SetTextColor(hdc, RGB(0,0,0 ));//����������ɫΪ��ɫ
        TextOut(hdc, 20, 30, L"�����װ��ť��ѡ��License",16);
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