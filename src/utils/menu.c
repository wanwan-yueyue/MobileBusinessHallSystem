/*
 * 文件名称：menu.c
 * 文件路径：.\MobileBusinessHallSystem\menu.c
 * 功能描述：菜单界面模块实现文件 - 实现系统界面显示和用户交互功能
 *          提供统一的界面样式规范和交互式菜单系统
 * 作    者：
 * 创建日期：2025-10-29
 * 版本信息：v2.0（统一界面样式）
 * 版权声明：© 2025 | 保留所有权利
 * 
 * 实现说明：
 * 1. 实现丰富的界面显示元素，包括颜色文本、分隔线、菜单项等
 * 2. 提供状态提示和信息显示的统一接口
 * 3. 实现箭头键导航的交互式菜单系统
 * 4. 协调各个功能模块的界面调用
 * 
 * 依赖说明：
 * - 标准库：stdio.h、stdlib.h、conio.h、windows.h、string.h
 * - 自定义模块：menu.h、user_interface.h、phone.h、data.h、global.h、utils.h
 * 
 * 修改记录：
 * 2025-10-29  新增文件，实现基础菜单功能（v1.0）
 * 2025-10-30  扩展界面元素，增加交互功能（v1.1）
 * 2025-10-30  统一界面样式，优化用户体验（v1.2）
 * 2025-11-1   重构代码结构，完善功能实现（v2.0）
 */

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

// ========== 界面显示函数实现 ==========

/**
 * @brief 清空屏幕内容
 * @retval 无
 * 
 * 实现细节：
 * - 使用system("cls")命令清除控制台内容
 * - 适用于Windows平台，其他平台需要调整
 * - 为新的界面显示准备干净状态
 */
void clearScreen() {
    system("cls");
}

/**
 * @brief 左侧对齐打印普通文本
 * @param text 要显示的文本内容
 * @retval 无
 * 
 * 实现细节：
 * - 在文本前添加4空格缩进
 * - 使用默认的白色文字颜色
 * - 自动换行，保持文本格式整洁
 */
void printLeft(const char* text) {
    printf("    %s\n", text);
}

/**
 * @brief 左侧对齐打印彩色文本
 * @param text 要显示的文本内容
 * @param color 颜色控制码
 * @retval 无
 * 
 * 实现细节：
 * - 应用指定的颜色样式
 * - 在文本前添加4空格缩进
 * - 显示后自动重置颜色样式避免影响后续输出
 */
void printLeftColor(const char* text, const char* color) {
    printf("%s    %s\n" RESET, color, text);
}

/**
 * @brief 打印分隔线
 * @retval 无
 * 
 * 实现细节：
 * - 使用Unicode制表符创建视觉分隔线
 * - 应用青色样式增强视觉效果
 * - 长度适配标准控制台宽度
 */
void printSeparator() {
    printf(CYAN "    ───────────────────────────────────────────────────" RESET "\n");
}

/**
 * @brief 打印区块标题
 * @param title 区块标题文字
 * @retval 无
 * 
 * 实现细节：
 * - 先输出空行增加上间距
 * - 显示上下分隔线包围标题
 * - 标题使用青色突出显示
 * - 最后输出空行增加下间距
 */
void printSectionTitle(const char* title) {
    printf("\n");
    printSeparator();
    printLeftColor(title, CYAN);
    printSeparator();
    printf("\n");
}

/**
 * @brief 打印操作标题
 * @param title 操作标题文字
 * @retval 无
 * 
 * 实现细节：
 * - 直接调用printSectionTitle函数
 * - 提供语义化的函数接口
 * - 用于标识具体操作上下文
 */
void printOperationTitle(const char* title) {
    printSectionTitle(title);
}

/**
 * @brief 打印菜单项
 * @param index 菜单项索引编号
 * @param text 菜单项显示文字
 * @param isSelected 是否为当前选中项
 * @retval 无
 * 
 * 实现细节：
 * - 选中项使用高亮背景和箭头图标
 * - 未选中项使用普通白色样式
 * - 统一编号格式，保持界面整齐
 */
