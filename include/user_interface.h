/*
 * 文件名称：user_interface.h
 * 文件路径：.\MobileBusinessHallSystem\user_interface.h
 * 功能描述：用户界面交互模块头文件 - 定义用户相关的界面交互函数接口
 *          提供用户管理功能的用户界面和交互逻辑
 * 作    者：
 * 创建日期：2025-10-30
 * 版本信息：v2.0（统一界面样式）
 * 版权声明：© 2025 | 保留所有权利
 * 
 * 实现说明：
 * 1. 提供用户管理功能的完整界面交互接口
 * 2. 支持用户信息的输入、显示、修改和删除操作
 * 3. 集成手机号管理功能，支持手机号的注册和注销
 * 4. 实现安全的用户输入验证和错误处理
 * 
 * 依赖说明：
 * - 自定义模块：user.h（用户数据结构定义）
 * 
 * 修改记录：
 * 2025-10-30  新增文件，定义用户界面交互接口（v1.0）
 * 2025-10-30  扩展接口，支持手机号管理功能（v1.1）
 * 2025-10-30  统一界面样式，优化用户体验（v1.2）
 * 2025-11-4   重构函数接口，完善文档注释（v2.0）
 */

#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "user.h"

// ========== 用户管理界面函数声明 ==========

/**
 * @brief 新增用户界面
 * @retval 无
 * 
 * 功能说明：
 * - 提供用户信息输入界面
 * - 自动从身份证号码中提取性别和年龄
 * - 验证身份证号码的唯一性和格式正确性
 * - 支持立即注册手机号的选项
 */
void addUserInterface();

/**
 * @brief 查找用户界面
 * @retval 无
 * 
 * 功能说明：
 * - 提供多种查找方式（身份证号、手机号、姓名）
 * - 支持同名用户的区分选择
 * - 显示完整的用户详细信息
 * - 处理查找失败的情况
 */
void searchUserInterface();

/**
 * @brief 修改用户信息界面
 * @retval 无
 * 
 * 功能说明：
 * - 提供用户信息修改选项
 * - 显示当前用户信息供参考
 * - 支持选择性修改不同字段
 * - 验证修改数据的合法性
 */
void modifyUserInterface();

/**
 * @brief 删除用户界面
 * @retval 无
 * 
 * 功能说明：
 * - 提供用户注销确认界面
 * - 检查用户是否存在关联手机号
 * - 执行用户状态更新操作
 * - 处理注销失败的情况
 */
void deleteUserInterface();

/**
 * @brief 显示所有用户界面
 * @retval 无
 * 
 * 功能说明：
 * - 显示系统中所有活跃用户的完整信息
 * - 支持多种排序方式（姓名、年龄、身份证号）
 * - 显示用户的手机号绑定情况
 * - 提供用户信息的完整概览
 */
void showAllUsersInterface();

/**
 * @brief 显示用户详细信息界面
 * @param userIndex 用户索引
 * @retval 无
 * 
 * 功能说明：
 * - 显示指定用户的完整详细信息
 * - 包括基本信息和手机号绑定情况
 * - 自动从身份证号码中提取省份信息
 * - 提供清晰的信息展示格式
 */
void displayUserDetailsInterface(int userIndex);

// ========== 手机号管理界面函数声明 ==========

/**
 * @brief 为用户注册手机号界面
 * @param userId 用户ID，为-1时需要先选择用户
 * @retval 无
 * 
 * 功能说明：
 * - 提供手机号注册的完整流程
 * - 支持手动输入和号段选择两种方式
 * - 验证手机号格式和唯一性
 * - 检查用户手机号数量限制
 */
void registerPhoneForUserInterface(int userId);

/**
 * @brief 注销手机号界面
 * @retval 无
 * 
 * 功能说明：
 * - 提供手机号注销确认界面
 * - 显示用户当前绑定的所有手机号
 * - 支持选择性注销单个手机号
 * - 验证注销操作的合法性
 */
void unregisterPhoneInterface();

// ========== 输入辅助函数声明 ==========

/**
 * @brief 等待任意键继续
 * @retval 无
 * 
 * 功能说明：
 * - 暂停程序执行，等待用户按键
 * - 显示友好的提示信息
 * - 用于界面切换的暂停控制
 */
void waitForAnyKey();

/**
 * @brief 安全字符串输入
 * @param buffer 输入缓冲区
 * @param size 缓冲区大小
 * @param prompt 输入提示信息
 * @retval 无
 * 
 * 功能说明：
 * - 提供安全的字符串输入功能
 * - 自动处理换行符和缓冲区溢出
 * - 显示友好的输入提示信息
 * - 确保输入数据的完整性
 */
void safeStringInput(char* buffer, size_t size, const char* prompt);

/**
 * @brief 安全整数输入
 * @param prompt 输入提示信息
 * @param min 最小值限制
 * @param max 最大值限制
 * @retval 输入的整数值
 * 
 * 功能说明：
 * - 提供安全的整数输入功能
 * - 验证输入范围和格式正确性
 * - 支持重试机制直到输入有效值
 * - 自动清理输入缓冲区
 */
int safeIntInput(const char* prompt, int min, int max);

/**
 * @brief 安全清空输入缓冲区
 * @retval 无
 * 
 * 功能说明：
 * - 清空标准输入缓冲区中的残留数据
 * - 防止后续输入操作受到干扰
 * - 确保输入数据的准确性
 */
void safeClearInputBuffer();

/**
 * @brief 创建选择界面
 * @param options 选项数组
 * @param count 选项数量
 * @param title 选择界面标题
 * @retval 用户选择的选项索引，取消返回-1
 * 
 * 功能说明：
 * - 提供统一的选项选择界面
 * - 支持箭头键导航选择
 * - 显示选择标题和选项列表
 * - 处理取消选择的情况
 */
int makeSelection(const char *options[], int count, const char *title);


#endif