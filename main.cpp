#include "stdio.h"
#include "math.h"
#include "string.h"
#include "stdlib.h"
#include "malloc.h"
#include "conio.h"
#include "windows.h"
#include "time.h"
#include "locale.h"

#define ERROR 0            // 定义错误码
#define OK 1               // 定义成功码
#define ElemType int       // 定义元素类型
#define LIST_INIT_SIZE 100 // 定义线性表初始长度
#define status int         // 定义状态码

// 定义用户结构体
typedef struct customer
{
    int id;               // 用户id
    char name[100];       // 用户名
    char password[256];   // 密码
    int VIP_level;        // 会员等级
    int VIP_point;        // 会员积分
    double consume_money; // 累计消费金额
} customer;

// 定义商品结构体
typedef struct goods
{
    int id;         // 商品ID（自增）
    char name[100]; // 商品名称
    double price;   // 单价
    int state;      // 0:正常 1:其他 2:缺货 3:过期
    int stock;      // 库存
    char type[100]; // 0:水果 1:蔬菜 2:肉类 3:海鲜 4:零食 5:饮料 6: 文具 7: 其他
    int sale_count; // 销售数量
} goods;

// 优惠方案结构体,第一层记录条件，第二层记录优惠金额
typedef struct discount_scheme
{
    double full_reduction[2][101]; // 满减
    double VIP_discount[2][101];   // 会员折扣
    int full_gift[2][101];         // 满赠
    double point_discount;         // 积分折扣
} discount_scheme;

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

// 用户购物车链表
typedef struct LNode_customer_cart
{
    goods goods;
    int number;
    struct LNode_customer_cart *next;
} LNode_customer_cart, *LinkList_customer_cart;

/*-------------------------------------------
              全局数据结构
-------------------------------------------*/

// 全局变量
LinkList_customer L_customer;     // 用户链表头节点
LinkList_goods L_goods;           // 商品链表头节点
double money;                     // 优惠的金额
discount_scheme current_discount; // 当前生效的优惠方案

// 声明函数
void HiddenInput(char *str);
status add_goods(LinkList_goods &L);
status modify_goods(LinkList_goods &L);
status delete_goods(LinkList_goods &L);
status calculate_sale_money(LinkList_goods &L);
status find_password(LinkList_customer &L);
int admin_menu();
void customer_menu(customer *customer);
status save_data();
status load_data();
status ListTraverse_goods(LinkList_goods L, int i);
status view_supply_info(LinkList_goods &L);
status modify_promotion_rule();
status settlement(LinkList_customer_cart &P, customer *cust, LinkList_goods &L);

/*-------------------------------------------
              核心功能函数
-------------------------------------------*/

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
        L->customer.id = 0; // 头结点id为0
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

// printCentered函数，实现文本居中输出
void printCentered(const char *format, int width)
{
    // 计算填充空格并居中输出文本
    int len = strlen(format);
    int padding = (width - len) / 2;
    for (int i = 0; i < padding; i++)
    {
        printf(" ");
    }
    printf("%s", format);
    for (int i = 0; i < padding; i++)
    {
        printf(" ");
    }
}

// 添加统一的标题和分隔线显示函数
void printHeader(const char *title)
{
    for (int i = 0; i < 124; i++)
        printf("=");
    printf("\n");
    printCentered(title, 124);
    printf("\n");
    for (int i = 0; i < 124; i++)
        printf("=");
    printf("\n");
}