void printMenuItem(int index, const char* text, int isSelected) {
    if (isSelected) {
        printf(HIGHLIGHT "    ➤ %d. %s" RESET "\n", index + 1, text);
    } else {
        printf(WHITE "      %d. %s" RESET "\n", index + 1, text);
    }
}

// ========== 状态提示函数实现 ==========

/**
 * @brief 打印成功信息
 * @param message 成功信息内容
 * @retval 无
 * 
 * 实现细节：
 * - 使用绿色文字和勾号图标
 * - 添加固定缩进保持对齐
 * - 自动换行，不影响后续输出
 */
void printSuccess(const char* message) {
    printf(GREEN "    ✓ %s\n" RESET, message);
}

/**
 * @brief 打印错误信息
 * @param message 错误信息内容
 * @retval 无
 * 
 * 实现细节：
 * - 使用红色文字和叉号图标
 * - 添加固定缩进保持对齐
 * - 引起用户注意的重要提示
 */
void printError(const char* message) {
    printf(RED "    ✗ %s\n" RESET, message);
}

/**
 * @brief 打印警告信息
 * @param message 警告信息内容
 * @retval 无
 * 
 * 实现细节：
 * - 使用黄色文字，无特殊图标
 * - 添加固定缩进保持对齐
 * - 用于需要用户注意的普通提示
 */
void printWarning(const char* message) {
    printf(YELLOW "    %s \n" RESET, message);
}

// ========== 信息显示函数实现 ==========

/**
 * @brief 打印标签-值格式的信息
 * @param label 信息标签文字
 * @param value 信息值内容
 * @retval 无
 * 
 * 实现细节：
 * - 标签使用洋红色突出显示
 * - 自动添加中文冒号分隔符
 * - 值使用默认颜色显示
 */
void printInfo(const char* label, const char* value) {
    printf(MAGENTA "    %s：" RESET "%s\n", label, value);
}

/**
 * @brief 打印整数值信息
 * @param label 信息标签文字
 * @param value 整数值
 * @retval 无
 * 
 * 实现细节：
 * - 标签和值都使用白色显示
 * - 使用%d格式输出整数值
 * - 保持简洁的信息显示风格
 */
void printInfoInt(const char* label, int value) {
    printf(WHITE "    %s：" RESET "%d\n", label, value);
}

/**
 * @brief 打印洋红色整数值信息
 * @param label 信息标签文字
 * @param value 整数值
 * @retval 无
 * 
 * 实现细节：
 * - 标签使用洋红色突出显示
 * - 值使用默认颜色弱化显示
 * - 适用于需要强调标签的场合
 */
void printInfoIntMagenta(const char* label, int value){
    printf(MAGENTA "    %s：" RESET "%d\n", label, value);
}

// ========== 菜单系统函数实现 ==========

/**
 * @brief 显示系统启动标题和加载动画
 * @retval 无
 * 
 * 实现细节：
 * - 清空屏幕显示干净启动界面
 * - 显示系统名称和装饰线条
 * - 播放进度条动画模拟加载过程
 * - 添加适当延迟增强视觉效果
 */
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

/**
 * @brief 处理菜单选择操作
 * @param choice 用户选择的菜单项索引
 * @retval 无
 * 
 * 实现细节：
 * - 根据选择索引调用对应的功能接口
 * - 显示操作标题提供上下文信息
 * - 处理退出系统的特殊逻辑
 * - 在操作完成后提示返回主菜单
 */
void handleMenuChoice(int choice) {
    clearScreen();
    
    // 操作标题定义
    const char* operationTitles[] = {
        "【新增用户】",
        "【查找用户】", 
        "【注册手机号】",
        "【注销手机号】",
        "【修改用户信息】",
        "【删除用户】",
        "【显示所有用户】",
        "【回收站】",
        "【退出系统】"
    };
    
    // 显示操作标题
    printOperationTitle(operationTitles[choice]);
    
    // 根据选择调用相应功能
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
		recycleBinInterface();
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
    
    // 非退出操作提供返回提示
    if (choice != 8) {
        printWarning("按任意键返回主菜单...");
        _getch();
    }
}

