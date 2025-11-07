/*
 * 文件名称：user.h
 * 文件路径：.\MobileBusinessHallSystem\user.h
 * 功能描述：用户管理模块头文件 - 定义用户数据结构和纯数据操作接口
 *          提供用户信息的存储、检索、修改和删除等核心数据操作
 * 作    者：
 * 创建日期：2025-10-29
 * 版本信息：v3.0（纯数据操作版本）
 * 版权声明：© 2025 | 保留所有权利
 * 
 * 实现说明：
 * 1. 定义用户数据结构和状态枚举
 * 2. 提供用户数据的增删改查接口
 * 3. 实现用户数据的排序功能
 * 4. 移除所有界面交互逻辑，专注于数据操作
 * 
 * 依赖说明：
 * - 标准库：stdio.h、stdbool.h
 * - 自定义模块：phone.h、menu.h
 * 
 * 修改记录：
 * 2025-10-29  新增文件，定义用户数据结构（v1.0）
 * 2025-10-30  扩展接口，支持用户状态管理（v2.0）
 * 2025-10-30  优化数据结构，移除手机号数组（v2.1）
 * 2025-11-1   重构为纯数据操作版本（v3.0）
 */

#ifndef _USER_H_
#define _USER_H_

#include <stdio.h>
#include <stdbool.h>
#include "phone.h"
#include "menu.h"                               // 包含样式函数

// ========== 常量定义 ==========

/**
 * @brief 用户管理相关常量定义
 * 
 * 功能说明：
 * - 定义用户数据存储的最大容量和字段长度限制
 * - 控制用户信息的存储格式和验证规则
 * - 确保系统用户管理的稳定性和数据完整性
 */

#define MAX_USERS 200                           // 最大用户数量
#define NAME_LEN 20                             // 姓名最大长度
#define GENDER_LEN 4                            // 性别最大长度
#define ID_LEN 19                               // 身份证号最大长度
#define JOB_LEN 30                              // 职业最大长度
#define ADDR_LEN 50                             // 地址最大长度

// ========== 数据类型定义 ==========

/**
 * @brief 用户状态枚举
 *
 * 功能说明：
 * - 定义用户在系统中的三种状态
 * - 支持用户账户的生命周期管理
 * - 确保用户状态转换的逻辑正确性
 */
typedef enum {
    USER_ACTIVE = 1,                            // 活跃用户 - 正常使用状态
    USER_INACTIVE = 0,                          // 已注销用户 - 不可用状态
    USER_DELETED = 2                            // 已删除用户 - 在回收站中
} UserStatus;

/**
 * @brief 用户结构体
 * 
 * 功能说明：
 * - 存储单个用户的完整信息
 * - 包含基本身份信息和联系信息
 * - 支持用户状态的动态管理
 */
typedef struct {
    char name[NAME_LEN];                        // 姓名
    char gender[GENDER_LEN];                    // 性别
    int age;                                    // 年龄
    char idCard[ID_LEN];                        // 身份证号
    char job[JOB_LEN];                          // 职业
    char address[ADDR_LEN];                     // 地址
    UserStatus status;                          // 用户状态（ACTIVE/INACTIVE）
} User;

// ========== 用户数据操作接口 ==========

/**
 * @brief 新增用户
 * @param user 指向用户数据的指针
 * @retval int 成功返回用户索引，失败返回-1
 * 
 * 功能说明：
 * - 将新用户数据添加到系统
 * - 检查系统容量限制
 * - 找到第一个空闲位置存储用户数据
 * - 设置用户状态为活跃并更新用户计数
 */
int addUser(const User* user);

/**
 * @brief 注销用户
 * @param userIndex 用户索引
 * @retval int 成功返回1，失败返回0
 * 
 * 功能说明：
 * - 将指定用户的状态设置为已注销
 * - 验证用户索引的有效性
 * - 减少活跃用户计数
 * - 保持用户数据在系统中的存在但标记为不可用
 */
int deleteUser(int userIndex);

/**
 * @brief 修改用户信息
 * @param userIndex 用户索引
 * @param newUserData 指向新用户数据的指针
 * @retval int 成功返回1，失败返回0
 * 
 * 功能说明：
 * - 更新指定用户的完整信息
 * - 验证用户索引和数据的有效性
 * - 确保用户状态保持不变
 * - 直接替换用户数据结构内容
 */
int modifyUser(int userIndex, const User* newUserData);

// ========== 用户查询接口 ==========

