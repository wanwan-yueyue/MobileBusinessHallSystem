/*
 * 文件名称：data.c
 * 文件路径：.\MobileBusinessHallSystem\data.c
 * 功能描述：数据持久化模块 - 重构以支持新的手机号管理器和用户数据结构
 * 作    者：
 * 创建日期：2025-10-29
 * 版本信息：v2.0（重构数据存储，支持独立手机号模块）
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
 */
#include "data.h"
#include "user.h"
#include "phone.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 系统初始化
void initSystem() {
    // 初始化用户数组
    for (int i = 0; i < MAX_USERS; i++) {
        users[i].isActive = 0;          // 标记用户为"未激活"（无效）
        memset(users[i].name, 0, NAME_LEN);       // 清空姓名
        memset(users[i].gender, 0, GENDER_LEN);   // 清空性别
        memset(users[i].idCard, 0, ID_LEN);       // 清空身份证号
        memset(users[i].job, 0, JOB_LEN);         // 清空职业
        memset(users[i].address, 0, ADDR_LEN);    // 清空地址
    }

    // 初始化手机号管理器
    if(phoneManager == NULL){
        phoneManager = initPhoneManager();
        if(phoneManager == NULL){
            printf("手机号管理器初始化失败！\n");
            exit(1);
        }

        // 初始化一些默认号段
        // initPhoneResources(phoneManager,"138", 50);
        // initPhoneResources(phoneManager,"139", 50);
        // initPhoneResources(phoneManager,"150", 50);
        if(!initPhoneResources(phoneManager,"138", 50)){
            printf("默认号段138初始化失败！\n");
        }
        if(!initPhoneResources(phoneManager,"139", 50)){
            printf("默认号段139初始化失败！\n");
        }
        if(!initPhoneResources(phoneManager,"150", 50)){
            printf("默认号段150初始化失败！\n");
        }

        printf("手机号管理器初始化成功,已加载默认号段！\n");
    }

    userCount = 0; // 初始化用户数量
}

//读取文件内容
int readData() {
    FILE* fp;
    errno_t err = fopen_s(&fp, "userData.dat", "rb");
    if (err != 0 || fp == NULL) {
            printf("无法打开数据文件！\n");
            return 0;
        }
    
    int i = 0;
    // 读取用户数据直到文件末尾
	while (i < MAX_USERS && fread(&users[i], sizeof(User), 1, fp) == 1) {
        if(users[i].isActive) {
            userCount++;
        }
		i++;
	}   

	fclose(fp);
	return userCount;
}

// 创建初始数据文件
int createData() {
    FILE* fp;
    errno_t err = fopen_s(&fp, "userData.dat", "wb");   
    if (err != 0 || fp == NULL) {
        printf("无法创建数据文件！\n");
        return 0;
    }

	printf("输入用户信息。\n");
	int n = readData();     // 读取现有数据
    
    // 将用户数据写入文件
    size_t written = fwrite(users, sizeof(User), n, fp);
    if (written != (size_t)n) {
        printf("数据写入不完整！\n");
        fclose(fp);
        return 0;
    }

    fclose(fp);
	
    // 同时保存手机号资源
    savePhoneResource(phoneManager, "phoneData.dat");

    printf("初始数据文件创建成功，包含 %d 位用户。\n", n);
    return n;
}

// // 加载所有数据
// void loadData() {
//     FILE* fp = fopen("userData.dat", "rb");
//     if (!fp) return;

//     fread(&userCount, sizeof(int), 1, fp);
//     fread(users, sizeof(User), MAX_USERS, fp);

//     // 加载手机号资源
//     if(phoneManager == NULL){
//         phoneManager = initPhoneManager();
//     }
//     loadPhoneResource(phoneManager, "phoneData.dat");

//     fclose(fp);
// }
// 加载所有数据
void loadData() {
    // 加载用户数据
    int loadedUsers = readData();
    if (loadedUsers > 0) {
        printf("成功加载%d个用户数据\n", loadedUsers);
    } else {
        printf("无用户数据或加载失败\n");
    }
    
    // 加载手机号资源
    if (phoneManager == NULL) {
        phoneManager = initPhoneManager();
    }
    
    if (loadPhoneResource(phoneManager, "phoneData.dat")) {
        printf("手机号资源加载成功\n");
    } else {
        printf("手机号资源加载失败或文件不存在，使用默认资源\n");
        // 如果加载失败，重新初始化默认资源
        initPhoneResources(phoneManager, "138", 50);
        initPhoneResources(phoneManager, "139", 50);
        initPhoneResources(phoneManager, "150", 50);
    }
}