/**
 * @brief 显示系统主菜单
 * @retval 无
 * 
 * 实现细节：
 * - 定义主菜单选项内容和数量
 * - 使用无限循环保持菜单持续显示
 * - 处理键盘输入实现导航功能
 * - 支持直接退出系统的快捷操作
 */
void showMainMenu() {
    // 菜单项定义
    const char* menuItems[] = {
        "新增用户",
        "查找用户", 
        "注册手机号",
        "注销手机号",
        "修改用户信息",
        "删除用户",
        "显示所有用户",
        "回收站",
        "退出系统"
    };
    
    const int menuCount = sizeof(menuItems) / sizeof(menuItems[0]);
    int choice = 0;  // 当前选中项索引

    // 主菜单循环
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


/**
 * @brief 回收站管理界面
 * @retval 无
 */
void recycleBinInterface() {
    clearScreen();
    printSectionTitle("回收站管理");

    // 统计已删除用户数量
    int deletedCount = 0;
    User deletedUsers[MAX_USERS];

    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].status == USER_DELETED) {
            deletedUsers[deletedCount++] = users[i];
        }
    }

    if (deletedCount == 0) {
        printWarning("回收站为空，没有已删除的用户！");
        waitForAnyKey();
        return;
    }

    printf("    回收站中共有 %d 个已删除用户\n\n", deletedCount);

    // 显示已删除用户列表
    for (int i = 0; i < deletedCount; i++) {
        User* user = &deletedUsers[i];
        printf(WHITE "    %d. " RESET, i + 1);
        printf("%s (身份证: %s, 年龄: %d, 性别: %s)\n",
            user->name, user->idCard, user->age, user->gender);
    }

    printf("\n");
    printSeparator();

    // 操作选项
    const char* recycleOptions[] = {
        "恢复用户",
        "永久删除用户",
        "清空回收站",
        "返回主菜单"
    };

    int choice = makeSelection(recycleOptions, 4, "选择操作");

    switch (choice) {
    case 0: { // 恢复用户
        if (deletedCount == 0) {
            printError("没有可恢复的用户！");
            break;
        }

        // 选择要恢复的用户
        char* userOptions[MAX_USERS + 1];
        for (int i = 0; i < deletedCount; i++) {
            User* user = &deletedUsers[i];
            userOptions[i] = (char*)malloc(100);
            snprintf(userOptions[i], 100, "%s (身份证: %s)", user->name, user->idCard);
        }
        userOptions[deletedCount] = "取消";

        int userChoice = makeSelection((const char**)userOptions, deletedCount + 1, "选择要恢复的用户");

        // 释放内存
        for (int i = 0; i < deletedCount; i++) {
            free(userOptions[i]);
        }

        if (userChoice >= 0 && userChoice < deletedCount) {
            // 使用新的函数查找用户索引（包括已删除用户）
            int originalIndex = findUserIndexByIdIncludeDeleted(deletedUsers[userChoice].idCard);
            if (originalIndex != -1) {
                // 使用新的恢复函数
                if (restoreUser(originalIndex)) {
                    saveData(); // 立即保存数据
                    printSuccess("用户恢复成功！");
                }
                else {
                    printError("用户恢复失败！");
                }
            }
            else {
                printError("找不到对应的用户！");
            }
        }
        break;
    }

    case 1: { // 永久删除用户
        if (deletedCount == 0) {
            printError("没有可永久删除的用户！");
            break;
        }

        // 选择要永久删除的用户
        char* userOptions[MAX_USERS + 1];
        for (int i = 0; i < deletedCount; i++) {
            User* user = &deletedUsers[i];
            userOptions[i] = (char*)malloc(100);
            snprintf(userOptions[i], 100, "%s (身份证: %s)", user->name, user->idCard);
        }
        userOptions[deletedCount] = "取消";

        int userChoice = makeSelection((const char**)userOptions, deletedCount + 1, "选择要永久删除的用户");

        // 释放内存
        for (int i = 0; i < deletedCount; i++) {
            free(userOptions[i]);
        }

        if (userChoice >= 0 && userChoice < deletedCount) {
            // 确认删除
            const char* confirmOptions[] = { "确认永久删除", "取消" };
            int confirm = makeSelection(confirmOptions, 2, "确认要永久删除该用户吗？此操作不可撤销！");

            if (confirm == 0) {
                // 使用新的函数查找用户索引（包括已删除用户）
                int originalIndex = findUserIndexByIdIncludeDeleted(deletedUsers[userChoice].idCard);
                if (originalIndex != -1) {
                    users[originalIndex].status = USER_INACTIVE;
                    // 清空用户数据
                    memset(users[originalIndex].name, 0, NAME_LEN);
                    memset(users[originalIndex].gender, 0, GENDER_LEN);
                    users[originalIndex].age = 0;
                    memset(users[originalIndex].idCard, 0, ID_LEN);
                    memset(users[originalIndex].job, 0, JOB_LEN);
                    memset(users[originalIndex].address, 0, ADDR_LEN);

                    saveData(); // 立即保存数据
                    printSuccess("用户已永久删除！");
                }
                else {
                    printError("找不到对应的用户！");
                }
            }
        }
        break;
    }

    case 2: { // 清空回收站
        if (deletedCount == 0) {
            printError("回收站已为空！");
            break;
        }

        // 确认清空
        const char* confirmOptions[] = { "确认清空回收站", "取消" };
        int confirm = makeSelection(confirmOptions, 2, "确认要清空回收站吗？此操作不可撤销！");

        if (confirm == 0) {
            int clearedCount = 0;
            for (int i = 0; i < MAX_USERS; i++) {
                if (users[i].status == USER_DELETED) {
                    users[i].status = USER_INACTIVE;
                    // 清空用户数据
                    memset(users[i].name, 0, NAME_LEN);
                    memset(users[i].gender, 0, GENDER_LEN);
                    users[i].age = 0;
                    memset(users[i].idCard, 0, ID_LEN);
                    memset(users[i].job, 0, JOB_LEN);
                    memset(users[i].address, 0, ADDR_LEN);
                    clearedCount++;
                }
            }
            saveData(); // 立即保存数据
            printf(GREEN "    ✓ 回收站已清空，共永久删除 %d 个用户\n" RESET, clearedCount);
        }
        break;
    }

    case 3: // 返回主菜单
        return;
    }

    waitForAnyKey();
};


