/*
 * 文件名称：data.c
 * 文件路径：.\MobileBusinessHallSystem\data.c
 * 功能描述：数据持久化模块实现文件 - 负责系统数据的初始化、加载、保存和恢复功能
 *          管理用户数据和手机号资源的文件存储与读取
 * 作    者：
 * 创建日期：2025-10-29
 * 版本信息：v2.2（适配全局变量管理）
 * 版权声明：© 2025 | 保留所有权利
 * 
 * 实现说明：
 * 1. 使用新的全局变量管理接口
 * 2. 用户数据和手机号资源分开存储
 * 3. 支持系统初始化和数据恢复的完整流程
 * 4. 提供数据完整性和一致性检查
 * 
 * 依赖说明：
 * - 标准库：stdio.h、stdlib.h、string.h
 * - 自定义模块：user.h、phone.h、data.h、global.h、menu.h
 * 
 * 修改记录：
 * 2025-10-29  重构文件，支持独立手机号管理模块（v2.0）
 * 2025-10-29  适配新的用户状态枚举，修复编译错误
 * 2025-10-29  彻底清空用户数据，修复身份证号重复检查问题
 * 2025-10-30  修改输出样式以适配统一界面（v2.1）
 * 2025-11-1   适配全局变量管理（v2.2）
 */

#include "data.h"
#include "menu.h" 
#include "global.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ========== 系统初始化函数 ==========

/**
 * @brief 初始化系统数据结构和手机号资源
 * @retval 无
 * 
 * 功能说明：
 * 1. 初始化全局变量和用户数组
 * 2. 检查并初始化默认手机号资源
 * 3. 创建默认号段并批量生成手机号
 * 4. 保存初始化的手机号资源到文件
 * 
 * 实现细节：
 * - 调用全局初始化函数准备用户数据结构
 * - 获取手机号管理器实例，确保管理器可用
 * - 如果手机号资源为空，批量初始化默认号段
 * - 保存初始化结果到数据文件
 */
void initSystem() {
    printLeft("系统初始化中...");
    
    // 初始化用户数组(使用全局初始化函数)
    initGlobalVariables();

    printSuccess("用户数组初始化完成");
    
    // 初始化手机号管理器
    PhoneManager* phoneMgr = getPhoneManager();
    if(phoneMgr == NULL){
        printError("手机号管理器初始化失败！");
        exit(1);
    }

    // 检查是否需要初始化默认号段
    if (phoneMgr->count == 0) {
        printLeft("初始化默认手机号资源...");
        
        // 定义所有默认号段
        const char* defaultSegments[] = {
            "130", "131", "132", "133", "134", "135", "136", "137", "138", "139",
            "145", "147", "149",
            "150", "151", "152", "153", "155", "156", "157", "158", "159",
            "165", "166", "167", 
            "170", "171", "172", "173", "175", "176", "177", "178",
            "180", "181", "182", "183", "185", "186", "187", "188", "189",
            "190", "191", "193", "195", "196", "199"
        };
        
        int segmentCount = sizeof(defaultSegments) / sizeof(defaultSegments[0]);
        
        // 使用批量初始化函数
        int totalPhones = batchInitPhoneResources(phoneMgr, defaultSegments, segmentCount, PHONES_PER_SEGMENT);
        
        if (totalPhones > 0) {
            printf(GREEN "    ✓ 批量初始化完成，总计 %d 个号码\n" RESET, totalPhones);
            
            // 立即保存初始化结果
            if (savePhoneResource(phoneMgr, "phoneData.dat")) {
                printSuccess("默认手机号资源保存成功");
            }
        } else {
            printError("手机号资源初始化失败！");
        }
    } else {
        printf(GREEN "    ✓ 使用已加载的手机号资源：%d 个号码\n" RESET, phoneMgr->count);
    }
}

// ========== 用户数据文件操作函数 ==========

/**
 * @brief 从文件读取用户数据到内存
 * @retval int 成功加载的用户数量
 * 
 * 功能说明：
 * 1. 打开用户数据文件进行二进制读取
 * 2. 初始化全局变量
 * 3. 读取活跃用户数据到用户数组
 * 4. 更新用户计数并返回结果
 * 
 * 实现细节：
 * - 使用安全文件打开函数防止文件操作错误
 * - 只读取状态为活跃的用户数据
 * - 维护准确的用户计数
 * - 提供详细的加载状态反馈
 */
