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
#define WINDOW_WIDTH 120   // 定义控制台窗口宽度
#define WINDOW_HEIGHT 30   // 定义控制台窗口高度

/*            运行数据储存               */

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
    int id;         // 商品ID(自增)
    char name[100]; // 商品名称
    double price;   // 单价
    int state;      // 0:正常 1:其他 2:缺货 3:过期
    int stock;      // 库存
    char type[100]; // 0:水果 1:蔬菜 2:肉类 3:海鲜 4:零食 5:饮料 6: 文具 7: 其他
    int sale_count; // 销售数量
} goods;

// 优惠方案结构体,第一层记录条件,第二层记录优惠金额
typedef struct discount_scheme
{
    double full_reduction[2][101]; // 满减
    double VIP_discount[2][101];   // 会员折扣
    int full_gift[2][101];         // 满赠
    double point_discount;         // 积分折扣(每百分)
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

/*              全局变量                   */

LinkList_customer L_customer;     // 用户链表头节点
LinkList_goods L_goods;           // 商品链表头节点
double money;                     // 优惠的金额
discount_scheme current_discount; // 当前生效的优惠方案

/*               声明函数                   */

// 基础功能
void HiddenInput(char *str);
void printCentered(const char *format, int width);
void printHeader(const char *title);

// 链表初始化
status InitList_customer(LinkList_customer &L);
status InitList_goods(LinkList_goods &L);

// 用户相关
status register_customer(LinkList_customer &L);
status customer_login(LinkList_customer &L, customer **customer);
status find_password(LinkList_customer &L, int i, char *name);
status ListTraverse_customer(LinkList_customer L);

// 商品相关
status add_goods(LinkList_goods &L);
status modify_goods(LinkList_goods &L);
status delete_goods(LinkList_goods &L);
status ListTraverse_goods(LinkList_goods L, int i);
status search_goods(char *s, LinkList_goods &L);
status view_supply_info(LinkList_goods &L);
status calculate_sale_money(LinkList_goods &L);

// 购物车相关
status add_customer_cart(LinkList_customer_cart &P, LinkList_goods &L);
status modify_cart_item(LinkList_customer_cart &L_customer_cart);
status delete_cart_item(LinkList_customer_cart &L_customer_cart);
status ListTraverse_customer_cart(LinkList_customer_cart L);
status settlement(LinkList_customer_cart &P, customer *cust, LinkList_goods &L);

// 优惠相关
status display_promotion_rules();
status modify_promotion_rule();

// 菜单
int admin_menu();
void customer_menu(customer *customer);
status admin_login();

// 文件数据储存
status save_data();
status load_data();

/*-------------------------------------------
              核心功能函数
-------------------------------------------*/

// 用户链表初始化
status InitList_customer(LinkList_customer &L)
{
    L = (LinkList_customer)malloc(sizeof(LNode_customer));
    if (!L)
    {
        printf("ERROR");
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
        printf("ERROR");
        return ERROR;
    }
    else
    {
        L->next = NULL;
        L->goods.id = 0;
        return OK;
    }
}

// printCentered函数,实现文本居中输出
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
    printf("\033[1;34m"); // 蓝色
    for (int i = 0; i < WINDOW_WIDTH; i++)
        printf("=");
    printf("\033[0m\n");

    printCentered(title, WINDOW_WIDTH);

    printf("\n");
    printf("\033[1;34m"); // 蓝色
    for (int i = 0; i < WINDOW_WIDTH; i++)
        printf("=");
    printf("\033[0m\n");
}

