#include "menu.h"
#include "data.h"
#include "utils.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>

void setConsoleEncoding() {
    // 设置控制台输出编码为UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

int main() {

    setConsoleEncoding();  // 设置控制台编码为UTF-8

    int choice;

    // 系统初始化
    printf("系统初始化中...\n");
    initSystem();                                           // 初始化系统数据结构
    loadData();                                             // 加载用户数据和手机号资源
    printf("系统初始化完成，欢迎使用移动营业厅管理系统！\n");
    system("pause");                                        // 暂停以便用户查看信息

    while (1) {
        showTitle();         // 显示启动画面
        showMainMenu();    // 显示主菜单

        // 读取用户选择
        scanf_s("%d", &choice);
        if (choice == 0) {
            printf("输入错误，请输入数字！\n");
            clearInputBuffer();
            system("pause");
            continue;
        }
  

        handleMenuChoice(choice);  // 处理选择
    }
}