/**
 * @brief 创建选择界面
 * @param options 选项文字数组
 * @param count 选项数量
 * @param title 选择界面标题
 * @retval int 用户选择的选项索引，取消返回-1
 * 
 * 实现细节：
 * - 显示可滚动的选项列表供用户选择
 * - 支持箭头键导航和Enter键确认
 * - 提供ESC键取消选择功能
 * - 返回用户最终选择的选项索引
 */
int makeSelection(const char *options[], int count, const char *title) {
    int choice = 0;
    
    // 选择循环
    while (1) {
        clearScreen();
        
        // 显示标题
        if (title) {
            printSectionTitle(title);
        }
        
        // 显示选项
        for (int i = 0; i < count; i++) {
            printMenuItem(i, options[i], i == choice);
        }
        
        // 底部提示
        printf("\n");
        printLeftColor("使用 ↑↓ 键选择，Enter 键确认", GRAY);
        printLeftColor("ESC 键返回上一级", GRAY);
        
        // 键盘输入处理
        int key = _getch();
        if (key == 224) { // 特殊键
            key = _getch();
            if (key == 72) { // ↑ 键
                choice = (choice - 1 + count) % count;
            } else if (key == 80) { // ↓ 键
                choice = (choice + 1) % count;
            }
        } else if (key == 13) { // Enter 键
            return choice;
        } else if (key == 27) { // ESC 键返回
            return -1;
        }
    }
}