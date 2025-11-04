/*
 * 文件名称：utils.c
 * 文件路径：.\MobileBusinessHallSystem\utils.c
 * 功能描述：工具函数模块实现文件 - 实现身份证号码验证相关工具函数
 *          含格式校验、出生日期验证、校验码验证、信息提取等完整功能
 * 作    者：
 * 创建日期：2025-10-28
 * 版本信息：v1.2（新增出生日期合法性校验，修复NULL指针崩溃问题）
 * 版权声明：© 2025 杜家宝，保留所有权利
 * 
 * 实现说明：
 * 1. 校验码验证基于GB 11643-1999标准，采用加权因子[7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2]计算；
 * 2. 出生日期验证依赖 getCurrentDate 函数获取系统时间，通过 getDaysInMonth 处理闰年和月份天数；
 * 3. 提供完整的身份证号码生命周期验证；
 * 4. 支持多种信息提取和计算功能。
 * 
 * 依赖说明：
 * - 标准库：stdio.h、stdlib.h、stdbool.h、string.h、ctype.h、time.h
 * 
 * 修改记录：
 * 2025-10-28  新增文件，实现基础校验功能（v1.0）
 * 2025-10-28  修复NULL指针处理漏洞，补充年份上界动态获取（v1.1）
 * 2025-10-28  新增出生日期不晚于当前日期的校验逻辑，修复获取当前时间函数的NULL指针崩溃问题（v1.2）
 */

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// ========== 内部辅助函数声明 ==========

/**
 * @brief 验证身份证号码长度和字符组成
 * @param idCard 待验证的身份证号码字符串
 * @retval true-验证通过, false-验证失败
 */
static bool validateIDCardLengthAndChars(const char *idCard);

/**
 * @brief 身份证校验码验证
 * @param idCard 待验证的身份证号码字符串
 * @retval true-校验码正确, false-校验码错误
 */
static bool validateIDCardCheckDigit(const char *idCard);

/**
 * @brief 获取当前系统年月日
 * @param currentYear 返回当前年份的指针
 * @param currentMonth 返回当前月份的指针
 * @param currentDay 返回当前日期的指针
 * @retval true-获取成功, false-获取失败
 */
static bool getCurrentDate(int* currentYear, int* currentMonth, int* currentDay);

/**
 * @brief 判断是否为闰年
 * @param year 待判断的年份
 * @retval true-闰年, false-平年
 */
static bool isLeapYear(int year);

/**
 * @brief 获取指定月份的天数
 * @param year 年份
 * @param month 月份
 * @retval 天数(无效月份返回-1)
 */
static int getDaysInMonth(int year, int month);

/**
 * @brief 从身份证提取出生日期字符串
 * @param idCard 身份证号码字符串
 * @param birthDate 返回出生日期的缓冲区(YYYYMMDD格式)
 * @param birthDateSize 缓冲区大小
 * @retval 无
 */
static void getBirthDateFromIDCard(const char *idCard, char *birthDate, size_t birthDateSize);

/**
 * @brief 从身份证提取出生年月日
 * @param idCard 身份证号码字符串
 * @param birthYear 返回出生年份的指针
 * @param birthMonth 返回出生月份的指针
 * @param birthDay 返回出生日期的指针
 * @retval true-提取成功, false-提取失败
 */
static bool extractBirthDate(const char *idCard, int *birthYear, int *birthMonth, int *birthDay);

/**
 * @brief 验证身份证出生日期有效性
 * @param IDCard 身份证号码字符串
 * @retval true-日期有效, false-日期无效
 */
static bool validateIDCardBirthDate(const char *IDCard);

// ========== 身份证验证函数实现 ==========

/**
 * @brief 验证身份证号码长度和字符组成
 * @param idCard 待验证的身份证号码字符串
 * @retval true-验证通过, false-验证失败
 * 
 * 实现细节：
 * - 检查身份证号码是否为18位长度
 * - 验证前17位是否全部为数字
 * - 验证最后一位是否为数字或字母X（大小写）
 * - 确保身份证号码的基本格式正确性
 */
