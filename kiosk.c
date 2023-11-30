//kiosk.c
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "kiosk.h"
#include <windows.h>
#include <stdbool.h>

// 구조체 정의
struct MenuItem {
    int id;
    char name[50];
    float price;
};

struct Order {
    struct MenuItem item;
    int quantity;
    int paymentMethod; // 결제 방법을 나타내는 필드 (1: 카드 결제, 2: 현금 결제)
    int isRefunded; //
    char cardNumber[40]; // 카드 번호를 저장하는 필드
};

struct Refund {
    struct Order order;
    int refundAmount; // 환불 금액
};

// 아이디와 비밀번호를 저장할 구조체 정의
struct AdminCredentials {
    char username[20];
    char password[20];
};

// 전역 변수
struct MenuItem menu[] = {
    {1, "짜장면", 6000},
    {2, "짬뽕", 8000},
    {3, "탕수육", 12990},
    {4, "군만두", 5000}
};

char adminUsername[20];
char adminPassword[20];

struct Order orders[100];
int orderCount = 0;

struct Refund refunds[100]; // 환불 목록
int refundCount = 0;

// 함수 프로토타입
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
void saveAdminCredentialsToFile();
void loadAdminCredentialsFromFile();
bool checkAdminCredentials(const char* username, const char* password);
int adminLogin();
void printRefundHistory();
void saveMenuToFile();
void loadMenuFromFile();

