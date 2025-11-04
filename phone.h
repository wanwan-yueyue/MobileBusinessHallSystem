/*
 * 文件名称：phone.h
 * 文件路径：.\MobileBusinessHallSystem\phone.h
 * 功能描述：手机号管理模块头文件 - 定义手机号资源管理相关数据结构和函数接口
 *          实现手机号资源的生命周期管理，支持一人多号功能
 * 作    者：
 * 创建日期：2025-10-28
 * 版本信息：v1.2（增批量手机号初始化功能）
 * 版权声明：© 2025 ，保留所有权利
 * 
 * 实现说明：
 * 1. 定义手机号资源状态枚举（空闲、已分配、不活跃），支持状态管理；
 * 2. 设计手机号资源结构体，包含号码、状态、用户ID、分配时间等字段；
 * 3. 支持一人多号功能，通过MAX_PHONE_PER_USER限制单用户最大绑定数量；
 * 4. 提供完整的手机号生命周期管理接口，包括初始化、绑定、解绑、查询等。
 * 
 * 依赖说明：
 * - 标准库：stdbool.h（布尔类型支持）
 * - 自定义模块：utils.h（手机号格式验证）
 * 
 * 修改记录：
 * 2025-10-28  新增文件，定义基础数据结构和函数接口（v1.0）
 * 2025-10-28  扩展状态枚举，增加不活跃状态；优化函数参数命名（v1.1）
 * 2025-11-4   新增批量手机号初始化功能（v1.2)
 */

#ifndef _PHONE_H_
#define _PHONE_H_

#include <stdbool.h>

// ========== 常量定义 ==========

/**
 * @brief 手机号相关常量定义
 * 
 * 功能说明：
 * - 定义手机号管理相关的长度限制和容量配置
 * - 控制手机号资源的分配和使用规则
 * - 确保系统资源管理的合理性和稳定性
 */

#define MAX_PHONE_LENGTH 12             // 最大手机号长度（11位+结束符）
#define MAX_SEGMENT_LENGTH 8            // 最大号段长度
#define MAX_PHONE_PER_USER 5            // 每用户最大绑定手机号数量
#define INIT_PHONE_CAPACITY 100         // 初始手机号池容量
#define PHONES_PER_SEGMENT 99999        // 每个号段的手机号数量（可调整）

// ========== 数据类型定义 ==========

/**
 * @brief 手机号资源状态枚举
 * 
 * 功能说明：
 * - 定义手机号在系统中的三种状态
 * - 支持手机号资源的全生命周期管理
 * - 确保状态转换的逻辑正确性
 */
typedef enum {
    PHONE_STATUS_FREE = 0,              // 空闲 - 可供分配使用
    PHONE_STATUS_ASSIGNED = 1,          // 已分配 - 已绑定到用户
    PHONE_STATUS_INACTIVE = 2           // 不活跃 - 保留状态，暂不可用
} PhoneStatus;

/**
 * @brief 手机号资源结构体
 * 
 * 功能说明：
 * - 存储单个手机号的完整信息
 * - 包含号码、状态、绑定关系和时间戳
 * - 支持手机号资源的精确管理和追踪
 */
typedef struct PhoneResource {
    char phoneNumber[MAX_PHONE_LENGTH]; // 手机号码（11位数字+结束符）
    PhoneStatus status;                 // 号码状态（空闲/已分配/不活跃）
    int userId;                         // 绑定用户ID(-1表示未绑定)
    char assignTime[20];                // 分配时间字符串（格式：YYYY-MM-DD HH:MM:SS）
} PhoneResource;

/**
 * @brief 手机号管理上下文结构体
 * 
 * 功能说明：
 * - 管理所有手机号资源的集合
 * - 支持动态扩容和资源池管理
 * - 提供手机号资源的统一访问接口
 */
typedef struct PhoneManager {
    PhoneResource *phones;              // 手机号资源数组（动态分配）
    int count;                          // 当前手机号数量
    int capacity;                       // 手机号池容量
} PhoneManager;