int readData() {
    FILE* fp;
    errno_t err = fopen_s(&fp, "userData.dat", "rb");
    if (err != 0 || fp == NULL) {
        printWarning("无用户数据文件，将创建新文件");
        return 0;
    }

    // 初始化全局变量
    initGlobalVariables();
    
    int i = 0;
    User tempUser;
    // 读取用户数据直到文件末尾
    while (i < MAX_USERS && fread(&tempUser, sizeof(User), 1, fp) == 1) {
        if(tempUser.status == USER_ACTIVE) {
            users[i] = tempUser;
            userCount++;
            i++;
        }
    }   

    fclose(fp);
    
    if (userCount > 0) {
        printf(GREEN "    ✓ 成功加载 %d 个用户数据\n" RESET, userCount);
    } else {
        printWarning("无用户数据或加载失败");
    }
    
    return userCount;
}

/**
 * @brief 创建初始数据文件
 * @retval int 成功创建的用户数据数量
 * 
 * 功能说明：
 * 1. 创建用户数据文件
 * 2. 写入现有用户数据
 * 3. 保存手机号资源
 * 4. 返回处理的数据数量
 * 
 * 实现细节：
 * - 创建二进制数据文件用于存储用户信息
 * - 写入所有活跃用户数据
 * - 同时保存手机号资源确保数据一致性
 * - 提供创建结果的详细反馈
 */
int createData() {
    FILE* fp;
    errno_t err = fopen_s(&fp, "userData.dat", "wb");   
    if (err != 0 || fp == NULL) {
        printError("无法创建数据文件！");
        return 0;
    }

    printLeft("输入用户信息。");
    int n = readData();     // 读取现有数据
    
    // 将用户数据写入文件
    size_t written = fwrite(users, sizeof(User), n, fp);
    if (written != (size_t)n) {
        printError("数据写入不完整！");
        fclose(fp);
        return 0;
    }

    fclose(fp);
    
    // 同时保存手机号资源
    if (phoneManager != NULL) {
        if (savePhoneResource(phoneManager, "phoneData.dat")) {
            printSuccess("手机号资源保存成功");
        } else {
            printError("手机号资源保存失败");
        }
    }

    printf(GREEN "    ✓ 初始数据文件创建成功，包含 %d 位用户。\n" RESET, n);
    return n;
}

// ========== 手机号资源管理函数 ==========

/**
 * @brief 重新初始化手机号资源
 * @retval 无
 * 
 * 功能说明：
 * 1. 清理现有手机号资源
 * 2. 重新生成默认号段的手机号
 * 3. 显示初始化进度
 * 4. 保存重新初始化的资源
 * 
 * 实现细节：
 * - 完全重置手机号管理器
 * - 使用与系统初始化相同的默认号段配置
 * - 提供可视化的初始化进度指示
 * - 自动保存重新初始化的结果
 */
void reinitializePhoneResources() {
    PhoneManager* phoneMgr = getPhoneManager();
    if (phoneMgr == NULL) return;
    
    // 清理现有资源
    cleanupPhoneManager(phoneMgr);
    phoneMgr = initPhoneManager();
    
    if (phoneMgr != NULL) {
        // 使用与 initSystem 相同的默认号段配置
        const char* defaultSegments[] = {
            "130", "131", "132", "133", "134", "135", "136", "137", "138", "139",
            "145", "147", "149",
            "150", "151", "152", "153", "155", "156", "157", "158", "159",
            "165", "166", "167", 
            "170", "171", "172", "173", "175", "176", "177", "178",
            "180", "181", "182", "183", "185", "186", "187", "188", "189",
            "190", "191", "193", "195", "196", "199"
        };
        
        int segmentCount = sizeof(defaultSegments) / sizeof(defaultSegments[0]);
        int successCount = 0;
        int totalPhones = 0;
        
        // 显示初始化进度
        printf("    重新初始化进度: ");
        fflush(stdout);
        
        for (int i = 0; i < segmentCount; i++) {
            if (initPhoneResources(phoneMgr, defaultSegments[i], PHONES_PER_SEGMENT)) {
                successCount++;
                totalPhones += PHONES_PER_SEGMENT;
                printf(GREEN "■" RESET);
            } else {
                printf(YELLOW "□" RESET);
            }
            fflush(stdout);
        }
        printf("\n");
        
        printf(GREEN "    ✓ 重新初始化 %d 个号段，总计 %d 个号码\n" RESET, successCount, totalPhones);
        
        // 立即保存
        if (savePhoneResource(phoneMgr, "phoneData.dat")) {
            printSuccess("默认手机号资源保存成功");
        }
    }
}

