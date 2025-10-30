/*
 * 文件名称：user.c
 * 文件路径：.\MobileBusinessHallSystem\user.c
 * 功能描述：用户管理模块实现文件 - 重构以适配新的手机号管理模块
 * 作    者：
 * 创建日期：2025-10-29
 * 版本信息：v2.2 （统一输出样式，优化用户交互体验）
 * 版权声明：? 2025 | 保留所有权利
 * 
 * 实现说明：
 * 1. 移除User结构体中的Phone数组，改为通过PhoneManager统一管理手机号；
 * 2. 所有手机号相关操作通过phoneManager全局变量调用phone模块接口；
 * 3. 保持原有用户界面和交互逻辑不变，仅修改底层数据管理方式。
 * 依赖说明：
 * - 标准库：stdio.h、stdlib.h、string.h
 * - 自定义模块：user.h、phone.h、data.h、utils.h
 * 
 * 修改记录：
 * 2025-10-29  重构文件，移除用户结构体中的手机号数组（v2.0）
 * 2025-10-29  新增用户状态管理，身份证号唯一性检查，注销前必须解绑手机号
 * 2025-10-30  优化输入处理，重构以适配新的手机号管理模块，增强用户交互体验（v2.1）
 * 2025-10-30  统一输出样式，使用menu模块的样式函数提升界面一致性（v2.2）
 */

#include "user.h"
#include "phone.h"
#include "data.h"
#include "utils.h"
#include "menu.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <conio.h>

// 全局用户数组定义
User users[MAX_USERS];
int userCount = 0;
PhoneManager *phoneManager = NULL;

// ========== 辅助函数 ==========

/**
 * @brief 安全的输入缓冲区清理
 */
static void safeClearInputBuffer() {
    clearInputBuffer();
}

/**
 * @brief 安全的字符串输入
 */
static void safeStringInput(char* buffer, size_t size, const char* prompt) {
    if (prompt) printf("    %s", prompt);
    fgets(buffer, (int)size, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}

/**
 * @brief 安全的整数输入
 */
static int safeIntInput(const char* prompt, int min, int max) {
    int value;
    while (1) {
        if (prompt) printf("    %s", prompt);
        if (scanf_s("%d", &value) == 1 && value >= min && value <= max) {
            safeClearInputBuffer();
            return value;
        }
        printError("输入错误，请重新输入！");
        safeClearInputBuffer();
    }
}

/**
 * @brief 验证并获取用户索引
 */
static int validateAndGetUserIndex() {
    char idCard[ID_LEN];
    safeStringInput(idCard, sizeof(idCard), "请输入用户身份证号：");
    
    int userIndex = findUserIndexById(idCard);
    if (userIndex == -1) {
        printError("未找到该用户！");
    }
    
    return userIndex;
}

/**
 * @brief 显示用户详细信息
 */
static void displayUserDetails(int userIndex) {
    User* user = &users[userIndex];
    
    printSectionTitle("用户信息");
    
    printInfo("姓名", user->name);
    printInfo("性别", user->gender);
    printInfoInt("年龄", user->age);
    printInfo("身份证号", user->idCard);
    printInfo("职业", user->job);
    printInfo("地址", user->address);
    printf(WHITE "    状态：" RESET "%s\n", user->status == USER_ACTIVE ? "活跃" : "已注销");

    // 使用phoneManager获取并显示手机号
    printf(WHITE "    手机号：" RESET);
    if (phoneManager != NULL) {
        char userPhones[MAX_PHONE_PER_USER][MAX_PHONE_LENGTH];
        int phoneCount = 0;
        getUserPhones(phoneManager, userIndex, userPhones, &phoneCount);
        
        if (phoneCount > 0) {
            for (int i = 0; i < phoneCount; i++) {
                printf("%s  ", userPhones[i]);
            }
        } else {
            printf("无");
        }
    } else {
        printf("手机号系统未初始化");
    }
    printf("\n");
    
    printSeparator();
}

// ========== 核心功能函数 ==========

void addUser() {
    clearScreen();
    printSectionTitle("新增用户");

    if (userCount >= MAX_USERS) {
        printError("系统用户数量已达上限，无法新增！");
        system("pause");
        return;
    }

    // 找到第一个空闲位置
    int index = -1;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].status == USER_INACTIVE) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        printError("系统错误，无法添加用户！");
        system("pause");
        return;
    }

    User* newUser = &users[index];
    newUser->status = USER_ACTIVE;

    // 输入基本信息
    // safeClearInputBuffer();
    safeStringInput(newUser->name, sizeof(newUser->name), "请输入姓名(不超过19字): ");
    
    // 输入性别
    while (1) {
        safeStringInput(newUser->gender, sizeof(newUser->gender), "请输入性别（男/女/其他）：");
        if (strcmp(newUser->gender, "男") == 0 || 
            strcmp(newUser->gender, "女") == 0 || 
            strcmp(newUser->gender, "其他") == 0) {
            break;
        }
        printError("性别输入错误，请重新输入！");
    }

    // 输入年龄
    safeClearInputBuffer();
    newUser->age = safeIntInput("请输入年龄(1-120): ", 1, 120);

    // 输入身份证号
    while (1) {
        safeStringInput(newUser->idCard, sizeof(newUser->idCard), "请输入18位身份证号: ");
        
        if (isValidIdCard(newUser->idCard)) {
            if (!isIdCardUnique(newUser->idCard)) {         // 先取消检查唯一性，以供调试
                break;
            } else {
                printError("该身份证号已被其他用户使用，请重新输入！");
            }
        } else {
            printError("身份证号格式错误，请重新输入！");
        }
    }

    // 输入职业和地址
    safeClearInputBuffer();
    safeStringInput(newUser->job, sizeof(newUser->job), "请输入职业(不超过29字): ");
    safeStringInput(newUser->address, sizeof(newUser->address), "请输入地址(不超过49字): ");

    userCount++;

    // 询问是否立即注册手机号
    int registerChoice = 0;
    registerChoice = safeIntInput("用户信息添加成功！是否立即注册手机号？(1-是/0-否): ", 0, 1);
    if (registerChoice == 1) {
        registerPhoneForUser(index);
    }
    else {
        printInfo("提示", "您可以稍后通过主菜单注册手机号。");
        saveData();             // 在 registerPhoneForUser() 函数内部成功注册手机号后已经调用了 saveData()
    }
         
    printSuccess("操作完成！");

    // 到此处可以直接返回菜单，就不用暂停程序了

    // system("pause");
    // printf(YELLOW "\n    按任意键继续...\n" RESET);
    // _getch();
}