// ========== 手机号管理器生命周期函数 ==========

/**
 * @brief 初始化手机号管理器
 * @retval PhoneManager* 手机号管理器指针，失败返回NULL
 * 
 * 功能说明：
 * - 分配手机号管理器内存空间
 * - 初始化手机号资源数组
 * - 设置初始容量和计数
 * - 返回初始化完成的管理器实例
 */
PhoneManager* initPhoneManager(void);

/**
 * @brief 清理手机号管理器
 * @param manager 手机号管理器指针
 * @retval 无
 * 
 * 功能说明：
 * - 释放手机号资源数组占用的内存
 * - 释放手机号管理器结构体本身
 * - 确保所有动态分配的资源被正确释放
 */
void cleanupPhoneManager(PhoneManager *manager);

// ========== 手机号资源管理接口 ==========

/**
 * @brief 初始化手机号资源
 * @param manager 手机号管理器指针
 * @param startSegment 起始号段字符串
 * @param num 要生成的手机号数量
 * @retval 1-成功, 0-失败
 * 
 * 功能说明：
 * - 根据指定号段生成指定数量的手机号
 * - 自动处理号段验证和序列号生成
 * - 支持手机号资源池的动态扩容
 */
int initPhoneResources(PhoneManager *manager, const char *startSegment, int num);

/**
 * @brief 释放手机号段资源
 * @param manager 手机号管理器指针
 * @param startSegment 起始号段字符串
 * @param num 手机号数量
 * @retval 1-成功, 0-失败
 * 
 * 功能说明：
 * - 释放指定号段的手机号资源
 * - 复用初始化函数的核心逻辑
 * - 用于号段资源的批量管理
 */
int releasePhoneSegment(PhoneManager *manager, const char *startSegment, int num);

/**
 * @brief 随机选择手机号资源
 * @param manager 手机号管理器指针
 * @param selectedPhone 输出参数，存储选中的手机号
 * @retval 1-成功, 0-失败
 * 
 * 功能说明：
 * - 从空闲手机号中随机选择一个
 * - 返回选中的手机号字符串
 * - 用于快速分配手机号资源
 */
int selectRandomPhone(PhoneManager *manager, char *selectedPhone);

/**
 * @brief 批量初始化手机号资源
 * @param manager 手机号管理器指针
 * @param segments 号段数组
 * @param segmentCount 号段数量
 * @param phonesPerSegment 每个号段的手机号数量
 * @retval 实际生成的手机号总数量
 * 
 * 功能说明：
 * - 批量处理多个号段的手机号生成
 * - 预分配足够的内存空间
 * - 返回成功生成的手机号总数
 */
int batchInitPhoneResources(PhoneManager *manager, const char *segments[], int segmentCount, int phonesPerSegment);

// ========== 手机号绑定/解绑接口 ==========

/**
 * @brief 绑定手机号到用户
 * @param manager 手机号管理器指针
 * @param userID 用户ID
 * @param phoneNumber 要绑定的手机号字符串
 * @retval 1-成功, 0-失败（号码已占用或超限）
 * 
 * 功能说明：
 * - 将指定手机号绑定到指定用户
 * - 检查手机号格式和唯一性
 * - 验证用户绑定数量限制
 * - 记录绑定时间戳
 */
int registerPhone(PhoneManager *manager, int userID, const char *phoneNumber);

/**
 * @brief 解绑手机号
 * @param manager 手机号管理器指针
 * @param userID 用户ID
 * @param phoneNumber 手机号字符串
 * @retval 1-成功, 0-失败(号码未绑定或不属于该用户)
 * 
 * 功能说明：
 * - 解除手机号与用户的绑定关系
 * - 验证绑定关系的正确性
 * - 重置手机号状态为空闲
 * - 清除分配时间信息
 */
int cancelPhone(PhoneManager *manager, int userID, const char *phoneNumber);

