/*
 * 文件名称：user.c
 * 文件路径：.\MobileBusinessHallSystem\user.c
 * 功能描述：用户管理模块实现文件 - 重构以适配新的手机号管理模块
 * 作    者：
 * 创建日期：2025-10-29
 * 版本信息：v2.0（重构用户管理，集成独立手机号模块）
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
 */
#include "user.h"
#include "phone.h"
#include "data.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// 全局用户数组定义
User users[MAX_USERS];
int userCount = 0;
PhoneManager *phoneManager = NULL; // 全局手机号管理器指针

// 新增用户
void addUser()
{
    system("cls");
    printf("======= 新增用户 =======\n");

    if (userCount >= MAX_USERS)
    {
        printf("系统用户数量已达上限，无法新增！\n");
        system("pause");
        return;
    }

    // 找到第一个空闲位置
    int index = -1;
    for (int i = 0; i < MAX_USERS; i++)
    {
        if (!users[i].isActive)
        {
            index = i;
            break;
        }
    }
    if (index == -1)
    {
        printf("系统错误，无法添加用户！\n");
        system("pause");
        return;
    }

    User* newUser = &users[index];
    newUser->isActive = 1;

    // 输入姓名
    clearInputBuffer();
    printf("请输入姓名(不超过19字):");
    fgets(newUser->name, NAME_LEN, stdin);
    newUser->name[strcspn(newUser->name, "\n")] = '\0';
    
    // 输入性别
    while (1)
    {
        printf("请输入性别（男/女/其他）：");
        fgets(newUser->gender, GENDER_LEN, stdin);
        newUser->gender[strcspn(newUser->gender, "\n")] = '\0';
        if (strcmp(newUser->gender, "男") == 0 ||
            strcmp(newUser->gender, "女") == 0 ||
            strcmp(newUser->gender, "其他") == 0)
        {
            break;
        }
        printf("性别输入错误，请重新输入！\n");
    }

    // 输入年龄
    while (1)
    {
        printf("请输入年龄(1-120):");
        if (scanf_s("%d", &newUser->age) == 1 && newUser->age >= 1 && newUser->age <= 120)
			//scanf_s("%d", &newUser->age) == 1确保为整数
            // 同时满足输入为数字且在范围内
        {
            clearInputBuffer();
            break;
        }
        printf("年龄输入错误，请重新输入！\n");
        clearInputBuffer();
    }

    // 输入身份证号
    while (1)
    {
        printf("请输入18位身份证号:");
        fgets(newUser->idCard, ID_LEN, stdin);
        newUser->idCard[strcspn(newUser->idCard, "\n")] = '\0';

        if (isValidIdCard(newUser->idCard))
        {
			break;
        }
        else
        {
            printf("身份证号格式错误，请重新输入！\n");
        }
    }

    // 输入职业
    printf("请输入职业(不超过29字):");
    fgets(newUser->job, JOB_LEN, stdin);
    newUser->job[strcspn(newUser->job, "\n")] = '\0';

    // 输入地址
    clearInputBuffer();
    printf("请输入地址(不超过49字):");
    fgets(newUser->address, ADDR_LEN, stdin);
    newUser->address[strcspn(newUser->address, "\n")] = '\0';

    userCount++;

    printf("用户信息添加成功！是否立即注册手机号？(1-是/0-否):");
    int choice;
    if (scanf_s("%d", &choice) == 1 && choice == 1)
    {
        clearInputBuffer();
        registerPhoneForUser(index); // 直接为新用户注册手机号
    }
    else
    {
        clearInputBuffer();
    }

    saveData();
    printf("操作完成！\n");
    system("pause");
}

// // 注册手机号给指定用户
// void registerPhoneForUser(int userId)
// {
//     if (phoneManager == NULL)
//     {
//         printf("手机号管理系统未初始化！\n");
//         system("pause");
//         return;
//     }

//     // 检查用户手机号数量限制
//     if (getUserPhoneCount(phoneManager, userId) >= MAX_PHONE_PER_USER)
//     {
//         printf("该用户已达最大手机号数量（%d个），无法继续注册！\n", MAX_PHONE_PER_USER);
//         system("pause");
//         return;
//     }

//     char phoneNum[12];
//     printf("\n1. 手动输入手机号\n");
//     printf("2. 从号段随机选号\n");
//     printf("请选择：");
//     int choice;
//     if (scanf_s("%d", &choice) != 1)
//     {
//         printf("输入错误！\n");
//         clearInputBuffer();
//         system("pause");
//         return;
//     }
//     clearInputBuffer();

