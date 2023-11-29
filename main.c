//main.c
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include "kiosk.h"
#include <windows.h>

int loggedIn = 0; // 전역 변수로 선언

int main() {
    int x, y;
    int choice;


    // 주문 내역 및 환불 내역 파일로부터 데이터를 로드합니다.
    loadOrdersFromFile();
    loadRefundsFromFile();
    loadAdminCredentialsFromFile();

    do {
        clearScreen();
        printf("\n\n\n\n\n");


        if (loggedIn)
        {
            showRefundOption = 1; // 관리자 로그인 시 환불 옵션 표시
            adminMenu();
            UI(50, 28, 15, 3, 53, 29, "원하는 작업을 선택하세요.", "\033[0;36m");
            scanf("%d", &choice);
            switch (choice) {
            case 1:
                // 메뉴 관리 함수 호출
                adminMenu();
                break;
            case 2:
                // 주문 내역 확인
                printOrderSummary();
                break;
            case 3:
                // 환불 처리
                processRefund();
                break;
            case 4:
                // 로그아웃
                //logoutAsAdmin(); // 관리자 로그아웃
                loggedIn = 0;
                break;
            case 6:
                // 환불 내역 확인
                printRefundHistory();
                break;

            default:
                printf("올바른 옵션을 선택하세요.\n");
            }
        }
        else
        {
            showRefundOption = 0; // 관리자 로그아웃 시 환불 옵션 숨김
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

            UI(50, 28, 15, 3, 53, 29, "원하는 작업을 선택하세요.", "\033[0;36m");
            scanf("%d", &choice);
            logoutAsAdmin();
            switch (choice) {
            case 1:
                placeOrder();
                break;
            case 2:
                printOrderSummary();
                break;

            case 3:
                // 관리자 로그인
                if (choice == 3) {
                    displayAdminLoginScreen();
                    if (adminLogin()) {
                        loggedIn = 1;
                    }
                    else {
                        printf("로그인 실패. 올바른 아이디와 비밀번호를 입력하세요.\n");
                    }
                }
                break;
            case 4:
                // 프로그램 종료 전 주문 내역 및 환불 내역을 파일로 저장합니다.z
                saveOrdersToFile();
                saveRefundsToFile();
                exit(0);
            default:
                printf("올바른 옵션을 선택하세요.\n");
            }
        }

    } while (1);

    return 0;
}
