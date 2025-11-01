/*
 * 文件名称：main.c
 * 文件路径：.\MobileBusinessHallSystem\main.c
 * 功能描述：程序主入口 - 适配新的全局变量管理
 * 作    者：
 * 创建日期：2025-10-29
 * 版本信息：v2.1（适配全局变量管理）
 * 版权声明：© 2025 | 保留所有权利
 */

#include "menu.h"
#include "data.h"
#include "utils.h"
#include "global.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

/**
 * @brief 设置控制台编码为UTF-8
 */
void setConsoleEncoding() {
    // 设置控制台输出编码为UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

/**
 * @brief 程序主入口
 */
int main() {
    // 设置控制台编码
    setConsoleEncoding();

    // 显示系统启动画面
    showTitle();

    // 系统初始化
    printSectionTitle("系统初始化");
    printLeft("正在初始化系统组件...");
    initSystem();                                           // 初始化系统数据结构
    loadData();                                             // 加载用户数据和手机号资源
    
    printSuccess("系统初始化完成！");
    printLeft("欢迎使用移动营业厅管理系统！");
    
    printf(YELLOW "\n    按任意键继续..." RESET);
    _getch();

    // 进入主菜单循环
    showMainMenu();

    // 程序退出前的清理
    cleanupGlobalResources();

    return 0;
}