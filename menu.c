#include "menu.h"
#include "user_interface.h"
#include "phone.h"
#include "data.h"
#include "global.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <string.h>

// ========== 统一样式函数实现 ==========
void clearScreen() {
    system("cls");
}

void printLeft(const char* text) {
    printf("    %s\n", text);
}

void printLeftColor(const char* text, const char* color) {
    printf("%s    %s\n" RESET, color, text);
}

void printSeparator() {
    printf(CYAN "    ───────────────────────────────────────────────────" RESET "\n");
}

void printSectionTitle(const char* title) {
    printf("\n");
    printSeparator();
    printLeftColor(title, CYAN);
    printSeparator();
    printf("\n");
}

void printOperationTitle(const char* title) {
    printSectionTitle(title);
}

void printMenuItem(int index, const char* text, int isSelected) {
    if (isSelected) {
        printf(HIGHLIGHT "    ➤ %d. %s" RESET "\n", index + 1, text);
    } else {
        printf(WHITE "      %d. %s" RESET "\n", index + 1, text);
    }
}

void printSuccess(const char* message) {
    printf(GREEN "    ✓ %s\n" RESET, message);
}

void printError(const char* message) {
    printf(RED "    ✗ %s\n" RESET, message);
}

void printWarning(const char* message) {
    printf(YELLOW "    %s \n" RESET, message);
}

void printInfo(const char* label, const char* value) {
    printf(MAGENTA "    %s：" RESET "%s\n", label, value);
}

void printInfoInt(const char* label, int value) {
    printf(WHITE "    %s：" RESET "%d\n", label, value);
}

void printInfoIntMagenta(const char* label, int value){
    printf(MAGENTA "    %s：" RESET "%d\n", label, value);
}

// ========== 菜单功能实现 ==========
void showTitle() {
    clearScreen();
    
    // 顶部装饰
    printf("\n\n");
    printSeparator();
    printLeftColor("移动营业厅管理系统", GREEN);
    printSeparator();
    printf("\n");
    
    // 加载动画
    printLeftColor("正在加载系统，请稍候...", YELLOW);
    printf("    ");
    for (int i = 0; i < 20; i++) {
        printf(MAGENTA "■" RESET);
        fflush(stdout);
        Sleep(40);
    }
    printf("\n\n");
    
    Sleep(500);
}

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
        addUserInterface(); 
        break;
    case 1:  
        searchUserInterface(); 
        break;
    case 2:  
        registerPhoneForUserInterface(-1); 
        break;
    case 3:  
        unregisterPhoneInterface(); 
        break;
    case 4:  
        modifyUserInterface(); 
        break;
    case 5:  
        deleteUserInterface(); 
        break;
    case 6:  
        showAllUsersInterface(); 
        break;
    case 7:
        saveData();
        printSuccess("数据已保存成功！");
        break;
    case 8:
        saveData();
        cleanupGlobalResources();  // 退出前清理资源
        printf(RED "\n    ❤ 感谢您的使用，再见！\n" RESET);
        Sleep(1500);
        exit(0);
    default:
        printError("无效的操作编号，请重新输入！");
        break;
    }
    
    if (choice != 8) {
        printWarning("按任意键返回主菜单...");
        _getch();
    }
}

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

    while (1) {
        clearScreen();
        
        // 显示系统标题
        printf("\n\n");
        printSeparator();
        printLeftColor("★ ★ ★ 移动营业厅管理系统 ★ ★ ★", BLUE);
        printSeparator();
        printf("\n\n");
        
        // 显示菜单项
        for (int i = 0; i < menuCount; i++) {
            printMenuItem(i, menuItems[i], i == choice);
        }
        
        // 底部提示信息
        printf("\n");
        printLeftColor("使用 ↑↓ 键选择，Enter 键确认", GRAY);
        printLeftColor("ESC 键可直接退出系统", GRAY);
        
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