//     if (choice == 1)
//     {
//         while (1)
//         {
//             printf("请输入11位手机号：");
//             fgets(phoneNum, 12, stdin);
//             phoneNum[strcspn(phoneNum, "\n")] = '\0';

//             if (isValidPhoneNumber(phoneNum))
//             {
//                 if (isPhoneUnique(phoneManager, phoneNum))
//                 {
//                     break;
//                 }
//                 else
//                 {
//                     printf("该手机号已被使用，请重新输入！\n");
//                 }
//             }
//             else
//             {
//                 printf("手机号格式错误，请重新输入！\n");
//             }
//         }
//     }
//     else if (choice == 2)
//     {
//         // 随机选择可用号码
//         if (selectRandomPhone(phoneManager, phoneNum) != 1)
//         {
//             printf("没有可用的手机号，请手动输入！\n");
//             system("pause");
//             return;
//         }
//         printf("随机分配的手机号：%s\n", phoneNum);
//     }
//     else
//     {
//         printf("无效选择！\n");
//         system("pause");
//         return;
//     }

//     // 使用phoneManager注册手机号
//     if (registerPhone(phoneManager, userId, phoneNum))
//     {
//         printf("手机号注册成功！\n");
//         saveData();
//     }
//     else
//     {
//         printf("手机号注册失败！\n");
//     }
//     system("pause");
// }

// 查找用户
void searchUser()
{
    system("cls");
    clearInputBuffer();                                      // 清空输入缓冲区函数
    printf("======= 查找用户 =======\n");
    printf("1. 按身份证号查找\n");
    printf("2. 按手机号查找\n");
    printf("3. 按姓名查找\n");
    printf("请选择查找方式：");

    int choice;
    if (scanf_s("%d", &choice) != 1)
    {
        printf("输入错误！\n");
        clearInputBuffer();
        system("pause");
        return;
    }
    clearInputBuffer();

    int userIndex = -1;
    char input[50];

    switch (choice)
    {
    case 1:
        printf("请输入身份证号：");
        fgets(input, ID_LEN, stdin);
        input[strcspn(input, "\n")] = '\0';
        userIndex = findUserIndexById(input);
        break;
    case 2:
        printf("请输入手机号：");
        fgets(input, 12, stdin);
        input[strcspn(input, "\n")] = '\0';
        userIndex = findUserIndexByPhone(input);
        break;
    case 3:
    {
        printf("请输入姓名：");
        fgets(input, NAME_LEN, stdin);
        input[strcspn(input, "\n")] = '\0';

        // 收集所有同名用户
        int foundIndices[100];
        int count = 0;
        for (int i = 0; i < MAX_USERS; i++)
        {
            if (users[i].isActive && strcmp(users[i].name, input) == 0)
            {
                foundIndices[count++] = i;
            }
        }

        if (count == 0)
        {
            printf("未找到该用户！\n");
            system("pause");
            return;
        }
        else if (count > 1)
        {
            printf("找到%d个同名用户,请补充身份证号:\n", count);
            fgets(input, ID_LEN, stdin);
            input[strcspn(input, "\n")] = '\0';
            userIndex = findUserIndexById(input);
        }
        else
        {
            userIndex = foundIndices[0];
        }
        break;
    }
    default:
        printf("无效选择！\n");
        system("pause");
        return;
    }

    if (userIndex != -1)
    {
        User* user = &users[userIndex];
        printf("\n======= 用户信息 =======\n");
        printf("姓名：%s\n", user->name);
        printf("性别：%s\n", user->gender);
        printf("年龄：%d\n", user->age);
        printf("身份证号：%s\n", user->idCard);
        printf("职业：%s\n", user->job);
        printf("地址：%s\n", user->address);

        // 使用phoneManager获取并显示手机号
        printf("手机号：");
        char userPhones[ MAX_PHONE_PER_USER][MAX_PHONE_LENGTH];
        int phoneCount = 0;
        getUserPhones(phoneManager, userIndex, userPhones, &phoneCount);
        if (phoneCount > 0)
        {
            for (int i = 0; i < phoneCount; i++)
            {
                printf("%s  ", userPhones[i]);
            }
        }
        else
        {
            printf("无");
        }
        printf("\n========================\n");
    }
    else
    {
        printf("未找到该用户！\n");
    }

    system("pause");
}

