#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "user.h"

// 用户界面交互函数
void addUserInterface();
void searchUserInterface();
void modifyUserInterface();
void deleteUserInterface();
void showAllUsersInterface();
void displayUserDetailsInterface(int userIndex);
void registerPhoneForUserInterface(int userId);
void unregisterPhoneInterface();

// 辅助函数
void waitForAnyKey();
void safeStringInput(char* buffer, size_t size, const char* prompt);
int safeIntInput(const char* prompt, int min, int max);
void safeClearInputBuffer();
int makeSelection(const char *options[], int count, const char *title);

#endif