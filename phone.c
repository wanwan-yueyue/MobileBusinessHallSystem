/*
 * 文件名称：phone.c
 * 文件路径：.\MobileBusinessHallSystem\phone.c
 * 功能描述：手机号管理模块实现文件 - 实现手机号资源的初始化、绑定、解绑、查询等核心功能
 * 创建日期：2025-10-28
 * 版本信息：v2.1（适配全局变量管理）
 * 版权声明：© 2025 ，保留所有权利
 * 
 * 实现说明：
 * 1. 采用动态内存管理，支持手机号资源池的自动扩容，初始容量100，按需翻倍扩展；
 * 2. 实现一人多号功能，通过严格的状态检查和数量限制确保数据一致性；
 * 3. 集成utils模块的手机号格式验证，确保输入数据的合法性；
 * 4. 提供完整的错误处理机制，包括参数校验、内存分配失败处理等。
 * 5. 移除对全局变量的直接依赖，所有函数通过参数接收管理器指针
 * 依赖说明：
 * - 标准库：stdio.h、stdlib.h、string.h、stdbool.h、ctype.h、assert.h、time.h
 * - 自定义模块：utils.h（数据验证）、phone.h（数据结构定义）
 * 
 * 修改记录：
 * 2025-10-28  新增文件，实现基础手机号管理功能（v1.0）
 * 2025-10-29  重构代码结构，提取辅助函数，提高可维护性（v1.1）
 * 2025-10-29  优化内存管理，修复资源泄露问题（v1.2）
 * 2025-10-29  增强错误处理，完善参数校验逻辑（v1.3）
 * 2025-10-29  实现完整的文件操作接口，支持数据持久化（v2.0）
 * 2025-11-1   适配全局变量管理(v2.1)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>
#include "utils.h"
#include "phone.h"

/**
 * @brief 初始化单个手机号资源的所有字段（重置为初始状态）
 * @param resource: 指向需要初始化的PhoneResource结构体的指针
 * @retval 无
 * @date   2025-10-28
 */
static void initPhoneResource(PhoneResource *resource) {
    if (resource == NULL) {
        return;                                                 // 入参为空时直接返回，避免空指针访问
    }
    resource->phoneNumber[0] = '\0';                            // 清空手机号字符串（设置为空串）
    resource->status = PHONE_STATUS_FREE;                       // 设置状态为未分配
    resource->userId = -1;                                      // 设置用户ID为-1（未绑定）
    resource->assignTime[0] = '\0';                             // 清空分配时间字符串（设置为空串）
}

/**
 * @brief 初始化单个手机号资源的状态，用户ID，分配时间字段，保留手机号字符串不变
 * @param resource: 指向需要初始化的PhoneResource结构体的指针
 * @retval 无
 * @date   2025-10-29
 */
void resetPhoneResource(PhoneResource *resource) {
    if (resource == NULL) {
        return;                                                 // 入参为空时直接返回，避免空指针访问
    }
    resource->status = PHONE_STATUS_FREE;                       // 设置状态为未分配
    resource->userId = -1;                                      // 设置用户ID为-1（未绑定）
    resource->assignTime[0] = '\0';                             // 清空分配时间字符串（设置为空串）
}

/**
  * @brief  初始化手机号管理器
  * @param  无
  * @retval PhoneManager* 手机号管理器指针，失败返回NULL
  * @date   2025-10-28
  */

PhoneManager* initPhoneManager(void){
    // 分配手机号管理器内存
    PhoneManager *manager = (PhoneManager *)malloc(sizeof(PhoneManager));
    if (manager == NULL) {
        return NULL;                                            // 内存分配失败
    }

    // 分配手机号资源数组内存
    manager->phones = (PhoneResource *)malloc(sizeof(PhoneResource) 
        * INIT_PHONE_CAPACITY);                                 // 初始容量
    if (manager->phones == NULL) {
        free(manager);
        return NULL;                                            // 内存分配失败
    }

    // 初始化手机号管理器字段
    manager->count = 0;                                         // 当前数量为0
    manager->capacity = INIT_PHONE_CAPACITY;                    // 设置初始容量

    // 初始化手机号资源
    for (int i = 0; i < manager->capacity; i++) {
        initPhoneResource(&manager->phones[i]);                 // 初始化每个手机号资源
    }

    return manager;                                             // 返回初始化好的手机号管理器指针
}