void GotoXY(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
/*
Black  : \033[0;30m
Red    : \033[0;31m
Green  : \033[0;32m
Yellow : \033[0;33m
Blue   : \033[0;34m
Purple : \033[0;35m
Cyan   : \033[0;36m
White  : \033[0;37m
*/
//x. y: 박스의 좌표   width: 박스의 가로길이  length: 박스의 세로길이 
//text: 텍스트   text_x,text_y: 텍스트의 좌표 

void UI(int x, int y, int width, double length, double text_x, double text_y, char* text[word], char* color[word]) {
    //맨 위 박스
    printf("%s", color);
    width = width * 2;
    GotoXY(x, y);
    printf("┏");
    GotoXY(x + 1, y);
    for (int i = 0; i < width - 2; i++) {
        printf("━");
    }
    GotoXY(x + width - 1, y);
    printf("┓");
    //박스 옆 줄
    for (int j = 0; j <= length - 1; j++) {

        GotoXY(x, y + 1 + j / 2);
        printf("┃");
        for (int a = 0; a < width - 2; a++) {
            printf(" ");
        }
        printf("┃");
    }
    //맨 아래 박스
    GotoXY(x, y + length - 1);
    printf("┗");
    GotoXY(x + 1, y + length - 1);
    for (int i = 0; i < width - 2; i++) {
        printf("━");
    }
    GotoXY(x + width - 1, y + length - 1);
    printf("┛");
    //텍스트 출력
    GotoXY(text_x, text_y);
    printf("%s", text);
    printf("\033[0;37m");
}
// 메뉴 관리 함수
void manageMenu() {
    int choice;
    int i, id;
    char name[50];
    float price;

    do {
        clearScreen();
        UI(23, 5, 37, 30, 45, 7, "********** 메뉴 관리 **********", "\033[0;36m");
        printf("\033[0;36m");
        for (int j = 0; j <= 30; j++) {

            GotoXY(23, 18 + j / 2);
            printf("┃");
            for (int a = 0; a < 72; a++) {
                printf(" ");
            }
            printf("┃");
        }
        printf("\033[0;37m");
        UI(50, 10, 11, 3, 53, 11, "1. 메뉴 추가", "\033[0;36m");
        UI(50, 13, 11, 3, 53, 14, "2. 메뉴 수정", "\033[0;36m");
        UI(50, 16, 11, 3, 53, 17, "3. 메뉴 삭제", "\033[0;36m");
        UI(50, 19, 11, 3, 53, 20, "4. 종료", "\033[0;36m");
        UI(50, 23, 15, 3, 53, 24, "원하는 작업을 선택하세요.", "\033[0;36m");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            // 메뉴 추가
            printf("\033[0;36m");
            GotoXY(51, 26);
            printf("새로운 메뉴를 추가합니다.");
            printf("메뉴 ID: ");
            scanf("%d", &id);

            int found = 0; // 메뉴 ID 중복 확인을 위한 변수
            for (i = 0; i < sizeof(menu) / sizeof(menu[0]); i++) {
                if (menu[i].id == id) {
                    GotoXY(51, 28);
                    printf("이미 존재하는 메뉴 ID입니다. 다른 ID를 선택하세요.");
                    found = 1; // 메뉴 ID가 이미 존재함을 표시
                    break;
                }
            }

            if (found == 0) {
                printf("메뉴 이름: ");
                scanf(" %49[^\n]", name);
                printf("메뉴 가격: ");
                scanf("%f", &price);

                struct MenuItem newMenuItem;
                newMenuItem.id = id;
                strcpy(newMenuItem.name, name);
                newMenuItem.price = price;

                for (i = 0; i < sizeof(menu) / sizeof(menu[0]); i++) {
                    if (menu[i].id == 0) {
                        menu[i] = newMenuItem;
                        saveMenuToFile();
                        printf("새로운 메뉴가 추가되었습니다.\n");
                        break;
                    }
                }
            }

            break;

        case 2:
            // 메뉴 수정
            printf("\033[0;36m");
            GotoXY(51, 26);
            printf("메뉴를 수정합니다.");

            printf("수정할 메뉴 ID: ");
            scanf("%d", &id);


            for (i = 0; i < sizeof(menu) / sizeof(menu[0]); i++) {
                if (menu[i].id == id) {
                    GotoXY(33, 30);
                    printf("새로운 메뉴 이름: ");
                    scanf(" %49[^\n]", name);
                    GotoXY(60, 30);
                    printf("새로운 메뉴 가격: ");
                    scanf("%f", &price);

                    menu[i].id = id;
                    strcpy(menu[i].name, name);
                    menu[i].price = price;
                    saveMenuToFile();
                    GotoXY(51, 32);
                    printf("메뉴가 수정되었습니다.\n");
                    found = 1; // 메뉴를 찾았음을 표시
                    break;
                }
            }

            if (found == 0) {
                printf("메뉴 ID가 일치하는 메뉴를 찾을 수 없습니다.\n");
            }

            break;


        case 3:
            // 메뉴 삭제
            printf("\033[0;36m");
            GotoXY(51, 26);
            printf("메뉴를 삭제합니다.");
            printf("삭제할 메뉴 ID: ");
            scanf("%d", &id);

            for (i = 0; i < sizeof(menu) / sizeof(menu[0]); i++) {
                if (menu[i].id == id) {
                    menu[i].id = 0; // ID 0으로 설정하여 비활성화
                    saveMenuToFile();
                    printf("메뉴가 삭제되었습니다.\n");
                    break;
                }
            }

            if (i == sizeof(menu) / sizeof(menu[0])) {
                printf("메뉴 ID가 일치하는 메뉴를 찾을 수 없습니다.\n");
            }

            break;
        case 4:
            // 종료
            break;
        default:
            printf("올바른 옵션을 선택하세요.\n");
            break;
        }
        GotoXY(51, 28);
        printf("아무 키나 누르면 돌아갑니다...\n");
        getchar(); // 사용자가 아무 키나 누를 때까지 대기
        getchar(); // Enter 키 입력을 처리
    } while (choice != 4);
}
int showRefundOption = 0; // 초기에는 환불 처리 옵션 숨김

