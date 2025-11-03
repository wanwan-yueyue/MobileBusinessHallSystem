#include "global.h"
#include <stdlib.h>
#include <string.h>

// 全局变量定义
User users[MAX_USERS];
int userCount = 0;
PhoneManager *phoneManager = NULL;

/**
 * @brief 初始化全局变量
 * @retval 无
 * 
 * 在系统启动时调用，确保所有全局变量处于正确的初始状态。
 * 包括：
 * - 清空用户数组
 * - 重置用户计数
 * - 初始化手机号管理器
 */
void initGlobalVariables() {
    // 重置用户计数
    userCount = 0;
    
    // 初始化用户数组
    for (int i = 0; i < MAX_USERS; i++) {
        users[i].status = USER_INACTIVE;            // 标记用户为"未激活"（无效）
        memset(users[i].name, 0, NAME_LEN);         // 清空姓名
        memset(users[i].gender, 0, GENDER_LEN);     // 清空性别
        users[i].age = 0;                           // 清空年龄
        memset(users[i].idCard, 0, ID_LEN);         // 清空身份证号码
        memset(users[i].job, 0, JOB_LEN);           // 清空职业
        memset(users[i].address, 0, ADDR_LEN);      // 清空地址
    }
    
    // 手机号管理器在需要时延迟初始化
    phoneManager = NULL;
}

/**
 * @brief 获取手机号管理器实例
 * @retval PhoneManager* 手机号管理器指针
 * 
 * 如果手机号管理器未初始化，会自动初始化并返回实例。
 * 确保在使用手机号功能前管理器已正确初始化。
 */
PhoneManager* getPhoneManager() {
    if (phoneManager == NULL) {
        phoneManager = initPhoneManager();
        if (phoneManager == NULL) {
            // 初始化失败，可以记录错误日志
            return NULL;
        }
    }
    return phoneManager;
}

/**
 * @brief 清理全局资源
 * @retval 无
 * 
 * 在程序退出前调用，释放所有动态分配的内存资源，
 * 防止内存泄漏。
 */
void cleanupGlobalResources() {
    if (phoneManager != NULL) {
        cleanupPhoneManager(phoneManager);
        phoneManager = NULL;
    }
    
    // 重置用户数据
    initGlobalVariables();
}

/**
 * @brief 设置手机号管理器实例
 * @param manager: 新的手机号管理器指针
 * @retval 无
 */
void setPhoneManager(PhoneManager* manager) {
    if (phoneManager != NULL) {
        cleanupPhoneManager(phoneManager);
    }
    phoneManager = manager;
}