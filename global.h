/**
 * @file global.h
 * @brief 全局变量管理头文件
 * 
 * 集中管理系统的全局变量，避免在多个模块中重复定义，
 * 提供统一的初始化和访问接口。
 */
#ifndef GLOBAL_H
#define GLOBAL_H

#include "user.h"
#include "phone.h"
#include "menu.h"
#include "data.h"

// 全局变量声明
extern User users[MAX_USERS];                   // 用户数组
extern int userCount;                           // 当前活跃用户数量
extern PhoneManager *phoneManager;              // 手机号指针管理器

// 全局资源函数接口
void initGlobalVariables();                     // 全局变量初始化函数
PhoneManager* getPhoneManager();                // 获取手机号管理器实例
void cleanupGlobalResources();                  // 清理全局资源
void setPhoneManager(PhoneManager* manager);    // 设置手机号管理器
#endif