/**
  * @brief  清理手机号管理器
  * @param  manager: 手机号管理器指针
  * @retval 无
  * @date   2025-10-28
  */
void cleanupPhoneManager(PhoneManager *manager){
    if(manager != NULL){
        if(manager->phones != NULL){
            free(manager->phones);                              // 释放手机号资源数组内存
        }
        free(manager);                                          // 释放手机号管理器内存
    }
}

/**
 * @brief 校验初始化手机号资源的输入参数
 * @param manager: 手机号管理器指针
 * @param startSegment: 起始号段字符串
 * @param num: 手机号数量
 * @retval 1-参数有效, 0-参数无效
 * @date   2025-10-28
 */
static int checkInitParams(PhoneManager *manager, const char *startSegment, int num) {
    // 检查指针是否为空，数量是否为正数
    return (manager != NULL && startSegment != NULL && num > 0) ? 1 : 0;
}

/**
 * @brief 检查并扩展手机号资源池容量
 * @param manager: 手机号管理器指针
 * @param requiredTotal: 需要的总容量（当前已有数量+新增数量）
 * @retval 1-扩展成功（或无需扩展）, 0-扩展失败（内存不足）
 * @date   2025-10-28
 */
static int expandPhoneCapacity(PhoneManager *manager, int requiredTotal) {
    // 若当前容量足够，直接返回成功
    if (requiredTotal <= manager->capacity) {
        return 1;
    }

    // 计算新容量（翻倍扩展，直到满足需求）
    int newCapacity = manager->capacity * 2;
    while (newCapacity < requiredTotal) {
        newCapacity *= 2;
    }

    // 重新分配内存
    PhoneResource *newPhones = (PhoneResource *)realloc(
        manager->phones, 
        sizeof(PhoneResource) * newCapacity
    );
    if (newPhones == NULL) {
        return 0; // 内存分配失败
    }

    // 更新管理器的资源指针和容量
    manager->phones = newPhones;
    manager->capacity = newCapacity;

    // 初始化新增的资源空间（从原count到新capacity）
    for (int i = manager->count; i < manager->capacity; i++) {
        initPhoneResource(&manager->phones[i]);                 // 初始化每个新增手机号资源
    }

    return 1;
}

/**
 * @brief 生成完整的11位手机号（基础号段+序列号）
 * @param baseNumber: 基础号段（前缀）
 * @param sequenceLength: 序列号长度
 * @param index: 序列号索引（用于生成递增序列号）
 * @param fullPhone: 输出参数，存储生成的完整手机号
 * @param fullPhoneSize: fullPhone的最大长度（防止溢出）
 * @retval 1-生成成功, 0-生成失败（长度不合法）
 * @date   2025-10-28
 */
static int generateFullPhoneNumber(const char *baseNumber, int sequenceLength, 
                                  int index, char *fullPhone, int fullPhoneSize) {
    // 校验基础号段+序列号的总长度是否为11位
    int baseLength = strlen(baseNumber);
    if (baseLength + sequenceLength != 11) {
        return 0;
    }

    // 生成固定长度的序列号（前导补0）
    char sequenceStr[sequenceLength + 1]; // +1预留结束符
    snprintf(sequenceStr, sizeof(sequenceStr), "%0*d", sequenceLength, index);

    // 拼接基础号段和序列号，生成完整手机号
    snprintf(fullPhone, fullPhoneSize, "%s%s", baseNumber, sequenceStr);

    // 最终校验手机号长度是否为11位
    return (strlen(fullPhone) == 11) ? 1 : 0;
}

