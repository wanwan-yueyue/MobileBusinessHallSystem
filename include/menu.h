/*
 * 文件名称：menu.h
 * 文件路径：.\MobileBusinessHallSystem\menu.h
 * 功能描述：菜单界面模块头文件 - 定义系统界面样式和菜单交互功能
 *          提供统一的界面显示规范和用户交互接口
 * 作    者：
 * 创建日期：2025-10-29
 * 版本信息：v2.0（统一界面样式）
 * 版权声明：© 2025 | 保留所有权利
 * 
 * 实现说明：
 * 1. 定义丰富的ANSI颜色代码，支持多彩界面显示
 * 2. 提供统一的界面元素显示函数接口
 * 3. 支持菜单选择、信息显示、状态提示等功能
 * 4. 实现箭头键导航的交互式菜单系统
 * 
 * 依赖说明：
 * - 标准库：无特殊依赖
 * - 平台相关：Windows控制台API
 * 
 * 修改记录：
 * 2025-10-29  新增文件，定义基础菜单功能（v1.0）
 * 2025-10-30  扩展颜色定义，增加界面元素（v1.1）
 * 2025-10-30  统一界面样式，优化用户体验（v1.2）
 * 2025-11-4   重构函数接口，完善文档注释（v2.0）
 */

#ifndef MENU_H
#define MENU_H

// ========== 增强的 ANSI 颜色定义 ==========

/**
 * @brief 颜色和样式控制宏定义
 * 
 * 功能说明：
 * - 提供丰富的颜色选择，支持前景色和背景色
 * - 定义常用的界面元素样式
 * - 支持高亮显示和状态提示
 * 
 * 使用示例：
 * printf(RED "错误信息" RESET);
 * printf(HIGHLIGHT "选中项" RESET);
 */

// 基础颜色
#define RESET       "\033[0m"      // 重置所有样式
#define RED         "\033[1;31m"   // 粗体红色 - 用于错误信息
#define GREEN       "\033[1;32m"   // 粗体绿色 - 用于成功信息
#define YELLOW      "\033[1;33m"   // 粗体黄色 - 用于警告信息
#define BLUE        "\033[1;34m"   // 粗体蓝色 - 用于标题信息
#define MAGENTA     "\033[1;35m"   // 粗体洋红色 - 用于标签信息
#define CYAN        "\033[1;36m"   // 粗体青色 - 用于操作提示
#define WHITE       "\033[1;37m"   // 粗体白色 - 用于普通文本
#define GRAY        "\033[1;90m"   // 粗体灰色 - 用于辅助信息

// 背景色
#define BG_BLUE     "\033[44m"     // 蓝色背景
#define BG_CYAN     "\033[46m"     // 青色背景
#define BG_GREEN    "\033[42m"     // 绿色背景

// 高亮选择项
#define HIGHLIGHT   BG_CYAN WHITE  // 高亮样式（青色背景+白色文字）

// ========== 界面显示函数声明 ==========

/**
 * @brief 清空屏幕内容
 * @retval 无
 * 
 * 功能说明：
 * - 清除控制台所有显示内容
 * - 将光标移动到屏幕左上角
 * - 为新的界面显示准备干净画布
 */
void clearScreen();

/**
 * @brief 左侧对齐打印普通文本
 * @param text 要显示的文本内容
 * @retval 无
 * 
 * 功能说明：
 * - 在左侧添加固定缩进后显示文本
 * - 使用默认的白色文字颜色
 * - 适用于普通的说明文字和内容显示
 */
void printLeft(const char* text);

/**
 * @brief 左侧对齐打印彩色文本
 * @param text 要显示的文本内容
 * @param color 颜色控制码
 * @retval 无
 * 
 * 功能说明：
 * - 在左侧添加固定缩进后显示彩色文本
 * - 显示后自动重置颜色样式
 * - 适用于需要突出显示的重要内容
 */
void printLeftColor(const char* text, const char* color);

/**
 * @brief 打印分隔线
 * @retval 无
 * 
 * 功能说明：
 * - 显示一条横跨屏幕的分隔线
 * - 使用青色样式，增强视觉效果
 * - 用于区分不同的内容区块
 */
void printSeparator();

/**
 * @brief 打印区块标题
 * @param title 区块标题文字
 * @retval 无
 * 
 * 功能说明：
 * - 显示包含上下分隔线的区块标题
 * - 使用青色突出显示标题内容
 * - 添加适当的上下间距，增强可读性
 */
void printSectionTitle(const char* title);

/**
 * @brief 打印操作标题
 * @param title 操作标题文字
 * @retval 无
 * 
 * 功能说明：
 * - 专为操作界面设计的标题显示
 * - 与printSectionTitle功能相同，提供语义化接口
 * - 用于标识当前执行的具体操作
 */
