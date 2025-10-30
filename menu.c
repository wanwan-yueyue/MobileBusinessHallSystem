#include "menu.h"
#include "user.h"
#include "phone.h"
#include "data.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <string.h>

// ========== 工具函数 ==========
void clearScreen() {
    system("cls");
}

// 控制台居中打印
void printCenter(const char* text) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns = 80;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        columns = csbi.dwSize.X;
    }
    int padding = (columns - (int)strlen(text)) / 2;
    if (padding < 0) padding = 0;
    printf("%*s%s\n", padding, "", text);
}

// 打印居中的彩色文本
void printCenterColor(const char* text, const char* color) {
    printf("%s", color);
    printCenter(text);
    printf(RESET);
}

// 打印居中的菜单项（左对齐内容）
void printMenuItem(int index, const char* text, int isSelected, int menuWidth) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns = 80;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        columns = csbi.dwSize.X;
    }
    
    int padding = (columns - menuWidth) / 2;
    if (padding < 0) padding = 0;
    
    char buffer[256];
    if (isSelected) {
        // 选中项：带背景色和箭头
        snprintf(buffer, sizeof(buffer), HIGHLIGHT " ➤ %d. %s" RESET, index + 1, text);
    } else {
        // 普通项
        snprintf(buffer, sizeof(buffer), WHITE "   %d. %s" RESET, index + 1, text);
    }
    
    // 左对齐填充
    printf("%*s%s", padding, "", buffer);
    
    // 填充剩余空间使所有行等宽
    int textLength = strlen(text) + 5; // 5 = " x. " 的宽度
    int spaces = menuWidth - textLength;
    for (int i = 0; i < spaces; i++) {
        printf(" ");
    }
    printf("\n");
}

// 打印分隔线
void printSeparator() {
    printCenterColor("───────────────────────────────────────────────────", CYAN);
}

// 打印操作标题
void printOperationTitle(const char* title) {
    printf("\n");
    printSeparator();
    printCenterColor(title, CYAN);
    printSeparator();
    printf("\n");
}

// 动画标题
void showTitle() {
    clearScreen();
    
    // 顶部装饰
    printf("\n\n");
    printCenterColor("═══════════════════════════════════════════════════", CYAN);
    printCenterColor("             移动营业厅管理系统", GREEN);
    printCenterColor("═══════════════════════════════════════════════════", CYAN);
    printf("\n");
    
    // 加载动画
    printCenterColor("正在加载系统，请稍候...", YELLOW);
    printf("\n");
    printf("    ");
    for (int i = 0; i < 20; i++) {
        printf(MAGENTA "■" RESET);
        fflush(stdout);
        Sleep(40);
    }
    printf("\n\n");
    
    Sleep(500);
}

// ========== 菜单功能处理 ==========
void handleMenuChoice(int choice) {
    clearScreen();
    
    const char* operationTitles[] = {
        "【新增用户】",
        "【查找用户】", 
        "【注册手机号】",
        "【注销手机号】",
        "【修改用户信息】",
        "【删除用户】",
        "【显示所有用户】",
        "【保存数据】",
        "【退出系统】"
    };
    
    printOperationTitle(operationTitles[choice]);
    
    switch (choice) {
    case 0:  
        addUser(); 
        break;
    case 1:  
        searchUser(); 
        break;
    case 2:  
        registerPhoneForUser(-1); 
        break;
    case 3:  
        unregisterPhone(); 
        break;
    case 4:  
        modifyUser(); 
        break;
    case 5:  
        deleteUser(); 
        break;
    case 6:  
        showAllUsers(); 
        break;
    case 7:
        saveData();
        printf(GREEN "\n\n    ✓ 数据已保存成功！\n" RESET);
        break;
    case 8:
        saveData();
        printf(RED "\n\n    ❤ 感谢您的使用，再见！\n" RESET);
        Sleep(1500);
        exit(0);
    default:
        printf(RED "\n    ✗ 无效的操作编号，请重新输入！\n" RESET);
        break;
    }
    
    printf(YELLOW "\n    按任意键返回主菜单..." RESET);
    _getch();
}

// ========== 增强的主菜单显示 ==========
void showMainMenu() {
    const char* menuItems[] = {
        "新增用户",
        "查找用户", 
        "注册手机号",
        "注销手机号",
        "修改用户信息",
        "删除用户",
        "显示所有用户",
        "保存数据",
        "退出系统"
    };
    
    const int menuCount = sizeof(menuItems) / sizeof(menuItems[0]);
    int choice = 0;
    const int menuWidth = 35; // 菜单项固定宽度，确保对齐

    while (1) {
        clearScreen();
        
        // 显示系统标题
        printf("\n\n");
        printCenterColor("★ ★ ★ 移动营业厅管理系统 ★ ★ ★", BLUE);
        printf("\n\n");
        
        // 显示菜单项
        for (int i = 0; i < menuCount; i++) {
            printMenuItem(i, menuItems[i], i == choice, menuWidth);
        }
        
        // 底部提示信息
        printf("\n");
        printCenterColor("使用 ↑↓ 键选择，Enter 键确认", GRAY);
        printCenterColor("ESC 键可直接退出系统", GRAY);
        
        // 键盘输入处理
        int key = _getch();
        if (key == 224) { // 特殊键
            key = _getch();
            if (key == 72) { // ↑ 键
                choice = (choice - 1 + menuCount) % menuCount;
            } else if (key == 80) { // ↓ 键
                choice = (choice + 1) % menuCount;
            }
        } else if (key == 13) { // Enter 键
            handleMenuChoice(choice);
        } else if (key == 27) { // ESC 键直接退出
            handleMenuChoice(8);
        }
    }
}