/**
 * @brief 将完整手机号添加到资源池
 * @param manager: 手机号管理器指针
 * @param fullPhone: 完整的11位手机号
 * @retval 无
 * @date   2025-10-28
 */
static void addPhoneToResource(PhoneManager *manager, const char *fullPhone) {
    // 存入手机号
    strcpy(manager->phones[manager->count].phoneNumber, fullPhone);
    // 初始化状态（未分配、未绑定、无分配时间）
    resetPhoneResource(&manager->phones[manager->count]);
    // 资源数量+1
    manager->count++;
}

/**
 * @brief 初始化手机号资源
 * @param manager: 手机号管理器指针
 * @param startSegment: 起始号段字符串
 * @param num: 手机号数量
 * @retval 1-成功, 0-失败
 * @date   2025-10-28
 */
int initPhoneResources(PhoneManager *manager, const char *startSegment, int num) {
    // 校验输入参数
    if (!checkInitParams(manager, startSegment, num)) {
        return 0;
    }

    // 校验号段格式
    if (!validatePhoneSegment(startSegment)) {
        return 0;
    }

    // 检查并扩展容量（需要的总容量=已有数量+新增数量）
    int requiredTotal = manager->count + num;
    if (!expandPhoneCapacity(manager, requiredTotal)) {
        return 0;
    }

    // 生成并添加手机号
    char baseNumber[MAX_PHONE_LENGTH];
    strcpy(baseNumber, startSegment);
    int baseLength = strlen(baseNumber);
    int sequenceLength = 11 - baseLength;

    // 校验序列号长度是否合法（必须为正数，否则无法生成11位手机号）
    if (sequenceLength <= 0) {
        return 0;
    }

    // 循环生成num个手机号并添加到资源池
    for (int i = 0; i < num; i++) {
        char fullPhone[MAX_PHONE_LENGTH];
        if (!generateFullPhoneNumber(baseNumber, sequenceLength, i, fullPhone, sizeof(fullPhone))) {
            return 0; // 生成手机号失败
        }
        addPhoneToResource(manager, fullPhone);
    }

    return 1;
}


/**
  * @brief  号段放号
  * @param  manager: 手机号管理器指针
  * @param  startSegment: 起始号段字符串
  * @param  num: 手机号数量
  * @retval 1-成功, 0-失败
  * @date   2025-10-28
  */
int releasePhoneSegment(PhoneManager *manager, const char *startSegment, int num){
    // 复用初始化手机号资源函数
    return initPhoneResources(manager, startSegment, num);
}

/**
  * @brief  随机选号
  * @param  manager: 手机号管理器指针
  * @param  selectedPhone: 输出参数，存储选中的手机号
  * @retval 1-成功, 0-失败
  * @date   2025-10-28
  */
int selectRandomPhone(PhoneManager *manager, char *selectedPhone){
    if(manager == NULL || selectedPhone == NULL){
        return 0;                                               // 参数无效
    }

    // 统计可用的手机号数量
    int availableCount = 0;
    int *availableIndices = (int *)malloc(sizeof(int) 
        * manager->count);                                      // 存储可用手机号的索引
    
    if(availableIndices == NULL){
        return 0;                                               // 内存分配失败
    }

    // 收集所有可用手机号的索引
    for(int i = 0; i < manager->count; i++){
        if(manager->phones[i].status == PHONE_STATUS_FREE){
            availableIndices[availableCount++] = i;             // 记录可用手机号索引
        }
    }

    // 检查是否有可用手机号
    if(availableCount == 0){
        free(availableIndices);
        return 0;                                               // 无可用手机号
    }

    // 随机选择一个可用手机号
    srand((unsigned int)time(NULL));                            // 初始化随机数种子
    int selectedIndex = availableIndices[rand() 
        % availableCount];                                      // 随机选择索引
    strcpy(selectedPhone, 
        manager->phones[selectedIndex].phoneNumber);            // 复制选中的手机号

    free(availableIndices);                                     // 释放内存
    return 1;                                                   // 成功
}