bool validateIDCardLengthAndChars(const char *idCard)
{
    if (idCard == NULL)
    {
        return false;
    }
    // 长度应为18位
    if (strlen(idCard) != 18)
    {
        return false;
    }
    // 前17位应为数字
    for (int i = 0; i < 17; i++)
    {
        if (!isdigit(idCard[i]))
        {
            return false;
        }
    }
    // 最后一位可以是数字或字母X的大小写
    if (!isdigit(idCard[17]) && idCard[17] != 'X' && idCard[17] != 'x')
    {
        return false;
    }
    return true;
}

/**
 * @brief 身份证校验码验证
 * @param idCard 待验证的身份证号码字符串
 * @retval true-校验码正确, false-校验码错误
 * 
 * 实现细节：
 * - 使用GB 11643-1999标准加权因子
 * - 计算前17位的加权和
 * - 根据模11结果获取预期校验码
 * - 比较预期校验码与实际校验码
 */
bool validateIDCardCheckDigit(const char *idCard)
{
   // 加权因子和校验码映射表
   int weights[] = {7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2};
   char checkDigits[] = {'1', '0', 'X', '9', '8', '7', '6', '5', '4', '3', '2'};
   int sum = 0; // 加权和
   // 计算加权和
   for (int i = 0; i < 17; i++)
   {
       sum += (idCard[i] - '0') * weights[i];
   }
   // 计算校验码
   int mod = sum % 11;
   char expectedCheckDigit = checkDigits[mod];
   char actualCheckDigit = idCard[17];
   // 处理小写x的情况
   if (actualCheckDigit == 'x')
   {
       actualCheckDigit = 'X';
   }
   return expectedCheckDigit == actualCheckDigit;
}

/**
 * @brief 获取当前系统年月日
 * @param currentYear 返回当前年份的指针
 * @param currentMonth 返回当前月份的指针
 * @param currentDay 返回当前日期的指针
 * @retval true-获取成功, false-获取失败
 * 
 * 实现细节：
 * - 获取当前系统时间戳
 * - 转换为本地时间结构体
 * - 提取年份、月份和日期信息
 * - 处理时间转换的异常情况
 */
bool getCurrentDate(int* currentYear, int* currentMonth, int* currentDay)
{
   // 检查输入参数合法性（避免NULL指针）
   if (currentYear == NULL || currentMonth == NULL || currentDay == NULL)
   {
       return false;
   }

   // 获取当前时间
   time_t now;
   struct tm localTime;  // 栈上分配tm结构体（localtime_s需要预分配的结构体）
   time(&now);  // 获取当前时间戳

   // 使用localtime_s转换时间戳为本地时间
   // 函数原型：errno_t localtime_s(struct tm* _tm, const time_t* _time);
   // 返回0表示成功，非0表示失败
   if (localtime_s(&localTime, &now) != 0)
   {
       return false;  // 转换失败
   }

   // 计算当前年月日（tm结构体的月份是0-based，需+1）
   *currentYear = localTime.tm_year + 1900;  // tm_year是自1900年的偏移量
   *currentMonth = localTime.tm_mon + 1;     // tm_mon范围是0-11，需+1
   *currentDay = localTime.tm_mday;          // tm_mday直接是1-31的日期

   return true;  // 获取成功
}

/**
 * @brief 判断是否为闰年
 * @param year 待判断的年份
 * @retval true-闰年, false-平年
 * 
 * 实现细节：
 * - 使用标准的闰年判断规则
 * - 能被4整除但不能被100整除，或者能被400整除
 * - 返回年份的闰年状态
 */
