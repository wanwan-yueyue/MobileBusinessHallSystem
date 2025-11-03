/*
 * 文件名称：utils.h
 * 文件路径：.\MobileBusinessHallSystem\utils.h
 * 功能描述：提供身份证、手机号验证相关的接口声明，含合法性校验、信息提取等工具函数。
 * 作    者：XXX（邮箱：xxx@xxx.com）。
 * 作    者：杜家宝（邮箱：B24050011@njupt.edu.cn）
 * 创建日期：2025-10-28
 * 版本信息：v1.2（新增出生日期合法性校验，修复NULL指针崩溃问题）
 * 版权声明：© 2025 杜家宝，保留所有权利
 * 
 * 接口概览：
 * - 身份证验证：bool isValidIdCard(const char *idCard); （主验证接口，整合所有规则）
 * - 手机号验证：bool isValidPhoneNumber(const char *phoneNumber); （基础格式校验）
 * - 号段验证：bool validatePhoneSegment(const char *segment); （手机号段格式校验）
 * 
 * - 辅助接口：int calculateAgeFromIDCard(const char *idCard); （年龄计算函数）
 *            const char* getGenderFromIDCard(const char *idCard); （性别判断函数）
 * 
 * 
 * 使用须知：
 * 1. 所有字符串参数（如 idCard）需保证非空，否则函数返回 false；
 * 2. 依赖标准库：stdbool.h（提供 bool 类型）、time.h（获取系统时间）；
 * 3. 头文件已包含保护宏（_UTILS_H_），无需额外处理重复包含。
 * 
 * 修改记录：
 * 2025-10-28  创建文件，声明基础校验接口（v1.0）
 * 2025-10-28  新增 extractBirthDate、getCurrentDate 接口声明（v1.1）
 * 2025-10-28  补充接口使用须知，明确参数非空要求（v1.2）
 */


#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <time.h>

// 函数声明
bool isValidIdCard(const char *idCard);                             // 身份证验证函数
int calculateAgeFromIDCard(const char *idCard);                     // 年龄计算函数
const char* getGenderFromIDCard(const char *idCard);                // 性别判断函数
bool isValidPhoneNumber(const char *phoneNumber);                   // 手机号验证函数
bool validatePhoneSegment(const char *segment);                     // 手机号段格式验证函数
void clearInputBuffer();                                            // 清空输入缓冲区函数
const char* getProvinceFromIDCard(const char* idCard);				// 从身份证获取省份信息

#endif // UTILS_H