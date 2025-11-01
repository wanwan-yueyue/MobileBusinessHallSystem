/*
 * 文件名称：phone.h
 * 文件路径：.\MobileBusinessHallSystem\phone.h
 * 功能描述：手机号管理模块头文件 - 定义手机号资源管理相关数据结构和函数接口
 * 作    者：
 * 创建日期：2025-10-28
 * 版本信息：v1.1（支持一人多号功能，完善状态枚举定义）
 * 版权声明：© 2025 ，保留所有权利
 * 
 * 实现说明：
 * 1. 定义手机号资源状态枚举（空闲、已分配、不活跃），支持状态管理；
 * 2. 设计手机号资源结构体，包含号码、状态、用户ID、分配时间等字段；
 * 3. 支持一人多号功能，通过MAX_PHONE_PER_USER限制单用户最大绑定数量；
 * 4. 提供完整的手机号生命周期管理接口，包括初始化、绑定、解绑、查询等。
 * 依赖说明：
 * - 标准库：stdbool.h（布尔类型支持）
 * - 自定义模块：utils.h（手机号格式验证）
 * 
 * 修改记录：
 * 2025-10-28  新增文件，定义基础数据结构和函数接口（v1.0）
 * 2025-10-28  扩展状态枚举，增加不活跃状态；优化函数参数命名（v1.1）
 */
#ifndef _PHONE_H_
#define _PHONE_H_

#include <stdbool.h>

#define MAX_PHONE_LENGTH 12             // 最大手机号长度（11位+结束符）
#define MAX_SEGMENT_LENGTH 8            // 最大号段长度
#define MAX_PHONE_PER_USER 5            // 每用户最大绑定手机号数量
#define INIT_PHONE_CAPACITY 100         // 初始手机号池容量

/**
 * @brief 手机号资源状态枚举
 */
typedef enum {
    PHONE_STATUS_FREE = 0,              // 空闲
    PHONE_STATUS_ASSIGNED = 1,          // 已分配
    PHONE_STATUS_INACTIVE = 2           // 不活跃
} PhoneStatus;

/**
 * @brief 手机号资源结构体
 */
typedef struct PhoneResource {
    char phoneNumber[MAX_PHONE_LENGTH]; // 手机号码
    PhoneStatus status;                 // 号码状态
    int userId;                         // 绑定用户ID(-1表示未绑定)
    char assignTime[20];                // 分配时间字符串
} PhoneResource;

/**
 * @brief 手机号管理上下文结构体
 */
typedef struct PhoneManager {
    PhoneResource *phones;              // 手机号资源数组
    int count;                          // 当前手机号数量
    int capacity;                       // 手机号池容量
} PhoneManager;

// 函数声明
// 手机号管理器相关接口
PhoneManager* initPhoneManager(void);                       // 初始化手机号管理器
void cleanupPhoneManager(PhoneManager *manager);            // 清理手机号管理器

// 手机号资源管理接口
int initPhoneResources(PhoneManager *manager, 
                    const char *startSegment, int num);     // 初始化手机号资源
int releasePhoneSegment(PhoneManager *manager, 
                    const char *startSegment, int num);     // 释放手机号段资源
int selectRandomPhone(PhoneManager *manager, 
                    char *selectedPhone);                   // 随机选择手机号资源

// 手机号绑定/解绑接口
int registerPhone(PhoneManager *manager, 
                  int userID, const char *phoneNumber);     // 绑定手机号
int cancelPhone(PhoneManager *manager, 
                  int userID, const char *phoneNumber);     // 解绑手机号
int cancelAllUserPhones(PhoneManager *manager, int userID); // 解绑用户所有手机号

// 查询统计函数接口
bool isPhoneUnique(PhoneManager *manager, 
                    const char *phoneNumber);               // 检查手机号唯一性
int getUserPhoneCount(const PhoneManager *manager, 
                        int userID);                        // 获取用户绑定手机号数量
int getAvailablePhoneCount(const PhoneManager *manager);    // 获取可用手机号数量
void getUserPhones(const PhoneManager *manager, int userID, 
                   char phoneNumbers[][MAX_PHONE_LENGTH], 
                   int *Count);                             // 获取用户绑定的手机号列表
int findPhoneIndex(PhoneManager *manager, 
                   const char *phoneNumber);                // 查找手机号索引位置
const PhoneResource* getPhoneResourceByIndex(const PhoneManager 
                                    *manager, int index);   // 获取手机号资源指针

// 文件操作接口
int savePhoneResource(const PhoneManager *manager, 
                        const char *fileName);              // 保存手机号资源到文件
int loadPhoneResource(PhoneManager *manager, 
                        const char *fileName);              // 从文件加载手机号资源

#endif // _PHONE_H_