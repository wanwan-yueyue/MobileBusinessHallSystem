#ifndef _USER
#define _USER

#include <stdio.h>
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

// 全局用户数组（在user.c中定义）
extern User users[MAX_USERS];                   // 用户数组
extern int userCount;                           // 当前用户数量
extern PhoneManager *phoneManager;              // 全局手机号管理器指针

// 用户管理函数声明
void addUser();                                 // 新增用户
void searchUser();                              // 查找用户
void modifyUser();                              // 修改用户信息
void deleteUser();                              // 注销用户
void showAllUsers();                            // 显示所有用户
void sortUsers();                               // 排序用户信息
int findUserIndexById(const char* idCard);      // 通过身份证号查找用户索引

// 手机号相关函数
void registerPhoneForUser(int userId);          // 为用户注册手机号
void unregisterPhone();                         // 注销用户手机号

// 查询函数
int findUserIndexById(const char* idCard);      // 通过身份证号查找用户索引
int findUserIndexByPhone(const char* phoneNum); // 通过手机号查找用户索引
bool isIdCardUnique(const char* idCard);        // 检查身份证号唯一性

#endif // _USER