/**
  * @brief  获取用户的手机号数量
  * @param  manager: 手机号管理器指针
  * @param  userID: 用户ID
  * @retval 手机号数量 
  * @date   2025-10-29
  */
int getUserPhoneCount(const PhoneManager *manager, int userID){
    if(manager == NULL || userID < 0){
        return 0;                                               // 参数无效
    }

    int count = 0;                                              // 统计手机号数量
    for(int i = 0; i < manager->count; i++){
        // 如果手机号绑定到该用户且状态为已分配
        if(manager->phones[i].userId == userID &&
           manager->phones[i].status == PHONE_STATUS_ASSIGNED){
            count++;                                            // 统计绑定的手机号数量
        }
    }

    return count;                                               // 返回手机号数量
}

/**
 * @brief 查找手机号在资源池中的索引
 * @param manager: 手机号管理器指针
 * @param phoneNumber: 待查找的手机号字符串
 * @retval 索引位置，未找到返回-1
 * @date   2025-10-29
 */
int findPhoneIndex(PhoneManager *manager, const char *phoneNumber){
    if(manager == NULL || phoneNumber == NULL){
        return -1;                                              // 参数无效
    }

    for(int i = 0; i < manager->count; i++){
        if(strcmp(manager->phones[i].phoneNumber, phoneNumber) == 0){
            return i;                                           // 找到匹配的手机号，返回索引
        }
    }

    return -1;                                                  // 未找到手机号
}

/**
  * @brief  绑定手机号到用户
  * @param  manager: 手机号管理器指针
  * @param  userId: 绑定的用户ID
  * @param  phoneNumber: 要绑定的手机号字符串
  * @retval 1-成功, 0-失败（号码已占用或超限）
  * @date   2025-10-29
  */
int registerPhone(PhoneManager *manager, int userID, const char *phoneNumber){
    if(manager == NULL || phoneNumber == NULL || userID < 0){
        return 0;                                               // 参数无效
    }

    // 使用utils.c中的手机号格式验证函数
    if(!isValidPhoneNumber(phoneNumber)){
        return 0;                                               // 手机号格式无效
    }

    // 检测用户绑定数量是否超限
    if(getUserPhoneCount(manager, userID) >= MAX_PHONE_PER_USER){
        return 0;                                               // 超过绑定上限
    }

    // 查找手机号索引
    int phoneIndex = findPhoneIndex(manager, phoneNumber);
    if(phoneIndex == -1){
        return 0;                                               // 手机号不存在
    }

    // 检查手机号是否可用
    if(manager->phones[phoneIndex].status != PHONE_STATUS_FREE){
        return 0;                                               // 手机号已被占用
    }

    // 绑定手机号到用户
    manager->phones[phoneIndex].status = PHONE_STATUS_ASSIGNED; // 设置状态为已分配
    manager->phones[phoneIndex].userId = userID;                // 绑定用户

    // 获取当前时间作为分配时间
    time_t now = time(NULL);
    struct tm *timeInfo;
    timeInfo = localtime(&now);
    strftime(manager->phones[phoneIndex].assignTime, 
        sizeof(manager->phones[phoneIndex].assignTime), 
        "%Y-%m-%d %H:%M:%S", timeInfo);                         // 格式化时间字符串

    return 1;                                                   // 绑定成功
}

/**
  * @brief  注销手机号
  * @param  manager: 手机号管理器指针
  * @param  userID: 用户ID
  * @param  phoneNumber: 手机号字符串
  * @retval 1-成功, 0-失败(号码未绑定或不属于该用户)
  * @date   2025-10-29
  */
int cancelPhone(PhoneManager *manager, int userID, const char *phoneNumber){
    if(manager == NULL || phoneNumber == NULL || userID < 0){
        return 0;                                               // 参数无效
    }

    int phoneIndex = findPhoneIndex(manager, phoneNumber);      // 查找手机号索引
    if(phoneIndex == -1){
        return 0;                                               // 手机号不存在
    }

    // 检查手机号是否属于该用户且状态为已分配
    if(manager->phones[phoneIndex].userId != userID || 
       manager->phones[phoneIndex].status != PHONE_STATUS_ASSIGNED){
        return 0;                                               // 号码未绑定或不属于该用户
    }

    // 注销手机号，重置为初始状态
    resetPhoneResource(&manager->phones[phoneIndex]);

    return 1;                                                   // 注销成功
}

