#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <commdlg.h>
#include <vector>
#include <fstream>
#include <algorithm> // 計算用

// ==========================================
// 1. データとグローバル変数
// ==========================================
struct SpringNode {
    float x, y;
};

// 設定値
int g_TrailLength = 30;
int g_Stiffness = 30;
int g_LineWidth = 5;
COLORREF g_LineColor = RGB(0, 255, 128);
COLORREF g_CustomColors[16];

std::vector<SpringNode> g_springs;

// ID定義
#define ID_SLIDER_LENGTH    101
#define ID_SLIDER_STIFFNESS 102
#define ID_SLIDER_WIDTH     103
#define ID_BTN_COLOR        104

#define WM_TRAYICON         (WM_USER + 1)
#define ID_TRAY_ICON        1
#define ID_MENU_EXIT        2001

HWND g_hSettings = NULL;
NOTIFYICONDATAW g_nid = { };

// ==========================================
// 設定の保存・読み込み
// ==========================================
void SaveSettings() {
    std::ofstream file("settings.txt");
    if (file.is_open()) {
        file << g_TrailLength << std::endl;
        file << g_Stiffness << std::endl;
        file << g_LineWidth << std::endl;
        file << g_LineColor << std::endl;
        file.close();
    }
}

void LoadSettings() {
    std::ifstream file("settings.txt");
    if (file.is_open()) {
        file >> g_TrailLength;
        file >> g_Stiffness;
        file >> g_LineWidth;
        file >> g_LineColor;
        file.close();
    }
}

