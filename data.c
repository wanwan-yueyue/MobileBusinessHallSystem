/*
 * 文件名称：data.c
 * 文件路径：.\MobileBusinessHallSystem\data.c
 * 功能描述：数据持久化模块 - 重构以支持新的手机号管理器和用户数据结构
 * 作    者：
 * 创建日期：2025-10-29
 * 版本信息：v2.1 （修改输出样式以适配统一界面）
 * 版权声明：© 2025  | 保留所有权利
 * 
 * 实现说明：
 * 1. 用户数据和手机号资源分开存储，分别保存到不同的文件；
 * 2. 使用新的PhoneManager结构进行手机号资源的序列化和反序列化；
 * 3. 支持系统初始化和数据恢复的完整流程；
 * 4. 提供数据文件创建和读取的兼容性处理。
 * 依赖说明：
 * - 标准库：stdio.h、stdlib.h、string.h
 * - 自定义模块：user.h、phone.h、data.h
 * 
 * 修改记录：
 * 2025-10-29  重构文件，支持独立手机号管理模块（v2.0）
 * 2025-10-29  适配新的用户状态枚举，修复编译错误
 * 2025-10-29  彻底清空用户数据，修复身份证号重复检查问题
 * 2025-10-30  修改输出样式以适配统一界面（v2.1）
 */

#include "data.h"
#include "user.h"
#include "phone.h"
#include "menu.h" 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 系统初始化
void initSystem() {
    printLeft("系统初始化中...");
    
    // 初始化用户数组
    for (int i = 0; i < MAX_USERS; i++) {
        users[i].status = USER_INACTIVE;          // 标记用户为"未激活"（无效）
        memset(users[i].name, 0, NAME_LEN);       // 清空姓名
        memset(users[i].gender, 0, GENDER_LEN);   // 清空性别
        users[i].age = 0;                         // 清空年龄
        memset(users[i].idCard, 0, ID_LEN);       // 清空身份证号
        memset(users[i].job, 0, JOB_LEN);         // 清空职业
        memset(users[i].address, 0, ADDR_LEN);    // 清空地址
    }

    userCount = 0; // 重置用户数量
    
    // 调试信息
    printSuccess("用户数组初始化完成，所有用户状态设置为USER_INACTIVE");
    
    // 初始化手机号管理器
    if(phoneManager == NULL){
        phoneManager = initPhoneManager();
        if(phoneManager == NULL){
            printError("手机号管理器初始化失败！");
            exit(1);
        }

        // 初始化一些默认号段
        if(!initPhoneResources(phoneManager,"138", 50)){
            printError("默认号段138初始化失败！");
        }
        if(!initPhoneResources(phoneManager,"139", 50)){
            printError("默认号段139初始化失败！");
        }
        if(!initPhoneResources(phoneManager,"150", 50)){
            printError("默认号段150初始化失败！");
        }

        // 调试信息
        printSuccess("手机号管理器初始化成功，已加载默认号段！");
    }
}

//读取文件内容
int readData() {
    FILE* fp;
    errno_t err = fopen_s(&fp, "userData.dat", "rb");
    if (err != 0 || fp == NULL) {
        printError("无法打开用户数据文件！");
        return 0;
    }
    
    int i = 0;
    // 读取用户数据直到文件末尾
    while (i < MAX_USERS && fread(&users[i], sizeof(User), 1, fp) == 1) {
        if(users[i].status == USER_ACTIVE) {
            userCount++;
        }
        i++;
    }   

    fclose(fp);
    
    if (userCount > 0) {
        printf(GREEN "    ✓ 成功加载 %d 个用户数据\n" RESET, userCount);
    } else {
        printWarning("无用户数据或加载失败");
    }
    
    return userCount;
}

// 创建初始数据文件
int createData() {
    FILE* fp;
    errno_t err = fopen_s(&fp, "userData.dat", "wb");   
    if (err != 0 || fp == NULL) {
        printError("无法创建数据文件！");
        return 0;
    }

    printLeft("输入用户信息。");
    int n = readData();     // 读取现有数据
    
    // 将用户数据写入文件
    size_t written = fwrite(users, sizeof(User), n, fp);
    if (written != (size_t)n) {
        printError("数据写入不完整！");
        fclose(fp);
        return 0;
    }

    fclose(fp);
    
    // 同时保存手机号资源
    if (phoneManager != NULL) {
        if (savePhoneResource(phoneManager, "phoneData.dat")) {
            printSuccess("手机号资源保存成功");
        } else {
            printError("手机号资源保存失败");
        }
    }

    printf(GREEN "    ✓ 初始数据文件创建成功，包含 %d 位用户。\n" RESET, n);
    return n;
}

// 加载所有数据
void loadData() {
    printSectionTitle("加载系统数据");
    
    // 加载用户数据
    int loadedUsers = readData();
    
    // 加载手机号资源
    if (phoneManager == NULL) {
        phoneManager = initPhoneManager();
    }
    
    if (phoneManager != NULL) {
        if (loadPhoneResource(phoneManager, "phoneData.dat")) {
            printSuccess("手机号资源加载成功");
        } else {
            printWarning("手机号资源加载失败或文件不存在，使用默认资源");
            
            // 如果加载失败，重新初始化默认资源
            if (initPhoneResources(phoneManager, "138", 50) &&
                initPhoneResources(phoneManager, "139", 50) &&
                initPhoneResources(phoneManager, "150", 50)) {
                printSuccess("默认手机号资源初始化成功");
            } else {
                printError("默认手机号资源初始化失败");
            }
        }
    } else {
        printError("手机号管理器初始化失败！");
    }
    
    printSeparator();
}

// 保存所有数据
void saveData() {
    printSectionTitle("保存系统数据");
    
    // 保存用户数据
    FILE* fp;
    errno_t err = fopen_s(&fp, "userData.dat", "wb");
    if (err != 0 || fp == NULL) {
        printError("保存失败，无法打开用户数据文件！");
        return;
    }
    
    // 统计有效用户数量
    int activeCount = 0;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].status == USER_ACTIVE) {
            activeCount++;
        }
    }
    
    // 写入有效用户数据
    size_t written = 0;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].status == USER_ACTIVE) {
            if (fwrite(&users[i], sizeof(User), 1, fp) != 1) {
                printError("用户数据写入失败！");
                fclose(fp);
                return;
            }
            written++;
        }
    }
    
    fclose(fp);
    
    // 保存手机号资源
    if (phoneManager != NULL) {
        if (savePhoneResource(phoneManager, "phoneData.dat")) {
            printf(GREEN "    ✓ 数据保存成功！用户数据：%d条，手机号资源：%d个\n" RESET, 
                   activeCount, phoneManager->count);
        } else {
            printf(YELLOW "    ! 用户数据保存成功（%d条），但手机号资源保存失败！\n" RESET, activeCount);
        }
    } else {
        printf(YELLOW "    ! 用户数据保存成功（%d条），但手机号管理器未初始化！\n" RESET, activeCount);
    }
    
    printSeparator();
}