/**
 * @brief 解绑用户所有手机号
 * @param manager 手机号管理器指针
 * @param userID 用户ID
 * @retval 注销的手机号数量
 * 
 * 功能说明：
 * - 解除指定用户的所有手机号绑定
 * - 用于用户注销或批量清理
 * - 返回成功解绑的手机号数量
 */
int cancelAllUserPhones(PhoneManager *manager, int userID);

// ========== 查询统计函数接口 ==========

/**
 * @brief 检查手机号唯一性
 * @param manager 手机号管理器指针
 * @param phoneNumber 手机号字符串
 * @retval true-唯一, false-已存在
 * 
 * 功能说明：
 * - 检查手机号是否未被分配
 * - 用于新手机号分配的验证
 * - 确保手机号资源的唯一性
 */
bool isPhoneUnique(PhoneManager *manager, const char *phoneNumber);

/**
 * @brief 获取用户绑定手机号数量
 * @param manager 手机号管理器指针
 * @param userID 用户ID
 * @retval 手机号数量
 * 
 * 功能说明：
 * - 统计指定用户绑定的手机号数量
 * - 用于绑定数量限制检查
 * - 返回当前绑定的有效手机号数
 */
int getUserPhoneCount(const PhoneManager *manager, int userID);

/**
 * @brief 获取可用手机号数量
 * @param manager 手机号管理器指针
 * @retval 可用手机号数量
 * 
 * 功能说明：
 * - 统计系统中所有空闲手机号的数量
 * - 用于系统资源监控
 * - 返回状态为空闲的手机号总数
 */
int getAvailablePhoneCount(const PhoneManager *manager);

/**
 * @brief 获取用户绑定的手机号列表
 * @param manager 手机号管理器指针
 * @param userID 用户ID
 * @param phoneNumbers 输出参数，存储手机号数组
 * @param count 输出参数，存储手机号数量
 * @retval 无
 * 
 * 功能说明：
 * - 获取指定用户的所有绑定手机号
 * - 返回手机号字符串数组和数量
 * - 用于用户信息的完整显示
 */
void getUserPhones(const PhoneManager *manager, int userID, char phoneNumbers[][MAX_PHONE_LENGTH], int *count);

/**
 * @brief 查找手机号索引位置
 * @param manager 手机号管理器指针
 * @param phoneNumber 手机号字符串
 * @retval 索引位置，未找到返回-1
 * 
 * 功能说明：
 * - 在手机号资源数组中查找指定手机号
 * - 返回手机号在数组中的索引位置
 * - 用于内部定位和快速访问
 */
int findPhoneIndex(PhoneManager *manager, const char *phoneNumber);

/**
 * @brief 获取手机号资源指针
 * @param manager 手机号管理器指针
 * @param index 资源索引
 * @retval PhoneResource* 指向手机号资源的指针，失败返回NULL
 * 
 * 功能说明：
 * - 根据索引获取手机号资源的直接指针
 * - 用于需要直接操作手机号资源的场景
 * - 提供资源级别的访问接口
 */
const PhoneResource* getPhoneResourceByIndex(const PhoneManager *manager, int index);

// ========== 手机号选择与分类接口 ==========

/**
 * @brief 获取多个可用手机号
 * @param manager 手机号管理器指针
 * @param phones 输出参数，存储获取的手机号数组
 * @param maxCount 最大获取数量
 * @retval 实际获取的可用手机号数量
 * 
 * 功能说明：
 * - 随机获取多个可用的手机号
 * - 限制获取数量避免资源耗尽
 * - 返回实际可用的手机号数量
 */
int getAvailablePhones(PhoneManager* manager, char phones[][MAX_PHONE_LENGTH], int maxCount);

/**
 * @brief 获取所有可用号段
 * @param manager 手机号管理器指针
 * @param segments 输出参数，存储获取的号段数组
 * @param maxCount 最大获取数量
 * @retval 实际获取的可用号段数量
 * 
 * 功能说明：
 * - 获取系统中所有不同的手机号段
 * - 用于号段分类和选择界面
 * - 返回发现的唯一号段数量
 */
