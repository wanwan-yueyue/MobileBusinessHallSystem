#ifndef MENU_H
#define MENU_H

// ========== 增强的 ANSI 颜色定义 ==========
#define RESET       "\033[0m"
#define RED         "\033[1;31m"
#define GREEN       "\033[1;32m"
#define YELLOW      "\033[1;33m"
#define BLUE        "\033[1;34m"
#define MAGENTA     "\033[1;35m"
#define CYAN        "\033[1;36m"
#define WHITE       "\033[1;37m"
#define GRAY        "\033[1;90m"

// 背景色
#define BG_BLUE     "\033[44m"
#define BG_CYAN     "\033[46m"
#define BG_GREEN    "\033[42m"

// 高亮选择项
#define HIGHLIGHT   BG_CYAN WHITE

// 菜单相关函数声明
void showTitle();
void showMainMenu();
void clearScreen();
void handleMenuChoice(int choice);
void printCenter(const char* text);
void printCenterColor(const char* text, const char* color);

#endif