// 修改用户信息
void modifyUser()
{
    system("cls");
    clearInputBuffer();                      // 清空输入缓冲区函数
    printf("======= 修改用户信息 =======\n");
    printf("请输入用户身份证号：");
    char idCard[ID_LEN];
    fgets(idCard, ID_LEN, stdin);
    idCard[strcspn(idCard, "\n")] = '\0';

    int userId = findUserIndexById(idCard);
    if (userId == -1)
    {
        printf("未找到该用户！\n");
        system("pause");
        return;
    }

    User* user = &users[userId];
    printf("\n当前信息:\n");
    printf("1. 姓名：%s\n", user->name);
    printf("2. 性别：%s\n", user->gender);
    printf("3. 年龄：%d\n", user->age);
    printf("4. 职业：%s\n", user->job);
    printf("5. 地址：%s\n", user->address);
    printf("请选择要修改的项目(1-5):");

    int choice;
    if (scanf_s("%d", &choice) != 1 || choice < 1 || choice > 5)
    {
        printf("输入错误！\n");
        clearInputBuffer();
        system("pause");
        return;
    }
    clearInputBuffer();

    switch (choice)
    {
    case 1:
        printf("请输入新姓名：");
        fgets(user->name, NAME_LEN, stdin);
        user->name[strcspn(user->name, "\n")] = '\0';
        break;
    case 2:
        while (1)
        {
            printf("请输入新性别（男/女/其他）：");
            fgets(user->gender, GENDER_LEN, stdin);
            user->gender[strcspn(user->gender, "\n")] = '\0';
            if (strcmp(user->gender, "男") == 0 ||
                strcmp(user->gender, "女") == 0 ||
                strcmp(user->gender, "其他") == 0)
            {
                break;
            }
            printf("性别输入错误，请重新输入！\n");
        }
        break;
    case 3:
        while (1)
        {
            printf("请输入新年龄（1-120）：");
            if (scanf_s("%d", &user->age) == 1 && user->age >= 1 && user->age <= 120)
            {
                clearInputBuffer();
                break;
            }
            printf("年龄输入错误，请重新输入！\n");
            clearInputBuffer();
        }
        break;
    case 4:
        printf("请输入新职业：");
        fgets(user->job, JOB_LEN, stdin);
        user->job[strcspn(user->job, "\n")] = '\0';
        break;
    case 5:
        printf("请输入新地址：");
        fgets(user->address, ADDR_LEN, stdin);
        user->address[strcspn(user->address, "\n")] = '\0';
        break;
    }

    saveData();
    printf("信息修改成功！\n");
    system("pause");
}

// 删除用户
void deleteUser()
{
    system("cls");
    clearInputBuffer();                                      // 清空输入缓冲区函数
    printf("======= 删除用户 =======\n");
    printf("请输入要删除的用户身份证号：");
    char idCard[ID_LEN];
    fgets(idCard, ID_LEN, stdin);
    idCard[strcspn(idCard, "\n")] = '\0';

    int userId = findUserIndexById(idCard);
    if (userId == -1)
    {
        printf("未找到该用户！\n");
        system("pause");
        return;
    }

    // 检查是否有未注销的手机号, 使用phoneManager检查
    if (getUserPhoneCount(phoneManager, userId) > 0)
    {
        printf("该用户仍有关联手机号，请先注销所有手机号！\n");
        system("pause");
        return;
    }

    // 标记为无效用户
    users[userId].isActive = 0;
    userCount--;
    saveData();
    printf("用户删除成功！\n");
    system("pause");
}

// 显示所有用户
void showAllUsers()
{
    system("cls");
    printf("======= 所有用户信息 =======\n");

    if (userCount == 0)
    {
        printf("当前无用户信息！\n");
        system("pause");
        return;
    }

    printf("共%d位用户:\n\n", userCount);
    int index = 1;
    for (int i = 0; i < MAX_USERS; i++)
    {
        if (users[i].isActive)
        {
            User* user = &users[i];
            printf("用户%d:\n", index++);
            printf("姓名：%s  性别：%s  年龄：%d\n", user->name, user->gender, user->age);
            printf("身份证号：%s\n", user->idCard);
            printf("职业：%s  地址：%s\n", user->job, user->address);

            printf("手机号：");

            // 使用phoneManager获取并显示手机号
            char userPhones[ MAX_PHONE_PER_USER][MAX_PHONE_LENGTH];
            int phoneCount = 0;
            getUserPhones(phoneManager, i, userPhones, &phoneCount);

            if (phoneCount > 0)
            {
                for (int j = 0; j < phoneCount; j++)
                {
                    printf("%s  ", userPhones[j]);
                }
            }
            else
            {
                printf("无");
            }
            printf("\n\n");
        }
    }

    printf("是否需要排序显示？(1-是/0-否):");
    int choice;
    if (scanf_s("%d", &choice) == 1 && choice == 1)
    {
        sortUsers();
    }
    clearInputBuffer();
    system("pause");
}

