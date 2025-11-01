#ifndef _USER_H_
#define _USER_H_

#include <stdio.h>
#include <stdbool.h>
#include "phone.h"
#include "menu.h"                               // 包含样式函数

#define MAX_USERS 200                           // 最大用户数量
#define NAME_LEN 20                             // 姓名最大长度
#define GENDER_LEN 4                            // 性别最大长度
#define ID_LEN 19                               // 身份证号最大长度
#define JOB_LEN 30                              // 职业最大长度
#define ADDR_LEN 50                             // 地址最大长度

// 用户状态枚举
typedef enum {
    USER_ACTIVE = 1,                            // 活跃用户
    USER_INACTIVE = 0                           // 已注销用户
} UserStatus;

// 用户结构体
typedef struct {
    char name[NAME_LEN];                        // 姓名
    char gender[GENDER_LEN];                    // 性别
    int age;                                    // 年龄
    char idCard[ID_LEN];                        // 身份证号
    char job[JOB_LEN];                          // 职业
    char address[ADDR_LEN];                     // 地址
    UserStatus status;                          // 用户状态（ACTIVE/INACTIVE）
} User;

// 用户管理函数声明 - 纯数据操作接口
int addUser(const User* user);                                  // 新增用户
int deleteUser(int userIndex);                                  // 注销用户
int modifyUser(int userIndex, const User* newUserData);         // 修改用户信息
int findUserIndexById(const char* idCard);                      // 通过身份证号查找用户索引
int findUserIndexByPhone(const char* phoneNum);                 // 通过手机号查找用户索引
bool isIdCardUnique(const char* idCard);                        // 检查身份证号唯一性
int getActiveUserCount();                                       // 获取活跃用户数量
User* getUserByIndex(int index);                                // 通过索引获取用户
int getAllActiveUsers(User* resultArray, int maxCount);         // 获取所有活跃用户

// 用户排序函数
void sortUsersByName(User* usersArray, int count);                  // 按姓名排序
void sortUsersByAge(User* usersArray, int count, bool ascending);   // 按年龄排序
void sortUsersByIdCard(User* usersArray, int count);                // 按身份证号排序

#endif // _USER