/**
  * @brief  注销用户的所有手机号
  * @param  manager: 手机号管理器指针
  * @param  userID: 用户ID
  * @retval 注销的手机号数量
  * @date   2025-10-29
  */
int cancelAllUserPhones(PhoneManager *manager, int userID){
    if(manager == NULL || userID < 0){
        return 0;                                               // 参数无效
    }

    int cancelCount = 0;                                        // 统计注销的手机号数量
    for(int i = 0; i < manager->count; i++){
        // 如果手机号绑定到该用户且状态为已分配
        if(manager->phones[i].userId == userID &&
           manager->phones[i].status == PHONE_STATUS_ASSIGNED){
            // 注销手机号，重置为初始状态
            resetPhoneResource(&manager->phones[i]);
            cancelCount++;                                      // 增加注销计数
        }
    }

    return cancelCount;                                        // 返回注销的手机号数量
}

/**
  * @brief  检查手机号是否唯一（未被分配）
  * @param  manager: 手机号管理器指针
  * @param  phoneNumber: 手机号字符串
  * @retval true-唯一, false-已存在
  * @date   2025-10-29
  */
bool isPhoneUnique(PhoneManager *manager, const char *phoneNumber){
    if(manager == NULL || phoneNumber == NULL){
        return false;                                          // 参数无效
    }

    int phoneIndex = findPhoneIndex(manager, phoneNumber);     // 查找手机号索引
    if(phoneIndex == -1){
        return true;                                           // 手机号不存在，唯一
    }

    // 检查手机号状态是否为未分配
    return (manager->phones[phoneIndex].status == PHONE_STATUS_FREE);
}


/**
  * @brief  获取可用手机号资源总数量
  * @param  manager: 手机号管理器指针
  * @retval 手机号可用资源总数量
  * @date   2025-10-29
  */
int getAvailablePhoneCount(const PhoneManager *manager){
    if(manager == NULL){
        return 0;                                               // 参数无效
    }

    int count = 0;                                              // 统计可用手机号数量
    for(int i = 0; i < manager->count; i++){
        // 如果手机号状态为未分配
        if(manager->phones[i].status == PHONE_STATUS_FREE){
            count++;                                            // 统计未分配的手机号数量
        }
    }

    return count;                                               // 返回可用手机号数量
}

/**
  * @brief  获取用户的所有手机号
  * @param  manager: 手机号管理器指针
  * @param  userID: 用户ID
  * @param  phones: 返回手机号数组
  * @param  count: 返回手机号数量
  * @retval 无
  * @date   2025-10-29
  */
void getUserPhones(const PhoneManager *manager, int userID, 
                   char phones[][MAX_PHONE_LENGTH], int *count){

    if(manager == NULL || phones == NULL || count == NULL || userID < 0){
        *count = 0;                                             // 参数无效，数量置0
        return;
    }

    int phoneCount = 0;                                         // 统计用户手机号数量
    for(int i = 0; i < manager->count; i++){
        // 如果手机号绑定到该用户且状态为已分配
        if(manager->phones[i].userId == userID &&
           manager->phones[i].status == PHONE_STATUS_ASSIGNED){
            strcpy(phones[phoneCount], 
                manager->phones[i].phoneNumber);                // 复制手机号到输出数组
            phoneCount++;                                       // 增加手机号计数
        }
    }

    *count = phoneCount;                                        // 返回手机号数量
}

/**
  * @brief  获取指定索引的手机号资源
  * @param  manager: 手机号管理器指针
  * @param  index: 资源索引
  * @retval PhoneResource* 指向手机号资源的指针，失败返回NULL
  * @date   2025-10-29
  */