void printOperationTitle(const char* title);

/**
 * @brief 打印菜单项
 * @param index 菜单项索引编号
 * @param text 菜单项显示文字
 * @param isSelected 是否为当前选中项
 * @retval 无
 * 
 * 功能说明：
 * - 显示带编号的菜单选项
 * - 选中项使用高亮样式突出显示
 * - 未选中项使用普通白色样式
 * - 支持箭头键导航的交互界面
 */
void printMenuItem(int index, const char* text, int isSelected);

// ========== 状态提示函数声明 ==========

/**
 * @brief 打印成功信息
 * @param message 成功信息内容
 * @retval 无
 * 
 * 功能说明：
 * - 使用绿色样式显示成功提示
 * - 添加成功图标前缀增强视觉效果
 * - 用于操作成功完成的状态反馈
 */
void printSuccess(const char* message);

/**
 * @brief 打印错误信息
 * @param message 错误信息内容
 * @retval 无
 * 
 * 功能说明：
 * - 使用红色样式显示错误提示
 * - 添加错误图标前缀引起注意
 * - 用于操作失败或异常情况的通知
 */
void printError(const char* message);

/**
 * @brief 打印警告信息
 * @param message 警告信息内容
 * @retval 无
 * 
 * 功能说明：
 * - 使用黄色样式显示警告提示
 * - 无特殊图标，保持简洁性
 * - 用于需要用户注意的提示信息
 */
void printWarning(const char* message);

// ========== 信息显示函数声明 ==========

/**
 * @brief 打印标签-值格式的信息
 * @param label 信息标签文字
 * @param value 信息值内容
 * @retval 无
 * 
 * 功能说明：
 * - 使用洋红色显示标签，默认色显示值
 * - 自动添加中文冒号分隔符
 * - 适用于属性-值的配对信息显示
 */
void printInfo(const char* label, const char* value);

/**
 * @brief 打印整数值信息
 * @param label 信息标签文字
 * @param value 整数值
 * @retval 无
 * 
 * 功能说明：
 * - 专为整数值设计的信息显示
 * - 标签使用默认白色，值也使用白色
 * - 适用于年龄、数量等数值信息
 */
void printInfoInt(const char* label, int value);

/**
 * @brief 打印洋红色整数值信息
 * @param label 信息标签文字
 * @param value 整数值
 * @retval 无
 * 
 * 功能说明：
 * - 标签使用洋红色，值使用默认白色
 * - 突出显示标签，弱化数值
 * - 适用于需要弱化数值显示的场合
 */
void printInfoIntMagenta(const char* label, int value);

// ========== 菜单系统函数声明 ==========

/**
 * @brief 显示系统启动标题和加载动画
 * @retval 无
 * 
 * 功能说明：
 * - 显示系统名称和版本信息
 * - 播放加载进度动画增强用户体验
 * - 为系统启动提供视觉反馈
 */
void showTitle();

/**
 * @brief 显示系统主菜单
 * @retval 无
 * 
 * 功能说明：
 * - 显示完整的系统功能菜单
 * - 支持箭头键导航和Enter键确认
 * - 提供ESC键快速退出功能
 * - 进入无限循环，直到用户选择退出
 */
void showMainMenu();

/**
 * @brief 处理菜单选择操作
 * @param choice 用户选择的菜单项索引
 * @retval 无
 *
 * 功能说明：
 * - 根据选择调用相应的功能模块
 * - 显示操作标题和执行结果
 * - 处理退出系统的特殊逻辑
 * - 在操作完成后提供返回提示
 */
void handleMenuChoice(int choice);

/**
 * @brief 回收站管理界面
 * @retval 无
 *
 * 功能说明：
 * - 显示已删除用户的完整信息
 * - 提供用户恢复和永久删除功能
 * - 管理回收站中的用户数据
 * - 支持批量操作和搜索功能
 */
void recycleBinInterface();

/**
 * @brief 创建选择界面
 * @param options 选项文字数组
 * @param count 选项数量
 * @param title 选择界面标题
 * @retval int 用户选择的选项索引，取消返回-1
 * 
 * 功能说明：
 * - 显示可滚动的选项列表
 * - 支持箭头键导航选择
 * - 提供ESC键取消选择功能
 * - 返回用户最终选择的选项索引
 */
int makeSelection(const char *options[], int count, const char *title);

// menu.h - 在文件末尾添加

/**
 * @brief 回收站管理界面
 * @retval 无
 *
 * 功能说明：
 * - 显示已删除用户的完整信息
 * - 提供用户恢复和永久删除功能
 * - 管理回收站中的用户数据
 * - 支持批量操作和搜索功能
 */
void recycleBinInterface();

#endif // MENU_H