// // 保存数据
// void saveData() {
//     FILE* fp = fopen("E:\\userData.dat", "wb");
//     if (!fp) {
//         printf("保存失败，无法打开文件！\n");
//         return;
//     }

//     // 保存用户数量和用户数据
//     fwrite(&userCount, sizeof(int), 1, fp);
// 	fwrite(users, sizeof(User), 1, fp);

//     // 保存手机号资源
//     savePhoneResource(phoneManager, "phoneData.dat");

// 	fclose(fp); 
// }

// 保存所有数据
void saveData() {
    // 保存用户数据
    FILE* fp;
    errno_t err = fopen_s(&fp, "userData.dat", "wb");
    if (err != 0 || fp == NULL) {
        printf("保存失败，无法打开用户数据文件！\n");
        return;
    }
    
    // 统计有效用户数量
    int activeCount = 0;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].isActive) {
            activeCount++;
        }
    }
    
    // 写入有效用户数据
    size_t written = 0;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].isActive) {
            if (fwrite(&users[i], sizeof(User), 1, fp) != 1) {
                printf("用户数据写入失败！\n");
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
            printf("数据保存成功！用户数据：%d条，手机号资源：%d个\n", 
                   activeCount, phoneManager->count);
        } else {
            printf("用户数据保存成功（%d条），但手机号资源保存失败！\n", activeCount);
        }
    } else {
        printf("用户数据保存成功（%d条），但手机号管理器未初始化！\n", activeCount);
    }
}

// // 导出数据到文本文件（新增功能）
// int exportDataToText(const char* userFile, const char* phoneFile) {
//     if (userFile == NULL || phoneFile == NULL) {
//         return 0;
//     }
    
//     // 导出用户数据到文本
//     FILE* userFp;
//     errno_t err = fopen_s(&userFp, userFile, "w");
//     if (err != 0 || userFp == NULL) {
//         printf("无法创建用户数据文本文件！\n");
//         return 0;
//     }
    
//     fprintf(userFp, "姓名,性别,年龄,身份证号,职业,地址,是否有效\n");
//     for (int i = 0; i < MAX_USERS; i++) {
//         if (users[i].isActive) {
//             fprintf(userFp, "%s,%s,%d,%s,%s,%s,%d\n",
//                     users[i].name,
//                     users[i].gender,
//                     users[i].age,
//                     users[i].idCard,
//                     users[i].job,
//                     users[i].address,
//                     users[i].isActive);
//         }
//     }
//     fclose(userFp);
    
//     // 导出手机号资源到文本
//     FILE* phoneFp;
//     err = fopen_s(&phoneFp, phoneFile, "w");
//     if (err != 0 || phoneFp == NULL) {
//         printf("无法创建手机号资源文本文件！\n");
//         return 0;
//     }
    
//     fprintf(phoneFp, "手机号,状态,用户ID,分配时间\n");
//     if (phoneManager != NULL) {
//         for (int i = 0; i < phoneManager->count; i++) {
//             const char* statusStr;
//             switch (phoneManager->phones[i].status) {
//                 case PHONE_STATUS_FREE:
//                     statusStr = "空闲";
//                     break;
//                 case PHONE_STATUS_ASSIGNED:
//                     statusStr = "已分配";
//                     break;
//                 case PHONE_STATUS_INACTIVE:
//                     statusStr = "不活跃";
//                     break;
//                 default:
//                     statusStr = "未知";
//             }
            
//             fprintf(phoneFp, "%s,%s,%d,%s\n",
//                     phoneManager->phones[i].phoneNumber,
//                     statusStr,
//                     phoneManager->phones[i].userId,
//                     phoneManager->phones[i].assignTime);
//         }
//     }
//     fclose(phoneFp);
    
//     printf("数据导出成功！用户数据：%s，手机号资源：%s\n", userFile, phoneFile);
//     return 1;
// }