void firstUI() {
    UI(26, 5, 37, 30, 45, 7, " ********** 중국집 메뉴판 키오스크 **********", "\033[0;36m");
    printf("\033[0;36m");
    for (int j = 0; j <= 30; j++) {

        GotoXY(26, 18 + j / 2);
        printf("┃");
        for (int a = 0; a < 72; a++) {
            printf(" ");
        }
        printf("┃");
    }
    printf("\033[0;37m");
    UI(50, 10, 11, 3, 53, 11, "1. 주문하기", "\033[0;36m");
    UI(50, 13, 11, 3, 53, 14, "2. 주문 내역 확인", "\033[0;36m");
    UI(50, 16, 11, 3, 53, 17, "3. 관리자 로그인", "\033[0;36m");
    UI(50, 19, 11, 3, 53, 20, "4. 종료", "\033[0;36m");
}

// 로그인 함수
int adminLogin() {
    char adminUsername[20];
    char adminPassword[20];
    UI(30, 5, 30, 15, 53, 8, " 관리자 로그인", "\033[0;36m");
    printf("\033[0;36m");
    for (int j = 0; j <= 10; j++) {

        GotoXY(30, 13 + j / 2);
        printf("┃");
        for (int a = 0; a < 58; a++) {
            printf(" ");
        }
        printf("┃");
    }
    printf("\033[0;37m");
    UI(50, 10, 11, 3, 53, 11, " ID: ", "\033[0;36m");
    scanf("%s", adminUsername);
    printf("\n");
    UI(50, 13, 11, 3, 53, 14, " Password: ", "\033[0;36m");
    scanf("%s", adminPassword);

    if (checkAdminCredentials(adminUsername, adminPassword)) {
        printf("로그인 성공\n");
        return 1; // 로그인 성공
    }
    else {
        printf("로그인 실패\n");
        return 0; // 로그인 실패
    }
}


// 관리자 메뉴 함수
void adminMenu() {
    int choice;

    do {
        clearScreen();
        UI(23, 5, 37, 30, 45, 7, "********** 관리자 메뉴 **********", "\033[0;36m");
        printf("\033[0;36m");
        for (int j = 0; j <= 30; j++) {

            GotoXY(23, 18 + j / 2);
            printf("┃");
            for (int a = 0; a < 72; a++) {
                printf(" ");
            }
            printf("┃");
        }
        printf("\033[0;37m");
        UI(50, 10, 11, 3, 53, 11, "1. 메뉴 관리", "\033[0;36m");
        UI(50, 13, 11, 3, 53, 14, "2. 주문 내역 확인", "\033[0;36m");
        if (showRefundOption) { // 관리자 로그인 시 환불 옵션 표시
            UI(50, 16, 11, 3, 53, 17, "3. 환불 처리", "\033[0;36m");
        }

        UI(50, 19, 11, 3, 53, 20, "4. 로그아웃", "\033[0;36m");
        UI(50, 22, 11, 3, 53, 23, "5. 종료", "\033[0;36m");
        UI(50, 28, 15, 3, 53, 29, "원하는 작업을 선택하세요.", "\033[0;36m");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            // 메뉴 관리 함수 호출
            manageMenu();
            break;
        case 2:
            // 주문 내역 확인
            printOrderSummary();
            break;
        case 3:
            if (showRefundOption) { // 관리자 로그인 상태에서만 환불 처리 가능
                // 환불 처리
                processRefund();
            }
            else {
                printf("권한이 없습니다.\n");
            }
            break;
        case 4:
            // 로그아웃
            logoutAsAdmin();
            return;
        case 5:
            exit(0); // 프로그램 종료
        default:
            printf("올바른 옵션을 선택하세요.\n");
        }
    } while (choice != 5);
}
// 아래의 함수를 호출하여 관리자로 로그인하고 로그아웃합니다.
void loginAsAdmin() {
    // 관리자 로그인 시, showRefundOption을 1로 설정하여 환불 처리 옵션을 표시
    showRefundOption = 1;
}