// ========== 系统数据综合管理函数 ==========

/**
 * @brief 加载所有系统数据
 * @retval 无
 * 
 * 功能说明：
 * 1. 加载用户数据到内存
 * 2. 加载手机号资源到管理器
 * 3. 检查数据完整性
 * 4. 处理异常情况
 * 
 * 实现细节：
 * - 分别加载用户数据和手机号资源
 * - 检查手机号资源数量的合理性
 * - 在数据异常时自动重新初始化
 * - 提供详细的加载状态报告
 */
void loadData() {
    printSectionTitle("加载系统数据");
    
    // 加载用户数据
    int loadedUsers = readData();
    
    // 获取手机号管理器
    PhoneManager* phoneMgr = getPhoneManager();
    if (phoneMgr == NULL) {
        printError("手机号管理器初始化失败！");
        return;
    }
    
    // 先保存当前状态，用于比较
    int initialCount = phoneMgr->count;
    
    if (loadPhoneResource(phoneMgr, "phoneData.dat")) {
        int loadedCount = phoneMgr->count - initialCount;
        printSuccess("手机号资源加载成功");
        printf("    ✓ 加载前: %d 个号码, 加载后: %d 个号码, 新增: %d 个号码\n", 
               initialCount, phoneMgr->count, loadedCount);
        printf("    ✓ 可用手机号数量：%d\n", getAvailablePhoneCount(phoneMgr));
        
        // 如果加载的数量异常，重新初始化
        int expectedCount = 48 * PHONES_PER_SEGMENT;
        if (abs(phoneMgr->count - expectedCount) > 100) {
            printWarning("手机号资源数量异常，重新初始化默认资源");
            reinitializePhoneResources();
        }
    } else {
        printWarning("手机号资源加载失败或文件不存在，使用默认资源");
        // 只有在当前没有号码或数量异常时才重新初始化
        if (phoneMgr->count == 0 || phoneMgr->count < 100) {
            reinitializePhoneResources();
        } else {
            printf(GREEN "    ✓ 使用已初始化的手机号资源：%d 个号码\n" RESET, phoneMgr->count);
            // 保存当前资源以便下次使用
            if (savePhoneResource(phoneMgr, "phoneData.dat")) {
                printSuccess("当前手机号资源已保存");
            }
        }
    }
}

/**
 * @brief 保存所有系统数据
 * @retval 无
 * 
 * 功能说明：
 * 1. 保存用户数据到文件
 * 2. 保存手机号资源到文件
 * 3. 统计有效数据数量
 * 4. 提供保存结果反馈
 * 
 * 实现细节：
 * - 分别保存用户数据和手机号资源
 * - 只保存状态为活跃的用户数据
 * - 统计并报告保存的数据数量
 * - 处理保存过程中的异常情况
 */
void saveData() {
    printSectionTitle("保存系统数据");
    
    // 保存用户数据
    FILE* fp;
    errno_t err = fopen_s(&fp, "userData.dat", "wb");
    if (err != 0 || fp == NULL) {
        printError("保存失败，无法打开用户数据文件！");
        return;
    }
    
    // 统计有效用户数量
    int activeCount = 0;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].status == USER_ACTIVE) {
            activeCount++;
        }
    }
    
    // 写入有效用户数据
    size_t written = 0;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].status == USER_ACTIVE) {
            if (fwrite(&users[i], sizeof(User), 1, fp) != 1) {
                printError("用户数据写入失败！");
                fclose(fp);
                return;
            }
            written++;
        }
    }
    
    fclose(fp);
    
    // 保存手机号资源
    PhoneManager* phoneMgr = getPhoneManager();
    if (phoneMgr != NULL) {
        if (savePhoneResource(phoneMgr, "phoneData.dat")) {
            printf(GREEN "    ✓ 数据保存成功！用户数据：%d条，手机号资源：%d个\n" RESET, 
                   activeCount, phoneMgr->count);
        } else {
            printf(YELLOW "    ! 用户数据保存成功（%d条），但手机号资源保存失败！\n" RESET, activeCount);
        }
    } else {
        printf(YELLOW "    ! 用户数据保存成功（%d条），但手机号管理器未初始化！\n" RESET, activeCount);
    }
}