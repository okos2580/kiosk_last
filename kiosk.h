// kiosk.h
#include <windows.h>
#ifndef KIOSK_H
#define KIOSK_H
#define word 1024
extern int loggedIn;
extern int showRefundOption;

void GotoXY(int x, int y);
void adminMenu();
int adminLogin();
void displayCenteredText(char* text);
void clearInputBuffer();
void clearScreen();
void displayMenu();
void printOrderNumber(int orderNumber);
void printReceipt(struct Order order);
void placeOrder();
void printOrderSummary();
void processRefund();
void saveOrdersToFile();
void loadOrdersFromFile();
void saveRefundsToFile();
void loadRefundsFromFile();
void loadAdminFromFile(char* username, char* password);
void saveAdminToFile(char* username, char* password);
void loginAsAdmin();
void logoutAsAdmin();
void drawBox(int x, int y, int width, int height);
void UI(int x, int y, int width, double length, double text_x, double text_y, char* text[word], char* color[word]);

#endif 