void searchUser() {
    clearScreen();
    printSectionTitle("查找用户");
    
    printf("    1. 按身份证号查找\n");
    printf("    2. 按手机号查找\n");
    printf("    3. 按姓名查找\n");
    
    int choice = safeIntInput("请选择查找方式：", 1, 3);
    int userIndex = -1;

    switch (choice) {
        case 1: {
            char idCard[ID_LEN];
            safeStringInput(idCard, sizeof(idCard), "请输入身份证号：");
            userIndex = findUserIndexById(idCard);
            break;
        }
        case 2: {
            char phoneNum[12];
            safeStringInput(phoneNum, sizeof(phoneNum), "请输入手机号：");
            userIndex = findUserIndexByPhone(phoneNum);
            break;
        }
        case 3: {
            char name[NAME_LEN];
            safeStringInput(name, sizeof(name), "请输入姓名：");

            // 收集所有同名用户
            int foundIndices[100];
            int count = 0;
            for (int i = 0; i < MAX_USERS; i++) {
                if (users[i].status == USER_ACTIVE && strcmp(users[i].name, name) == 0) {
                    foundIndices[count++] = i;
                }
            }

            if (count == 0) {
                printError("未找到该用户！");
                system("pause");
                return;
            } else if (count > 1) {
                char idCard[ID_LEN];
                safeStringInput(idCard, sizeof(idCard), "找到多个同名用户，请补充身份证号:\n");
                userIndex = findUserIndexById(idCard);
            } else {
                userIndex = foundIndices[0];
            }
            break;
        }
    }

    if (userIndex != -1) {
        displayUserDetails(userIndex);
    } else {
        printError("未找到该用户！");
    }

    system("pause");
}