// 排序用户
void sortUsers()
{
    if (userCount <= 1)
        return;

    // 复制有效用户索引到数组
    int userIndices[100];
    int count = 0;
    for (int i = 0; i < MAX_USERS; i++)
    {
        if (users[i].isActive)
        {
            userIndices[count++] = i;
        }
    }

    printf("\n1. 按姓名排序\n");
    printf("2. 按年龄升序\n");
    printf("3. 按年龄降序\n");
    printf("4. 按身份证号排序\n");
    printf("请选择排序方式：");

    int choice;
    if (scanf_s("%d", &choice) != 1)
    {
        printf("输入错误！\n");
        return;
    }

    // 冒泡排序
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            int a = userIndices[j];
            int b = userIndices[j + 1];
            int swap = 0;

            switch (choice)
            {
            case 1:
                if (strcmp(users[a].name, users[b].name) > 0)
                    swap = 1;
                break;
            case 2:
                if (users[a].age > users[b].age)
                    swap = 1;
                break;
            case 3:
                if (users[a].age < users[b].age)
                    swap = 1;
                break;
            case 4:
                if (strcmp(users[a].idCard, users[b].idCard) > 0)
                    swap = 1;
                break;
            default:
                printf("无效选择！\n");
                return;
            }

            if (swap)
            {
                int temp = userIndices[j];
                userIndices[j] = userIndices[j + 1];
                userIndices[j + 1] = temp;
            }
        }
    }

    // 显示排序结果
    system("cls");
    printf("======= 排序后用户信息 =======\n");
    for (int i = 0; i < count; i++)
    {
        int idx = userIndices[i];
        User* user = &users[idx];
        printf("用户%d:\n", i + 1);
        printf("姓名：%s  性别：%s  年龄：%d\n", user->name, user->gender, user->age);
        printf("身份证号：%s\n\n", user->idCard);
    }
}

// 通过身份证查找用户索引
int findUserIndexById(const char* idCard)
{
    for (int i = 0; i < MAX_USERS; i++)
    {
        if (users[i].isActive && strcmp(users[i].idCard, idCard) == 0)
        {
            return i; // 返回索引
        }
    }
    return -1; // 未找到
}

// 通过手机号查找用户索引
int findUserIndexByPhone(const char* phoneNum)
{
    if (phoneManager == NULL)
    {
        return -1;
    }
    
    // 遍历所有手机号资源，查找拥有该手机号的用户
    for (int i = 0; i < phoneManager->count; i++)
    {
        if (phoneManager->phones[i].status == PHONE_STATUS_ASSIGNED &&
            strcmp(phoneManager->phones[i].phoneNumber, phoneNum) == 0)
        {
            return phoneManager->phones[i].userId;
        }
    }
    return -1; // 未找到
}

// 检查身份证唯一性
int isIdCardUnique(const char* idCard)
{
    return (findUserIndexById(idCard) == -1);
}

/**
 * @brief 手机号注销功能
 * @retval 无
 */