// ==========================================
// 2. 設定ウィンドウ
// ==========================================
LRESULT CALLBACK SettingsProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        {
            // UI作成
            CreateWindowW(L"STATIC", L"\u7D10\u306E\u9577\u3055", WS_VISIBLE | WS_CHILD, 10, 10, 200, 20, hwnd, NULL, NULL, NULL);
            HWND hLen = CreateWindowW(L"msctls_trackbar32", L"", WS_VISIBLE | WS_CHILD | TBS_AUTOTICKS,
                10, 30, 260, 30, hwnd, (HMENU)ID_SLIDER_LENGTH, NULL, NULL);
            SendMessage(hLen, TBM_SETRANGE, TRUE, MAKELPARAM(2, 200));
            SendMessage(hLen, TBM_SETPOS, TRUE, g_TrailLength);

            CreateWindowW(L"STATIC", L"\u30D0\u30CD\u306E\u786C\u3055", WS_VISIBLE | WS_CHILD, 10, 70, 200, 20, hwnd, NULL, NULL, NULL);
            HWND hStiff = CreateWindowW(L"msctls_trackbar32", L"", WS_VISIBLE | WS_CHILD | TBS_AUTOTICKS,
                10, 90, 260, 30, hwnd, (HMENU)ID_SLIDER_STIFFNESS, NULL, NULL);
            SendMessage(hStiff, TBM_SETRANGE, TRUE, MAKELPARAM(1, 100));
            SendMessage(hStiff, TBM_SETPOS, TRUE, g_Stiffness);

            CreateWindowW(L"STATIC", L"\u7DDA\u306E\u592A\u3055", WS_VISIBLE | WS_CHILD, 10, 130, 200, 20, hwnd, NULL, NULL, NULL);
            HWND hWid = CreateWindowW(L"msctls_trackbar32", L"", WS_VISIBLE | WS_CHILD | TBS_AUTOTICKS,
                10, 150, 260, 30, hwnd, (HMENU)ID_SLIDER_WIDTH, NULL, NULL);
            SendMessage(hWid, TBM_SETRANGE, TRUE, MAKELPARAM(1, 50));
            SendMessage(hWid, TBM_SETPOS, TRUE, g_LineWidth);

            CreateWindowW(L"BUTTON", L"\u8272\u3092\u5909\u66F4\u3059\u308B...", 
                WS_VISIBLE | WS_CHILD, 10, 200, 260, 30, hwnd, (HMENU)ID_BTN_COLOR, NULL, NULL);

            CreateWindowW(L"STATIC", L"\u203B\u00D7\u30DC\u30BF\u30F3\u3067\u96A0\u308C\u307E\u3059\u3002\n\u7D42\u4E86\u306F\u30C8\u30EC\u30A4\u30A2\u30A4\u30B3\u30F3\u3092\u53F3\u30AF\u30EA\u30C3\u30AF\u3002", 
                WS_VISIBLE | WS_CHILD, 10, 250, 280, 40, hwnd, NULL, NULL, NULL);
        }
        return 0;

    case WM_HSCROLL:
        {
            HWND hSlider = (HWND)lParam;
            int id = GetDlgCtrlID(hSlider);
            int pos = SendMessage(hSlider, TBM_GETPOS, 0, 0);

            if (id == ID_SLIDER_LENGTH) {
                g_TrailLength = pos;
                if (g_springs.size() != g_TrailLength) g_springs.resize(g_TrailLength, { -100.0f, -100.0f });
            }
            else if (id == ID_SLIDER_STIFFNESS) g_Stiffness = pos;
            else if (id == ID_SLIDER_WIDTH) g_LineWidth = pos;
        }
        return 0;

    case WM_COMMAND:
        if (LOWORD(wParam) == ID_BTN_COLOR) {
            CHOOSECOLOR cc = { 0 };
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hwnd;
            cc.lpCustColors = g_CustomColors;
            cc.rgbResult = g_LineColor;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;

            if (ChooseColor(&cc)) {
                g_LineColor = cc.rgbResult;
            }
        }
        else if (LOWORD(wParam) == ID_MENU_EXIT) {
            SaveSettings();
            Shell_NotifyIconW(NIM_DELETE, &g_nid);
            PostQuitMessage(0);
        }
        return 0;

    case WM_TRAYICON:
        if (lParam == WM_LBUTTONUP) {
            ShowWindow(hwnd, SW_SHOW);
            SetForegroundWindow(hwnd);
        }
        else if (lParam == WM_RBUTTONUP) {
            POINT pt;
            GetCursorPos(&pt);
            HMENU hMenu = CreatePopupMenu();
            AppendMenuW(hMenu, MF_STRING, ID_MENU_EXIT, L"\u7D42\u4E86");
            SetForegroundWindow(hwnd);
            TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
            DestroyMenu(hMenu);
        }
        return 0;

    case WM_CLOSE:
        SaveSettings();
        ShowWindow(hwnd, SW_HIDE);
        return 0; 
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK OverlayProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_ERASEBKGND) return 1;
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// ==========================================
// 4. メイン関数
// ==========================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    LoadSettings();

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);

    const wchar_t SETTINGS_CLASS[] = L"SettingsWindow";
    WNDCLASSW wcS = { };
    wcS.lpfnWndProc = SettingsProc;
    wcS.hInstance = hInstance;
    wcS.lpszClassName = SETTINGS_CLASS;
    wcS.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcS.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcS.hIcon = LoadIcon(NULL, IDI_APPLICATION); 
    RegisterClassW(&wcS);

    g_hSettings = CreateWindowW(
        SETTINGS_CLASS, L"\u30B4\u30E0\u7D10\u8A2D\u5B9A", 
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        100, 100, 300, 340, 
        NULL, NULL, hInstance, NULL
    );

    g_nid.cbSize = sizeof(NOTIFYICONDATAW);
    g_nid.hWnd = g_hSettings;
    g_nid.uID = ID_TRAY_ICON;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    lstrcpyW(g_nid.szTip, L"\u30B4\u30E0\u7D10\u30AA\u30FC\u30D0\u30FC\u30EC\u30A4");
    Shell_NotifyIconW(NIM_ADD, &g_nid);

    ShowWindow(g_hSettings, nCmdShow);

    const wchar_t OVERLAY_CLASS[] = L"OverlayWindow";
    WNDCLASSW wcO = { };
    wcO.lpfnWndProc = OverlayProc;
    wcO.hInstance = hInstance;
    wcO.lpszClassName = OVERLAY_CLASS;
    wcO.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClassW(&wcO);

    int vScreenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vScreenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vScreenW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int vScreenH = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    HWND hOverlay = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        OVERLAY_CLASS, L"Overlay",
        WS_POPUP, 
        vScreenX, vScreenY, vScreenW, vScreenH,
        NULL, NULL, hInstance, NULL
    );
    SetLayeredWindowAttributes(hOverlay, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(hOverlay, SW_SHOW);

    g_springs.resize(g_TrailLength, { -100.0f, -100.0f });

    // ★前回描画した範囲を覚えておくための変数
    RECT lastRect = {0, 0, 0, 0}; 

    MSG msg = { };
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hOverlay, &pt);

            g_springs[0].x = (float)pt.x;
            g_springs[0].y = (float)pt.y;

            float k = (float)g_Stiffness / 100.0f;
            
            // ★今回の描画範囲を計算するための最小・最大値
            float minX = g_springs[0].x, maxX = g_springs[0].x;
            float minY = g_springs[0].y, maxY = g_springs[0].y;

            for (size_t i = 1; i < g_springs.size(); ++i) {
                g_springs[i].x += (g_springs[i-1].x - g_springs[i].x) * k;
                g_springs[i].y += (g_springs[i-1].y - g_springs[i].y) * k;
                
                // 範囲更新
                if (g_springs[i].x < minX) minX = g_springs[i].x;
                if (g_springs[i].x > maxX) maxX = g_springs[i].x;
                if (g_springs[i].y < minY) minY = g_springs[i].y;
                if (g_springs[i].y > maxY) maxY = g_springs[i].y;
            }

            // マージン（線の太さ分＋余裕）を追加
            int margin = g_LineWidth + 20;
            RECT currentRect;
            currentRect.left   = (int)minX - margin;
            currentRect.top    = (int)minY - margin;
            currentRect.right  = (int)maxX + margin;
            currentRect.bottom = (int)maxY + margin;

            // ★「前回の場所」と「今回の場所」を合体させる（ここだけ消せばOK）
            RECT updateRect;
            UnionRect(&updateRect, &lastRect, &currentRect);

            // 描画開始
            HDC hdc = GetDC(hOverlay);
            
            // ★全体ではなく、updateRect（変化があった場所）だけを黒塗り！
            HBRUSH hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
            FillRect(hdc, &updateRect, hBlackBrush);
            DeleteObject(hBlackBrush);

            // 線を描く
            HPEN hPen = CreatePen(PS_SOLID, g_LineWidth, g_LineColor);
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
            MoveToEx(hdc, (int)g_springs[0].x, (int)g_springs[0].y, NULL);
            for (size_t i = 1; i < g_springs.size(); ++i) {
                LineTo(hdc, (int)g_springs[i].x, (int)g_springs[i].y);
            }
            SelectObject(hdc, hOldPen);
            DeleteObject(hPen);
            
            ReleaseDC(hOverlay, hdc);

            // 次回のために今回の範囲を記憶
            lastRect = currentRect;

            // ★60FPS程度に制限（早すぎると逆に重くなるため少し増やす）
            Sleep(16); 
        }
    }
    return 0;
}