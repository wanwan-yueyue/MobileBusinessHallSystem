/*
 * 文件名称：utils.h
 * 文件路径：.\MobileBusinessHallSystem\utils.h
 * 功能描述：工具函数模块头文件 - 定义身份证号码和手机号码验证相关工具函数
 *          提供数据验证、信息提取和格式检查等通用功能
 * 作    者：
 * 创建日期：2025-10-28
 * 版本信息：v1.2（新增出生日期合法性校验，修复NULL指针崩溃问题）
 * 
 * 实现说明：
 * 1. 校验码验证基于GB 11643-1999标准，采用加权因子[7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2]计算；
 * 2. 出生日期验证依赖 getCurrentDate 函数获取系统时间，通过 getDaysInMonth 处理闰年和月份天数；
 * 3. 提供完整的身份证号码和手机号码格式验证功能；
 * 4. 支持从身份证号码中自动提取性别、年龄、省份等信息。
 * 
 * 依赖说明：
 * - 标准库：stdbool.h（布尔类型支持）、time.h（时间函数支持）
 * 
 * 修改记录：
 * 2025-10-28  新增文件，声明基础校验接口（v1.0）
 * 2025-10-28  新增 extractBirthDate、getCurrentDate 接口声明（v1.1）
 * 2025-10-28  补充接口使用须知，明确参数非空要求（v1.2）
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <time.h>

// ========== 身份证验证函数声明 ==========

/**
 * @brief 身份证号码综合验证
 * @param idCard 待验证的身份证号码字符串
 * @retval true-身份证有效, false-身份证无效
 * 
 * 功能说明：
 * - 整合所有身份证验证规则的主验证接口
 * - 验证身份证长度、字符组成、校验码和出生日期
 * - 符合GB 11643-1999国家标准
 * - 返回身份证号码的整体有效性
 */
bool isValidIdCard(const char *idCard);

/**
 * @brief 根据身份证计算年龄
 * @param idCard 身份证号码字符串
 * @retval 年龄值(失败返回-1)
 * 
 * 功能说明：
 * - 从身份证号码中提取出生日期
 * - 计算相对于当前系统时间的年龄
 * - 考虑月份和日期的精确计算
 * - 返回准确的周岁年龄
 */
int calculateAgeFromIDCard(const char *idCard);

/**
 * @brief 根据身份证判断性别
 * @param idCard 身份证号码字符串
 * @retval 性别字符串("男"或"女")
 * 
 * 功能说明：
 * - 从身份证号码第17位提取性别信息
 * - 奇数为男性，偶数为女性
 * - 返回中文性别标识
 * - 支持未知情况的处理
 */
const char* getGenderFromIDCard(const char *idCard);

/**
 * @brief 根据身份证前2位获取省份信息
 * @param idCard 身份证号码字符串
 * @retval 省份字符串
 * 
 * 功能说明：
 * - 从身份证号码前2位提取省级行政区划代码
 * - 映射到对应的省份名称
 * - 支持全国所有省级行政区的识别
 * - 返回中文省份名称
 */
const char* getProvinceFromIDCard(const char* idCard);

// ========== 手机号验证函数声明 ==========

/**
 * @brief 手机号码长度和字符验证
 * @param phoneNumber 待验证的手机号码字符串
 * @retval true-手机号格式正确, false-手机号格式错误
 * 
 * 功能说明：
 * - 验证手机号码的长度是否为11位
 * - 检查手机号码是否全部由数字组成
 * - 符合中国手机号码的基本格式要求
 * - 返回手机号码的格式有效性
 */
bool isValidPhoneNumber(const char *phoneNumber);

/**
 * @brief 验证手机号段格式
 * @param segment 手机号段
 * @retval true-格式正确, false-格式错误
 * 
 * 功能说明：
 * - 验证手机号段的长度和字符组成
 * - 检查号段是否符合中国手机号段规则
 * - 验证号段开头数字和范围
 * - 返回手机号段的格式有效性
 */
bool validatePhoneSegment(const char *segment);

// ========== 输入处理函数声明 ==========

/**
 * @brief 清空输入缓冲区
 * @retval 无
 * 
 * 功能说明：
 * - 清除标准输入缓冲区中的残留数据
 * - 防止后续输入操作受到干扰
 * - 确保输入数据的准确性和完整性
 * - 提高用户输入的可靠性
 */
void clearInputBuffer();

#endif // UTILS_H