// 用户注册：添加到链表尾部并分配ID
status register_customer(LinkList_customer &L)
{
    printHeader("用户注册");
    // 遍历链表到末尾，创建新节点并输入信息
    LNode_customer *p = L;
    while (p->next != NULL) // 找到尾节点
    {
        p = p->next;
    }
    LNode_customer *s = (LNode_customer *)malloc(sizeof(LNode_customer));
    if (!s)
    {
        printf("ERROR: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    s->customer.id = p->customer.id + 1;
    printf("请输入用户名：");
    scanf("%s", s->customer.name);
    char c1[100], c2[100];
    while (1)
    {
        printf("请输入密码：");
        HiddenInput(c1);
        printf("\n");
        printf("请再次输入密码：");
        HiddenInput(c2);
        printf("\n");
        printf("密码输入完成\n");
        if (strcmp(c1, c2) == 0)
        {
            strcpy(s->customer.password, c1);
            break;
        }
        else
        {
            printf("密码不一致,请重新输入\n");
        }
    }
    s->next = NULL;
    p->next = s;
    s->customer.VIP_level = 0;
    s->customer.VIP_point = 0;
    s->customer.consume_money = 0.0;
    printf("恭喜 %s 用户注册成功，正在返回上层界面\n", s->customer.name);
    return OK;
}

// 管理员登陆验证
status admin_login()
{
    printHeader("管理员登录");
    char name[30];
    char password[20];
    printf("请输入管理员账号：");
    scanf("%s", name);
    if (strcmp(name, "admin") == 0)
    {
        printf("请输入管理员密码：");
        HiddenInput(password);
        if (strcmp(password, "0012345") == 0) // 输入账号密码并与硬编码值比较
        {
            printf("登陆成功\n");
            return OK;
        }
        else
        {
            printf("密码错误登陆失败,请重试\n");
            system("pause");
            return ERROR;
        }
    }
    else
    {
        printf("名称错误登陆失败,请重试\n");
        system("pause");
        return ERROR;
    }
}

// 用户登录验证
status customer_login(LinkList_customer &L, customer **customer)
{
    printHeader("用户登录");
    char name[100];
    char password[256];
    printf("请输入用户名：");
    scanf("%s", name);
    LNode_customer *p = L->next;
    while (p != NULL)
    {
        if (strcmp(p->customer.name, name) == 0) // 输入用户名密码，遍历链表验证
        {
            printf("请输入密码：");
            HiddenInput(password);
            if (strcmp(p->customer.password, password) == 0)
            {
                printf("登陆成功\n");
                *customer = &(p->customer);
                return OK;
            }
            else 
            {
                printf("密码错误,请重试\n");
                system("pause");
                return ERROR;
            }
        }
        p = p->next;
    }
    printf("用户名不存在,请重试\n");
    system("pause");
    return ERROR;
}

// 遍历用户链表并格式化输出
status ListTraverse_customer(LinkList_customer L)
{
    LNode_customer *p = L->next;

    // 打印表头，遍历节点输出详细信息
    for (int i = 0; i < 124; i++)
        printf("=");
    printf("\n");
    printCentered("序号", 5);
    printCentered("用户名", 15);
    printCentered("会员等级", 5);
    printCentered("会员积分", 15);
    printCentered("消费金额", 15);
    printf("\n");
    while (p != NULL)
    {
        printf("%4d.", p->customer.id);
        printf("%-20s", p->customer.name);
        printf("%-5d", p->customer.VIP_level);
        printf("%-15d", p->customer.VIP_point);
        printf("%-15lf", p->customer.consume_money);
        printf("\n");
        p = p->next;
    }
    for (int i = 0; i < 124; i++)
        printf("=");
    system("pause");
    return OK;
}

// 遍历用户购物车
status ListTraverse_customer_cart(LinkList_customer_cart L)
{
    printHeader("购物车信息");
    LNode_customer_cart *p = L->next;
    printCentered("序号", 5);
    printCentered("商品名", 40);
    printCentered("数量", 5);
    printCentered("价格", 15);
    printCentered("种类", 10);
    printCentered("状态", 10);
    printCentered("库存", 10);
    printf("\n");
    while (p != NULL)
    {
        printf("%4d. ", p->goods.id);
        printf("%-40s", p->goods.name);
        printf("%-5d", p->number);
        printf("%-15.2lf", p->goods.price);
        printf("%10s", p->goods.type);
        switch (p->goods.state)
        {
        case 0:
            printCentered("正常", 10);
            break;
        case 1:
            printCentered("其他", 10);
            break;
        case 2:
            printCentered("缺货", 10);
            break;
        case 3:
            printCentered("过期", 10);
            break;
        }
        printf("%-10d", p->goods.stock);
        printf("\n");
        p = p->next;
    }
    for (int i = 0; i < 124; i++)
        printf("=");
    return OK;
}

// 遍历商品链表
status ListTraverse_goods(LinkList_goods L, int i)
{
    printHeader("商品信息");
    LNode_goods *p = L->next;
    printCentered("序号", 5);
    printCentered("商品名", 40);
    printCentered("价格", 15);
    printCentered("种类", 10);
    printCentered("状态", 10);
    printCentered("库存", 10);
    printf("\n");
    while (p != NULL)
    {
        printf("%4d. ", p->goods.id);
        printf("%-40s", p->goods.name);
        printf("%-15.2lf", p->goods.price);
        printf("%-10s", p->goods.type);
        switch (p->goods.state)
        {
        case 0:
            printCentered("正常", 10);
            break;
        case 1:
            printCentered("其他", 10);
            break;
        case 2:
            printCentered("缺货", 10);
            break;
        case 3:
            printCentered("过期", 10);
            break;
        }
        printf("%-15d", p->goods.stock);
        printf("\n");
        p = p->next;
    }
    for (int i = 0; i < 124; i++)
        printf("=");
    if (i == 0)
    {
        system("pause");
        return OK;
    }

    while (1)
    {
        printf("请输入操作序号：\n");
        printf("1.加入商品\n");
        printf("2.修改商品信息\n");
        printf("3.删除商品\n");
        printf("0.返回上层界面\n");
        int c;
        scanf("%d", &c);
        switch (c)
        {
        case 1:
            if (add_goods(L_goods))
                return OK;
            break;
        case 2:
            if (modify_goods(L_goods))
                return OK;
            break;
        case 3:
            if (delete_goods(L_goods))
                return OK;
            break;
        case 0:
            return OK;
        default:
            printf("输入错误,请重新输入\n");
            break;
        }
    }
}

// 遍历商品链表并输出销售金额
status calculate_sale_money(LinkList_goods &L)
{
    printHeader("销售金额统计");
    double s = 0;
    LNode_goods *p = L->next;
    if (p == NULL) // 如果链表为空
    {
        printf("暂无商品记录\n");
        printf("请加入商品\n");
        return ERROR;
    }
    while (p != NULL)
    {
        printf("%-46s", p->goods.name);
        printf("销售量：%d\n", p->goods.sale_count);
        s += p->goods.sale_count * p->goods.price;
        p = p->next;
    }
    printf("优惠总价格：%.2lf\n", money);
    for (int i = 0; i < 124; i++)
        printf("=");
    printf("总收入金额：%.2lf\n", s - money);
    system("pause");
    return OK;
}

// 模糊搜索算法
status search_goods(char *s, LinkList_goods &L)
{
    LNode_goods *p = L->next;
    int t = 0;
    while (p != NULL)
    {
        if (strstr(p->goods.name, s) != NULL || strstr(p->goods.type, s) != NULL) // 如果找到了
        {
            if (t == 0) // 如果是第一次找到
            {
                for (int i = 0; i < 124; i++)
                    printf("=");
                printf("\n");
                printCentered("序号", 5);
                printCentered("商品名", 40);
                printCentered("价格", 15);
                printCentered("种类", 10);
                printCentered("状态", 15);
                printf("\n");
            }
            printf("%4d. ", p->goods.id);
            printf("%-40s", p->goods.name);
            printf("%-15.2lf", p->goods.price);
            printf("%-10s", p->goods.type);
            switch (p->goods.state)
            {
            case 0:
                printCentered("正常", 10);
                break;
            case 1:
                printCentered("其他", 10);
                break;
            case 2:
                printCentered("缺货", 10);
                break;
            case 3:
                printCentered("过期", 10);
                break;
            }
            if (p->goods.stock == 0)
            {
                printf("%-15d", p->goods.stock);
                printf("  库存不足");
            }
            printf("\n");
            t++;
        }
        p = p->next;
    }
    if (t != 0)
    {
        printf("共找到%d商品\n", t);
        for (int i = 0; i < 124; i++)
            printf("=");
        system("pause");
        return OK;
    }
    else
    {
        printf("找不到商品,\n");
        system("pause");
        return ERROR;
    }
}

// 无数字输密码
void HiddenInput(char password[256])
{
    char p = 0;
    int i = 0;
    while ((p = getch()) != '\r')
    {
        if (p == 8 && i != 0)
        {
            if (i > 0)
            {
                i--;
            }
        }
        else
        {
            if (!((p <= 'Z' && p >= 'A') || (p <= 'z' && p >= 'a') || (p <= '9' && p >= '0')))

                continue;
            else
            {
                password[i] = p;
                i++;
            }
        }
    }
    password[i] = '\0';
    printf("\n");
}

// 增加商品
status add_goods(LinkList_goods &L)
{
    LinkList_goods p = L;
    while (p->next != NULL) // 找到尾节点
    {
        p = p->next;
    }
    LNode_goods *s = (LNode_goods *)malloc(sizeof(LNode_goods));
    if (!s)
    {
        printf("EEEOR");
        return ERROR;
    }
    s->goods.id = p->goods.id + 1;
    printf("请输入商品名：");
    scanf("%s", s->goods.name);
    printf("请输入商品价格：");
    scanf("%lf", &s->goods.price);
    printf("请输入商品状态(0:正常 1:其他 2:缺货 3:过期)：");
    scanf("%d", &s->goods.state);
    printf("请输入商品库存：");
    scanf("%d", &s->goods.stock);
    printf("请输入商品类型 (水果 ,蔬菜 ,肉类 ,海鲜 ,零食 ,饮料 ,文具 ,其他)：");
    scanf("%s", s->goods.type);
    s->next = NULL;
    p->next = s;
    s->goods.sale_count = 0;
    printf("商品信息：");
    printf("%d. %s %lf %d %d %s\n", s->goods.id, s->goods.name, s->goods.price, s->goods.state, s->goods.stock, s->goods.type);
    printf("商品添加成功,返回管理界面\n");
    system("pause");
    return OK;
}

// 修改商品信息
status modify_goods(LinkList_goods &L)
{
    int id;
    printf("输入商品编号：");
    scanf("%d", &id);
    LinkList_goods p = L->next;
    while (p != NULL)
    {
        if (p->goods.id == id)
            break;
        p = p->next;
    }
    if (p == NULL)
    {
        printf("商品不存在\n");
        return ERROR;
    }
    goods good;
    good.id = p->goods.id;
    printf("请输入商品名：");
    scanf("%s", good.name);
    printf("请输入商品价格：");
    scanf("%lf", &good.price);
    printf("请输入商品状态(0:正常 1:其他 2:缺货 3:过期)：");
    scanf("%d", &good.state);
    printf("请输入商品库存：");
    scanf("%d", &good.stock);
    printf("请输入商品类型 (水果 ，蔬菜 ，肉类 ，海鲜 ，零食 ，饮料 ，文具 ，其他)：");
    scanf("%s", good.type);
    printf("是否重置销售数量？(0:否 1:是)：");
    int c;
    scanf("%d", &c);
    if (c == 1)
        good.sale_count = 0;
    else
        good.sale_count = p->goods.sale_count;
    printf("商品信息：");
    printf("%d. %s %lf %d %d %s\n", good.id, good.name, good.price, good.state, good.stock, good.type);
    p->goods = good;
    printf("商品修改成功,返回管理界面\n");
    system("pause");
    return OK;
}

// 删除商品
status delete_goods(LinkList_goods &L)
{
    int id;
    printf("输入商品编号：");
    scanf("%d", &id);
    LinkList_goods p = L->next, s = (LNode_goods *)malloc(sizeof(LNode_goods));
    while (p != NULL)
    {
        if (p->goods.id == id)
            break;
        s = p;
        p = p->next;
    }
    if (p == NULL)
    {
        printf("商品不存在\n");
        return ERROR;
    }
    printf("%d. %s %lf %d %d %s\n", p->goods.id, p->goods.name, p->goods.price, p->goods.state, p->goods.stock, p->goods.type);
    printf("是否删除？(0:否 1:是)：");
    int c;
    scanf("%d", &c);
    if (c == 1)
    {
        LNode_goods *q = p;
        s->next = p->next;
        p = p->next;
        free(q);
        while (p != NULL)
        {
            p->goods.id = p->goods.id - 1;
            p = p->next;
        }
        printf("商品删除成功,返回管理界面\n");
        system("pause");
        return OK;
    }
}

// 商品加入购物车
status add_customer_cart(LinkList_customer_cart &P, LinkList_goods &L)
{
    int id, num;
    printf("输入商品编号(-1结束)：");
    scanf("%d", &id);
    if (id == -1)
        return NULL;
    printf("输入商品数量：");
    scanf("%d", &num);
    LinkList_goods p = L->next;
    while (p != NULL)
    {
        if (p->goods.id == id)
            break;
        p = p->next;
    }
    if (p == NULL)
    {
        printf("商品不存在请重新输入\n");
        return ERROR;
    }
    LNode_customer_cart *s = (LNode_customer_cart *)malloc(sizeof(LNode_customer_cart));
    if (!s)
    {
        printf("EEEOR");
        return ERROR;
    }
    s->goods = p->goods;
    s->next = NULL;

    s->number = num;

    LNode_customer_cart *q = P->next;
    while (q != NULL) // 找到尾节点
    {
        if (q->goods.id == id)
        {
            q->number += num;
            printf("%s商品加入购物车成功,返回上层界面\n", q->goods.name);
            system("pause");
            return OK;
        }
        q = q->next;
    }
    if (P->next == NULL)
        P->next = s;
    else
    {
        q = P->next;
        while (q->next != NULL) // 找到尾节点
        {
            q = q->next;
        }
        q->next = s;
    }
    printf("商品加入购物车成功\n", q->goods.name);
    return OK;

    /*while (q!= NULL) // 找到尾节点
    {
        if (q->goods.id == id)
        {
            q->number++;
            printf("%s商品加入购物车成功,返回上层界面\n", q->goods.name);
            system("pause");
            return OK;
        }
        if (q->next == NULL)
        {
            q->next = s;
            printf("%s商品加入购物车成功,返回上层界面\n", q->goods.name);
            system("pause");
            return OK;
        }
    }*/
}

// 结算（优惠取优）：计算总价，应用优惠，更新库存和用户数据，清空购物车
status settlement(LinkList_customer_cart &P, customer *c, LinkList_goods &L)
{
    double s = 0.0;
    for (int i = 0; i < 124; i++)
        printf("=");
    printf("\n");
    printCentered("结算界面", 124);
    printf("\n");
    for (int i = 0; i < 124; i++)
        printf("=");
    printf("\n");

    LNode_customer_cart *p = P->next;
    if (p == NULL) // 如果购物车为空
    {
        printf("您还未选购商品，无需结账\n");
        return ERROR;
    }
    if (p != NULL)
    {
        printCentered("序号", 5);
        printCentered("商品名", 30);
        printCentered("价格", 15);
        printCentered("数量", 5);
        printCentered("小计", 15);
        printf("\n");
    }
    // 计算总金额，匹配满减/VIP折扣，处理积分，更新销售数据
    int num = 1;
    while (p != NULL)
    {
        printf("%4d. ", num);
        printf("%-30s", p->goods.name);
        printf("%-15.2lf", p->goods.price);
        printf("%-5d", p->number);
        printf("%-15.2lf", p->goods.price * p->number);
        s = s + p->goods.price * p->number;
        printf("\n");
        num++;
        p = p->next;
    }

    // 正确操作链表
    LNode_customer_cart *current = P->next; // P是头节点，从第一个有效节点开始
  LNode_customer_cart *prev = NULL;
    while (current != NULL)
    {
        // 更新商品销售和库存
        current->goods.sale_count += current->number;
        current->goods.stock -= current->number;
        // 释放节点
        prev = current;
        current = current->next;
        free(prev);
    }
    P->next = NULL; // 清空购物车
    system("pause");
    return OK;

    printf("共%d件商品\n", num - 1);  
    for (int i = 0; i < 124; i++)
        printf("=");
    printf("\n");
    printf("总价：%.2lf\n", s);

    /*
    检查优惠
    */

    double total_discount[3] = {0};
    int i;
    // 满减
    for (i = 0; i < 100; i++)
    {
        if ((s >= current_discount.full_reduction[0][i] && s < current_discount.full_reduction[0][i + 1]) || current_discount.full_reduction[0][i + 1] == -1)
            break;
    }
    total_discount[0] = s - current_discount.full_reduction[1][i];
    // VIP
    for (i = 0; i < 100; i++)
    {
        if ((c->VIP_level >= current_discount.VIP_discount[0][i] && c->VIP_level < current_discount.VIP_discount[0][i + 1]) || current_discount.VIP_discount[0][i + 1] == -1)
            break;
    }
    total_discount[1] = s * current_discount.VIP_discount[1][i] / 100.0;
    // 最终结算界面
    printf("系统已自动匹配最优折扣\n");
    if (total_discount[0] > total_discount[1])
    {
        total_discount[2] = total_discount[1];
        printf("已使用会员打折优惠：%.2lf\n", total_discount[1]);
    }
    else
    {
        total_discount[2] = total_discount[0];
        printf("已使用满减优惠：%.2lf\n", total_discount[0]);
    }
    printf("目前您有%d积分\n", c->VIP_point);
    printf("目前每100积分可抵现：%.2lf\n", current_discount.point_discount);
    printf("是否使用积分抵现？(0:否 1:是)：");
    int choice;
    scanf("%d", &choice);
    if (choice == 1)
    {
        int point_chioce;
        point_chioce = (c->VIP_point) / 100;
        total_discount[2] = total_discount[2] - point_chioce * current_discount.point_discount;
        printf("已使用%d积分抵现\n", point_chioce * 100);
        c->VIP_point = c->VIP_point - point_chioce;
    }
    printf("最终价格：%.2lf\n", total_discount[2]);
    srand(time(NULL));
    int a = rand() % 1000000;
    printf("确认是否结账（验证码：%d）：", a);
    int pay_method;
    scanf("%d", &pay_method);
    if (pay_method == a)
    {
        // 增加满赠逻辑
        for (int i = 0; i < 101; i++)
        {
            if ((total_discount[2] >= current_discount.full_gift[0][i] && total_discount[2] < current_discount.full_gift[0][i + 1]) || current_discount.full_gift[0][i + 1] != -1)
            {
                LNode_goods *l = L->next;
                while (l != NULL)
                {
                    if (l->goods.id == current_discount.full_gift[1][i])
                    {
                        l->goods.stock--;
                        l->goods.sale_count++;
                        printf("恭喜获得赠品：%s\n", l->goods.name);
                        break;
                    }
                    else
                        l = l->next;
                }
                break;
            }
        }

        printf("支付成功\n");
        money = money + (s - total_discount[2]);
        c->consume_money = c->consume_money + total_discount[2];

        // 增加VIP等级
        int i = (int)c->consume_money;
        if ((i % 100) >= c->VIP_level)
            c->VIP_level++;

        // 增加VIP积分
        c->VIP_point = c->VIP_point + c->consume_money;

        // 释放购物车内存并清空链表
        LNode_customer_cart *current = P->next; // P是头节点，从第一个有效节点开始
        LNode_customer_cart *prev = NULL;
        while (current != NULL)
        {
            // 更新商品销售和库存
            current->goods.sale_count += current->number;
            current->goods.stock -= current->number;
            // 释放节点
            prev = current;
            current = current->next;
            free(prev);
        }
        P->next = NULL; // 清空购物车
        system("pause");
        system("cls");
        return OK;
    }
    else
    {
        printf("支付失败,返回上层界面\n");
        system("pause");
        system("cls");
        return ERROR;
    }
}

// 修改优惠规则
status modify_promotion_rule()
{
    int i;
    for (i = 0; i < 124; i++)
        printf("=");
    printf("\n");
    printCentered("修改优惠", 124);
    printf("\n");
    for (i = 0; i < 124; i++)
        printf("=");
    printf("\n");
    printf("1.满减\n");
    printf("2.VIP积分规则\n");
    printf("3.会员等级打折\n");
    printf("4.满赠\n");
    printf("0.返回上层界面\n");
    int c;
    scanf("%d", &c);
    switch (c)
    {
    case 1:
    {
        current_discount.full_reduction[0][0] = 0;
        current_discount.full_reduction[1][0] = 0;
        for (i = 0; i < 100; i++)
        {
            printf("请输入满减金额%d层金额(输入-1结束)：", i + 1);
            scanf("%lf", &current_discount.full_reduction[0][i]);
            if (current_discount.full_reduction[0][i] == -1)
                break;
            printf("\n");
            printf("请输入满减金额%lf对应的优惠金额：", current_discount.full_reduction[0][i]);
            scanf("%lf", &current_discount.full_reduction[1][i]);
        }
        printf("修改成功\n");
        system("pause");
        return OK;
    }
    case 3:
    {
        current_discount.VIP_discount[0][0] = 0;
        current_discount.VIP_discount[1][0] = 0;
        for (i = 0; i < 100; i++)
        {
            printf("请输入VIP等级%d层等级(输入-1结束)：", i + 1);
            scanf("%lf", &current_discount.VIP_discount[0][i]);
            if (current_discount.VIP_discount[0][i] == -1)
                break;
            printf("\n");
            printf("请输入VIP等级%lf对应的折扣：", current_discount.VIP_discount[0][i]);
            scanf("%lf", &current_discount.VIP_discount[1][i]);
        }
        printf("修改成功\n");
        system("pause");
        return OK;
    }
    case 4:
    {
        current_discount.full_gift[0][0] = 0;
        current_discount.full_gift[1][0] = 0;
        printf("请输入满赠金额%d层金额(输入-1结束)：", i + 1);
        scanf("%lf", &current_discount.full_gift[0][i]);
        if (current_discount.full_gift[0][i] == -1)
            break;
        printf("\n");
        printf("请输入满赠金额%d对应的赠品ID：", current_discount.full_gift[0][i]);
        scanf("%d", &current_discount.full_gift[1][i]);
        printf("修改成功\n");
        system("pause");
        return OK;
    }
    case 2:
    {
        printf("请输入积分100积分抵的金额：");
        scanf("%lf", &current_discount.point_discount);
        printf("修改成功\n");
        system("pause");
        return OK;
    }
    case 0:
        system("pause");
        return OK;
    default:
        printf("输入错误,请重试\n");
        system("pause");
        return ERROR;
    }
}

// 查看供货信息
status view_supply_info(LinkList_goods &L)
{
    LNode_goods *p = L->next;
    if (p == NULL) // 如果链表为空
    {
        printf("暂无商品记录\n");
        printf("请加入商品\n");
        return ERROR;
    }
    while (p != NULL)
    {
        if (p->goods.stock <= 10)
        {
            printf("%d. %s %lf\n", p->goods.id, p->goods.name, p->goods.price);
        }
        p = p->next;
    }
    printf("请输入供应商数量");
    int n;
    char name[256];
    double price;
    scanf("%d", &n);
    printf("请输入供应商1的名称：");
    scanf("%s", name);
    printf("请输入供应商1的价格：");
    scanf("%lf", &price);
    for (int i = 2; i <= n; i++)
    {
        char nnn[256];

        printf("请输入供应商%d的名称(以-1结束)：", i);
        scanf("%s", nnn);
        if (strcmp(nnn, "-1") == 0)
            break;
        printf("请输入供应商%d的价格：", i);
        double p;
        scanf("%lf", &p);
        if (p < price)
        {
            strcpy(name, nnn);
            price = p;
        }
    }
    printf("供应商名称：%s 价格：%lf\n是最优的可以从其中进货\n", name, price);
    printf("返回管理员清单");
    system("pause");
    return OK;
}

// 找回/改密码
status find_password(LinkList_customer &L, int i)
{
    char name[100];
    char c1[256], c2[256];
    printf("请输入用户名：");
    scanf("%s", name);
    LNode_customer *p = L->next;
    while (p != NULL)
    {
        if (strcmp(p->customer.name, name) == 0)
            break;
        p = p->next;
    }
    if (p == NULL)
    {
        printf("用户名不存在,请重试\n");
        system("pause");
        return ERROR;
    }
    if (i == 0)
    {
        printf("请输入原密码：");
        HiddenInput(c1);
        if (strcmp(p->customer.password, c1) == 0)
            ;
        else
        {
            printf("密码错误,请重试\n");
            system("pause");
            return ERROR;
        }
    }
    srand(time(NULL));
    int a = rand() % 1000000;
    printf("请输入验证码(%d)：", a);
    int b;
    scanf("%d", &b);
    if (a == b)
    {
        printf("验证码正确");
        while (1)
        {
            printf("请输入新密码：");
            HiddenInput(c1);
            printf("\n确认密码：");
            HiddenInput(c2);
            if (strcmp(c1, c2) == 0)
            {
                strcpy(p->customer.password, c1);
                printf("密码修改成功,返回上层界面\n");
                system("pause");
                return OK;
            }
        }
    }
    else
        printf("验证码错误,请重试\n");
    printf("返回上层界面\n");
    system("pause");
    return ERROR;
}

// 管理员菜单
int admin_menu()
{
    while (1)
    {
        system("cls");
        printHeader("欢迎进入管理员菜单");
        printf("0.保存数据并关闭系统\n");
        printf("1.查看所有用户\n");
        printf("2.查看商品及库存情况\n");
        printf("3.查看销售记录\n");
        printf("4.供货信息\n");
        printf("5.修改优惠规则\n");
        printf("6.关闭管理员菜单\n");
        printf("请选择：\n");
        int c;
        scanf("%d", &c);
        switch (c)
        {
        case 1:
            if (!ListTraverse_customer(L_customer))
                printf("错误请重试\n");
            break;
        case 2:
            if (!ListTraverse_goods(L_goods, 1))
                printf("错误请重试\n");
            break;
        case 3:
            if (!calculate_sale_money(L_goods))
            {
                printf("返回菜单请选择 加入商品\n");
            }
            break;
        case 4:
            if (!view_supply_info(L_goods))
                printf("错误请重试\n");
            break;
        case 5:
            if (!modify_promotion_rule())
                printf("错误请重试\n");
            break;
        case 6:
            return 0;
        case 0:
            return -1;
        default:
            printf("输入错误,请重试\n");
            break;
        }
    }
    return 0;
}

// 用户菜单
void customer_menu(customer *customer)
{
    // 初始化购物车
    LinkList_customer_cart L_customer_cart;
    L_customer_cart = (LinkList_customer_cart)malloc(sizeof(LNode_customer_cart));
    if (!L_customer_cart)
    {
        printf("ERROR");
        return;
    }
    L_customer_cart->next = NULL;
    while (1)
    {
        printHeader("欢迎光临无人超市");
        printf("%s %d级 积分：%d\n", customer->name, customer->VIP_level, customer->VIP_point);
        for (int i = 0; i < 124; i++)
            printf("=");
        printf("/n");
        printf("购物车：\n");
        LinkList_customer_cart p = L_customer_cart->next;
        printCentered("序号", 5);
        printCentered("商品名", 40);
        printCentered("数量", 5);
        printCentered("价格", 15);
        printf("\n");
        while (p != NULL)
        {
            printf("%4d. %-30s %-5d %-15.2lf\n", p->goods.id, p->goods.name, p->number, p->goods.price);
            p = p->next;
        }
        printf("1.浏览商品\n");
        printf("2.搜索商品\n");
        printf("3.自助结账\n");
        printf("4.更改密码\n");
        printf("0.退出购物\n");
        int c;
        scanf("%d", &c);
        system("cls");
        switch (c)
        {
        case 1:
            ListTraverse_goods(L_goods, 0);
            while (1)
            {
                if (add_customer_cart(L_customer_cart, L_goods) == NULL)
                    break;
            }
            system("cls");
            break;
        case 2:
        {
            char s[100];
            printf("请输入商品名或商品种类(支持模糊搜索)：");
            scanf("%s", s);
            search_goods(s, L_goods);
            while (1)
            {
                if (add_customer_cart(L_customer_cart, L_goods) == NULL)
                    break;
            }
            system("cls");
            break;
        }
        case 3:
            if (!settlement(L_customer_cart, customer, L_goods))
                printf("错误请重试\n");
            break;
        case 4:
            find_password(L_customer, 0);
            break;
        case 0:
            return;
        default:
            printf("输入错误,请重试\n");
            break;
        }
    }
}

/*-------------------------------------------
                文件操作
-------------------------------------------*/

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
    LinkList_customer current1 = L_customer;
    current1 = current1->next;
    while (current1)
    {
        fwrite(&(current1->customer), sizeof(customer), 1, fp);
        current1 = current1->next;
    }
    fclose(fp);

    // 保存商品信息
    fp = fopen("goods.dll", "wb");
    if (!fp)
    {
        printf("保存商品信息失败\n");
        return ERROR;
    }
    LinkList_goods current2 = L_goods;
    current2 = current2->next;
    while (current2)
    {
        fwrite(&(current2->goods), sizeof(goods), 1, fp);
        current2 = current2->next;
    }
    fclose(fp);    // 保存优惠规则到文本文件
    fp = fopen("discounts.txt", "w");
    if (!fp)
    {
        printf("保存优惠规则失败\n");
        return ERROR;
    }
    
    // 保存满减规则
    for (int i = 0; i < 101; i++)
    {
        if (current_discount.full_reduction[0][i] == -1)
            break;
        fprintf(fp, "%.2lf %.2lf\n", current_discount.full_reduction[0][i], current_discount.full_reduction[1][i]);
    }
    
    // 保存VIP折扣规则
    for (int i = 0; i < 101; i++)
    {
        if (current_discount.VIP_discount[0][i] == -1)
            break;
        fprintf(fp, "%.2lf %.2lf\n", current_discount.VIP_discount[0][i], current_discount.VIP_discount[1][i]);
    }
    
    // 保存满赠规则
    for (int i = 0; i < 101; i++)
    {
        if (current_discount.full_gift[0][i] == -1)
            break;
        fprintf(fp, "%d %d\n", current_discount.full_gift[0][i], current_discount.full_gift[1][i]);
    }
    
    // 保存积分折扣规则
    fprintf(fp, "%.2lf", current_discount.point_discount);
    
    fclose(fp);
    return OK;
}

// 从文件加载数据
status load_data()
{
    FILE *fp;
    // 加载用户信息
    fp = fopen("customer.dll", "rb");
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
    while (fread(&cus, sizeof(customer), 1, fp))
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
    fp = fopen("goods.dll", "rb");
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
    while (fread(&good, sizeof(goods), 1, fp))
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

    // 加载优惠规则
    fp = fopen("discounts.txt", "r");
    if (!fp)
    {
        printf("加载失败\n");
        return ERROR;
    }

    // 逐行读取文本文件中的优惠规则
    for (int i = 0; i < 101; i++)
    {
        if (fscanf(fp, "%lf %lf", &current_discount.full_reduction[0][i], &current_discount.full_reduction[1][i]) != 2)
            break;
    }
    for (int i = 0; i < 101; i++)
    {
        if (fscanf(fp, "%lf %lf", &current_discount.VIP_discount[0][i], &current_discount.VIP_discount[1][i]) != 2)
            break;
    }
    for (int i = 0; i < 101; i++)
    {
        if (fscanf(fp, "%d %d", &current_discount.full_gift[0][i], &current_discount.full_gift[1][i]) != 2)
            break;
    }
    if (fscanf(fp, "%lf", &current_discount.point_discount) != 1)
    {
        printf("积分折扣加载失败\n");
        fclose(fp);
        return ERROR;
    }

    fclose(fp);
    return OK;
}

/*-------------------------------------------
              主程序流程
-------------------------------------------*/

int main()
{
    // setlocale(LC_ALL, ".936");
    /*system("chcp 936 > nul");// 切换到 GBK 编码
    SetConsoleOutputCP(936);
    SetConsoleCP(936);
    setlocale(LC_ALL, "Chinese");*/

    // 控制台初始化（编码、标题、颜色等）
    system("title 欢迎光临无人超市");
    // system("color f0");
    system("mode con cols=124 lines=30");
    // 初始化用户链表
    while (1)
    {
        if (!InitList_customer(L_customer))
            ;
        else
            break;
    }

    // 初始化商品链表
    while (1)
    {
        if (!InitList_goods(L_goods))
            ;
        else
            break;
    }

    // 加载数据文件
    if (!load_data())
    {
        printf("加载失败\n");
        printf("请输入1重新加载\n");
        int c;
        scanf("%d", &c);
        if (c == 1)
        {
            if (!load_data())
            {
                printf("加载失败\n");
                return 0;
            }
        }
    }

    // 主循环：显示登录/注册菜单，处理用户选择
    while (1)
    {
        system("cls");
        printHeader("欢迎光临无人超市 v1.0.5");
        printf("1.会员登陆\n");
        printf("2.新会员注册\n");
        printf("3.会员找回密码\n");
        printf("4.管理员登陆\n");
        printf("请选择：\n");
        // 处理用户输入，调用对应功能模块
        int c;
        scanf("%d", &c);
        system("cls");
        switch (c)
        {

        case 1:
        {
            customer *p = (customer *)malloc(sizeof(customer));
            if (!customer_login(L_customer, &p))
                ;
            else
            {
                system("cls");
                customer_menu(p);
                system("cls");
            }
            break;
        }

        case 2:
            if (!register_customer(L_customer))
                printf("用户创建失败,请重试\n");
            break;

        case 4:
            if (!admin_login())
                ;
            else
            {
                int k;
                k = admin_menu();
                if (k == -1)
                {
                    if (!save_data())
                    {
                        printf("保存失败,返回管理界面\n");
                        admin_menu();
                    }
                    else
                        printf("保存成功,退出系统\n");
                    return 0;
                }
                else
                    break;
            }

        case 3:
            if (!find_password(L_customer, 1))
                printf("找回密码失败,请重试\n");
            break;

        case 0:
        {
            if (!save_data())
            {
                printf("保存失败,返回管理界面\n");
                admin_menu();
            }
            else
                printf("保存成功,退出系统\n");
            return 0;
        }
        default:
            printf("输入错误,请重试\n");
            break;
        }
    }
    return 0;
}
