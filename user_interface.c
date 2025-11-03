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
        if (scanf("%d", &value) == 1 && value >= min && value <= max) {
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
            }
            else {
                printError("该身份证号已被其他用户使用，请重新输入！");
            }
        }
        else {
            printError("身份证号格式错误，请重新输入！");
        }
    }

    // 自动从身份证号码中获取性别和年龄
    const char* gender = getGenderFromIDCard(newUser.idCard);
    if (strcmp(gender, "未知") == 0) {
        printError("无法从身份证号码中识别性别，请手动输入！");
        // 手动输入性别
        const char* genderOptions[] = {"男", "女", "其他"};
        int genderChoice = makeSelection(genderOptions, 3, "选择性别");
        if (genderChoice == 0) strcpy(newUser.gender, "男");
        else if (genderChoice == 1) strcpy(newUser.gender, "女");
        else strcpy(newUser.gender, "其他");
    }
    else {
        strcpy(newUser.gender, gender);
        printf(GREEN "    ✓ 自动识别性别: %s\n" RESET, gender);
    }

    // 获取年龄
    int age = calculateAgeFromIDCard(newUser.idCard);
    if (age == -1) {
        printError("无法从身份证号计算年龄，请手动输入！");
        safeClearInputBuffer();
        newUser.age = safeIntInput("请输入年龄(1-120): ", 1, 120);
    }
    else {
        newUser.age = age;
        printf(GREEN "    ✓ 自动计算年龄: %d\n" RESET, age);
    }

    // 自动从身份证获取省份信息
    const char* autoProvince = getProvinceFromIDCard(newUser.idCard);
    if (strcmp(autoProvince, "未知") != 0) {
        printf(GREEN "    ✓ 自动识别省份: %s\n" RESET, autoProvince);
    }

    // 输入职业和地址
    safeClearInputBuffer();
    safeStringInput(newUser.job, sizeof(newUser.job), "请输入职业(不超过29字): ");
    safeStringInput(newUser.address, sizeof(newUser.address), "请输入详细地址(不超过49字): ");

    newUser.status = USER_ACTIVE;

    int userIndex = addUser(&newUser);
    if (userIndex >= 0) {
        printSuccess("用户添加成功！");

        // 使用箭头键选择是否立即注册手机号
        const char* registerOptions[] = {"立即注册手机号", "稍后注册"};
        int registerChoice = makeSelection(registerOptions, 2, "是否立即注册手机号？");
        if (registerChoice == 0) {
            registerPhoneForUserInterface(userIndex);
        }
        else {
            saveData();
        }
    }
    else {
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

    // 显示自动识别的省份信息
    const char* autoProvince = getProvinceFromIDCard(user->idCard);
    if (strcmp(autoProvince, "未知") != 0) {
        printf(MAGENTA "    户籍省份：" RESET "%s\n", autoProvince);
    }

    printInfo("详细地址", user->address);
    printInfo("职业", user->job);
    printf(WHITE "    状态：" RESET "%s\n", user->status == USER_ACTIVE ? "活跃" : "已注销");

    // 显示手机号
    printf(WHITE "    手机号：" RESET);
    PhoneManager* phoneMgr = getPhoneManager();
    if (phoneMgr != NULL) {
        char userPhones[MAX_PHONE_PER_USER][MAX_PHONE_LENGTH];
        int phoneCount = 0;
        getUserPhones(phoneMgr, userIndex, userPhones, &phoneCount);

        if (phoneCount > 0) {
            for (int i = 0; i < phoneCount; i++) {
                printf("%s  ", userPhones[i]);
            }
        }
        else {
            printf("无");
        }
    }
    else {
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

        // 显示自动识别的省份信息
        const char* autoProvince = getProvinceFromIDCard(user->idCard);
        if (strcmp(autoProvince, "未知") != 0) {
            printf(MAGENTA "    户籍省份：" RESET "%s\n", autoProvince);
        }

        printInfo("职业", user->job);
        printInfo("地址", user->address);
        printf(WHITE "    状态：" RESET "%s\n", user->status == USER_ACTIVE ? "活跃" : "已注销");

        // 显示手机号
        printf(WHITE "    手机号：" RESET);
        PhoneManager* phoneMgr = getPhoneManager();
        if (phoneMgr != NULL) {
            char userPhones[MAX_PHONE_PER_USER][MAX_PHONE_LENGTH];
            int phoneCount = 0;
            int userIndex = findUserIndexById(user->idCard);
            getUserPhones(phoneMgr, userIndex, userPhones, &phoneCount);

            if (phoneCount > 0) {
                for (int j = 0; j < phoneCount; j++) {
                    printf("%s  ", userPhones[j]);
                }
            }
            else {
                printf("无");
            }
        }
        else {
            printf("手机号系统未初始化");
        }
        printf("\n\n");
    }

    // 排序选择使用箭头键
    const char* sortOptions[] = {
        "按姓名排序",
        "按年龄升序",
        "按年龄降序", 
        "按身份证号排序",
        "不排序，返回"
    };

    int sortChoice = makeSelection(sortOptions, 5, "选择排序方式");
    if (sortChoice >= 0 && sortChoice < 4) {
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

            // 显示自动识别的省份信息
            const char* autoProvince = getProvinceFromIDCard(user->idCard);
            if (strcmp(autoProvince, "未知") != 0) {
                printf(MAGENTA "    户籍省份：" RESET "%s\n", autoProvince);
            }
            printf("\n");
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
    getUserPhones(getPhoneManager(), userIndex, userPhones, &phoneCount);

    if (phoneCount == 0) {
        printError("该用户没有绑定的手机号！");
        waitForAnyKey();
        return;
    }

    // 创建手机号选项数组
    char* phoneOptions[MAX_PHONE_PER_USER + 1];
    for (int i = 0; i < phoneCount; i++) {
        phoneOptions[i] = (char*)malloc(MAX_PHONE_LENGTH + 10);
        snprintf(phoneOptions[i], MAX_PHONE_LENGTH + 10, "%s", userPhones[i]);
    }
    phoneOptions[phoneCount] = "返回上一级";

    int choice = makeSelection((const char**)phoneOptions, phoneCount + 1, "选择要注销的手机号");
    
    // 释放内存
    for (int i = 0; i < phoneCount; i++) {
        free(phoneOptions[i]);
    }

    if (choice == phoneCount) {
        return; // 选择返回
    }

    if (choice >= 0 && choice < phoneCount) {
        if (cancelPhone(getPhoneManager(), userIndex, userPhones[choice])) {
            printf(GREEN "    ✓ 手机号 %s 注销成功！\n" RESET, userPhones[choice]);
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
    if (getUserPhoneCount(getPhoneManager(), userId) >= MAX_PHONE_PER_USER) {
        printError("该用户已达最大手机号数量，无法继续注册！");
        waitForAnyKey();
        return;
    }

    const char* phoneOptions[] = {
        "手动输入手机号",
        "从号段随机选号",
        "返回上一级"
    };
    
    int choice = makeSelection(phoneOptions, 3, "选择手机号获取方式");
    if (choice == 2) return;

    char phoneNum[MAX_PHONE_LENGTH] = {0};

    if (choice == 0) {
        // 手动输入手机号
        while (1) {
            safeStringInput(phoneNum, sizeof(phoneNum), "请输入11位手机号：");
            
            if (isValidPhoneNumber(phoneNum)) {
                if (isPhoneUnique(getPhoneManager(), phoneNum)) {
                    break;
                } else {
                    printError("该手机号已被使用，请重新输入！");
                }
            } else {
                printError("手机号格式错误，请重新输入！");
            }
        }
    } else if (choice == 1) {
        // 从号段选择手机号 - 使用箭头键选择
        char selectedCategory[3] = {0};
        char selectedSegment[4] = {0};
        int refreshCount = 0;
        char availablePhones[10][MAX_PHONE_LENGTH];

        // 主循环控制
        int selectionComplete = 0;
        while (!selectionComplete) {
            // 选择号段分类
            int categorySelected = 0;
            while (!categorySelected && !selectionComplete) {
                clearScreen();
                printSectionTitle("选择号段分类");

                // 获取所有号段分类
                char categories[10][MAX_SEGMENT_LENGTH];
                int categoryCount = getSegmentCategories(getPhoneManager(), categories, 10);

                if (categoryCount == 0) {
                    printError("没有可用的号段分类！");
                    waitForAnyKey();
                    return;
                }

                // 创建分类选项数组
                char* categoryOptions[11]; // 最多10个分类 + 返回
                for (int i = 0; i < categoryCount; i++) {
                    categoryOptions[i] = (char*)malloc(20);
                    const char* categoryName = "";
                    if (strcmp(categories[i], "13") == 0) categoryName = "13x 系列";
                    else if (strcmp(categories[i], "14") == 0) categoryName = "14x 系列";
                    else if (strcmp(categories[i], "15") == 0) categoryName = "15x 系列";
                    else if (strcmp(categories[i], "16") == 0) categoryName = "16x 系列";
                    else if (strcmp(categories[i], "17") == 0) categoryName = "17x 系列";
                    else if (strcmp(categories[i], "18") == 0) categoryName = "18x 系列";
                    else if (strcmp(categories[i], "19") == 0) categoryName = "19x 系列";
                    else categoryName = "其他号段";
                    
                    snprintf(categoryOptions[i], 20, "%s (%s)", categories[i], categoryName);
                }
                categoryOptions[categoryCount] = "返回上一级";

                int categoryChoice = makeSelection((const char**)categoryOptions, categoryCount + 1, "选择号段分类");
                
                // 释放内存
                for (int i = 0; i < categoryCount; i++) {
                    free(categoryOptions[i]);
                }

                if (categoryChoice == categoryCount) {
                    return; // 返回上一级
                } else if (categoryChoice >= 0 && categoryChoice < categoryCount) {
                    strcpy(selectedCategory, categories[categoryChoice]);
                    categorySelected = 1;
                }
            }

            // 选择具体号段
            int segmentSelected = 0;
            while (!segmentSelected && !selectionComplete && categorySelected) {
                clearScreen();
                printSectionTitle("选择具体号段");

                // 获取该分类下的具体号段
                char segments[20][MAX_SEGMENT_LENGTH];
                int segmentCount = getSegmentsByCategory(getPhoneManager(), selectedCategory, segments, 20);

                if (segmentCount == 0) {
                    printError("该分类下没有可用的具体号段！");
                    waitForAnyKey();
                    return;
                }

                // 创建号段选项数组
                char* segmentOptions[21]; // 最多20个号段 + 返回
                for (int i = 0; i < segmentCount; i++) {
                    segmentOptions[i] = (char*)malloc(30);
                    int segmentPhoneCount = getAvailablePhoneCountBySegment(getPhoneManager(), segments[i]);
                    snprintf(segmentOptions[i], 30, "%sxxx (可用：%d个)", segments[i], segmentPhoneCount);
                }
                segmentOptions[segmentCount] = "返回分类选择";

                int segmentChoice = makeSelection((const char**)segmentOptions, segmentCount + 1, "选择具体号段");
                
                // 释放内存
                for (int i = 0; i < segmentCount; i++) {
                    free(segmentOptions[i]);
                }

                if (segmentChoice == segmentCount) {
                    // 返回分类选择 - 跳出当前循环，重新选择分类
                    segmentSelected = 0;
                    categorySelected = 0;
                    break;
                } else if (segmentChoice >= 0 && segmentChoice < segmentCount) {
                    strcpy(selectedSegment, segments[segmentChoice]);
                    segmentSelected = 1;
                }
            }

            // 如果选择了号段，进入手机号选择
            if (segmentSelected && !selectionComplete) {
                int phoneSelected = 0;
                while (!phoneSelected && !selectionComplete) {
                    clearScreen();
                    printSectionTitle("选择手机号码");

                    // 获取该号段下的可用手机号
                    int phoneCount = getAvailablePhonesBySegment(getPhoneManager(), selectedSegment, availablePhones, 10);

                    if (phoneCount == 0) {
                        printError("该号段没有可用的手机号！");
                        waitForAnyKey();
                        // 返回号段选择
                        segmentSelected = 0;
                        break;
                    }

                    // 对手机号进行排序
                    sortPhoneNumbers(availablePhones, phoneCount);

                    printf("    号段：%s | 刷新次数：%d\n\n", selectedSegment, refreshCount);
                    printf("    为您推荐以下手机号（已按顺序排列）：\n\n");

                    // 显示手机号选项（使用数字序号）
                    for (int i = 0; i < phoneCount; i++) {
                        const char* tailNumber = availablePhones[i] + 7;
                        
                        // 根据尾号特征显示提示
                        const char* tag = "";
                        if (strcmp(tailNumber, "8888") == 0 || strcmp(tailNumber, "6666") == 0) {
                            tag = " " RED "【超级靓号】" RESET;
                        } else if (strcmp(tailNumber, "888") == 0 || strcmp(tailNumber, "666") == 0) {
                            tag = " " YELLOW "【靓号】" RESET;
                        } else if (tailNumber[0] == tailNumber[1] && tailNumber[1] == tailNumber[2]) {
                            tag = " " GREEN "【三连号】" RESET;
                        }
                        
                        printf("    %2d. %s%s\n", i + 1, availablePhones[i], tag);
                    }

                    // 分隔线
                    printf("\n");
                    printSeparator();
                    printf("\n");

                    // 操作选项（使用字母按键）
                    printf("    " YELLOW "操作选项：" RESET "\n");
                    printf("    " CYAN "R" RESET " - 刷新号码列表\n");
                    printf("    " CYAN "B" RESET " - 返回号段选择\n");
                    printf("    " CYAN "C" RESET " - 返回分类选择\n");
                    printf("    " CYAN "ESC" RESET " - 返回主菜单\n");

                    printf("\n");
                    printLeftColor("请选择：输入数字选择手机号，或按字母键执行操作", CYAN);

                    // 处理用户输入
                    int key = _getch();
                    if (key == 27) { // ESC键
                        return; // 返回主菜单
                    } else if (key == 'r' || key == 'R') {
                        // 刷新号码列表
                        refreshCount++;
                        continue;
                    } else if (key == 'b' || key == 'B') {
                        // 返回号段选择 - 跳出当前循环，重新选择号段
                        phoneSelected = 0;
                        segmentSelected = 0;
                        break;
                    } else if (key == 'c' || key == 'C') {
                        // 返回分类选择 - 跳出所有循环，重新选择分类
                        phoneSelected = 0;
                        segmentSelected = 0;
                        categorySelected = 0;
                        break;
                    } else if (key >= '1' && key <= '9') {
                        // 数字键选择手机号
                        int selection = key - '0';
                        if (selection >= 1 && selection <= phoneCount) {
                            strcpy(phoneNum, availablePhones[selection - 1]);
                            phoneSelected = 1;
                            selectionComplete = 1;
                        } else {
                            printError("无效的选择，请重新输入！");
                            waitForAnyKey();
                        }
                    } else if (key == '0') {
                        // 处理10号选项
                        if (phoneCount >= 10) {
                            strcpy(phoneNum, availablePhones[9]);
                            phoneSelected = 1;
                            selectionComplete = 1;
                        } else {
                            printError("无效的选择，请重新输入！");
                            waitForAnyKey();
                        }
                    } else {
                        printError("无效的输入，请重新选择！");
                        waitForAnyKey();
                    }
                }
            }
        }
    }

    // 使用phoneManager注册手机号
    if (registerPhone(getPhoneManager(), userId, phoneNum)) {
        printSuccess("手机号注册成功！");
        saveData();
    } else {
        printError("手机号注册失败！");
    }
    waitForAnyKey();
}