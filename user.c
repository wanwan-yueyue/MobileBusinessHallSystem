/*
 * 文件名称：user.c
 * 文件路径：.\MobileBusinessHallSystem\user.c
 * 功能描述：用户管理模块实现文件 - 纯数据操作，移除界面逻辑
 * 作    者：
 * 创建日期：2025-10-29
 * 版本信息：v3.0 (纯数据操作版本)
 * 版权声明：? 2025 | 保留所有权利
 * 
 * 实现说明：
 * 1. 1. 移除所有界面交互逻辑，专注于数据操作
 * 2. 提供清晰的数据操作接口
 * 3. 通过全局变量管理用户数据。
 * 依赖说明：
 * - 标准库：stdio.h、stdlib.h、string.h
 * - 自定义模块：user.h、phone.h、data.h、utils.h
 * 
 * 修改记录：
 * 2025-10-29  重构文件，移除用户结构体中的手机号数组（v2.0）
 * 2025-10-29  新增用户状态管理，身份证号唯一性检查，注销前必须解绑手机号
 * 2025-10-30  优化输入处理，重构以适配新的手机号管理模块，增强用户交互体验（v2.1）
 * 2025-10-30  统一输出样式，使用menu模块的样式函数提升界面一致性（v2.2）
 * 2025-10-30  修改用户唯一性检测逻辑错误，同时增添从身份证中自动识别性别和年龄的功能
 * 2025-11-1   纯数据操作版本
 */

#include "user.h"
#include "global.h"
#include <string.h>
#include <stdio.h>
// #include <stdlib.h>
// #include <stdbool.h>

int addUser(const User* user) {
    if (user == NULL) {
        return -1;
    }

    if (userCount >= MAX_USERS) {
        return -1;
    }

    // 找到第一个空闲位置
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].status == USER_INACTIVE) {
            users[i] = *user;
            users[i].status = USER_ACTIVE;
            userCount++;
            return i; // 返回用户索引
        }
    }
    
    return -1;
}

int deleteUser(int userIndex) {
    if (userIndex < 0 || userIndex >= MAX_USERS) {
        return 0;
    }

    if (users[userIndex].status == USER_INACTIVE) {
        return 0;
    }

    users[userIndex].status = USER_INACTIVE;
    userCount--;
    return 1;
}

int modifyUser(int userIndex, const User* newUserData) {
    if (userIndex < 0 || userIndex >= MAX_USERS || newUserData == NULL) {
        return 0;
    }

    if (users[userIndex].status == USER_INACTIVE) {
        return 0;
    }

    users[userIndex] = *newUserData;
    return 1;
}

int findUserIndexById(const char* idCard) {
    if (idCard == NULL) {
        return -1;
    }
    
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].status == USER_ACTIVE && strcmp(users[i].idCard, idCard) == 0) {
            return i;
        }
    }
    return -1;
}

int findUserIndexByPhone(const char* phoneNum) {
    if (phoneNum == NULL) {
        return -1;
    }
    
    PhoneManager* phoneMgr = getPhoneManager();
    if (phoneMgr == NULL) {
        return -1;
    }
    
    for (int i = 0; i < phoneMgr->count; i++) {
        if (phoneMgr->phones[i].status == PHONE_STATUS_ASSIGNED &&
            strcmp(phoneMgr->phones[i].phoneNumber, phoneNum) == 0) {
            return phoneMgr->phones[i].userId;
        }
    }
    return -1;
}

bool isIdCardUnique(const char* idCard) {
    return (findUserIndexById(idCard) == -1);
}

int getActiveUserCount() {
    return userCount;
}

User* getUserByIndex(int index) {
    if (index < 0 || index >= MAX_USERS) {
        return NULL;
    }
    
    if (users[index].status == USER_INACTIVE) {
        return NULL;
    }
    
    return &users[index];
}

int getAllActiveUsers(User* resultArray, int maxCount) {
    if (resultArray == NULL || maxCount <= 0) {
        return 0;
    }
    
    int count = 0;
    for (int i = 0; i < MAX_USERS && count < maxCount; i++) {
        if (users[i].status == USER_ACTIVE) {
            resultArray[count++] = users[i];
        }
    }
    return count;
}

void sortUsersByName(User* usersArray, int count) {
    if (usersArray == NULL || count <= 1) {
        return;
    }
    
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (strcmp(usersArray[j].name, usersArray[j + 1].name) > 0) {
                User temp = usersArray[j];
                usersArray[j] = usersArray[j + 1];
                usersArray[j + 1] = temp;
            }
        }
    }
}

void sortUsersByAge(User* usersArray, int count, bool ascending) {
    if (usersArray == NULL || count <= 1) {
        return;
    }
    
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            bool shouldSwap = ascending ? 
                (usersArray[j].age > usersArray[j + 1].age) : 
                (usersArray[j].age < usersArray[j + 1].age);
                
            if (shouldSwap) {
                User temp = usersArray[j];
                usersArray[j] = usersArray[j + 1];
                usersArray[j + 1] = temp;
            }
        }
    }
}

void sortUsersByIdCard(User* usersArray, int count) {
    if (usersArray == NULL || count <= 1) {
        return;
    }
    
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (strcmp(usersArray[j].idCard, usersArray[j + 1].idCard) > 0) {
                User temp = usersArray[j];
                usersArray[j] = usersArray[j + 1];
                usersArray[j + 1] = temp;
            }
        }
    }
}