void modifyUser() {
    clearScreen();
    printSectionTitle("修改用户信息");
    
    int userIndex = validateAndGetUserIndex();
    if (userIndex == -1) {
        system("pause");
        return;
    }

    User* user = &users[userIndex];
    printf("\n    当前信息:\n");
    printf("    1. 姓名：%s\n", user->name);
    printf("    2. 性别：%s\n", user->gender);
    printf("    3. 年龄：%d\n", user->age);
    printf("    4. 职业：%s\n", user->job);
    printf("    5. 地址：%s\n", user->address);
    
    int choice = safeIntInput("请选择要修改的项目(1-5): ", 1, 5);

    switch (choice) {
        case 1:
            safeStringInput(user->name, sizeof(user->name), "请输入新姓名：");
            break;
        case 2:
            while (1) {
                safeStringInput(user->gender, sizeof(user->gender), "请输入新性别（男/女/其他）：");
                if (strcmp(user->gender, "男") == 0 || 
                    strcmp(user->gender, "女") == 0 || 
                    strcmp(user->gender, "其他") == 0) {
                    break;
                }
                printError("性别输入错误，请重新输入！");
            }
            break;
        case 3:
            user->age = safeIntInput("请输入新年龄（1-120）：", 1, 120);
            break;
        case 4:
            safeStringInput(user->job, sizeof(user->job), "请输入新职业：");
            break;
        case 5:
            safeStringInput(user->address, sizeof(user->address), "请输入新地址：");
            break;
    }

    saveData();
    printSuccess("信息修改成功！");
    system("pause");
}

void deleteUser() {
    clearScreen();
    printSectionTitle("注销用户");
    
    int userIndex = validateAndGetUserIndex();
    if (userIndex == -1) {
        system("pause");
        return;
    }

    // 检查用户状态
    if (users[userIndex].status == USER_INACTIVE) {
        printError("该用户已经是注销状态！");
        system("pause");
        return;
    }

    // 检查是否有未注销的手机号
    if (phoneManager != NULL && getUserPhoneCount(phoneManager, userIndex) > 0) {
        printError("该用户仍有关联手机号，请先注销所有手机号！");
        system("pause");
        return;
    }

    // 设置为注销状态
    users[userIndex].status = USER_INACTIVE;
    userCount--;
    saveData();
    printSuccess("用户注销成功！");
    system("pause");
}

void showAllUsers() {
    clearScreen();
    printSectionTitle("所有用户信息");

    if (userCount == 0) {
        printError("当前无用户信息！");
        system("pause");
        return;
    }

    printf("    共%d位用户:\n\n", userCount);
    int index = 1;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].status == USER_ACTIVE) {
            User* user = &users[i];
            printf(WHITE "    用户%d:\n" RESET, index++);
            printInfo("姓名", user->name);
            printInfo("性别", user->gender);
            printInfoInt("年龄", user->age);
            printInfo("身份证号", user->idCard);
            printInfo("职业", user->job);
            printInfo("地址", user->address);
            printf(WHITE "    状态：" RESET "%s\n", user->status == USER_ACTIVE ? "活跃" : "已注销");

            // 显示手机号
            printf(WHITE "    手机号：" RESET);
            if (phoneManager != NULL) {
                char userPhones[MAX_PHONE_PER_USER][MAX_PHONE_LENGTH];
                int phoneCount = 0;
                getUserPhones(phoneManager, i, userPhones, &phoneCount);

                if (phoneCount > 0) {
                    for (int j = 0; j < phoneCount; j++) {
                        printf("%s  ", userPhones[j]);
                    }
                } else {
                    printf("无");
                }
            } else {
                printf("手机号系统未初始化");
            }
            printf("\n\n");
        }
    }

    if (safeIntInput("是否需要排序显示？(1-是/0-否): ", 0, 1) == 1) {
        sortUsers();
    }
    
    system("pause");
}

void sortUsers() {
    if (userCount <= 1) return;

    // 复制有效用户索引到数组
    int userIndices[100];
    int count = 0;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].status == USER_ACTIVE) {
            userIndices[count++] = i;
        }
    }

    printf("\n    1. 按姓名排序\n");
    printf("    2. 按年龄升序\n");
    printf("    3. 按年龄降序\n");
    printf("    4. 按身份证号排序\n");
    
    int choice = safeIntInput("请选择排序方式：", 1, 4);

    // 冒泡排序
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            int a = userIndices[j];
            int b = userIndices[j + 1];
            int swap = 0;

            switch (choice) {
                case 1:
                    if (strcmp(users[a].name, users[b].name) > 0) swap = 1;
                    break;
                case 2:
                    if (users[a].age > users[b].age) swap = 1;
                    break;
                case 3:
                    if (users[a].age < users[b].age) swap = 1;
                    break;
                case 4:
                    if (strcmp(users[a].idCard, users[b].idCard) > 0) swap = 1;
                    break;
            }

            if (swap) {
                int temp = userIndices[j];
                userIndices[j] = userIndices[j + 1];
                userIndices[j + 1] = temp;
            }
        }
    }

    // 显示排序结果
    clearScreen();
    printSectionTitle("排序后用户信息");
    for (int i = 0; i < count; i++) {
        int idx = userIndices[i];
        printf(WHITE "    用户%d:\n" RESET, i + 1);
        printInfo("姓名", users[idx].name);
        printInfo("性别", users[idx].gender);
        printInfoInt("年龄", users[idx].age);
        printInfo("身份证号", users[idx].idCard);
        printf("\n");
    }
}

