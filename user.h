#ifndef _USER
#define _USER

#include <stdio.h>
#include "phone.h"

#define MAX_USERS 20                // 最大用户数量
#define NAME_LEN 20                 // 姓名最大长度
#define GENDER_LEN 4                // 性别最大长度
#define ID_LEN 19                   // 身份证号最大长度
#define JOB_LEN 30                  // 职业最大长度
#define ADDR_LEN 50                 // 地址最大长度

// 用户结构体
typedef struct {
    char name[NAME_LEN];
    char gender[GENDER_LEN];
    int age;
    char idCard[ID_LEN];
    char job[JOB_LEN];
    char address[ADDR_LEN];
    int isActive;  // 标记用户是否有效（1-有效，0-已删除）
} User;

// 全局用户数组（在user.c中定义）
extern User users[MAX_USERS];
extern int userCount;
extern PhoneManager *phoneManager;      // 全局手机号管理器指针

// 用户管理函数声明
void addUser();
void searchUser();
void modifyUser();
void deleteUser();
void showAllUsers();
void sortUsers();
int findUserIndexById(const char* idCard);

// 手机号相关函数
void registerPhoneForUser(int userId);
void unregisterPhone();

// 查询函数
int findUserIndexById(const char* idCard);
int findUserIndexByPhone(const char* phoneNum);
int isIdCardUnique(const char* idCard);

#endif // _USER
