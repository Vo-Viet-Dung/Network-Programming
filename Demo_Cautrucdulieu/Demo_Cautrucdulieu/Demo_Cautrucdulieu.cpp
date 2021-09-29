#include<string.h>
#include <stdio.h>
#include <math.h>
#define MAX 256

char command[256];
char param[256];
char str[256];
int Param;
int n = 4; //Khoi tao so quay thu ngan ban dau = 4
typedef struct customer {
    int id;
    int line;
}customer;
//mang hai chieu Line[lineid][customerId]
int Line[MAX][MAX];
// moi mot phan tu mang lineIndex[lineId] chinh la so khach hang hien dang cho o line tuong ung 
int lineIndex[MAX];
int customerId = 1;
int lineEnter = 0;

/// <summary>
/// Them 1 khach hang vao quay thu ngan theo round robin
/// </summary>
void Enter() {
    lineEnter++;
    if (lineEnter > n) {
        lineEnter = 1;
    }
    while (Line[lineEnter][0] == -1) {
        lineEnter++;
    }
    if (lineEnter > n) {
        lineEnter = 1;
    }
    customer a;
    a.id = customerId;
    a.line = lineEnter;
    Line[lineEnter][lineIndex[lineEnter]] = a.id;
    //tang so khach hang hien co cua hang do len 1 de chuan bi sap nguoi tiep theo
    lineIndex[lineEnter]++;
    customerId++;
    printf("%d %d\n", a.id, a.line);
}

/// <summary>
/// Them 1 khach hang vao quay thu ngan theo phuong phap toi uu
/// </summary>
void AdvancedEnter() {
    customer b;
    int MIN = 9999;
    int i;
    int lineAdvance;
    for (i = 1; i <= n; i++)
    {
        if (Line[i][0] != -1 && lineIndex[i] < MIN) {
            MIN = lineIndex[i];
            lineAdvance = i;
        }
    }
    b.id = customerId;
    b.line = lineAdvance;
    Line[lineAdvance][lineIndex[lineAdvance]] = b.id;
    lineIndex[lineAdvance]++;
    customerId++;
    printf("%d %d\n", b.id, b.line);
}

/// <summary>
/// Thanh toan cho khach hang dau tien tai quay duoc nhap vao
/// </summary>
/// <param name="line"> Nhap vao ma so cua quay thu ngan muon thanh toan </param>
void CheckOut(int line) {
    if (lineIndex[line] == 0) {
        printf("Error\n");
    }
    else {
        printf("%d\n", Line[line][0]);
        int i;
        for (i = 0;i < lineIndex[line] - 1; i++) {
            Line[line][i] = Line[line][i + 1];
        }
        lineIndex[line] -= 1;
    }
}

/// <summary>
/// Dem so khach hang hien co trong 1 quay thu ngan
/// </summary>
/// <param name="line">Nhap vao ma so cua quay thu ngan muon thanh toan</param>
void CountNumberCustomerInLine(int line) {
    printf("%d\n", lineIndex[line]);
}

/// <summary>
/// Liet ke cac khach hang dang co trong 1 quay thu ngan
/// </summary>
/// <param name="line">Nhap vao ma so cua quay thu ngan muon thanh toan</param>
void ListCustomerInLine(int line) {
    if (lineIndex[line] == 0) {
        printf("Error\n");
    }
    else {
        int i;
        for (i = 0;i < lineIndex[line]; i++) {
            printf("%d ", Line[line][i]);
        }
        printf("\n");
    }

}
/// <summary>
/// Dem tat ca so khac dang co trong cac quay thu ngan
/// </summary>
void CountNumberCustomerInAllLines() {
    int countCustomer = 0;
    int i;
    for (i = 1; i <= n;i++) {
        countCustomer += lineIndex[i];
    }
    printf("%d \n", countCustomer);
}

/// <summary>
/// Them mot quay thu ngan moi
/// </summary>
void OpenCashier() {
    n += 1;
    //    if(lineEnter == (n-1)) lineEnter = n;
    printf("%d\n", n);
}