int findUserIndexById(const char* idCard) {
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].status == USER_ACTIVE && strcmp(users[i].idCard, idCard) == 0) {
            return i;
        }
    }
    return -1;
}

int findUserIndexByPhone(const char* phoneNum) {
    if (phoneManager == NULL) {
        return -1;
    }
    
    for (int i = 0; i < phoneManager->count; i++) {
        if (phoneManager->phones[i].status == PHONE_STATUS_ASSIGNED &&
            strcmp(phoneManager->phones[i].phoneNumber, phoneNum) == 0) {
            return phoneManager->phones[i].userId;
        }
    }
    return -1;
}

bool isIdCardUnique(const char* idCard) {
    return (findUserIndexById(idCard) == -1);
}

void unregisterPhone() {
    clearScreen();
    printSectionTitle("注销手机号");
    
    if (phoneManager == NULL) {
        printError("手机号管理系统未初始化！");
        system("pause");
        return;
    }

    int userIndex = validateAndGetUserIndex();
    if (userIndex == -1) {
        system("pause");
        return;
    }

    // 获取用户当前手机号
    char userPhones[MAX_PHONE_PER_USER][MAX_PHONE_LENGTH];
    int phoneCount = 0;
    getUserPhones(phoneManager, userIndex, userPhones, &phoneCount);

    if (phoneCount == 0) {
        printError("该用户没有绑定的手机号！");
        system("pause");
        return;
    }

    printf("    该用户当前绑定的手机号：\n");
    for (int i = 0; i < phoneCount; i++) {
        printf("    %d. %s\n", i + 1, userPhones[i]);
    }

    if (phoneCount == 1) {
        // 只有一个手机号，直接注销
        if (cancelPhone(phoneManager, userIndex, userPhones[0])) {
            printf(GREEN "    ✓ 手机号 %s 注销成功！\n" RESET, userPhones[0]);
            saveData();
        } else {
            printError("手机号注销失败！");
        }
    } else {
        // 多个手机号，让用户选择
        int choice = safeIntInput("请选择要注销的手机号编号: ", 1, phoneCount);
        if (cancelPhone(phoneManager, userIndex, userPhones[choice - 1])) {
            printf(GREEN "    ✓ 手机号 %s 注销成功！\n" RESET, userPhones[choice - 1]);
            saveData();
        } else {
            printError("手机号注销失败！");
        }
    }

    system("pause");
}

void registerPhoneForUser(int userId) {
    clearScreen();
    printSectionTitle("注册手机号");
    
    if (phoneManager == NULL) {
        printError("手机号管理系统未初始化！");
        system("pause");
        return;
    }

    // 如果userId为-1，需要先选择用户
    if (userId == -1) {
        userId = validateAndGetUserIndex();
        if (userId == -1) {
            system("pause");
            return;
        }
    }

    // 检查用户手机号数量限制
    if (getUserPhoneCount(phoneManager, userId) >= MAX_PHONE_PER_USER) {
        printError("该用户已达最大手机号数量，无法继续注册！");
        system("pause");
        return;
    }

    char phoneNum[MAX_PHONE_LENGTH];
    printf("\n    1. 手动输入手机号\n");
    printf("    2. 从号段随机选号\n");
    
    int choice = safeIntInput("请选择：", 1, 2);

    if (choice == 1) {
        // 手动输入手机号
        while (1) {
            safeStringInput(phoneNum, sizeof(phoneNum), "请输入11位手机号：");
            
            if (isValidPhoneNumber(phoneNum)) {
                if (isPhoneUnique(phoneManager, phoneNum)) {
                    break;
                } else {
                    printError("该手机号已被使用，请重新输入！");
                }
            } else {
                printError("手机号格式错误，请重新输入！");
            }
        }
    } else if (choice == 2) {
        // 随机选择可用号码
        if (selectRandomPhone(phoneManager, phoneNum) != 1) {
            printError("没有可用的手机号，请手动输入！");
            system("pause");
            return;
        }
        printf("    随机分配的手机号：%s\n", phoneNum);
    }

    // 使用phoneManager注册手机号
    if (registerPhone(phoneManager, userId, phoneNum)) {
        printSuccess("手机号注册成功！");
        saveData();
    } else {
        printError("手机号注册失败！");
    }
    system("pause");
}