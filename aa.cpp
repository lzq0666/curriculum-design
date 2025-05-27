#include "stdio.h"
#include "math.h"
#include "string.h"
#include "stdlib.h"
#include "malloc.h"
#include "conio.h"
#include "windows.h"
#include "time.h"
#include "locale.h"  
//请以gbk编码打开

#define ERROR 0
#define OK 1
#define ElemType int
#define LIST_INIT_SIZE 100
#define status int
// 定义用户结构体
typedef struct customer
{
    int id;
    char name[100];
    char password[256];
    int VIP_level;
    int VIP_point;
    double consume_money;
} customer;

// 定义商品结构体
typedef struct goods
{
    int id;
    char name[100];
    double price;
    int state;      // 0:正常 1:促销 2:缺货 3:过期
    int stock;      // 库存
    int type;       // 0:水果 1:蔬菜 2:肉类 3:海鲜 4:零食 5:饮料 6: 文具 7: 其他
    int sale_count; // 销售数量
} goods;

// 定义用户链表
typedef struct LNode_customer
{
    customer customer;
    struct LNode_customer *next;
} LNode_customer, *LinkList_customer;
// 定义商品链表
typedef struct LNode_goods
{
    goods goods;
    struct LNode_goods *next;
} LNode_goods, *LinkList_goods;

// 全局变量
LinkList_customer L_customer;
LinkList_goods L_goods;

// 用户链表初始化
status InitList_customer(LinkList_customer &L)
{
    L = (LinkList_customer)malloc(sizeof(LNode_customer));
    if (!L)
    {
        printf("EEEOR");
        return ERROR;
    }
    else
    {
        L->next = NULL;
        L->customer.id = 0;
        return OK;
    }
}

// 商品链表初始化
status InitList_goods(LinkList_goods &L)
{
    L = (LinkList_goods)malloc(sizeof(LNode_goods));
    if (!L)
    {
        printf("EEEOR");
        return ERROR;
    }
    else
    {
        L->next = NULL;
        L->goods.id = 0;
        return OK;
    }
}

// 遍历用户链表
status ListTraverse_customer(LinkList_customer L)
{
    LNode_customer *p = L->next;
    while (p != NULL)
    {
        for (int i = 0; i < 124; i++)
            printf("=");
        printf("%d. ", p->customer.id);
        printf("用户名：%s", p->customer.name);
        printf("会员等级：%d", p->customer.VIP_level);
        printf("会员积分：%d", p->customer.VIP_point);
        printf("消费金额：%lf", p->customer.consume_money);
        for (int i = 0; i < 124; i++)
            printf("=");
        p = p->next;
    }
    system("pause");
    return OK;
}

// 遍历商品链表
status ListTraverse_goods(LinkList_goods L)
{
    LNode_goods *p = L->next;
    for (int i = 0; i < 124; i++)
        printf("=");
    while (p != NULL)
    {
        printf("序号 商品名 类型 价格 状态 库存\n");
        printf("%d. ", p->goods.id);
        printf("%-47s", p->goods.name);
        switch (p->goods.type)
        {
        case 0:
            printf("水果    ");
            break;
        case 1:
            printf("蔬菜    ");
            break;
        case 2:
            printf("肉类    ");
            break;
        case 3:
            printf("海鲜    ");
            break;
        case 4:
            printf("零食    ");
            break;
        case 5:
            printf("饮料    ");
            break;
        case 6:
            printf("文具    ");
            break;
        case 7:
            printf("其他    ");
            break;
        }
        printf("%-15.2lf", p->goods.price);
        printf("%-15d", p->goods.state);
        printf("%-15d", p->goods.stock);
        p = p->next;
    }
    for (int i = 0; i < 124; i++)
        printf("=");
    system("pause");
    return OK;
}

// 文件操作
// 保存数据到文件
status save_data()
{
    FILE *fp;
    // 保存用户信息
    fp = fopen("customer.dll", "wb");
    if (!fp)
    {
        printf("保存用户信息失败\n");
        return ERROR;
    }
    LinkList_customer current1 = L_customer->next, c;
    while (current1)
    {
        fwrite(&(current1->customer), sizeof(customer), 1, fp);
        c = current1;
        current1 = current1->next;
        free(c);
    }
    fclose(fp);

    // 保存商品信息
    fp = fopen("goods.dll", "wb");
    if (!fp)
    {
        printf("保存商品信息失败\n");
        return ERROR;
    }
    LinkList_goods current2 = L_goods, g;
    current2 = current2->next;
    while (current2)
    {
        fwrite(&(current2->goods), sizeof(goods), 1, fp);
        g = current2;
        current2 = current2->next;
        free(g);
    }
    fclose(fp);
    return OK;
}
// 从文件加载数据
status load_data()
{
    FILE *fp;
    // 加载用户信息
    fp = fopen("user.txt", "r");
    if (!fp)
    {
        printf("加载失败\n");
        return ERROR;
    }
    LinkList_customer p = L_customer->next;
    while (p)
    {
        LinkList_customer temp = p;
        p = p->next;
        free(temp);
    }
    L_customer->next = NULL;
    customer cus;
    while (fscanf(fp, "%d %s %s %d %d %lf", &cus.id, cus.name, cus.password, &cus.VIP_level, &cus.VIP_point, &cus.consume_money) == 6)
    {
        LinkList_customer newNode = (LinkList_customer)malloc(sizeof(LNode_customer));
        newNode->customer = cus;
        newNode->next = NULL;
        LinkList_customer tail = L_customer;
        while (tail->next)
            tail = tail->next;
        tail->next = newNode;
    }
    fclose(fp);

    // 加载商品信息
    fp = fopen("pro1.txt", "r");
    if (!fp)
    {
        printf("加载失败\n");
        return ERROR;
    }
    LinkList_goods p2 = L_goods->next;
    while (p2)
    {
        LinkList_goods temp = p2;
        p2 = p2->next;
        free(temp);
    }
    L_goods->next = NULL;

    goods good;
    while (fscanf(fp, "%d %s %d %lf %d %d", &good.id, good.name, &good.type, &good.price, &good.state, &good.stock) == 6)
    {
        LinkList_goods newNode = (LinkList_goods)malloc(sizeof(LNode_goods));
        newNode->goods = good;
        newNode->next = NULL;
        LinkList_goods tail = L_goods;
        while (tail->next)
            tail = tail->next;
        tail->next = newNode;
    }
    fclose(fp);
    return OK;
}

int main()
{
    setlocale(LC_ALL, ".936");
    while (1)
    {
        if (!InitList_customer(L_customer))
            ;
        else
            break;
    }
    while (1)
    {
        if (!InitList_goods(L_goods))
            ;
        else
            break;
    }
    printf("1");
    system("pause");
    load_data();
    printf("加载成功\n");
    if (!ListTraverse_goods(L_goods))
        printf("错误请重试\n");
    printf("准备写入\n");
    system("pause");
    if (!save_data())
    {
        printf("保存失败,返回管理界面\n");
    }
    else
        printf("保存成功,退出系统\n");
    system("pause");
    return 0;
}