void logoutAsAdmin() {
    // 관리자 로그아웃 시, showRefundOption을 0으로 설정하여 환불 처리 옵션을 숨김
    showRefundOption = 0;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

// clearScreen 함수 정의
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


void displayMenu() {
    clearScreen();
    printf("\n");
    UI(26, 5, 37, 30, 45, 7, " ********** 중국집 메뉴판 **********\n", "\033[0;36m");
    printf("\033[0;36m");
    for (int j = 0; j <= 30; j++) {

        GotoXY(26, 18 + j / 2);
        printf("┃");

        for (int a = 0; a < 72; a++) {
            printf(" ");
        }
        printf("┃");

    }
    printf("\033[0;37m");
    UI(38, 10, 24, 3, 42, 11, " 번호   음식명               가격(원)\n\n ", "\033[0;36m");

    for (int i = 0; i < sizeof(menu) / sizeof(menu[0]); i++) {
        printf("\033[0;36m");
        GotoXY(44, 15 + i);
        printf("%-5d %-20s %-10.0f\n", menu[i].id, menu[i].name, menu[i].price);
    }


}

void printOrderNumber(int orderNumber) {
    clearScreen();
    printf("\n\n\n\n");
    UI(26, 5, 35, 18, 45, 7, " ********** 주문 순번표 **********\n", "\033[0;36m");
    printf("\033[0;36m");
    for (int j = 0; j <= 18; j++) {

        GotoXY(26, 12 + j / 2);
        printf("┃");
        for (int a = 0; a < 68; a++) {
            printf(" ");
        }
        printf("┃");
    }
    printf("\033[0;37m");
    UI(50, 10, 11, 3, 53, 11, "주문번호:", "\033[0;36m");
    printf("%d\n\n", orderNumber);
    for (int i = 0; i < orderCount; i++) {
        struct Order order = orders[i];
        printf("\033[0;36m");
        GotoXY(52, 14 + i);
        printf("%d. %s x %d\n\n", i + 1, order.item.name, order.quantity);
    }

    printf("\n");
}

void printReceipt(struct Order order) {
    clearScreen();
    printf("\n\n\n\n");
    UI(26, 5, 35, 18, 45, 7, " ********** 영수증 **********\n", "\033[0;36m");
    printf("\033[0;36m");
    for (int j = 0; j <= 18; j++) {

        GotoXY(26, 12 + j / 2);
        printf("┃");
        for (int a = 0; a < 68; a++) {
            printf(" ");
        }
        printf("┃");
    }
    printf("\033[0;36m");
    GotoXY(43, 10);
    printf("음식명: %s\n\n", order.item.name);
    GotoXY(43, 12);
    printf("수량: %d\n\n", order.quantity);
    GotoXY(43, 14);
    printf("총 가격(원): %.0f\n\n", order.item.price * order.quantity);
    GotoXY(43, 16);
    printf("결제 방법: %s\n\n", order.paymentMethod == 1 ? "카드 결제" : "현금 결제");

    if (order.paymentMethod == 1) {
        printf("                                           카드 번호: %s\n", order.cardNumber);
    }

    printf("\n");
}

void placeOrder() {
    int choice, quantity;

    while (1) {
        displayMenu(); // 메뉴 표시

        UI(37, 21, 25, 3, 39, 22, "주문할 메뉴 번호를 선택하세요 (0으로 종료): ", "\033[0;36m");
        scanf("%d", &choice);

        if (choice == 0) {
            break;
        }

        if (choice < 1 || choice > sizeof(menu) / sizeof(menu[0])) {
            printf("올바른 메뉴 번호를 입력하세요.\n");
            continue;
        }

        UI(37, 24, 18, 3, 39, 25, "수량을 입력하세요: ", "\033[0;36m");
        scanf("%d", &quantity);

        struct Order newOrder;
        newOrder.item = menu[choice - 1];
        newOrder.quantity = quantity;

        // 결제 방법 선택
        UI(37, 27, 30, 3, 39, 28, "결제 방법을 선택하세요 (1. 카드 결제, 2. 현금 결제): ", "\033[0;36m");
        scanf("%d", &newOrder.paymentMethod);

        if (newOrder.paymentMethod == 1) {
            // 카드 결제일 경우 카드 번호 입력
            UI(37, 30, 28, 3, 39, 31, "카드 번호를 입력하세요: ", "\033[0;36m");
            scanf(" %39[0-9 ]", newOrder.cardNumber);
        }

        orders[orderCount++] = newOrder;

        printf("%s %d개를 주문했습니다. (총 가격: %.0f원)\n", newOrder.item.name, newOrder.quantity, newOrder.item.price * newOrder.quantity);

        // 순번표 출력
        printOrderNumber(orderCount);

        // 영수증 출력 여부 확인
        int isValidChoice = 0;

        while (!isValidChoice) {
            UI(38, 17, 26, 3, 40, 18, "영수증을 출력하시겠습니까? (1: 예, 2: 아니오): ", "\033[0;36m");
            scanf("%d", &choice);

            if (choice == 1) {
                printReceipt(newOrder);
                isValidChoice = 1;
            }
            else if (choice == 2) {
                isValidChoice = 1;
                break; // "2"를 입력하면 루프 종료
            }
            else {
                printf("올바른 옵션을 선택하세요 (1 또는 2).\n");
            }
        }

        if (choice == 1) {
            // 영수증을 출력한 후에 주문창으로 돌아가기 위해 아래 라인 추가
            GotoXY(43, 18);
            printf("아무 키나 누르면 돌아갑니다...\n");
            getchar(); // 사용자가 아무 키나 누를 때까지 대기
            getchar(); // Enter 키 입력을 처리
        }

        // 입력 버퍼 비우기
        clearInputBuffer();
    }
    // 주문을 추가한 후 주문 내역을 파일에 저장합니다.
    saveOrdersToFile();
}

void printOrderSummary() {
    clearScreen(); // 화면을 지우고
    displayMenu(); // 메뉴를 다시 표시합니다.

    printf("\n********** 주문 내역 **********\n\n");
    printf("%-20s %-10s %-10s %-20s %-20s\n", "음식명", "수량", "총 가격(원)", "결제 방법", "카드 번호");
    printf("ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ\n");

    for (int i = 0; i < orderCount; i++) {
        struct Order order = orders[i];
        printf("%-20s %-10d %-11.0f %-20s ", order.item.name, order.quantity, order.item.price * order.quantity,
            order.paymentMethod == 1 ? "카드 결제" : "현금 결제");

        if (order.paymentMethod == 1) {
            // 카드 결제인 경우 카드 번호 출력
            printf("%s", order.cardNumber);
        }
        else {
            // 현금 결제인 경우 공백 출력
            printf("                    "); // 20자리 공백
        }

        printf("\n");
    }

    printf("ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ\n");
    printf("                                               아무 키나 누르면 돌아갑니다...\n");
    getchar(); // 사용자가 아무 키나 누를 때까지 대기
    getchar(); // Enter 키 입력을 처리
}

void processRefund() {
    if (orderCount == 0) {
        printf("환불할 주문이 없습니다.\n");
        return;
    }

    clearScreen();
    UI(26, 5, 35, 18, 45, 7, " ********** 환불 **********\n", "\033[0;36m");
    printf("\033[0;36m");
    for (int j = 0; j <= 18; j++) {
        GotoXY(26, 12 + j / 2);
        printf("┃");
        for (int a = 0; a < 68; a++) {
            printf(" ");
        }
        printf("┃");
    }
    printf("\033[0;36m");
    GotoXY(50, 10);
    printf("주문 내역:\n");

    // 출력된 주문 내역을 저장할 배열
    struct Order displayedOrders[100];
    int displayedOrderCount = 0;

    for (int i = 0; i < orderCount; i++) {
        struct Order order = orders[i];
        GotoXY(50, 12 + i);
        printf("%d. %s x %d\n", i + 1, order.item.name, order.quantity);

        // 환불된 주문은 표시하지 않기 위해 조건 추가
        if (order.quantity > 0) {
            displayedOrders[displayedOrderCount++] = order;
        }
    }

    int refundChoice;
    GotoXY(50, 12 + orderCount);
    printf("환불할 주문 번호를 선택하세요 (0으로 종료): ");
    scanf("%d", &refundChoice);

    if (refundChoice == 0) {
        return;
    }

    if (refundChoice < 1 || refundChoice > displayedOrderCount) {
        printf("올바른 주문 순번표를 입력하세요.\n");
        return;
    }

    struct Order refundedOrder = displayedOrders[refundChoice - 1];

    int refundAmount;
    GotoXY(50, 14 + orderCount);
    printf("환불할 금액을 입력하세요 (최대 %.0f원): ", refundedOrder.item.price * refundedOrder.quantity);
    scanf("%d", &refundAmount);

    if (refundAmount <= 0 || refundAmount > refundedOrder.item.price * refundedOrder.quantity) {
        printf("올바른 환불 금액을 입력하세요.\n");
        return;
    }

    // 환불 처리
    if (refundedOrder.paymentMethod == 1) {
        // 카드 결제인 경우 카드 번호 확인
        if (strcmp(refundedOrder.cardNumber, "0") != 0) {
            // 카드 번호가 있는 경우
            printf("\n카드 번호: %s\n", refundedOrder.cardNumber);
        }
        else {
            // 카드 번호가 없는 경우 (0으로 설정된 경우)
            printf("환불 실패: 카드 번호가 없습니다.\n");
            return;
        }
    }

    // 환불 정보를 저장하고 환불 금액을 차감합니다.
    struct Refund refund;
    refund.order = refundedOrder;
    refund.refundAmount = refundAmount;
    refunds[refundCount++] = refund;

    refundedOrder.quantity -= refundAmount / refundedOrder.item.price;

    printf("환불이 완료되었습니다.\n");

    // 환불 처리 후 주문 내역을 파일에 저장합니다.
    saveOrdersToFile();
    // 환불 처리 후 환불 내역을 파일에 저장합니다.
    saveRefundsToFile();

 

    // 환불 내역 출력
    printRefundHistory();
}

void loadAdminCredentialsFromFile() {
    FILE* adminFile = fopen("admin.txt", "r");
    if (adminFile == NULL) {
        printf("관리자 정보 파일을 열 수 없습니다.\n");
        return;
    }

    struct AdminCredentials adminCredentials;
    if (fscanf(adminFile, "%s %s", adminCredentials.username, adminCredentials.password) == 2) {
        // 파일에서 읽은 정보를 전역 변수로 설정
        strcpy(adminUsername, adminCredentials.username);
        strcpy(adminPassword, adminCredentials.password);
    }

    fclose(adminFile);
}


// checkAdminCredentials 함수 추가
bool checkAdminCredentials(const char* enteredUsername, const char* enteredPassword) {
    // 전역 변수로 설정된 아이디와 비밀번호와 입력된 정보를 비교하여 일치하는지 확인
    return (strcmp(enteredUsername, adminUsername) == 0 && strcmp(enteredPassword, adminPassword) == 0);
}

void saveOrdersToFile() {
    FILE* file = fopen("orders.txt", "w");
    if (file == NULL) {
        printf("주문 내역 파일을 열 수 없습니다.\n");
        return;
    }

    for (int i = 0; i < orderCount; i++) {
        struct Order order = orders[i];
        fprintf(file, "%d %s %d %d %s\n", order.item.id, order.item.name, order.quantity, order.paymentMethod, order.cardNumber);
    }

    fclose(file);
}

void loadOrdersFromFile() {
    FILE* file = fopen("orders.txt", "r");
    if (file == NULL) {
        printf("주문 내역 파일을 열 수 없습니다.\n");
        return;
    }

    while (fscanf(file, "%d %s %d %d %s", &orders[orderCount].item.id, orders[orderCount].item.name,
        &orders[orderCount].quantity, &orders[orderCount].paymentMethod, orders[orderCount].cardNumber) == 5) {
        orderCount++;
    }

    fclose(file);
}

void printRefundHistory() {
    clearScreen();
    UI(26, 5, 35, 18, 45, 7, " ********** 환불 내역 **********\n", "\033[0;36m");
    printf("\033[0;36m");
    for (int j = 0; j <= 18; j++) {
        GotoXY(26, 12 + j / 2);
        printf("┃");
        for (int a = 0; a < 68; a++) {
            printf(" ");
        }
        printf("┃");
    }
    printf("\033[0;36m");
    GotoXY(50, 12);
    printf("%-20s %-10s\n", "음식명", "환불 금액(원)");
    GotoXY(28, 13);
    printf("ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ\n");

    for (int i = 0; i < refundCount; i++) {
        printf("\033[0;36m");
        GotoXY(50, 14);
        struct Refund refund = refunds[i];
        printf("%-20s %-10d\n", refund.order.item.name, refund.refundAmount);
    }
    GotoXY(28, 16);
    printf("ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ\n");
    GotoXY(50, 19);
    printf("아무 키나 누르면 돌아갑니다...\n");
    getchar(); // 사용자가 아무 키나 누를 때까지 대기
    getchar(); // Enter 키 입력을 처리
}

void saveRefundsToFile() {
    FILE* file = fopen("refunds.txt", "w");
    if (file == NULL) {
        printf("환불 내역 파일을 열 수 없습니다.\n");
        return;
    }

    for (int i = 0; i < refundCount; i++) {
        struct Refund refund = refunds[i];
        fprintf(file, "%d %s %d\n", refund.order.item.id, refund.order.item.name, refund.refundAmount);
    }

    fclose(file);
}

void loadRefundsFromFile() {
    FILE* file = fopen("refunds.txt", "r");
    if (file == NULL) {
        printf("환불 내역 파일을 열 수 없습니다.\n");
        return;
    }

    refundCount = 0; // 기존 환불 내역 초기화

    while (fscanf(file, "%d %s %d", &refunds[refundCount].order.item.id, refunds[refundCount].order.item.name,
        &refunds[refundCount].refundAmount) != EOF) {
        refundCount++;
    }

    fclose(file);
}

// 메뉴 데이터를 파일에 저장하는 함수
void saveMenuToFile() {
    FILE* file = fopen("menu.txt", "w");
    if (file == NULL) {
        printf("메뉴 데이터 파일을 열 수 없습니다.\n");
        return;
    }

    for (int i = 0; i < sizeof(menu) / sizeof(menu[0]); i++) {
        if (menu[i].id != 0) {
            fprintf(file, "%d %s %.2f\n", menu[i].id, menu[i].name, menu[i].price);
        }
    }

    fclose(file);
}

// 메뉴 데이터를 파일에서 읽어오는 함수
void loadMenuFromFile() {
    FILE* file = fopen("menu.txt", "r");
    if (file == NULL) {
        printf("메뉴 데이터 파일을 열 수 없습니다.\n");
        return;
    }

    // 파일에서 읽은 데이터로 메뉴 배열 초기화
    for (int i = 0; i < sizeof(menu) / sizeof(menu[0]); i++) {
        if (fscanf(file, "%d %49[^\n] %f", &menu[i].id, menu[i].name, &menu[i].price) != 3) {
            // 파일 읽기 실패 시 초기화
            menu[i].id = 0;
            menu[i].name[0] = '\0';
            menu[i].price = 0.0;
        }
    }

    fclose(file);
}

void displayAdminLoginScreen() {
    clearScreen();


    printf("\n");

}
