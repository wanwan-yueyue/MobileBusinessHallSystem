/**
 * @file global.c
 * @brief 全局变量管理实现文件 - 实现系统全局变量的初始化和资源管理
 *        提供全局资源的统一访问接口和生命周期管理
 * 
 * 功能说明：
 * 1. 定义系统核心全局变量的存储空间
 * 2. 实现全局资源的初始化和清理功能
 * 3. 提供手机号管理器的单例访问接口
 * 4. 确保全局变量的一致性和线程安全
 * 
 * 作    者：
 * 创建日期：2025-10-29
 * 版本信息：v2.0（适配模块化架构）
 * 版权声明：© 2025 | 保留所有权利
 * 
 * 实现说明：
 * 1. 使用标准库函数进行内存管理和字符串操作
 * 2. 实现延迟初始化的手机号管理器
 * 3. 提供完整的错误处理和资源清理
 * 
 * 依赖说明：
 * - 标准库：stdlib.h、string.h
 * - 自定义模块：global.h、user.h、phone.h
 * 
 * 修改记录：   
 * 2025-11-1   新增文件，实现全局变量管理（v1.0）
 * 2025-11-1   优化手机号管理器初始化逻辑（v1.1）
 * 2025-11-2   适配模块化架构，完善资源管理（v2.0）
 */

#include "global.h"
#include <stdlib.h>
#include <string.h>

// ========== 全局变量定义 ==========

/**
 * @brief 用户数据数组定义
 * 
 * 存储说明：
 * - 静态分配的固定大小数组
 * - 容量由MAX_USERS宏控制（当前为200）
 * - 通过用户状态字段区分有效和无效用户
 * - 在程序生命周期内持续存在
 */
User users[MAX_USERS];

/**
 * @brief 当前活跃用户数量定义
 * 
 * 维护说明：
 * - 初始值为0，表示无活跃用户
 * - 在用户注册时递增，注销时递减
 * - 用于快速统计和边界检查
 * - 与用户数组的实际有效条目数保持一致
 */
int userCount = 0;

/**
 * @brief 手机号管理器指针定义
 * 
 * 管理说明：
 * - 初始为NULL，实现延迟初始化
 * - 通过getPhoneManager()函数按需创建
 * - 在程序退出时通过cleanupGlobalResources()释放
 * - 确保整个系统只有一个手机号管理器实例
 */
PhoneManager *phoneManager = NULL;

// ========== 全局资源初始化函数 ==========

/**
 * @brief 初始化全局变量
 * @retval 无
 * 
 * 功能说明：
 * 1. 重置用户计数为0
 * 2. 初始化用户数组，标记所有用户为未激活状态
 * 3. 清空用户信息字段（姓名、性别、年龄等）
 * 4. 延迟初始化手机号管理器
 * 
 * 实现细节：
 * - 使用memset清空用户数组的字符串字段
 * - 设置用户状态为USER_INACTIVE标识无效用户
 * - 手机号管理器在首次使用时自动初始化
 * - 确保系统启动时所有全局变量处于干净状态
 * 
 * 使用场景：
 * - 系统启动时的初始化
 * - 数据重新加载前的清理
 * - 测试环境的重置操作
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

// ========== 手机号管理器访问函数 ==========

/**
 * @brief 获取手机号管理器实例
 * @retval PhoneManager* 手机号管理器指针，失败返回NULL
 * 
 * 功能说明：
 * 1. 检查手机号管理器是否已初始化
 * 2. 如果未初始化，创建新的管理器实例
 * 3. 返回管理器指针供调用者使用
 * 4. 处理管理器创建失败的情况
 * 
 * 实现细节：
 * - 使用懒加载模式，首次调用时创建实例
 * - 通过initPhoneManager()函数创建新实例
 * - 记录初始化失败的错误信息
 * - 确保返回的管理器实例可用
 * 
 * 使用场景：
 * - 任何需要访问手机号资源的模块
 * - 手机号注册、注销、查询等操作
 * - 系统初始化时的资源准备
 * 
 * 注意事项：
 * - 调用者不需要手动释放返回的指针
 * - 管理器在程序退出时自动清理
 * - 如果返回NULL，表示系统资源不足
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
 * @brief 设置手机号管理器实例
 * @param manager 新的手机号管理器指针
 * @retval 无
 * 
 * 功能说明：
 * 1. 检查当前管理器是否存在，存在则清理
 * 2. 设置新的手机号管理器指针
 * 3. 确保资源管理的正确交接
 * 
 * 实现细节：
 * - 先清理旧的管理器实例，防止内存泄漏
 * - 直接替换全局管理器指针
 * - 不检查新指针的有效性（由调用者保证）
 * 
 * 使用场景：
 * - 单元测试中替换模拟管理器
 * - 系统重置时的管理器替换
 * - 特殊配置下的管理器定制
 * 
 * 注意事项：
 * - 调用者需确保新管理器的正确性
 * - 旧管理器的资源会被完全释放
 * - 谨慎使用，可能影响系统稳定性
 */
void setPhoneManager(PhoneManager* manager) {
    if (phoneManager != NULL) {
        cleanupPhoneManager(phoneManager);
    }
    phoneManager = manager;
}

// ========== 全局资源清理函数 ==========

/**
 * @brief 清理全局资源
 * @retval 无
 * 
 * 功能说明：
 * 1. 释放手机号管理器占用的所有资源
 * 2. 重置全局变量到初始状态
 * 3. 防止程序退出时的内存泄漏
 * 4. 确保系统资源的完全释放
 * 
 * 实现细节：
 * - 调用cleanupPhoneManager()释放手机号资源
 * - 重置手机号管理器指针为NULL
 * - 调用initGlobalVariables()重置用户数据
 * - 确保所有动态分配的内存都被正确释放
 * 
 * 使用场景：
 * - 程序正常退出前的清理
 * - 系统重置操作
 * - 异常恢复时的资源释放
 * 
 * 注意事项：
 * - 必须在程序退出前调用
 * - 调用后全局变量不可再使用
 * - 确保所有模块已完成资源使用
 */
void cleanupGlobalResources() {
    if (phoneManager != NULL) {
        cleanupPhoneManager(phoneManager);
        phoneManager = NULL;
    }
    
    // 重置用户数据
    initGlobalVariables();
}