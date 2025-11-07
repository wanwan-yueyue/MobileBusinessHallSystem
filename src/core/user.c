/*
 * 文件名称：user.c
 * 文件路径：.\MobileBusinessHallSystem\user.c
 * 功能描述：用户管理模块实现文件 - 实现用户数据的存储、检索、修改和删除等核心数据操作
 *          提供用户信息的纯数据操作接口，不包含任何界面交互逻辑
 * 作    者：
 * 创建日期：2025-10-29
 * 版本信息：v3.0（纯数据操作版本）
 * 版权声明：© 2025 | 保留所有权利
 * 
 * 实现说明：
 * 1. 实现用户数据的增删改查等核心数据操作
 * 2. 提供用户信息的排序和检索功能
 * 3. 通过全局变量管理用户数据存储
 * 4. 移除所有界面交互逻辑，专注于数据操作
 * 
 * 依赖说明：
 * - 标准库：string.h
 * - 自定义模块：user.h、global.h
 * 
 * 修改记录：
 * 2025-10-29  新增文件，实现基础用户管理功能（v1.0）
 * 2025-10-30  扩展功能，支持用户状态管理（v2.0）
 * 2025-10-30  优化数据结构，移除手机号数组（v2.1）
 * 2025-11-1   重构为纯数据操作版本（v3.0）
 */

#include "user.h"
#include "global.h"
#include <string.h>

// ========== 用户数据操作函数实现 ==========

/**
 * @brief 新增用户
 * @param user 指向用户数据的指针
 * @retval int 成功返回用户索引，失败返回-1
 * 
 * 实现细节：
 * - 验证输入参数的有效性
 * - 检查系统用户数量限制
 * - 在用户数组中查找第一个空闲位置
 * - 复制用户数据并设置状态为活跃
 * - 更新用户计数并返回用户索引
 */
int addUser(const User* user) {
    if (user == NULL) {
        return -1;
    }

    // 检查系统用户数量限制
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

/**
 * @brief 注销用户
 * @param userIndex 用户索引
 * @retval int 成功返回1，失败返回0
 * 
 * 实现细节：
 * - 验证用户索引的有效性
 * - 检查用户当前状态
 * - 将用户状态设置为已注销
 * - 减少活跃用户计数
 */
int deleteUser(int userIndex) {
    if (userIndex < 0 || userIndex >= MAX_USERS) {
        return 0;
    }

    // 检查用户当前状态
    if (users[userIndex].status != USER_ACTIVE) {
        return 0;
    }

    // 设置用户状态为已注销
    users[userIndex].status = USER_DELETED;
    userCount--;
    return 1;
}

/**
 * @brief 修改用户信息
 * @param userIndex 用户索引
 * @param newUserData 指向新用户数据的指针
 * @retval int 成功返回1，失败返回0
 * 
 * 实现细节：
 * - 验证用户索引和数据的有效性
 * - 检查用户当前状态
 * - 直接替换用户数据结构内容
 * - 保持用户状态不变
 */
int modifyUser(int userIndex, const User* newUserData) {
    if (userIndex < 0 || userIndex >= MAX_USERS || newUserData == NULL) {
        return 0;
    }

    // 检查用户当前状态
    if (users[userIndex].status == USER_INACTIVE) {
        return 0;
    }

    // 直接替换用户数据
    users[userIndex] = *newUserData;
    return 1;
}

// ========== 用户查询函数实现 ==========

/**
 * @brief 通过身份证号查找用户索引
 * @param idCard 身份证号字符串
 * @retval int 找到返回用户索引，未找到返回-1
 * 
 * 实现细节：
 * - 验证输入参数的有效性
 * - 遍历用户数组查找匹配的身份证号
 * - 只搜索状态为活跃的用户
 * - 返回第一个匹配的用户索引
 */
int findUserIndexById(const char* idCard) {
    if (idCard == NULL) {
        return -1;
    }
    
    // 遍历用户数组查找匹配的身份证号
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].status == USER_ACTIVE && strcmp(users[i].idCard, idCard) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief 通过手机号查找用户索引
 * @param phoneNum 手机号字符串
 * @retval int 找到返回用户索引，未找到返回-1
 * 
 * 实现细节：
 * - 验证输入参数的有效性
 * - 获取手机号管理器实例
 * - 在手机号资源中查找绑定的用户ID
 * - 返回对应的用户索引
 */
int findUserIndexByPhone(const char* phoneNum) {
    if (phoneNum == NULL) {
        return -1;
    }
    
    // 获取手机号管理器实例
    PhoneManager* phoneMgr = getPhoneManager();
    if (phoneMgr == NULL) {
        return -1;
    }
    
    // 在手机号资源中查找绑定的用户ID
    for (int i = 0; i < phoneMgr->count; i++) {
        if (phoneMgr->phones[i].status == PHONE_STATUS_ASSIGNED &&
            strcmp(phoneMgr->phones[i].phoneNumber, phoneNum) == 0) {
            return phoneMgr->phones[i].userId;
        }
    }
    return -1;
}

/**
 * @brief 检查身份证号唯一性
 * @param idCard 身份证号字符串
 * @retval bool 唯一返回true，已存在返回false
 * 
 * 实现细节：
 * - 通过身份证号查找功能验证唯一性
 * - 未找到表示身份证号唯一
 * - 找到表示身份证号已存在
 */
bool isIdCardUnique(const char* idCard) {
    return (findUserIndexById(idCard) == -1);
}

/**
 * @brief 获取活跃用户数量
 * @retval int 当前活跃用户数量
 * 
 * 实现细节：
 * - 直接返回全局用户计数
 * - 反映系统中活跃用户的实际数量
 */
int getActiveUserCount() {
    return userCount;
}

/**
 * @brief 通过索引获取用户
 * @param index 用户索引
 * @retval User* 指向用户数据的指针，失败返回NULL
 * 
 * 实现细节：
 * - 验证用户索引的有效性
 * - 检查用户当前状态
 * - 返回用户数据的直接指针
 */
User* getUserByIndex(int index) {
    if (index < 0 || index >= MAX_USERS) {
        return NULL;
    }
    
    // 检查用户当前状态
    if (users[index].status == USER_INACTIVE) {
        return NULL;
    }
    
    return &users[index];
}

/**
 * @brief 获取所有活跃用户
 * @param resultArray 输出参数，存储用户数据的数组
 * @param maxCount 最大获取数量
 * @retval int 实际获取的用户数量
 * 
 * 实现细节：
 * - 验证输入参数的有效性
 * - 遍历用户数组收集活跃用户数据
 * - 限制输出数量防止数组溢出
 * - 返回实际收集的用户数量
 */
int getAllActiveUsers(User* resultArray, int maxCount) {
    if (resultArray == NULL || maxCount <= 0) {
        return 0;
    }
    
    int count = 0;
    // 遍历用户数组收集活跃用户数据
    for (int i = 0; i < MAX_USERS && count < maxCount; i++) {
        if (users[i].status == USER_ACTIVE) {
            resultArray[count++] = users[i];
        }
    }
    return count;
}

// ========== 用户排序函数实现 ==========

/**
 * @brief 按姓名排序用户数组
 * @param usersArray 用户数组
 * @param count 用户数量
 * @retval 无
 * 
 * 实现细节：
 * - 验证输入参数的有效性
 * - 使用冒泡排序算法对姓名进行升序排序
 * - 基于字符串比较确定排序顺序
 * - 直接修改原数组的顺序
 */
void sortUsersByName(User* usersArray, int count) {
    if (usersArray == NULL || count <= 1) {
        return;
    }
    
    // 使用冒泡排序对姓名进行升序排序
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (strcmp(usersArray[j].name, usersArray[j + 1].name) > 0) {
                // 交换用户数据
                User temp = usersArray[j];
                usersArray[j] = usersArray[j + 1];
                usersArray[j + 1] = temp;
            }
        }
    }
}