/// <summary>
/// Dong mot quay thu ngan
/// </summary>
/// <param name="line">Nhap vao ma so cua quay thu ngan muon thanh toan</param>
void CloseCashier(int line) {
    //for (int i = line + 1; i <= n; i++) {
        /*for (int j = 0; j < lineIndex[line];j++) {

        }*/
    int countLine = 0;
    int l;
    for (l = 1; l <= n; l++) {
        if (Line[l][0] == -1) continue;
        else if (Line[l][0] != -1) {
            countLine += 1;
        }
    }
    if (countLine >= 2) {
        if (lineIndex[line] > 0) {
            int j = 0;
            int i = lineEnter;
            while (j < lineIndex[line]) {
                if (i != line && Line[i][0] != -1) {
                    Line[i][lineIndex[i]] = Line[line][j];
                    lineIndex[i] += 1;
                    j++;
                    i++;
                }
                else {
                    i++;
                }
                if (i > n) i = 1;
            }
            lineEnter = i;
            int k;
            for (k = 0; k <= lineIndex[line]; k++) {
                Line[line][k] = -1;
            }
            lineIndex[line] = 0;
        }
        else {
            Line[line][0] = -1;
        }
        printf("Closed\n");
    }
    else {
        printf("Error\n");
    }

    //}
}


int main() {


    int j; // Chi so de duyet trong mang param
    for (j = 0; j < MAX; j++) {
        lineIndex[j] = 0;
    }
    while (1) {
        Param = 0;
        memset(str, 0, sizeof(str));
        memset(command, 0, sizeof(command));
        memset(param, 0, sizeof(param));
        gets_s(str);
        sscanf(str, "%s%s", command, param);
        if (strcmp(str, "***") == 0) break;
        else if (strcmp(str, "$Enter") == 0) {
            Enter();
        }
        else if (strcmp(command, "$Checkout") == 0) {

            //			printf("%s\n", command);
            //			printf("%s\n", param);
                        //Chuyen param ve so dua vao bien int Param
                        //Chuyen den khi nao dung ham printf("%d", param") ma no in ra duoc dung so nguyen minh can
                        //Checkout(Param);

            for (j = (strlen(param) - 1); j >= 0; j--) {
                Param += (param[j] - 48) * pow(10, (strlen(param) - j - 1));
            }
            CheckOut(Param);
        }
        else if (strcmp(str, "$AdvancedEnter") == 0) {
            AdvancedEnter();
        }
        else if (strcmp(command, "$CountNumberCustomerInLine") == 0) {
            //Can tach tham so
            //Chuyen param ve so dua vao bien int Param
            //Chuyen den khi nao dung ham printf("%d", param") ma no in ra duoc dung so nguyen minh can
            //Checkout(Param);

            for (j = (strlen(param) - 1); j >= 0; j--) {
                Param += (param[j] - 48) * pow(10, (strlen(param) - j - 1));
            }
            //printf("param = %d", Param);
            CountNumberCustomerInLine(Param);

        }
        else if (strcmp(command, "$ListCustomerInLine") == 0) {
            //Can tach tham so
//			ListCustomerInLine(1);
            for (j = (strlen(param) - 1); j >= 0; j--) {
                Param += (param[j] - 48) * pow(10, (strlen(param) - j - 1));
            }
            //			printf("param = %d", Param);
            ListCustomerInLine(Param);

        }
        else if (strcmp(str, "$CountNumberCustomerInAllLines") == 0) {
            CountNumberCustomerInAllLines();
        }

        else if (strcmp(str, "$OpenCashier") == 0) {
            OpenCashier();
        }
        else if (strcmp(command, "$CloseCashier") == 0) {
            for (j = (strlen(param) - 1); j >= 0; j--) {
                Param += (param[j] - 48) * pow(10, (strlen(param) - j - 1));
            }
            CloseCashier(Param);
        }
    }
    return 0;
}