/**
 * @brief 通过身份证号查找用户索引
 * @param idCard 身份证号字符串
 * @retval int 找到返回用户索引，未找到返回-1
 * 
 * 功能说明：
 * - 在用户数组中查找指定身份证号的用户
 * - 只搜索状态为活跃的用户
 * - 返回用户在数组中的位置索引
 * - 用于用户身份的快速定位
 */
int findUserIndexById(const char* idCard);

/**
 * @brief 通过手机号查找用户索引
 * @param phoneNum 手机号字符串
 * @retval int 找到返回用户索引，未找到返回-1
 * 
 * 功能说明：
 * - 通过手机号查找绑定的用户
 * - 查询手机号管理器获取用户ID
 * - 返回对应的用户索引位置
 * - 用于手机号到用户的逆向查找
 */
int findUserIndexByPhone(const char* phoneNum);

/**
 * @brief 检查身份证号唯一性
 * @param idCard 身份证号字符串
 * @retval bool 唯一返回true，已存在返回false
 * 
 * 功能说明：
 * - 验证身份证号是否未被其他用户使用
 * - 用于新增用户时的身份证号验证
 * - 确保用户身份标识的唯一性
 * - 防止重复用户账户的创建
 */
bool isIdCardUnique(const char* idCard);

/**
 * @brief 获取活跃用户数量
 * @retval int 当前活跃用户数量
 * 
 * 功能说明：
 * - 统计系统中状态为活跃的用户数量
 * - 用于系统容量检查和统计显示
 * - 反映系统的当前使用情况
 */
int getActiveUserCount();

/**
 * @brief 通过索引获取用户
 * @param index 用户索引
 * @retval User* 指向用户数据的指针，失败返回NULL
 * 
 * 功能说明：
 * - 根据索引获取用户数据的直接指针
 * - 验证索引的有效性和用户状态
 * - 提供用户数据的直接访问接口
 * - 用于用户信息的读取和显示
 */
User* getUserByIndex(int index);

/**
 * @brief 获取所有活跃用户
 * @param resultArray 输出参数，存储用户数据的数组
 * @param maxCount 最大获取数量
 * @retval int 实际获取的用户数量
 * 
 * 功能说明：
 * - 收集系统中所有活跃用户的数据
 * - 限制输出数量防止数组溢出
 * - 返回实际有效的用户数据数量
 * - 用于用户列表的批量处理
 */
int getAllActiveUsers(User* resultArray, int maxCount);

// ========== 用户排序接口 ==========

/**
 * @brief 按姓名排序用户数组
 * @param usersArray 用户数组
 * @param count 用户数量
 * @retval 无
 * 
 * 功能说明：
 * - 对用户数组按姓名进行升序排序
 * - 使用冒泡排序算法实现
 * - 直接修改原数组的顺序
 * - 用于用户列表的有序显示
 */
void sortUsersByName(User* usersArray, int count);

/**
 * @brief 按年龄排序用户数组
 * @param usersArray 用户数组
 * @param count 用户数量
 * @param ascending 排序方向（true为升序，false为降序）
 * @retval 无
 * 
 * 功能说明：
 * - 对用户数组按年龄进行排序
 * - 支持升序和降序两种排序方向
 * - 使用冒泡排序算法实现
 * - 提供灵活的年龄排序选项
 */
void sortUsersByAge(User* usersArray, int count, bool ascending);

/**
 * @brief 按身份证号排序用户数组
 * @param usersArray 用户数组
 * @param count 用户数量
 * @retval 无
 * 
 * 功能说明：
 * - 对用户数组按身份证号进行升序排序
 * - 使用冒泡排序算法实现
 * - 基于字符串比较进行排序
 * - 用于身份证号的有序排列
 */
void sortUsersByIdCard(User* usersArray, int count);

/**
 * @brief 通过身份证号查找用户索引（包括已删除用户）
 * @param idCard 身份证号字符串
 * @retval int 找到返回用户索引，未找到返回-1
 *
 * 功能说明：
 * - 在用户数组中查找指定身份证号的用户
 * - 搜索所有状态的用户（包括已删除）
 * - 返回用户在数组中的位置索引
 * - 用于回收站管理等场景
 */
int findUserIndexByIdIncludeDeleted(const char* idCard);

/**
 * @brief 恢复已删除用户
 * @param userIndex 用户索引
 * @retval int 成功返回1，失败返回0
 *
 * 功能说明：
 * - 将指定用户的状态从已删除恢复为活跃
 * - 验证用户索引的有效性
 * - 增加活跃用户计数
 * - 用于回收站中的用户恢复操作
 */
int restoreUser(int userIndex);
#endif // _USER