int getAllSegments(PhoneManager *manager, char segments[][MAX_SEGMENT_LENGTH], int maxCount);

/**
 * @brief 根据号段获取可用的手机号
 * @param manager 手机号管理器指针
 * @param segment 指定的号段
 * @param phones 输出参数，存储获取的手机号数组
 * @param maxCount 最大获取数量
 * @retval 实际获取的可用手机号数量
 * 
 * 功能说明：
 * - 获取指定号段下的可用手机号
 * - 支持按号段筛选手机号资源
 * - 返回该号段可用的手机号数量
 */
int getAvailablePhonesBySegment(PhoneManager *manager, const char *segment, char phones[][MAX_PHONE_LENGTH], int maxCount);

/**
 * @brief 获取号段分类
 * @param manager 手机号管理器指针
 * @param categories 输出参数，存储号段分类数组
 * @param maxCount 最大获取数量
 * @retval 实际获取的号段分类数量
 * 
 * 功能说明：
 * - 获取手机号的号段分类（前2位）
 * - 用于分类选择界面
 * - 返回不同的号段分类数量
 */
int getSegmentCategories(PhoneManager *manager, char categories[][MAX_SEGMENT_LENGTH], int maxCount);

/**
 * @brief 根据分类获取具体号段
 * @param manager 手机号管理器指针
 * @param category 号段分类
 * @param segments 输出参数，存储具体号段数组
 * @param maxCount 最大获取数量
 * @retval 实际获取的具体号段数量
 * 
 * 功能说明：
 * - 获取指定分类下的具体号段（前3位）
 * - 用于细化手机号选择
 * - 返回该分类下的具体号段数量
 */
int getSegmentsByCategory(PhoneManager *manager, const char *category, char segments[][MAX_SEGMENT_LENGTH], int maxCount);

/**
 * @brief 对手机号数组进行排序
 * @param phones 手机号数组
 * @param count 手机号数量
 * @retval 无
 * 
 * 功能说明：
 * - 对手机号字符串数组进行升序排序
 * - 使用冒泡排序算法实现
 * - 确保手机号列表的有序显示
 */
void sortPhoneNumbers(char phones[][MAX_PHONE_LENGTH], int count);

/**
 * @brief 根据分类获取可用手机号数量
 * @param manager 手机号管理器指针
 * @param category 号段分类
 * @retval 可用手机号数量
 * 
 * 功能说明：
 * - 统计指定分类下的可用手机号数量
 * - 用于分类资源统计
 * - 返回该分类的空闲手机号总数
 */
int getAvailablePhoneCountByCategory(PhoneManager *manager, const char *category);

/**
 * @brief 根据具体号段获取可用手机号数量
 * @param manager 手机号管理器指针
 * @param segment 具体号段
 * @retval 可用手机号数量
 * 
 * 功能说明：
 * - 统计指定号段下的可用手机号数量
 * - 用于号段资源统计
 * - 返回该号段的空闲手机号总数
 */
int getAvailablePhoneCountBySegment(PhoneManager *manager, const char *segment);

// ========== 文件操作接口 ==========

/**
 * @brief 保存手机号资源到文件
 * @param manager 手机号管理器指针
 * @param fileName 文件名
 * @retval 1-成功, 0-失败
 * 
 * 功能说明：
 * - 将手机号资源保存到二进制文件
 * - 包含版本信息和完整资源数据
 * - 确保数据持久化和后续恢复
 */
int savePhoneResource(const PhoneManager *manager, const char *fileName);

/**
 * @brief 从文件加载手机号资源
 * @param manager 手机号管理器指针
 * @param fileName 文件名
 * @retval 1-成功, 0-失败
 * 
 * 功能说明：
 * - 从二进制文件加载手机号资源
 * - 验证文件版本兼容性
 * - 恢复手机号资源到内存管理器
 */
int loadPhoneResource(PhoneManager *manager, const char *fileName);
                  
#endif // _PHONE_H_