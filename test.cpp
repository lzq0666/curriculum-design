#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

// 定义与main.cpp相同的结构体
typedef struct customer {
    int id;
    char name[100];
    char password[256];
    int VIP_level;
    int VIP_point;
    double consume_money;
} customer;

typedef struct goods {
    int id;
    char name[100];
    double price;
    int state;
    int stock;
    char type[100]; 
    int sale_count;
} goods;

typedef struct discount_scheme {
    double full_reduction[2][101];
    double VIP_discount[2][101];
    int full_gift[2][101];
    double point_discount;
} discount_scheme;

// 定义与main.cpp相同的链表结构
typedef struct LNode_customer
{
    customer customer;
    struct LNode_customer *next;
} LNode_customer, *LinkList_customer;

typedef struct LNode_goods
{
    goods goods;
    struct LNode_goods *next;
} LNode_goods, *LinkList_goods;

// 初始化用户链表
void InitList_customer(LinkList_customer &L) {
    L = (LinkList_customer)malloc(sizeof(LNode_customer));
    if (!L) {
        printf("ERROR");
        exit(1);
    }
    L->next = NULL;
}

// 初始化商品链表
void InitList_goods(LinkList_goods &L) {
    L = (LinkList_goods)malloc(sizeof(LNode_goods));
    if (!L) {
        printf("ERROR");
        exit(1);
    }
    L->next = NULL;
}

// 将用户文本文件转换为链表并写入二进制文件
void convert_users() {
    FILE *txt = fopen("user.txt", "r");
    FILE *bin = fopen("customer.dll", "wb");

    LinkList_customer L;
    InitList_customer(L);
    LNode_customer *p = L;

    customer c;
    while (fscanf(txt, "%d,%[^,],%[^,],%d,%d,%lf", 
        &c.id, c.name, c.password, &c.VIP_level, 
        &c.VIP_point, &c.consume_money) == 6) {
        LNode_customer *s = (LNode_customer *)malloc(sizeof(LNode_customer));
        s->customer = c;
        s->next = NULL;
        p->next = s;
        p = s;
    }

    p = L->next;
    while (p) {
        fwrite(&p->customer, sizeof(customer), 1, bin);
        p = p->next;
    }

    fclose(txt);
    fclose(bin);
}

// 将商品文本文件转换为链表并写入二进制文件
void convert_goods() {
    FILE *txt = fopen("goods.txt", "r");
    FILE *bin = fopen("goods.dll", "wb");

    LinkList_goods L;
    InitList_goods(L);
    LNode_goods *p = L;

    goods g;
    while (fscanf(txt, "%d,%[^,],%lf,%d,%d,%[^,],%d", 
        &g.id, g.name, &g.price, &g.state, &g.stock, 
        g.type, &g.sale_count) == 7) {
        LNode_goods *s = (LNode_goods *)malloc(sizeof(LNode_goods));
        s->goods = g;
        s->next = NULL;
        p->next = s;
        p = s;
    }

    p = L->next;
    while (p) {
        fwrite(&p->goods, sizeof(goods), 1, bin);
        p = p->next;
    }

    fclose(txt);
    fclose(bin);
}

// 将优惠信息文本文件转换为二进制文件
void convert_discounts() {
    FILE *txt = fopen("discounts.txt", "r");
    FILE *bin = fopen("discount_scheme.dll", "wb");

    discount_scheme ds = {0};
    char tag[20];
    int i = 0;

    // 读取所有优惠信息
    while (fscanf(txt, "%19[^,],", tag) == 1) {
        if (strcmp(tag, "满减") == 0) {
            fscanf(txt, "%lf,%lf\n", &ds.full_reduction[0][i], &ds.full_reduction[1][i]);
            i++;
        } 
        else if (strcmp(tag, "VIP折扣") == 0) {
            fscanf(txt, "%lf,%lf\n", &ds.VIP_discount[0][i], &ds.VIP_discount[1][i]);
            i++;
        }
        else if (strcmp(tag, "满赠") == 0) {
            fscanf(txt, "%d,%d\n", &ds.full_gift[0][i], &ds.full_gift[1][i]);
            i++;
        }
        else if (strcmp(tag, "积分折扣") == 0) {
            fscanf(txt, "%lf\n", &ds.point_discount);
        }
    }

    fwrite(&ds, sizeof(discount_scheme), 1, bin);
    fclose(txt);
    fclose(bin);
}

int main() {
    convert_users();
    system("pause");
    convert_goods();
    system("pause");
    convert_discounts();
    system("pause");
    printf("文件转换完成\n");
    system("pause");
    return 0;
}
