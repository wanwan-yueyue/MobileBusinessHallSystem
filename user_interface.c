/*
 * 文件名称：user_interface.c
 * 文件路径：.\MobileBusinessHallSystem\user_interface.c
 * 功能描述：用户界面交互模块 - 处理所有用户相关的界面交互
 * 作    者：
 * 创建日期：2025-10-30
 * 版本信息：v1.0
 * 版权声明：? 2025 | 保留所有权利
 */

#include "user_interface.h"
#include "global.h"
#include "menu.h"
#include "utils.h"
#include "data.h"
#include <conio.h>
#include <string.h>
#include <stdlib.h>

// ========== 辅助函数 ==========

void waitForAnyKey() {
    printf(YELLOW "\n    按任意键继续..." RESET);
    _getch();
}

void safeStringInput(char* buffer, size_t size, const char* prompt) {
    if (prompt) printf("    %s", prompt);
    fgets(buffer, (int)size, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}

int safeIntInput(const char* prompt, int min, int max) {
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

void safeClearInputBuffer() {
    clearInputBuffer();
}

int makeSelection(const char *options[], int count, const char *title) {
    int choice = 0;
    
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

// ========== 核心界面功能 ==========

void addUserInterface() {
    clearScreen();
    printSectionTitle("新增用户");

    if (getActiveUserCount() >= MAX_USERS) {
        printError("系统用户数量已达上限，无法新增！");
        waitForAnyKey();
        return;
    }

    User newUser;
    memset(&newUser, 0, sizeof(User));
    
    // 输入基本信息
    safeStringInput(newUser.name, sizeof(newUser.name), "请输入姓名(不超过19字): ");
    
    // 输入身份证号
    while (1) {
        safeStringInput(newUser.idCard, sizeof(newUser.idCard), "请输入18位身份证号: ");
        
        if (isValidIdCard(newUser.idCard)) {
            if (isIdCardUnique(newUser.idCard)) {
                break;
            } else {
                printError("该身份证号已被其他用户使用，请重新输入！");
            }
        } else {
            printError("身份证号格式错误，请重新输入！");
        }
    }

    // 自动从身份证号码中获取性别和年龄
    const char* gender = getGenderFromIDCard(newUser.idCard);
    if(strcmp(gender,"未知") == 0){
        printError("无法从身份证号码中识别性别，请手动输入！");
        // 手动输入性别
        while (1) {
            safeStringInput(newUser.gender, sizeof(newUser.gender), "请输入性别（男/女）：");
            if(strcmp(newUser.gender, "男") == 0 || strcmp(newUser.gender, "女") == 0){
                break;
            }
            printError("性别输入错误，请重新输入（男/女）！");
        }
    } else {
        strcpy_s(newUser.gender, sizeof(newUser.gender), gender);
        printf(GREEN "    ✓ 自动识别性别: %s\n" RESET, gender);
    }

    // 获取年龄
    int age = calculateAgeFromIDCard(newUser.idCard);
    if (age == -1) {
        printError("无法从身份证号计算年龄，请手动输入！");
        safeClearInputBuffer();
        newUser.age = safeIntInput("请输入年龄(1-120): ", 1, 120);
    } else {
        newUser.age = age;
        printf(GREEN "    ✓ 自动计算年龄: %d\n" RESET, age);
    }

    // 输入职业和地址
    safeClearInputBuffer();
    safeStringInput(newUser.job, sizeof(newUser.job), "请输入职业(不超过29字): ");
    safeStringInput(newUser.address, sizeof(newUser.address), "请输入地址(不超过49字): ");

    newUser.status = USER_ACTIVE;

    int userIndex = addUser(&newUser);
    if (userIndex >= 0) {
        printSuccess("用户添加成功！");
        
        int registerChoice = safeIntInput("是否立即注册手机号？(1-是/0-否): ", 0, 1);
        if (registerChoice == 1) {
            registerPhoneForUserInterface(userIndex);
        } else {
            saveData();
        }
    } else {
        printError("用户添加失败！");
    }
}

void searchUserInterface() {
    clearScreen();
    printSectionTitle("查找用户");
    
    const char* searchOptions[] = {
        "按身份证号查找",
        "按手机号查找", 
        "按姓名查找",
        "返回主菜单"
    };

    int choice = makeSelection(searchOptions, 4, "选择查找方式");
    if (choice == 3) return;
    
    int userIndex = -1;
    char input[ID_LEN];

    switch (choice) {
        case 0: { // 身份证号查找
            safeStringInput(input, sizeof(input), "请输入身份证号：");
            userIndex = findUserIndexById(input);
            break;
        }
        case 1: { // 手机号查找
            safeStringInput(input, sizeof(input), "请输入手机号：");
            userIndex = findUserIndexByPhone(input);
            break;
        }
        case 2: { // 姓名查找
            safeStringInput(input, sizeof(input), "请输入姓名：");

            // 收集所有同名用户
            User foundUsers[100];
            int count = getAllActiveUsers(foundUsers, 100);
            int matchingCount = 0;
            int matchingIndices[100];
            
            for (int i = 0; i < count; i++) {
                if (strcmp(foundUsers[i].name, input) == 0) {
                    matchingIndices[matchingCount++] = findUserIndexById(foundUsers[i].idCard);
                }
            }

            if (matchingCount == 0) {
                printError("未找到该用户！");
                waitForAnyKey();
                return;
            } else if (matchingCount > 1) {
                printf("    找到 %d 个同名用户，请选择:\n", matchingCount);
                for (int i = 0; i < matchingCount; i++) {
                    User* user = getUserByIndex(matchingIndices[i]);
                    printf("    %d. %s (身份证: %s)\n", i + 1, user->name, user->idCard);
                }
                int select = safeIntInput("请选择用户编号: ", 1, matchingCount) - 1;
                userIndex = matchingIndices[select];
            } else {
                userIndex = matchingIndices[0];
            }
            break;
        }
    }

    if (userIndex != -1) {
        displayUserDetailsInterface(userIndex);
    } else {
        printError("未找到该用户！");
        waitForAnyKey();
    }
}

void displayUserDetailsInterface(int userIndex) {
    User* user = getUserByIndex(userIndex);
    if (user == NULL) {
        printError("用户不存在！");
        return;
    }
    
    printSectionTitle("用户信息");
    
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
    waitForAnyKey();
}

void modifyUserInterface() {
    clearScreen();
    printSectionTitle("修改用户信息");
    
    char idCard[ID_LEN];
    safeStringInput(idCard, sizeof(idCard), "请输入用户身份证号：");
    
    int userIndex = findUserIndexById(idCard);
    if (userIndex == -1) {
        printError("未找到该用户！");
        waitForAnyKey();
        return;
    }

    User* user = getUserByIndex(userIndex);
    printf("\n    当前信息:\n");
    printf("    1. 姓名：%s\n", user->name);
    printf("    2. 性别：%s\n", user->gender);
    printf("    3. 年龄：%d\n", user->age);
    printf("    4. 职业：%s\n", user->job);
    printf("    5. 地址：%s\n", user->address);
    
    const char* modifyOptions[] = {
        "修改姓名",
        "修改性别", 
        "修改年龄",
        "修改职业", 
        "修改地址"
    };
    const int optionCount = sizeof(modifyOptions) / sizeof(modifyOptions[0]);
    
    int choice = makeSelection(modifyOptions, optionCount, "选择修改项目");
    if (choice == -1) return;

    User newUserData = *user; // 复制原数据

    switch (choice) {
        case 0: // 修改姓名
            safeStringInput(newUserData.name, sizeof(newUserData.name), "请输入新姓名：");
            break;
        case 1: // 修改性别
            while (1) {
                safeStringInput(newUserData.gender, sizeof(newUserData.gender), "请输入新性别（男/女/其他）：");
                if (strcmp(newUserData.gender, "男") == 0 || 
                    strcmp(newUserData.gender, "女") == 0 || 
                    strcmp(newUserData.gender, "其他") == 0) {
                    break;
                }
                printError("性别输入错误，请重新输入！");
            }
            break;
        case 2: // 修改年龄
            newUserData.age = safeIntInput("请输入新年龄（1-120）：", 1, 120);
            break;
        case 3: // 修改职业
            safeStringInput(newUserData.job, sizeof(newUserData.job), "请输入新职业：");
            break;
        case 4: // 修改地址
            safeStringInput(newUserData.address, sizeof(newUserData.address), "请输入新地址：");
            break;
    }

    if (modifyUser(userIndex, &newUserData)) {
        saveData();
        printSuccess("信息修改成功！");
    } else {
        printError("信息修改失败！");
    }
    waitForAnyKey();
}

void deleteUserInterface() {
    clearScreen();
    printSectionTitle("注销用户");
    
    char idCard[ID_LEN];
    safeStringInput(idCard, sizeof(idCard), "请输入用户身份证号：");
    
    int userIndex = findUserIndexById(idCard);
    if (userIndex == -1) {
        printError("未找到该用户！");
        waitForAnyKey();
        return;
    }

    // 检查用户状态
    User* user = getUserByIndex(userIndex);
    if (user == NULL) {
        printError("用户不存在！");
        waitForAnyKey();
        return;
    }

    // 检查是否有未注销的手机号
    if (phoneManager != NULL && getUserPhoneCount(phoneManager, userIndex) > 0) {
        printError("该用户仍有关联手机号，请先注销所有手机号！");
        waitForAnyKey();
        return;
    }

    // 设置为注销状态
    if (deleteUser(userIndex)) {
        saveData();
        printSuccess("用户注销成功！");
    } else {
        printError("用户注销失败！");
    }
    waitForAnyKey();
}

void showAllUsersInterface() {
    clearScreen();
    printSectionTitle("所有用户信息");

    if (getActiveUserCount() == 0) {
        printError("当前无用户信息！");
        waitForAnyKey();
        return;
    }

    printf("    共%d位用户:\n\n", getActiveUserCount());
    User allUsers[MAX_USERS];
    int count = getAllActiveUsers(allUsers, MAX_USERS);
    
    for (int i = 0; i < count; i++) {
        User* user = &allUsers[i];
        printf(WHITE "    用户%d:\n" RESET, i + 1);
        printInfo("姓名", user->name);
        printInfo("性别", user->gender);
        printInfoIntMagenta("年龄", user->age);
        printInfo("身份证号", user->idCard);
        printInfo("职业", user->job);
        printInfo("地址", user->address);
        printf(WHITE "    状态：" RESET "%s\n", user->status == USER_ACTIVE ? "活跃" : "已注销");

        // 显示手机号
        printf(WHITE "    手机号：" RESET);
        if (phoneManager != NULL) {
            char userPhones[MAX_PHONE_PER_USER][MAX_PHONE_LENGTH];
            int phoneCount = 0;
            int userIndex = findUserIndexById(user->idCard);
            getUserPhones(phoneManager, userIndex, userPhones, &phoneCount);

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

    if (safeIntInput("是否需要排序显示？(1-是/0-否): ", 0, 1) == 1) {
        // 排序功能
        const char* sortOptions[] = {
            "按姓名排序",
            "按年龄升序", 
            "按年龄降序",
            "按身份证号排序"
        };
        
        int sortChoice = makeSelection(sortOptions, 4, "选择排序方式");
        if (sortChoice != -1) {
            switch (sortChoice) {
                case 0: sortUsersByName(allUsers, count); break;
                case 1: sortUsersByAge(allUsers, count, true); break;
                case 2: sortUsersByAge(allUsers, count, false); break;
                case 3: sortUsersByIdCard(allUsers, count); break;
            }
            
            // 显示排序结果
            clearScreen();
            printSectionTitle("排序后用户信息");
            for (int i = 0; i < count; i++) {
                User* user = &allUsers[i];
                printf(WHITE "    用户%d:\n" RESET, i + 1);
                printInfo("姓名", user->name);
                printInfo("性别", user->gender);
                printInfoInt("年龄", user->age);
                printInfo("身份证号", user->idCard);
                printf("\n");
            }
        }
    }
    
    waitForAnyKey();
}

void unregisterPhoneInterface() {
    clearScreen();
    printSectionTitle("注销手机号");
    
    if (getPhoneManager() == NULL) {
        printError("手机号管理系统未初始化！");
        waitForAnyKey();
        return;
    }

    char idCard[ID_LEN];
    safeStringInput(idCard, sizeof(idCard), "请输入用户身份证号：");
    
    int userIndex = findUserIndexById(idCard);
    if (userIndex == -1) {
        printError("未找到该用户！");
        waitForAnyKey();
        return;
    }

    // 获取用户当前手机号
    char userPhones[MAX_PHONE_PER_USER][MAX_PHONE_LENGTH];
    int phoneCount = 0;
    getUserPhones(phoneManager, userIndex, userPhones, &phoneCount);

    if (phoneCount == 0) {
        printError("该用户没有绑定的手机号！");
        waitForAnyKey();
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

    waitForAnyKey();
}

void registerPhoneForUserInterface(int userId) {
    clearScreen();
    printSectionTitle("注册手机号");
    
    if (getPhoneManager() == NULL) {
        printError("手机号管理系统未初始化！");
        waitForAnyKey();
        return;
    }

    // 如果userId为-1，需要先选择用户
    if (userId == -1) {
        char idCard[ID_LEN];
        safeStringInput(idCard, sizeof(idCard), "请输入用户身份证号：");
        
        userId = findUserIndexById(idCard);
        if (userId == -1) {
            printError("未找到该用户！");
            waitForAnyKey();
            return;
        }
    }

    // 检查用户手机号数量限制
    if (getUserPhoneCount(phoneManager, userId) >= MAX_PHONE_PER_USER) {
        printError("该用户已达最大手机号数量，无法继续注册！");
        waitForAnyKey();
        return;
    }

    const char* phoneOptions[] = {
        "手动输入手机号",
        "从号段随机选号"
    };
    const int optionCount = sizeof(phoneOptions) / sizeof(phoneOptions[0]);
    
    int choice = makeSelection(phoneOptions, optionCount, "选择手机号获取方式");
    if (choice == -1) return;

    char phoneNum[MAX_PHONE_LENGTH];

    if (choice == 0) {
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
    } else if (choice == 1) {
        // 随机选择可用号码
        if (selectRandomPhone(phoneManager, phoneNum) != 1) {
            printError("没有可用的手机号，请手动输入！");
            waitForAnyKey();
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
    waitForAnyKey();
}