void unregisterPhone() {
    system("cls");
    printf("======= 注销手机号 =======\n");
    
    if (phoneManager == NULL) {
        printf("手机号管理系统未初始化！\n");
        system("pause");
        return;
    }

    char idCard[ID_LEN];
    printf("请输入用户身份证号：");
    clearInputBuffer();
    fgets(idCard, ID_LEN, stdin);
    idCard[strcspn(idCard, "\n")] = '\0';

    int userId = findUserIndexById(idCard);
    if (userId == -1) {
        printf("未找到该用户！\n");
        system("pause");
        return;
    }

    // 获取用户当前手机号
    char userPhones[MAX_PHONE_PER_USER][MAX_PHONE_LENGTH];
    int phoneCount = 0;
    getUserPhones(phoneManager, userId, userPhones, &phoneCount);

    if (phoneCount == 0) {
        printf("该用户没有绑定的手机号！\n");
        system("pause");
        return;
    }

    printf("该用户当前绑定的手机号：\n");
    for (int i = 0; i < phoneCount; i++) {
        printf("%d. %s\n", i + 1, userPhones[i]);
    }

    if (phoneCount == 1) {
        // 只有一个手机号，直接注销
        if (cancelPhone(phoneManager, userId, userPhones[0])) {
            printf("手机号 %s 注销成功！\n", userPhones[0]);
            saveData();
        } else {
            printf("手机号注销失败！\n");
        }
    } else {
        // 多个手机号，让用户选择
        printf("请选择要注销的手机号编号(1-%d): ", phoneCount);
        int choice;
        if (scanf_s("%d", &choice) == 1 && choice >= 1 && choice <= phoneCount) {
            clearInputBuffer();
            if (cancelPhone(phoneManager, userId, userPhones[choice - 1])) {
                printf("手机号 %s 注销成功！\n", userPhones[choice - 1]);
                saveData();
            } else {
                printf("手机号注销失败！\n");
            }
        } else {
            printf("无效的选择！\n");
            clearInputBuffer();
        }
    }

    system("pause");
}

/**
 * @brief 为指定用户注册手机号（userId为-1时需要先选择用户）
 * @param userId 用户ID，-1表示需要选择用户
 */
void registerPhoneForUser(int userId) {
    system("cls");
    printf("======= 注册手机号 =======\n");
    
    if (phoneManager == NULL) {
        printf("手机号管理系统未初始化！\n");
        system("pause");
        return;
    }

    // 如果userId为-1，需要先选择用户
    if (userId == -1) {
        char idCard[ID_LEN];
        printf("请输入用户身份证号：");
        clearInputBuffer();
        fgets(idCard, ID_LEN, stdin);
        idCard[strcspn(idCard, "\n")] = '\0';

        userId = findUserIndexById(idCard);
        if (userId == -1) {
            printf("未找到该用户！\n");
            system("pause");
            return;
        }
    }

    // 检查用户手机号数量限制
    if (getUserPhoneCount(phoneManager, userId) >= MAX_PHONE_PER_USER) {
        printf("该用户已达最大手机号数量（%d个），无法继续注册！\n", MAX_PHONE_PER_USER);
        system("pause");
        return;
    }

    char phoneNum[MAX_PHONE_LENGTH];
    printf("\n1. 手动输入手机号\n");
    printf("2. 从号段随机选号\n");
    printf("请选择：");
    
    int choice;
    if (scanf_s("%d", &choice) != 1) {
        printf("输入错误！\n");
        clearInputBuffer();
        system("pause");
        return;
    }
    clearInputBuffer();

    if (choice == 1) {
        // 手动输入手机号
        while (1) {
            printf("请输入11位手机号：");
            fgets(phoneNum, MAX_PHONE_LENGTH, stdin);
            phoneNum[strcspn(phoneNum, "\n")] = '\0';

            if (isValidPhoneNumber(phoneNum)) {
                if (isPhoneUnique(phoneManager, phoneNum)) {
                    break;
                } else {
                    printf("该手机号已被使用，请重新输入！\n");
                }
            } else {
                printf("手机号格式错误，请重新输入！\n");
            }
        }
    } else if (choice == 2) {
        // 随机选择可用号码
        if (selectRandomPhone(phoneManager, phoneNum) != 1) {
            printf("没有可用的手机号，请手动输入！\n");
            system("pause");
            return;
        }
        printf("随机分配的手机号：%s\n", phoneNum);
    } else {
        printf("无效选择！\n");
        system("pause");
        return;
    }

    // 使用phoneManager注册手机号
    if (registerPhone(phoneManager, userId, phoneNum)) {
        printf("手机号注册成功！\n");
        saveData();
    } else {
        printf("手机号注册失败！\n");
    }
    system("pause");
}

// 检查身份证唯一性
//int isIdCardUnique(const char* idCard)
//{
//    for (int i = 0; i < MAX_USERS; i++)
//    {
//        if (users[i].isActive && strcmp(users[i].idCard, idCard) == 0)
//        {
//            return 0; // 不唯一
//        }
//    }
//    return 1; // 唯一
//}