// 用户注册 : 添加到链表尾部并分配ID
status register_customer(LinkList_customer &L)
{
    printHeader("用户注册");
    // 遍历链表到末尾,创建新节点并输入信息
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
    printf("请输入用户名 : ");
    scanf("%s", s->customer.name);
    char c1[100], c2[100];
    while (1)
    {
        printf("请输入密码 : ");
        HiddenInput(c1);
        printf("\n");
        printf("请再次输入密码 : ");
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
    printf("恭喜 %s 用户注册成功,正在返回上层界面\n", s->customer.name);
    system("pause");
    return OK;
}

// 管理员登陆验证
status admin_login()
{
    printHeader("管理员登录");
    char name[30];
    char password[20];
    printf("请输入管理员账号 : ");
    scanf("%s", name);
    if (strcmp(name, "admin") == 0)
    {
        printf("请输入管理员密码 : ");
        HiddenInput(password);
        if (strcmp(password, "0012345") == 0) // 输入账号密码并与硬编码值比较
        {
            printf("登陆成功\n");
            return OK;
        }
        else
        {
            printf("\033[91m"); // 红色
            printf("密码错误登陆失败,请重试\n");
            printf("\033[0m");
            system("pause");
            return ERROR;
        }
    }
    else
    {
        printf("\033[91m"); // 红色
        printf("名称错误登陆失败,请重试\n");
        printf("\033[0m");
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
    printf("请输入用户名 : ");
    scanf("%s", name);
    LNode_customer *p = L->next;
    while (p != NULL)
    {
        if (strcmp(p->customer.name, name) == 0) // 输入用户名密码,遍历链表验证
        {
            printf("请输入密码 : ");
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

    // 打印表头,遍历节点输出详细信息
    printf("\033[92m"); // 浅绿色
    for (int i = 0; i < WINDOW_WIDTH; i++)
        printf("=");
    printf("\033[0m\n");
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
    printf("\033[92m"); // 浅绿色
    for (int i = 0; i < WINDOW_WIDTH; i++)
        printf("=");
    printf("\033[0m\n");
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
    for (int i = 0; i < WINDOW_WIDTH; i++)
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
    for (int j = 0; j < WINDOW_WIDTH; j++)
        printf("=");
    if (i == 0)
    {
        system("pause");
        return OK;
    }

    while (1)
    {
        printf("请输入操作序号 : \n");
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
        printf("销售量 : %d\n", p->goods.sale_count);
        s += p->goods.sale_count * p->goods.price;
        p = p->next;
    }
    printf("优惠总价格 : %.2lf\n", money);
    for (int i = 0; i < WINDOW_WIDTH; i++)
        printf("=");
    printf("总收入金额 : %.2lf\n", s - money);
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
                printf("\033[92m"); // 浅绿色
                for (int i = 0; i < WINDOW_WIDTH; i++)
                    printf("=");
                printf("\033[0m\n");
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
        printf("\033[92m"); // 浅绿色
        for (int i = 0; i < WINDOW_WIDTH; i++)
            printf("=");
        printf("\033[0m\n");
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
        printf("ERROR");
        return ERROR;
    }
    s->goods.id = p->goods.id + 1;
    printf("请输入商品名 : ");
    scanf("%s", s->goods.name);
    printf("请输入商品价格 : ");
    scanf("%lf", &s->goods.price);
    printf("请输入商品状态(0:正常 1:其他 2:缺货 3:过期) : ");
    scanf("%d", &s->goods.state);
    printf("请输入商品库存 : ");
    scanf("%d", &s->goods.stock);
    printf("请输入商品类型 (水果 ,蔬菜 ,肉类 ,海鲜 ,零食 ,饮料 ,文具 ,其他) : ");
    scanf("%s", s->goods.type);
    s->next = NULL;
    p->next = s;
    s->goods.sale_count = 0;
    printf("商品信息 : ");
    printf("%d. %s %lf %d %d %s\n", s->goods.id, s->goods.name, s->goods.price, s->goods.state, s->goods.stock, s->goods.type);
    printf("商品添加成功,返回管理界面\n");
    system("pause");
    return OK;
}

// 修改商品信息
status modify_goods(LinkList_goods &L)
{
    int id;
    printf("输入商品编号 : ");
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
    printf("请输入商品名 : ");
    scanf("%s", good.name);
    printf("请输入商品价格 : ");
    scanf("%lf", &good.price);
    printf("请输入商品状态(0:正常 1:其他 2:缺货 3:过期) : ");
    scanf("%d", &good.state);
    printf("请输入商品库存 : ");
    scanf("%d", &good.stock);
    printf("请输入商品类型 (水果 ,蔬菜 ,肉类 ,海鲜 ,零食 ,饮料 ,文具 ,其他) : ");
    scanf("%s", good.type);
    printf("是否重置销售数量？(0:否 1:是) : ");
    int c;
    scanf("%d", &c);
    if (c == 1)
        good.sale_count = 0;
    else
        good.sale_count = p->goods.sale_count;
    printf("商品信息 : ");
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
    printf("输入商品编号 : ");
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
    printf("是否删除？(0:否 1:是) : ");
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
    return ERROR; // 添加这个返回值
}

// 商品加入购物车
status add_customer_cart(LinkList_customer_cart &P, LinkList_goods &L)
{
    while (1)
    {
        int id, num;
        printf(">");
        printf("请输入要添加的商品ID(-1结束) : \n");
        scanf("%d", &id);

        if (id == -1)
        {
            break;
        }

        do
        {
            printf(">");
            printf("请输入数量 : \n");
            scanf("%d", &num);
            if (num <= 0)
            {
                printf("数量必须大于0,请重新输入\n");
            }
        } while (num <= 0);

        // 查找商品
        LinkList_goods p = L->next;
        while (p != NULL && p->goods.id != id)
        {
            p = p->next;
        }

        if (p == NULL)
        {
            printf("商品ID %d 不存在\n", id);
            continue;
        }

        // 检查库存
        if (p->goods.stock < num)
        {
            printf("商品 %s 库存不足,当前库存: %d\n", p->goods.name, p->goods.stock);
            continue;
        }

        // 查找购物车中是否已有该商品
        LNode_customer_cart *cart = P->next;
        while (cart != NULL)
        {
            if (cart->goods.id == id)
            {
                break;
            }
            cart = cart->next;
        }

        if (cart != NULL)
        {
            // 更新已有商品数量
            cart->number += num;
            printf("已将 %s 的数量增加 %d 件\n", p->goods.name, num);
        }
        else
        {
            // 添加新商品到购物车
            LNode_customer_cart *s = (LNode_customer_cart *)malloc(sizeof(LNode_customer_cart));
            if (!s)
            {
                printf("\033[91m"); // 红色
                printf("内存分配失败\n");
                printf("\033[0m");
                return ERROR;
            }
            s->goods = p->goods;
            s->number = num;
            s->next = P->next;
            P->next = s;
            printf("已添加 %s %d 件到购物车\n", p->goods.name, num);
        }
    }
    printf("添加完成\n");
    return OK;
}

// 修改购物车中商品数量
status modify_cart_item(LinkList_customer_cart &L_customer_cart)
{
    printHeader("修改购物车商品数量");
    // 显示购物车内容
    printf("购物车 : \n");
    LinkList_customer_cart p = L_customer_cart->next;
    if (p != NULL)
    {
        printCentered("序号", 5);
        printCentered("商品名", 40);
        printCentered("数量", 5);
        printCentered("价格", 15);
        printf("\n");
        while (p != NULL)
        {
            printf("%4d. ", p->goods.id);
            printf("%-40s", p->goods.name);
            printf("%-5d", p->number);
            printf("%-15.2lf\n", p->goods.price);
            p = p->next;
        }
        for (int i = 0; i < WINDOW_WIDTH; i++)
            printf("=");
        printf("\n");
    }
    // 如果购物车为空,提示并返回
    else
    {
        printf("\033[91m"); // 红色
        printf("购物车是空的\n");
        printf("\033[0m");
        system("pause");
        return ERROR;
        for (int i = 0; i < WINDOW_WIDTH; i++)
            printf("=");
        printf("\n");
    }

    while (1)
    {
        int id, num;
        printf(">");
        printf("请输入要修改的商品ID(-1结束) : \n");
        scanf("%d", &id);

        if (id == -1)
        {
            break;
        }

        do
        {
            printf(">");
            printf("请输入新数量 : \n");
            scanf("%d", &num);
            if (num <= 0)
            {
                printf("数量必须大于0,请重新输入\n");
            }
        } while (num <= 0);

        LNode_customer_cart *cart = L_customer_cart->next;
        while (cart != NULL && cart->goods.id != id)
        {
            cart = cart->next;
        }

        if (cart == NULL)
        {
            printf("购物车中没有ID为%d的商品\n", id);
            continue;
        }

        if (num <= 0)
        {
            // 从购物车中删除
            LNode_customer_cart *prev = L_customer_cart;
            while (prev->next != cart)
            {
                prev = prev->next;
            }
            prev->next = cart->next;
            free(cart);
            printf("已从购物车中删除该商品\n");
        }
        else
        {
            cart->number = num;
            printf("已更新商品数量\n");
        }
    }
    system("pause");
    return OK;
}

// 从购物车中删除商品
status delete_cart_item(LinkList_customer_cart &L_customer_cart)
{
    printHeader("删除购物车商品");
    // 显示购物车内容
    printf("购物车 : \n");
    LinkList_customer_cart p = L_customer_cart->next;
    if (p != NULL)
    {
        printCentered("序号", 5);
        printCentered("商品名", 40);
        printCentered("数量", 5);
        printCentered("价格", 15);
        printf("\n");
        while (p != NULL)
        {
            printf("%4d. ", p->goods.id);
            printf("%-40s", p->goods.name);
            printf("%-5d", p->number);
            printf("%-15.2lf\n", p->goods.price);
            p = p->next;
        }
        for (int i = 0; i < WINDOW_WIDTH; i++)
            printf("=");
        printf("\n");
    }
    // 如果购物车为空,提示并返回
    else
    {
        printf("\033[91m"); // 红色
        printf("购物车是空的\n");
        printf("\033[0m");
        system("pause");
        return ERROR;
        for (int i = 0; i < WINDOW_WIDTH; i++)
            printf("=");
        printf("\n");
    }

    while (1)
    {
        int id;
        printf(">");
        printf("请输入要删除的商品ID(-1结束) : \n");
        scanf("%d", &id);

        if (id == -1)
        {
            break;
        }

        LNode_customer_cart *prev = L_customer_cart;
        LNode_customer_cart *cart = L_customer_cart->next;
        while (cart != NULL && cart->goods.id != id)
        {
            prev = cart;
            cart = cart->next;
        }

        if (cart == NULL)
        {
            printf("购物车中没有ID为%d的商品\n", id);
            continue;
        }

        prev->next = cart->next;
        free(cart);
        printf("已从购物车中删除该商品\n");
    }
    system("pause");
    return OK;
}

// 结算(优惠取优) : 计算总价,应用优惠,更新库存和用户数据,清空购物车
status settlement(LinkList_customer_cart &P, customer *c, LinkList_goods &L)
{
    double s = 0.0;
    printHeader("结算界面");

    LNode_customer_cart *p = P->next;
    if (p == NULL) // 如果购物车为空
    {
        printf("您还未选购商品,无需结账\n");
        system("pause");
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
    // 计算总金额,匹配满减/VIP折扣,处理积分,更新销售数据
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
    /*
        // 正确操作链表
        LNode_customer_cart *current = P->next; // P是头节点,从第一个有效节点开始
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
    */
    printf("共%d件商品\n", num - 1);
    for (int i = 0; i < WINDOW_WIDTH; i++)
        printf("=");
    printf("\n");
    printf("总价 : %.2lf\n", s);

    /*
    检查优惠
    */

    double total_discount[3] = {0};
    total_discount[0] = s;
    int i;

    // 满减
    double max_reduction = 0;
    int selected_level = -1;
    // 找到适用的最大满减优惠
    for (i = 0; i < 100; i++)
    {
        if (current_discount.full_reduction[0][i] == -1)
            break;

        if (s >= current_discount.full_reduction[0][i] &&
            current_discount.full_reduction[1][i] > max_reduction)
        {
            max_reduction = current_discount.full_reduction[1][i];
            selected_level = i;
        }
    }
    if (selected_level != -1)
    {
        total_discount[1] = s - max_reduction;
        printf("满减可用满%.2lf减%.2lf\n", current_discount.full_reduction[0][selected_level], max_reduction);
    }
    else
    {
        total_discount[1] = s; // 没有满足任何满减条件
    }

    // VIP
    double min_vip_price = s; // 初始化为原价
    int selected_vip_level = -1;

    // 查找适用的最优VIP折扣
    for (i = 0; i < 100; i++)
    {
        if (current_discount.VIP_discount[0][i] == -1.0)
            break;

        if (c->VIP_level >= (int)current_discount.VIP_discount[0][i])
        {
            double current_price = s * current_discount.VIP_discount[1][i] / 100.0;
            if (current_price < min_vip_price)
            {
                min_vip_price = current_price;
                selected_vip_level = i;
            }
        }
    }
    total_discount[1] = min_vip_price;
    if (selected_vip_level != -1)
    {
        printf("VIP%d可用%.1f折优惠\n",
               c->VIP_level,
               current_discount.VIP_discount[1][selected_vip_level] / 10.0);
    }

    // 最终结算界面
    printf("系统已自动匹配最优折扣\n");
    if (total_discount[0] > total_discount[1])
    {
        total_discount[2] = total_discount[1];
        printf("已使用会员打折优惠 : %.2lf\n", total_discount[1]);
    }
    else
    {
        total_discount[2] = total_discount[0];
        printf("已使用满减优惠 : %.2lf\n", total_discount[0]);
    }
    printf("目前您有%d积分\n", c->VIP_point);
    printf("目前每100积分可抵现 : %.2lf\n", current_discount.point_discount);
    printf("是否使用积分抵现？(0:否 1:是) : ");
    int choice;
    scanf("%d", &choice);
    if (choice == 1)
    {
        int point_chioce;
        point_chioce = (c->VIP_point) / 100;
        total_discount[2] = total_discount[2] - point_chioce * current_discount.point_discount;
        printf("已使用%d积分抵现\n", point_chioce * 100);
        c->VIP_point = c->VIP_point - point_chioce * 100;
    }
    printf("最终价格 : %.2lf\n", total_discount[2]);
    srand(time(NULL));
    int a = rand() % 1000000;
    printf("确认是否结账(验证码 : %d) : ", a);
    int pay_method;
    scanf("%d", &pay_method);
    if (pay_method == a)
    {
        // 增加满赠逻辑
        for (int i = 0; i < 101; i++)
        {
            if ((total_discount[2] >= current_discount.full_gift[0][i] && total_discount[2] < current_discount.full_gift[0][i + 1]) || current_discount.full_gift[0][i + 1] == -1)
            {
                LNode_goods *l = L->next;
                while (l != NULL)
                {
                    if (l->goods.id == current_discount.full_gift[1][i])
                    {
                        l->goods.stock--;
                        l->goods.sale_count++;
                        printf("恭喜获得赠品 : %s\n", l->goods.name);
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
        c->VIP_point = c->VIP_point + total_discount[2];

        // 更新商品销售和库存并释放购物车内存
        LNode_customer_cart *current = P->next;
        LNode_goods *goods_ptr;
        while (current != NULL)
        {
            // 查找对应商品并更新销售和库存
            goods_ptr = L->next;
            while (goods_ptr != NULL)
            {
                if (goods_ptr->goods.id == current->goods.id)
                {
                    goods_ptr->goods.sale_count += current->number;
                    goods_ptr->goods.stock -= current->number;
                    break;
                }
                goods_ptr = goods_ptr->next;
            }

            // 释放当前购物车节点
            LNode_customer_cart *temp = current;
            current = current->next;
            free(temp);
        }
        P->next = NULL; // 清空购物车

        system("pause");
        system("cls");
        return OK;
    }
    else
    {
        printf("\033[91m"); // 红色
        printf("支付失败,返回上层界面\n");
        printf("\033[0m");
        system("pause");
        system("cls");
        return ERROR;
    }
}

// 显示当前优惠方案
status display_promotion_rules()
{
    printHeader("当前优惠方案");

    // 1. 满减优惠
    printf("\n【满减优惠】\n");
    printf("金额阶梯    优惠金额    折扣率\n");
    printf("--------------------------------\n");
    for (int i = 0; i < 101; i++)
    {
        if (current_discount.full_reduction[0][i] == -1.0)
            break;
        double discount_rate = (current_discount.full_reduction[1][i] / current_discount.full_reduction[0][i]) * 100;
        printf("满 %-8.2lf  减 %-8.2lf  %.1f%%\n", current_discount.full_reduction[0][i], current_discount.full_reduction[1][i], discount_rate);
    }
    printf("\n");

    // 2. 会员折扣
    printf("\n【会员折扣】\n");
    printf("会员等级    折扣比例    每消费100元节省\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < 101; i++)
    {
        if (current_discount.VIP_discount[0][i] == -1.0)
            break;
        double savings = 100 - current_discount.VIP_discount[1][i];
        printf("VIP%-8d  %.1f折      %.2lf元\n", (int)current_discount.VIP_discount[0][i], current_discount.VIP_discount[1][i] / 10, savings);
    }
    printf("\n");

    // 3. 满赠活动
    printf("\n【满赠活动】\n");
    printf("消费金额    赠送商品\n");
    printf("------------------------\n");
    for (int i = 0; i < 101; i++)
    {
        if (current_discount.full_gift[0][i] == -1)
            break;
        LinkList_goods p = L_goods->next;
        while (p != NULL)
        {
            if (p->goods.id == current_discount.full_gift[1][i])
            {
                printf("满%-8d  %s(价值%.2lf元)\n", current_discount.full_gift[0][i], p->goods.name, p->goods.price);
                break;
            }
            p = p->next;
        }
    }
    printf("\n");

    // 4. 积分优惠
    printf("\n【积分优惠】\n");
    printf("----------------------------------------\n");
    printf("每100积分可抵现 : %.2lf元\n", current_discount.point_discount);
    printf("积分获取规则 : 每消费1元可得1积分\n");
    printf("----------------------------------------\n");

    printf("\n");
    for (int i = 0; i < WINDOW_WIDTH; i++)
        printf("=");
    system("pause");
    return OK;
}

// 修改优惠规则
status modify_promotion_rule()
{
    int i;
    printHeader("修改优惠规则");
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
            printf("请输入满减金额%d层金额(输入-1结束) : ", i + 1);
            scanf("%lf", &current_discount.full_reduction[0][i]);
            if (current_discount.full_reduction[0][i] == -1)
                break;
            printf("\n");
            printf("请输入满减金额%lf对应的优惠金额 : ", current_discount.full_reduction[0][i]);
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
            printf("请输入VIP等级%d层等级(输入-1结束) : ", i + 1);
            scanf("%lf", &current_discount.VIP_discount[0][i]);
            if (current_discount.VIP_discount[0][i] == -1)
                break;
            printf("\n");
            printf("请输入VIP等级%lf对应的折扣 : ", current_discount.VIP_discount[0][i]);
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
        printf("请输入满赠金额%d层金额(输入-1结束) : ", i + 1);
        scanf("%lf", &current_discount.full_gift[0][i]);
        if (current_discount.full_gift[0][i] == -1)
            break;
        printf("\n");
        printf("请输入满赠金额%d对应的赠品ID : ", current_discount.full_gift[0][i]);
        scanf("%d", &current_discount.full_gift[1][i]);
        printf("修改成功\n");
        system("pause");
        return OK;
    }
    case 2:
    {
        printf("请输入积分100积分抵的金额 : ");
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
    printf("请输入供应商1的名称 : ");
    scanf("%s", name);
    printf("请输入供应商1的价格 : ");
    scanf("%lf", &price);
    for (int i = 2; i <= n; i++)
    {
        char nnn[256];

        printf("请输入供应商%d的名称(以-1结束) : ", i);
        scanf("%s", nnn);
        if (strcmp(nnn, "-1") == 0)
            break;
        printf("请输入供应商%d的价格 : ", i);
        double p;
        scanf("%lf", &p);
        if (p < price)
        {
            strcpy(name, nnn);
            price = p;
        }
    }
    printf("供应商名称 : %s 价格 : %lf\n是最优的可以从其中进货\n", name, price);
    printf("返回管理员清单");
    system("pause");
    return OK;
}

// 找回/改密码
status find_password(LinkList_customer &L, int i, char *name) // i = 1表示忘记密码,0表示修改密码
{
    char c1[256], c2[256];
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
        printf("请输入原密码 : ");
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
    printf("请输入验证码(%d) : ", a);
    int b;
    scanf("%d", &b);
    if (a == b)
    {
        printf("验证码正确");
        while (1)
        {
            printf("请输入新密码 : ");
            HiddenInput(c1);
            printf("\n确认密码 : ");
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
    {
        printf("验证码错误,请重试\n");
        printf("返回上层界面\n");
        system("pause");
    }
    return ERROR;
}

/*-------------------------------------------
                菜单界面
-------------------------------------------*/

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
        printf("5.查看优惠方案\n");
        printf("6.修改优惠规则\n");
        printf("7.关闭管理员菜单\n");
        printf("请选择 : \n");
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
            {
                printf("\033[91m"); // 红色
                printf("错误请重试\n");
                printf("\033[0m");
            }
            break;

        case 5:
            if (!display_promotion_rules())
            {
                printf("\033[91m"); // 红色
                printf("显示优惠方案失败\n");
                printf("\033[0m");
            }
            break;

        case 6:
            if (!modify_promotion_rule())
            {
                printf("\033[91m"); // 红色
                printf("错误请重试\n");
                printf("\033[0m");
            }
            break;

        case 7:
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
        system("cls");
        printHeader("欢迎光临无人超市");
        printf("%s %d级 积分 : %d\n", customer->name, customer->VIP_level, customer->VIP_point);
        printf("\033[92m"); // 浅绿色
        for (int i = 0; i < WINDOW_WIDTH; i++)
            printf("=");
        printf("\033[0m\n");

        // 显示购物车内容
        printf("购物车 : \n");
        LinkList_customer_cart p = L_customer_cart->next;
        if (p != NULL)
        {
            printCentered("序号", 5);
            printCentered("商品名", 40);
            printCentered("数量", 5);
            printCentered("价格", 15);
            printf("\n");
            while (p != NULL)
            {
                printf("%4d. ", p->goods.id);
                printf("%-40s", p->goods.name);
                printf("%-5d", p->number);
                printf("%-15.2lf\n", p->goods.price);
                p = p->next;
            }
            printf("\033[92m"); // 浅绿色
            for (int i = 0; i < WINDOW_WIDTH; i++)
                printf("=");
            printf("\033[0m\n");
            printf("\n");
        }
        else
        {
            printf("购物车是空的\n");
            printf("\033[92m"); // 浅绿色
            for (int i = 0; i < WINDOW_WIDTH; i++)
                printf("=");
            printf("\033[0m\n");
        }

        printf("1. 浏览商品\n");
        printf("2. 搜索商品\n");
        printf("3. 修改购物车商品数量\n");
        printf("4. 删除购物车商品\n");
        printf("5. 结算\n");
        printf("6. 修改密码\n");
        printf("0. 退出购物\n");
        printf("请选择 : ");

        int c;
        scanf("%d", &c);
        system("cls");

        switch (c)
        {
        case 1:
            ListTraverse_goods(L_goods, 0);
            if (!add_customer_cart(L_customer_cart, L_goods))
            {
                printf("\033[91m"); // 红色
                printf("添加商品失败\n");
                printf("\033[0m");
            }
            system("cls");
            break;

        case 2:
        {
            printHeader("搜索商品");
            char s[100];
            printf("请输入商品名或商品种类(支持模糊搜索) : ");
            scanf("%s", s);
            search_goods(s, L_goods);
            if (!add_customer_cart(L_customer_cart, L_goods))
            {
                printf("\033[91m"); // 红色
                printf("添加商品失败\n");
                printf("\033[0m");
            }
            system("cls");
            break;
        }

        case 3:
            modify_cart_item(L_customer_cart);
            break;

        case 4:
            delete_cart_item(L_customer_cart);
            break;

        case 5:
            if (!settlement(L_customer_cart, customer, L_goods))
            {
                printf("\033[91m"); // 红色
                printf("结算失败\n");
                printf("\033[0m");
            }
            break;

        case 6:
            // 修改密码
            printHeader("修改密码");
            printf("请输入用户名 : ");
            char name[256];
            scanf("%s", name);
            if (strcmp(customer->name, name) == 0)
            {
                find_password(L_customer, 0, name);
            }
            else
            {
                printf("\033[91m"); // 红色
                printf("用户名不匹配, 请重试\n");
                printf("\033[0m");
                system("pause");
            }
            break;

        case 0:
            // 释放购物车内存
            while (L_customer_cart->next != NULL)
            {
                LNode_customer_cart *temp = L_customer_cart->next;
                L_customer_cart->next = temp->next;
                free(temp);
            }
            free(L_customer_cart);
            return;

        default:
            printf("\033[91m"); // 红色
            printf("输入错误,请重试\n");
            printf("\033[0m");
            system("pause");
            break;
        }
    }
}

/*-------------------------------------------
                保存和加载数据
-------------------------------------------*/

// 保存数据到文件
status save_data()
{
    FILE *fp;
    // 保存用户信息
    fp = fopen("customer.bin", "wb");
    if (!fp)
    {
        printf("\033[91m"); // 红色
        printf("保存用户信息失败\n");
        printf("\033[0m");
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
    fp = fopen("goods.bin", "wb");
    if (!fp)
    {
        printf("\033[91m"); // 红色
        printf("保存商品信息失败\n");
        printf("\033[0m");
        return ERROR;
    }
    LinkList_goods current2 = L_goods;
    current2 = current2->next;
    while (current2)
    {
        fwrite(&(current2->goods), sizeof(goods), 1, fp);
        current2 = current2->next;
    }
    fclose(fp);

    // 保存优惠规则到文本文件
    fp = fopen("discounts.txt", "w");
    if (!fp)
    {
        printf("\033[91m"); // 红色
        printf("保存优惠规则失败\n");
        printf("\033[0m");
        return ERROR;
    }

    // 保存满减规则
    for (int i = 0; i < 101; i++)
    {
        if (current_discount.full_reduction[0][i] == -1.0)
            break;
        fprintf(fp, "%.2lf %.2lf\n", current_discount.full_reduction[0][i], current_discount.full_reduction[1][i]);
    }
    fprintf(fp, "\n"); // 添加空行分隔

    // 保存VIP折扣规则
    for (int i = 0; i < 101; i++)
    {
        if (current_discount.VIP_discount[0][i] == -1.0)
            break;
        fprintf(fp, "%.2lf %.2lf\n", current_discount.VIP_discount[0][i], current_discount.VIP_discount[1][i]);
    }
    fprintf(fp, "\n"); // 添加空行分隔

    // 保存满赠规则
    for (int i = 0; i < 101; i++)
    {
        if (current_discount.full_gift[0][i] == -1)
            break;
        fprintf(fp, "%d %d\n", current_discount.full_gift[0][i], current_discount.full_gift[1][i]);
    }
    fprintf(fp, "\n"); // 添加空行分隔

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
    fp = fopen("customer.bin", "rb");
    if (!fp)
    {

        printf("\033[91m"); // 红色
        printf("加载失败.1\n");
        printf("\033[0m");

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
        if (!newNode)
        {
            printf("\033[91m"); // 红色
            printf("内存分配失败\n");
            printf("\033[0m");
            fclose(fp);
            return ERROR;
        }
        newNode->customer = cus;
        newNode->next = NULL;
        LinkList_customer tail = L_customer;
        while (tail->next)
            tail = tail->next;
        tail->next = newNode;
    }
    fclose(fp);

    // 加载商品信息
    fp = fopen("goods.bin", "rb");
    if (!fp)
    {
        printf("\033[91m"); // 红色
        printf("加载失败.2\n");
        printf("\033[0m");
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
        if (!newNode)
        {
            printf("\033[91m"); // 红色
            printf("内存分配失败\n");
            printf("\033[0m");
            fclose(fp);
            return ERROR;
        }
        newNode->goods = good;
        newNode->next = NULL;
        LinkList_goods tail = L_goods;
        while (tail->next)
            tail = tail->next;
        tail->next = newNode;
    }
    fclose(fp);

    // 初始化优惠规则数组
    memset(&current_discount, 0, sizeof(current_discount));

    // 将满减、会员折扣和满赠数组的结束标记设为-1
    for (int i = 0; i < 101; i++)
    {
        current_discount.full_reduction[0][i] = -1.0;
        current_discount.full_reduction[1][i] = 0.0;
        current_discount.VIP_discount[0][i] = -1.0;
        current_discount.VIP_discount[1][i] = 0.0;
        current_discount.full_gift[0][i] = -1;
        current_discount.full_gift[1][i] = 0;
    }

    // 加载优惠规则
    fp = fopen("discounts.txt", "r");
    if (!fp)
    {
        printf("加载优惠规则失败,将使用默认值\n");
        current_discount.point_discount = 1.0; // 默认100积分抵1元
        return OK;                             // 优惠规则加载失败不影响系统运行
    }

    // 初始化计数器
    int full_reduction_count = 0;
    int vip_discount_count = 0;
    int full_gift_count = 0;
    char line[256];
    int section = 0; // 0=满减, 1=会员折扣, 2=满赠, 3=积分折扣

    // 逐行读取文件
    while (fgets(line, sizeof(line), fp))
    {
        // 跳过空行,遇到空行切换到下一个section
        if (line[0] == '\n' || line[0] == '\r')
        {
            section++;
            continue;
        }

        // 根据当前section处理不同类型的优惠规则
        switch (section)
        {
        case 0: // 满减规则
            if (sscanf(line, "%lf %lf", &current_discount.full_reduction[0][full_reduction_count], &current_discount.full_reduction[1][full_reduction_count]) == 2)
            {
                full_reduction_count++;
            }
            break;

        case 1: // 会员折扣规则
            if (sscanf(line, "%lf %lf", &current_discount.VIP_discount[0][vip_discount_count], &current_discount.VIP_discount[1][vip_discount_count]) == 2)
            {
                vip_discount_count++;
            }
            break;

        case 2: // 满赠规则
            if (sscanf(line, "%d %d", &current_discount.full_gift[0][full_gift_count], &current_discount.full_gift[1][full_gift_count]) == 2)
            {
                full_gift_count++;
            }
            break;

        case 3: // 积分折扣
            sscanf(line, "%lf", &current_discount.point_discount);
            break;
        }
    }

    // 设置结束标记
    if (full_reduction_count < 101)
    {
        current_discount.full_reduction[0][full_reduction_count] = -1.0;
    }
    if (vip_discount_count < 101)
    {
        current_discount.VIP_discount[0][vip_discount_count] = -1.0;
    }
    if (full_gift_count < 101)
    {
        current_discount.full_gift[0][full_gift_count] = -1;
    }

    // 如果没有读取到积分折扣,设置默认值
    if (current_discount.point_discount <= 0)
    {
        current_discount.point_discount = 1.0; // 默认100积分抵1元
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
    /*system("chcp 936 > nul");
    SetConsoleOutputCP(936);
    SetConsoleCP(936);
    setlocale(LC_ALL, "Chinese");*/

    // 控制台初始化(编码、标题、颜色等)
    system("title 欢迎光临无人超市");

    char cmd[128];
    sprintf(cmd, "mode con cols=%d lines=%d", WINDOW_WIDTH, WINDOW_HEIGHT);
    system(cmd);

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
        printf("\033[91m"); // 红色
        printf("加载失败\n");
        printf("\033[0m");
        printf("请输入1重新加载\n");
        int c;
        scanf("%d", &c);
        if (c == 1)
        {
            if (!load_data())
            {
                printf("\033[91m"); // 红色
                printf("加载失败\n");
                printf("\033[0m");
                return 0;
            }
        }
    }

    // 主循环 : 显示登录/注册菜单,处理用户选择
    while (1)
    {
        system("cls");
        printHeader("欢迎光临无人超市 v1.0.5");
        printf("1.会员登陆\n");
        printf("2.新会员注册\n");
        printf("3.会员找回密码\n");
        printf("4.管理员登陆\n");
        printf("请选择 : \n");
        // 处理用户输入,调用对应功能模块
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
            {
                printf("\033[91m"); // 红色
                printf("用户创建失败,请重试\n");
                printf("\033[0m");
            }
            break;

        case 3:
            printHeader("找回密码");
            printf("请输入用户名 : ");
            char name[256];
            scanf("%s", name);
            if (!find_password(L_customer, 1, name))
            {
                printf("\033[91m"); // 红色
                printf("找回密码失败,请重试\n");
                printf("\033[0m");
            }
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
                        printf("\033[91m"); // 红色
                        printf("保存失败,返回管理界面\n");
                        printf("\033[0m");
                        admin_menu();
                    }
                    else
                        printf("保存成功,退出系统\n");
                    return 0;
                }
                else
                    break;
            }

        case 0:
        {
            if (!save_data())
            {
                printf("\033[91m"); // 红色
                printf("保存失败,返回管理界面\n");
                printf("\033[0m");
                admin_menu();
            }
            else
                printf("保存成功,退出系统\n");
            return 0;
        }

        default:
            printf("\033[91m"); // 红色
            printf("输入错误,请重试\n");
            printf("\033[0m");
            break;
        }
    }
    return 0;
}