bool isLeapYear(int year)
{
   return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

/**
 * @brief 获取指定月份的天数
 * @param year 年份
 * @param month 月份
 * @retval 天数(无效月份返回-1)
 * 
 * 实现细节：
 * - 处理各个月份的天数差异
 * - 特别处理2月份的闰年情况
 * - 返回指定年份月份的天数
 */
int getDaysInMonth(int year, int month)
{
   switch (month)
   {
   case 1:case 3:case 5:case 7:case 8:case 10:case 12:
       return 31;
   case 4:case 6:case 9:case 11:
       return 30;
   case 2:
       return isLeapYear(year) ? 29 : 28;
   default:
       return -1; // 无效月份
   }
}

/**
 * @brief 从身份证提取出生日期字符串
 * @param idCard 身份证号码字符串
 * @param birthDate 返回出生日期的缓冲区(YYYYMMDD格式)
 * @param birthDateSize 缓冲区大小
 * @retval 无
 * 
 * 实现细节：
 * - 从身份证第7-14位提取出生日期
 * - 确保缓冲区足够大（至少9字节）
 * - 验证身份证号码长度
 * - 安全地复制日期字符串
 */
void getBirthDateFromIDCard(const char *idCard, char *birthDate, size_t birthDateSize)
{
   // 检查输入参数合法性
   if (idCard == NULL || birthDate == NULL || birthDateSize < 9)
   {
       return; // 输入无效，直接返回（也可根据需求设置错误标识）
   }

   // 检查身份证号长度至少为18位（避免越界访问）
   if (strlen(idCard) < 18)
   {
       return;
   }

   // 复制身份证第7-14位（索引6到13，共8个字符）到birthDate
   // strncpy_s参数：目标缓冲区、目标缓冲区大小、源字符串、要复制的字符数
   strncpy_s(birthDate, birthDateSize, idCard + 6, 8);

   // 确保字符串以'\0'结尾（strncpy_s在成功时会自动添加，但显式处理更安全）
   birthDate[8] = '\0';
}

/**
 * @brief 从身份证提取出生年月日
 * @param idCard 身份证号码字符串
 * @param birthYear 返回出生年份的指针
 * @param birthMonth 返回出生月份的指针
 * @param birthDay 返回出生日期的指针
 * @retval true-提取成功, false-提取失败
 * 
 * 实现细节：
 * - 调用getBirthDateFromIDCard获取日期字符串
 * - 将字符串转换为整数
 * - 拆分年、月、日分量
 * - 返回提取结果状态
 */
bool extractBirthDate(const char *idCard, int *birthYear, int *birthMonth, int *birthDay)
{
   // 检查输入参数合法性（避免NULL指针）
   if (idCard == NULL || birthYear == NULL || birthMonth == NULL || birthDay == NULL)
   {
       return false;
   }

   // 提取出生日期字符串（YYYYMMDD）
   char birthDate[9];
   getBirthDateFromIDCard(idCard, birthDate, sizeof(birthDate)); // 复用已有函数

   // 转换为数字并拆分年、月、日
   int birthDatenumber = atoi(birthDate);
   *birthYear = birthDatenumber / 10000;
   *birthMonth = (birthDatenumber / 100) % 100;
   *birthDay = birthDatenumber % 100;

   return true; // 提取成功
}

/**
 * @brief 验证身份证出生日期有效性
 * @param IDCard 身份证号码字符串
 * @retval true-日期有效, false-日期无效
 * 
 * 实现细节：
 * - 提取出生日期年、月、日
 * - 验证年份范围（1900-当前年份）
 * - 验证月份范围（1-12）
 * - 验证日期范围和各月份天数
 */
bool validateIDCardBirthDate(const char *IDCard)
{
   // 提取出生日期部分
   int BirthYear, BirthMonth, BirthDay;
   if (!extractBirthDate(IDCard, &BirthYear, &BirthMonth, &BirthDay))
   {
       return false; // 提取失败
   }

   // 获取当前年份
   int currentYear, currentMonth, currentDay;
   if(!getCurrentDate(&currentYear, &currentMonth, &currentDay))
   {
       return false; // 获取当前日期失败
   }

   // 简单的日期范围检查
   if (BirthYear < 1900 || BirthYear > currentYear)
   {
       return false;
   }
   if (BirthMonth < 1 || BirthMonth > 12)
   {
       return false;
   }
   if (BirthDay < 1 || BirthDay > 31)
   {
       return false;
   }

   // 闰年、月份天数验证
   int daysInMonth = getDaysInMonth(BirthYear, BirthMonth);
   if (BirthDay > daysInMonth)
   {
       return false;
   }

   return true;
}

/**
 * @brief 身份证号码综合验证
 * @param idCard 待验证的身份证号码字符串
 * @retval true-身份证有效, false-身份证无效
 * 
 * 实现细节：
 * - 依次调用各个验证函数
 * - 长度和字符验证
 * - 校验码验证
 * - 出生日期验证
 * - 全部通过才返回有效
 */
bool isValidIdCard(const char *idCard)
{
    if (!validateIDCardLengthAndChars(idCard))                  // 长度和字符验证
    {
        return false;
    }
    if (!validateIDCardCheckDigit(idCard))                      // 校验码验证
    {
       return false;
    }
    if (!validateIDCardBirthDate(idCard))                       // 出生日期验证
    {
       return false;
    }
    return true;
}

/**
 * @brief 根据身份证计算年龄
 * @param idCard 身份证号码字符串
 * @retval 年龄值(失败返回-1)
 * 
 * 实现细节：
 * - 提取出生日期
 * - 获取当前日期
 * - 计算年份差值
 * - 考虑月份和日期的精确调整
 */
int calculateAgeFromIDCard(const char *idCard)
{
   // 提取出生日期部分
   int birthYear, birthMonth, birthDay;
   if (!extractBirthDate(idCard, &birthYear, &birthMonth, &birthDay))
   {
       return -1; // 提取失败，返回-1表示错误
   }

   // 获取当前日期
   int currentYear, currentMonth, currentDay;
   if (!getCurrentDate(&currentYear, &currentMonth, &currentDay))
   {
       return -1; // 获取当前日期失败，返回-1表示错误
   }

   // 计算年龄
   int age = currentYear - birthYear;
   // 如果当前月份和日期在出生月份和日期之前，年龄减1
   if (currentMonth < birthMonth || (currentMonth == birthMonth && currentDay < birthDay))
   {
       age--;
   }
   return age;
}

/**
 * @brief 根据身份证判断性别
 * @param idCard 身份证号码字符串
 * @retval 性别字符串("男"或"女")
 * 
 * 实现细节：
 * - 检查输入参数合法性
 * - 验证身份证号码长度
 * - 提取第17位数字
 * - 根据奇偶性判断性别
 */
const char* getGenderFromIDCard(const char* idCard)
{
   // 1. 检查输入参数合法性
   if (idCard == NULL)
   {
       return "未知";
   }

   // 2. 检查身份证号长度（18位）
   if (strlen(idCard) != 18)
   {
       return "未知";
   }

   // 3. 提取第17位（索引16）并检查是否为数字
   char genderChar = idCard[16];
   if (!isdigit((unsigned char)genderChar))  // 确保是数字字符
   {
       return "未知";
   }

   // 4. 判断性别（奇数为男，偶数为女）
   int genderNum = genderChar - '0';
   return (genderNum % 2 == 0) ? "女" : "男";
}

// ========== 手机号验证函数实现 ==========

/**
 * @brief 手机号码长度和字符验证
 * @param phoneNumber 待验证的手机号码字符串
 * @retval true-手机号格式正确, false-手机号格式错误
 * 
 * 实现细节：
 * - 验证手机号码长度为11位
 * - 检查所有字符是否为数字
 * - 符合中国手机号码基本格式要求
 */
bool isValidPhoneNumber(const char *phoneNumber)
{
    if (phoneNumber == NULL)
    {
        return false;
    }
    // 长度应为11位
    if (strlen(phoneNumber) != 11)
    {
        return false;
    }
    // 全部应为数字
    for (int i = 0; i < 11; i++)
    {
        if (!isdigit(phoneNumber[i]))
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief 验证手机号段格式
 * @param segment 手机号段
 * @retval true-格式正确, false-格式错误
 * 
 * 实现细节：
 * - 验证号段长度范围（3-7位）
 * - 检查所有字符是否为数字
 * - 验证号段开头为1，第二位为3-9
 * - 符合中国手机号段分配规则
 */
bool validatePhoneSegment(const char *segment)
{
    if (segment == NULL) {
        return false;
    }
    
    int length = strlen(segment);
    if (length < 3 || length > 7) { // 号段长度3-7位
        return false;
    }
    
    // 全部应为数字
    for (int i = 0; i < length; i++) {
        if (!isdigit(segment[i])) {
            return false;
        }
    }
    
    // 号段验证(1开头，第二位为3-9)
    if (segment[0] != '1' || segment[1] < '3' || segment[1] > '9') {
        return false;
    }
    
    return true;
}

// ========== 输入处理函数实现 ==========

/**
 * @brief 清空输入缓冲区
 * @retval 无
 * 
 * 实现细节：
 * - 循环读取输入缓冲区直到清空
 * - 处理换行符和文件结束符
 * - 确保后续输入不受残留数据影响
 */
void clearInputBuffer()
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

// ========== 信息提取函数实现 ==========

/**
 * @brief 根据身份证前2位获取省份信息
 * @param idCard 身份证号码字符串
 * @retval 省份字符串
 * 
 * 实现细节：
 * - 提取身份证前2位省级代码
 * - 映射到对应的省份名称
 * - 支持全国所有省级行政区的识别
 * - 返回中文省份名称或"未知"
 */
const char* getProvinceFromIDCard(const char* idCard) {
    if (idCard == NULL || strlen(idCard) < 2) {
        return "未知";
    }

    // 提取身份证前2位（省级代码）
    char provinceCode[3] = { 0 };
    strncpy_s(provinceCode, sizeof(provinceCode), idCard, 2);

    // 省级代码映射
    if (strcmp(provinceCode, "11") == 0) return "北京市";
    if (strcmp(provinceCode, "12") == 0) return "天津市";
    if (strcmp(provinceCode, "13") == 0) return "河北省";
    if (strcmp(provinceCode, "14") == 0) return "山西省";
    if (strcmp(provinceCode, "15") == 0) return "内蒙古自治区";
    if (strcmp(provinceCode, "21") == 0) return "辽宁省";
    if (strcmp(provinceCode, "22") == 0) return "吉林省";
    if (strcmp(provinceCode, "23") == 0) return "黑龙江省";
    if (strcmp(provinceCode, "31") == 0) return "上海市";
    if (strcmp(provinceCode, "32") == 0) return "江苏省";
    if (strcmp(provinceCode, "33") == 0) return "浙江省";
    if (strcmp(provinceCode, "34") == 0) return "安徽省";
    if (strcmp(provinceCode, "35") == 0) return "福建省";
    if (strcmp(provinceCode, "36") == 0) return "江西省";
    if (strcmp(provinceCode, "37") == 0) return "山东省";
    if (strcmp(provinceCode, "41") == 0) return "河南省";
    if (strcmp(provinceCode, "42") == 0) return "湖北省";
    if (strcmp(provinceCode, "43") == 0) return "湖南省";
    if (strcmp(provinceCode, "44") == 0) return "广东省";
    if (strcmp(provinceCode, "45") == 0) return "广西壮族自治区";
    if (strcmp(provinceCode, "46") == 0) return "海南省";
    if (strcmp(provinceCode, "50") == 0) return "重庆市";
    if (strcmp(provinceCode, "51") == 0) return "四川省";
    if (strcmp(provinceCode, "52") == 0) return "贵州省";
    if (strcmp(provinceCode, "53") == 0) return "云南省";
    if (strcmp(provinceCode, "54") == 0) return "西藏自治区";
    if (strcmp(provinceCode, "61") == 0) return "陕西省";
    if (strcmp(provinceCode, "62") == 0) return "甘肃省";
    if (strcmp(provinceCode, "63") == 0) return "青海省";
    if (strcmp(provinceCode, "64") == 0) return "宁夏回族自治区";
    if (strcmp(provinceCode, "65") == 0) return "新疆维吾尔自治区";

    return "未知";
}

/*
 * 测试函数（已注释，供开发调试使用）
 * 
 * 功能说明：
 * - 提供身份证验证功能的测试用例
 * - 验证各个功能模块的正确性
 * - 开发阶段用于功能调试
 */
// int main(void){
//     // 测试身份证验证函数
//     const char *testID = "310104199211056720";      
//     if (isValidIdCard(testID))
//     {
//         printf("身份证号码 %s 有效。\n", testID);
//         int age = calculateAgeFromIDCard(testID);
//         const char *gender = getGenderFromIDCard(testID);
//         printf("年龄: %d, 性别: %s\n", age, gender); 
//     }
//     else
//     {
//         printf("身份证号码 %s 无效。\n", testID);
//     }
//     return 0;
// }