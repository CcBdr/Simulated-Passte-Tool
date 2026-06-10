#include <iostream>
#include <windows.h>
#include <string>
#include <thread>
#include <vector>

// 删除了 get_clipboard_text 逻辑，改为全局或 main 函数内的变量存储
std::wstring g_input_text = L"";

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
    // ⭐ 用真实回车（关键）
    INPUT input[2] = {};

    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = VK_RETURN;

    input[1] = input[0];
    input[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(2, input, sizeof(INPUT));

    Sleep(0 + rand() % 25);
}

void move_to_home() {
    INPUT input[2] = {};

    // 按下 Home
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = VK_HOME;

    // 抬起 Home
    input[1] = input[0];
    input[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(2, input, sizeof(INPUT));

    Sleep(10);
}

void send_unicode_string(const std::wstring& text) {
    for (size_t i = 0; i < text.length(); ++i) {
        wchar_t ch = text[i];

        if (ch == L'\r') continue;

        if (ch == L'\n') {
            send_enter();
            move_to_home();
        }
        else {
            send_unicode_char(ch);
        }

        Sleep(20 + rand() % 20); // 适当加快速度，减少 IDE 反应时间
    }
}

void up_ctrl_shift() {
    INPUT inputs[2] = {}; // 原来这里开了4个空间的数组，但只发了2个，这里修正为2

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
    // 设置控制台支持 UTF-8 / 宽字符本地化，防止读取中文乱码
    std::wcin.imbue(std::locale(""));
    std::wcout.imbue(std::locale(""));

    std::wcout << L"=== 智能模拟键盘输入程序 ===" << std::endl;
    std::wcout << L"请粘贴/输入你需要发送的文本（完成后在新的一行按 Ctrl+Z 键，然后回车结束输入）：" << std::endl;
    std::wcout << L"--------------------------------------------------" << std::endl;

    // 循环读取控制台多行输入
    std::wstring line;
    while (std::getline(std::wcin, line)) {
        g_input_text += line + L"\n";
    }

    // 移除最后多出来的一个换行符（如果不需要的话）
    if (!g_input_text.empty() && g_input_text.back() == L'\n') {
        g_input_text.pop_back();
    }

    std::wcout << L"--------------------------------------------------" << std::endl;
    std::wcout << L"文本读取成功！" << std::endl;
    std::wcout << L"正在隐藏控制台，请切换到你的 IDE 中..." << std::endl;
    std::wcout << L"按下 Ctrl + Shift + V 将开始模拟输入。" << std::endl;

    Sleep(1500); // 留出时间让用户看清提示

    // 获取并隐藏控制台
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_MINIMIZE); // 最小化
    // 如果想彻底看不见，也可以用 ShowWindow(hwnd, SW_HIDE);

    // 注册热键 Ctrl + Shift + V
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
                // 直接使用从控制台获取好的全局文本
                send_unicode_string(g_input_text);

                // 输入完成后直接退出程序，防止常驻后台。如果想多次使用，可以把 return 0 删掉
                break;
            }
        }
    }

    UnregisterHotKey(nullptr, 1);
    return 0;
}