const PhoneResource* getPhoneResourceByIndex(const PhoneManager *manager, int index){
    if(manager == NULL || index < 0 || index >= manager->count){
        return NULL;                                            // 参数无效
    }

    return &manager->phones[index];                             // 返回指定索引的手机号资源指针
}

/**
  * @brief  保存手机号资源到文件
  * @param  manager: 手机号管理器指针
  * @param  fileName: 文件名
  * @retval 1-成功, 0-失败
  * @date   2025-10-28
  */

int savePhoneResource(const PhoneManager *manager, const char *fileName)
{
    if (manager == NULL || fileName == NULL) {
        return 0;
    }
    
    FILE *file = fopen(fileName, "wb");
    if (file == NULL) {
        return 0;
    }
    
    // 写入文件头：版本信息和数据量
    int version = 1; // 文件格式版本
    fwrite(&version, sizeof(int), 1, file);
    fwrite(&manager->count, sizeof(int), 1, file);
    fwrite(&manager->capacity, sizeof(int), 1, file);
    
    // 写入手机号资源数据
    for (int i = 0; i < manager->count; i++) {
        // 写入手机号
        fwrite(manager->phones[i].phoneNumber, sizeof(char), MAX_PHONE_LENGTH, file);
        // 写入状态
        fwrite(&manager->phones[i].status, sizeof(PhoneStatus), 1, file);
        // 写入用户ID
        fwrite(&manager->phones[i].userId, sizeof(int), 1, file);
        // 写入分配时间
        fwrite(manager->phones[i].assignTime, sizeof(char), 20, file);
    }
    
    fclose(file);
    return 1;
}

/**
  * @brief  从文件加载手机号资源(预留接口)
  * @param  manager: 手机号管理器指针
  * @param  fileName: 文件名
  * @retval 1-成功, 0-失败
  * @date   2025-10-28
  */

int loadPhoneResource(PhoneManager *manager, const char *fileName)
{
    if (manager == NULL || fileName == NULL) {
        return 0;
    }
    
    FILE *file = fopen(fileName, "rb");
    if (file == NULL) {
        return 0;
    }
    
    // 读取文件头
    int version, count, capacity;
    if (fread(&version, sizeof(int), 1, file) != 1 ||
        fread(&count, sizeof(int), 1, file) != 1 ||
        fread(&capacity, sizeof(int), 1, file) != 1) {
        fclose(file);
        return 0;
    }
    
    // 检查版本兼容性
    if (version != 1) {
        printf("不支持的手机号资源文件版本：%d\n", version);
        fclose(file);
        return 0;
    }
    
    // 确保管理器有足够的容量
    if (manager->capacity < capacity) {
        // 需要扩容
        PhoneResource *newPhones = (PhoneResource*)realloc(
            manager->phones, sizeof(PhoneResource) * capacity);
        if (newPhones == NULL) {
            fclose(file);
            return 0;
        }
        manager->phones = newPhones;
        manager->capacity = capacity;
        
        // 初始化新增的空间
        for (int i = manager->count; i < manager->capacity; i++) {
            initPhoneResource(&manager->phones[i]);
        }
    }
    
    // 读取手机号资源数据
    manager->count = count;
    for (int i = 0; i < count; i++) {
        // 读取手机号
        if (fread(manager->phones[i].phoneNumber, sizeof(char), MAX_PHONE_LENGTH, file) != MAX_PHONE_LENGTH) {
            fclose(file);
            return 0;
        }
        // 读取状态
        if (fread(&manager->phones[i].status, sizeof(PhoneStatus), 1, file) != 1) {
            fclose(file);
            return 0;
        }
        // 读取用户ID
        if (fread(&manager->phones[i].userId, sizeof(int), 1, file) != 1) {
            fclose(file);
            return 0;
        }
        // 读取分配时间
        if (fread(manager->phones[i].assignTime, sizeof(char), 20, file) != 20) {
            fclose(file);
            return 0;
        }
    }
    
    fclose(file);
    return 1;
}