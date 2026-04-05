#include <iostream>
#include <windows.h>
#include <string>
#include <thread>

std::wstring get_clipboard_text() {
    if (!OpenClipboard(nullptr)) {
        return L"";
    }

    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (hData == nullptr) {
        CloseClipboard();
        return L"";
    }

    wchar_t* pszText = static_cast<wchar_t*>(GlobalLock(hData));
    if (pszText == nullptr) {
        CloseClipboard();
        return L"";
    }

    std::wstring text(pszText);

    GlobalUnlock(hData);
    CloseClipboard();

    return text;
}


void send_unicode_char(wchar_t ch) {
    INPUT input[2] = {};

    // 按下
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wScan = ch;
    input[0].ki.dwFlags = KEYEVENTF_UNICODE;

    // 抬起
    input[1] = input[0];
    input[1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;

    SendInput(2, input, sizeof(INPUT));
}

void send_enter() {
    INPUT input[2] = {};

    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = VK_RETURN;

    input[1] = input[0];
    input[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(2, input, sizeof(INPUT));
}

void send_unicode_string(const std::wstring& text) {
    for (wchar_t ch : text) {
        if (ch== L'\r') continue;
     
        if (ch == L'\n') {
            send_enter();   // ⭐ 用回车代替
        }
        else {
            send_unicode_char(ch);
        }

           
    
        Sleep(8 + rand() % 30);
    }
}

void up_ctrl_shift() {
    INPUT inputs[4] = {};

    // Ctrl up
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;
    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;

    // Shift up
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_SHIFT;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(2, inputs, sizeof(INPUT));

    Sleep(50); // 给系统一点时间
}

int main() {
   

  
        HWND hwnd = GetConsoleWindow();
        ShowWindow(hwnd, SW_MINIMIZE); // 隐藏控制台
 
        if (!RegisterHotKey(nullptr, 1, MOD_CONTROL | MOD_SHIFT, 'V')) {
            MessageBox(nullptr, L"热键注册失败！", L"Error", MB_ICONERROR);
            return 1;
        }

        MSG msg = { 0 };

        // 消息循环等待热键触发
        while (GetMessage(&msg, nullptr, 0, 0)) {
            if (msg.message == WM_HOTKEY) {
                int id = msg.wParam;
                if (id == 1) {
                    up_ctrl_shift();
                        std::wstring text = get_clipboard_text();
                            send_unicode_string(text);
                }
            }
        }

        UnregisterHotKey(nullptr, 1);

   
  
    return 0;
}