/**
 * @brief 按年龄排序用户数组
 * @param usersArray 用户数组
 * @param count 用户数量
 * @param ascending 排序方向（true为升序，false为降序）
 * @retval 无
 * 
 * 实现细节：
 * - 验证输入参数的有效性
 * - 使用冒泡排序算法对年龄进行排序
 * - 根据排序方向参数确定比较逻辑
 * - 直接修改原数组的顺序
 */
void sortUsersByAge(User* usersArray, int count, bool ascending) {
    if (usersArray == NULL || count <= 1) {
        return;
    }
    
    // 使用冒泡排序对年龄进行排序
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            // 根据排序方向确定比较逻辑
            bool shouldSwap = ascending ? 
                (usersArray[j].age > usersArray[j + 1].age) : 
                (usersArray[j].age < usersArray[j + 1].age);
                
            if (shouldSwap) {
                // 交换用户数据
                User temp = usersArray[j];
                usersArray[j] = usersArray[j + 1];
                usersArray[j + 1] = temp;
            }
        }
    }
}

/**
 * @brief 按身份证号排序用户数组
 * @param usersArray 用户数组
 * @param count 用户数量
 * @retval 无
 * 
 * 实现细节：
 * - 验证输入参数的有效性
 * - 使用冒泡排序算法对身份证号进行升序排序
 * - 基于字符串比较确定排序顺序
 * - 直接修改原数组的顺序
 */
void sortUsersByIdCard(User* usersArray, int count) {
    if (usersArray == NULL || count <= 1) {
        return;
    }
    
    // 使用冒泡排序对身份证号进行升序排序
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (strcmp(usersArray[j].idCard, usersArray[j + 1].idCard) > 0) {
                // 交换用户数据
                User temp = usersArray[j];
                usersArray[j] = usersArray[j + 1];
                usersArray[j + 1] = temp;
            }
        }
    }
}
/**
 * @brief 通过身份证号查找用户索引（包括已删除用户）
 * @param idCard 身份证号字符串
 * @retval int 找到返回用户索引，未找到返回-1
 *
 * 实现细节：
 * - 验证输入参数的有效性
 * - 遍历用户数组查找匹配的身份证号
 * - 搜索所有状态的用户（包括已删除）
 * - 返回第一个匹配的用户索引
 */
int findUserIndexByIdIncludeDeleted(const char* idCard) {
    if (idCard == NULL) {
        return -1;
    }

    // 遍历用户数组查找匹配的身份证号（包括已删除用户）
    for (int i = 0; i < MAX_USERS; i++) {
        if ((users[i].status == USER_ACTIVE || users[i].status == USER_DELETED) &&
            strcmp(users[i].idCard, idCard) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief 恢复已删除用户
 * @param userIndex 用户索引
 * @retval int 成功返回1，失败返回0
 *
 * 实现细节：
 * - 验证用户索引的有效性
 * - 检查用户当前状态是否为已删除
 * - 将用户状态设置为活跃
 * - 增加活跃用户计数
 */
int restoreUser(int userIndex) {
    if (userIndex < 0 || userIndex >= MAX_USERS) {
        return 0;
    }

    // 检查用户当前状态
    if (users[userIndex].status != USER_DELETED) {
        return 0;
    }

    // 设置用户状态为活跃
    users[userIndex].status = USER_ACTIVE;
    userCount++;
    return 1;
}