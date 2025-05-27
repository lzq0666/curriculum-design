#include "stdio.h"
#include "math.h"
#include "string.h"
#include "stdlib.h"
#include "malloc.h"
#include "conio.h"
#include "windows.h"
#include "time.h"
#include "locale.h"

#define ERROR 0            // ���������
#define OK 1               // ����ɹ���
#define ElemType int       // ����Ԫ������
#define LIST_INIT_SIZE 100 // �������Ա��ʼ����
#define status int         // ����״̬��

// �����û��ṹ��
typedef struct customer
{
    int id;               // �û�id
    char name[100];       // �û���
    char password[256];   // ����
    int VIP_level;        // ��Ա�ȼ�
    int VIP_point;        // ��Ա����
    double consume_money; // �ۼ����ѽ��
} customer;

// ������Ʒ�ṹ��
typedef struct goods
{
    int id;         // ��ƷID��������
    char name[100]; // ��Ʒ����
    double price;   // ����
    int state;      // 0:���� 1:���� 2:ȱ�� 3:����
    int stock;      // ���
    char type[100]; // 0:ˮ�� 1:�߲� 2:���� 3:���� 4:��ʳ 5:���� 6: �ľ� 7: ����
    int sale_count; // ��������
} goods;

// �Żݷ����ṹ��,��һ���¼�������ڶ����¼�Żݽ��
typedef struct discount_scheme
{
    double full_reduction[2][101]; // ����
    double VIP_discount[2][101];   // ��Ա�ۿ�
    int full_gift[2][101];         // ����
    double point_discount;         // �����ۿۣ�ÿ�ٷ֣�
} discount_scheme;

// �����û�����
typedef struct LNode_customer
{
    customer customer;
    struct LNode_customer *next;
} LNode_customer, *LinkList_customer;
// ������Ʒ����
typedef struct LNode_goods
{
    goods goods;
    struct LNode_goods *next;
} LNode_goods, *LinkList_goods;

// �û����ﳵ����
typedef struct LNode_customer_cart
{
    goods goods;
    int number;
    struct LNode_customer_cart *next;
} LNode_customer_cart, *LinkList_customer_cart;

/*-------------------------------------------
              ȫ�����ݽṹ
-------------------------------------------*/

// ȫ�ֱ���
LinkList_customer L_customer;     // �û�����ͷ�ڵ�
LinkList_goods L_goods;           // ��Ʒ����ͷ�ڵ�
double money;                     // �ŻݵĽ��
discount_scheme current_discount; // ��ǰ��Ч���Żݷ���

// ��������
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
status display_promotion_rules(); // ������ʾ�Żݷ����ĺ���

/*-------------------------------------------
              ���Ĺ��ܺ���
-------------------------------------------*/

// �û������ʼ��
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
        L->customer.id = 0; // ͷ���idΪ0
        return OK;
    }
}

// ��Ʒ�����ʼ��
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

// printCentered������ʵ���ı��������
void printCentered(const char *format, int width)
{
    // �������ո񲢾�������ı�
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

// ���ͳһ�ı���ͷָ�����ʾ����
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

// �û�ע�᣺��ӵ�����β��������ID
status register_customer(LinkList_customer &L)
{
    printHeader("�û�ע��");
    // ��������ĩβ�������½ڵ㲢������Ϣ
    LNode_customer *p = L;
    while (p->next != NULL) // �ҵ�β�ڵ�
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
    printf("�������û�����");
    scanf("%s", s->customer.name);
    char c1[100], c2[100];
    while (1)
    {
        printf("���������룺");
        HiddenInput(c1);
        printf("\n");
        printf("���ٴ��������룺");
        HiddenInput(c2);
        printf("\n");
        printf("�����������\n");
        if (strcmp(c1, c2) == 0)
        {
            strcpy(s->customer.password, c1);
            break;
        }
        else
        {
            printf("���벻һ��,����������\n");
        }
    }
    s->next = NULL;
    p->next = s;
    s->customer.VIP_level = 0;
    s->customer.VIP_point = 0;
    s->customer.consume_money = 0.0;
    printf("��ϲ %s �û�ע��ɹ������ڷ����ϲ����\n", s->customer.name);
    return OK;
}

// ����Ա��½��֤
status admin_login()
{
    printHeader("����Ա��¼");
    char name[30];
    char password[20];
    printf("���������Ա�˺ţ�");
    scanf("%s", name);
    if (strcmp(name, "admin") == 0)
    {
        printf("���������Ա���룺");
        HiddenInput(password);
        if (strcmp(password, "0012345") == 0) // �����˺����벢��Ӳ����ֵ�Ƚ�
        {
            printf("��½�ɹ�\n");
            return OK;
        }
        else
        {
            printf("��������½ʧ��,������\n");
            system("pause");
            return ERROR;
        }
    }
    else
    {
        printf("���ƴ����½ʧ��,������\n");
        system("pause");
        return ERROR;
    }
}

// �û���¼��֤
status customer_login(LinkList_customer &L, customer **customer)
{
    printHeader("�û���¼");
    char name[100];
    char password[256];
    printf("�������û�����");
    scanf("%s", name);
    LNode_customer *p = L->next;
    while (p != NULL)
    {
        if (strcmp(p->customer.name, name) == 0) // �����û������룬����������֤
        {
            printf("���������룺");
            HiddenInput(password);
            if (strcmp(p->customer.password, password) == 0)
            {
                printf("��½�ɹ�\n");
                *customer = &(p->customer);
                return OK;
            }
            else
            {
                printf("�������,������\n");
                system("pause");
                return ERROR;
            }
        }
        p = p->next;
    }
    printf("�û���������,������\n");
    system("pause");
    return ERROR;
}

// �����û�������ʽ�����
status ListTraverse_customer(LinkList_customer L)
{
    LNode_customer *p = L->next;

    // ��ӡ��ͷ�������ڵ������ϸ��Ϣ
    for (int i = 0; i < 124; i++)
        printf("=");
    printf("\n");
    printCentered("���", 5);
    printCentered("�û���", 15);
    printCentered("��Ա�ȼ�", 5);
    printCentered("��Ա����", 15);
    printCentered("���ѽ��", 15);
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

// �����û����ﳵ
status ListTraverse_customer_cart(LinkList_customer_cart L)
{
    printHeader("���ﳵ��Ϣ");
    LNode_customer_cart *p = L->next;
    printCentered("���", 5);
    printCentered("��Ʒ��", 40);
    printCentered("����", 5);
    printCentered("�۸�", 15);
    printCentered("����", 10);
    printCentered("״̬", 10);
    printCentered("���", 10);
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
            printCentered("����", 10);
            break;
        case 1:
            printCentered("����", 10);
            break;
        case 2:
            printCentered("ȱ��", 10);
            break;
        case 3:
            printCentered("����", 10);
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

// ������Ʒ����
status ListTraverse_goods(LinkList_goods L, int i)
{
    printHeader("��Ʒ��Ϣ");
    LNode_goods *p = L->next;
    printCentered("���", 5);
    printCentered("��Ʒ��", 40);
    printCentered("�۸�", 15);
    printCentered("����", 10);
    printCentered("״̬", 10);
    printCentered("���", 10);
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
            printCentered("����", 10);
            break;
        case 1:
            printCentered("����", 10);
            break;
        case 2:
            printCentered("ȱ��", 10);
            break;
        case 3:
            printCentered("����", 10);
            break;
        }
        printf("%-15d", p->goods.stock);
        printf("\n");
        p = p->next;
    }
    for (int j = 0; j < 124; j++)
        printf("=");
    if (i == 0)
    {
        system("pause");
        return OK;
    }

    while (1)
    {
        printf("�����������ţ�\n");
        printf("1.������Ʒ\n");
        printf("2.�޸���Ʒ��Ϣ\n");
        printf("3.ɾ����Ʒ\n");
        printf("0.�����ϲ����\n");
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
            printf("�������,����������\n");
            break;
        }
    }
}

// ������Ʒ����������۽��
status calculate_sale_money(LinkList_goods &L)
{
    printHeader("���۽��ͳ��");
    double s = 0;
    LNode_goods *p = L->next;
    if (p == NULL) // �������Ϊ��
    {
        printf("������Ʒ��¼\n");
        printf("�������Ʒ\n");
        return ERROR;
    }
    while (p != NULL)
    {
        printf("%-46s", p->goods.name);
        printf("��������%d\n", p->goods.sale_count);
        s += p->goods.sale_count * p->goods.price;
        p = p->next;
    }
    printf("�Ż��ܼ۸�%.2lf\n", money);
    for (int i = 0; i < 124; i++)
        printf("=");
    printf("�������%.2lf\n", s - money);
    system("pause");
    return OK;
}

// ģ�������㷨
status search_goods(char *s, LinkList_goods &L)
{
    LNode_goods *p = L->next;
    int t = 0;
    while (p != NULL)
    {
        if (strstr(p->goods.name, s) != NULL || strstr(p->goods.type, s) != NULL) // ����ҵ���
        {
            if (t == 0) // ����ǵ�һ���ҵ�
            {
                for (int i = 0; i < 124; i++)
                    printf("=");
                printf("\n");
                printCentered("���", 5);
                printCentered("��Ʒ��", 40);
                printCentered("�۸�", 15);
                printCentered("����", 10);
                printCentered("״̬", 15);
                printf("\n");
            }
            printf("%4d. ", p->goods.id);
            printf("%-40s", p->goods.name);
            printf("%-15.2lf", p->goods.price);
            printf("%-10s", p->goods.type);
            switch (p->goods.state)
            {
            case 0:
                printCentered("����", 10);
                break;
            case 1:
                printCentered("����", 10);
                break;
            case 2:
                printCentered("ȱ��", 10);
                break;
            case 3:
                printCentered("����", 10);
                break;
            }
            if (p->goods.stock == 0)
            {
                printf("%-15d", p->goods.stock);
                printf("  ��治��");
            }
            printf("\n");
            t++;
        }
        p = p->next;
    }
    if (t != 0)
    {
        printf("���ҵ�%d��Ʒ\n", t);
        for (int i = 0; i < 124; i++)
            printf("=");
        system("pause");
        return OK;
    }
    else
    {
        printf("�Ҳ�����Ʒ,\n");
        system("pause");
        return ERROR;
    }
}

// ������������
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

// ������Ʒ
status add_goods(LinkList_goods &L)
{
    LinkList_goods p = L;
    while (p->next != NULL) // �ҵ�β�ڵ�
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
    printf("��������Ʒ����");
    scanf("%s", s->goods.name);
    printf("��������Ʒ�۸�");
    scanf("%lf", &s->goods.price);
    printf("��������Ʒ״̬(0:���� 1:���� 2:ȱ�� 3:����)��");
    scanf("%d", &s->goods.state);
    printf("��������Ʒ��棺");
    scanf("%d", &s->goods.stock);
    printf("��������Ʒ���� (ˮ�� ,�߲� ,���� ,���� ,��ʳ ,���� ,�ľ� ,����)��");
    scanf("%s", s->goods.type);
    s->next = NULL;
    p->next = s;
    s->goods.sale_count = 0;
    printf("��Ʒ��Ϣ��");
    printf("%d. %s %lf %d %d %s\n", s->goods.id, s->goods.name, s->goods.price, s->goods.state, s->goods.stock, s->goods.type);
    printf("��Ʒ��ӳɹ�,���ع������\n");
    system("pause");
    return OK;
}

// �޸���Ʒ��Ϣ
status modify_goods(LinkList_goods &L)
{
    int id;
    printf("������Ʒ��ţ�");
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
        printf("��Ʒ������\n");
        return ERROR;
    }
    goods good;
    good.id = p->goods.id;
    printf("��������Ʒ����");
    scanf("%s", good.name);
    printf("��������Ʒ�۸�");
    scanf("%lf", &good.price);
    printf("��������Ʒ״̬(0:���� 1:���� 2:ȱ�� 3:����)��");
    scanf("%d", &good.state);
    printf("��������Ʒ��棺");
    scanf("%d", &good.stock);
    printf("��������Ʒ���� (ˮ�� ���߲� ������ ������ ����ʳ ������ ���ľ� ������)��");
    scanf("%s", good.type);
    printf("�Ƿ���������������(0:�� 1:��)��");
    int c;
    scanf("%d", &c);
    if (c == 1)
        good.sale_count = 0;
    else
        good.sale_count = p->goods.sale_count;
    printf("��Ʒ��Ϣ��");
    printf("%d. %s %lf %d %d %s\n", good.id, good.name, good.price, good.state, good.stock, good.type);
    p->goods = good;
    printf("��Ʒ�޸ĳɹ�,���ع������\n");
    system("pause");
    return OK;
}

// ɾ����Ʒ
status delete_goods(LinkList_goods &L)
{
    int id;
    printf("������Ʒ��ţ�");
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
        printf("��Ʒ������\n");
        return ERROR;
    }
    printf("%d. %s %lf %d %d %s\n", p->goods.id, p->goods.name, p->goods.price, p->goods.state, p->goods.stock, p->goods.type);
    printf("�Ƿ�ɾ����(0:�� 1:��)��");
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
        printf("��Ʒɾ���ɹ�,���ع������\n");
        system("pause");
        return OK;
    }
    return ERROR; // ����������ֵ
}

// ��Ʒ���빺�ﳵ
status add_customer_cart(LinkList_customer_cart &P, LinkList_goods &L)
{
    printf("������Ҫ��ӵ���ƷID������(��ʽ��ID ���� ����-1 0����)��\n");
    while (1)
    {
        int id, num;
        printf(">");
        printf("������Ҫ��ӵ���ƷID(-1����)��\n");
        scanf("%d", &id);

        if (id == -1)
        {
            break;
        }
        printf(">");
        printf("������������\n");
        scanf("%d", &num);

        if (num <= 0)
        {
            printf("�����������0\n");
            continue;
        }

        // ������Ʒ
        LinkList_goods p = L->next;
        while (p != NULL && p->goods.id != id)
        {
            p = p->next;
        }

        if (p == NULL)
        {
            printf("��ƷID %d ������\n", id);
            continue;
        }

        // �����
        if (p->goods.stock < num)
        {
            printf("��Ʒ %s ��治�㣬��ǰ���: %d\n", p->goods.name, p->goods.stock);
            continue;
        }

        // ���ҹ��ﳵ���Ƿ����и���Ʒ
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
            // ����������Ʒ����
            cart->number += num;
            printf("�ѽ� %s ���������� %d ��\n", p->goods.name, num);
        }
        else
        {
            // �������Ʒ�����ﳵ
            LNode_customer_cart *s = (LNode_customer_cart *)malloc(sizeof(LNode_customer_cart));
            if (!s)
            {
                printf("�ڴ����ʧ��\n");
                return ERROR;
            }
            s->goods = p->goods;
            s->number = num;
            s->next = P->next;
            P->next = s;
            printf("����� %s %d �������ﳵ\n", p->goods.name, num);
        }
    }
    printf("������\n");
    return OK;
}

// ���㣨�Ż�ȡ�ţ��������ܼۣ�Ӧ���Żݣ����¿����û����ݣ���չ��ﳵ
status settlement(LinkList_customer_cart &P, customer *c, LinkList_goods &L)
{
    double s = 0.0;
    for (int i = 0; i < 124; i++)
        printf("=");
    printf("\n");
    printCentered("�������", 124);
    printf("\n");
    for (int i = 0; i < 124; i++)
        printf("=");
    printf("\n");

    LNode_customer_cart *p = P->next;
    if (p == NULL) // ������ﳵΪ��
    {
        printf("����δѡ����Ʒ���������\n");
        return ERROR;
    }
    if (p != NULL)
    {
        printCentered("���", 5);
        printCentered("��Ʒ��", 30);
        printCentered("�۸�", 15);
        printCentered("����", 5);
        printCentered("С��", 15);
        printf("\n");
    }
    // �����ܽ�ƥ������/VIP�ۿۣ�������֣�������������
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
        // ��ȷ��������
        LNode_customer_cart *current = P->next; // P��ͷ�ڵ㣬�ӵ�һ����Ч�ڵ㿪ʼ
      LNode_customer_cart *prev = NULL;
        while (current != NULL)
        {
            // ������Ʒ���ۺͿ��
            current->goods.sale_count += current->number;
            current->goods.stock -= current->number;
            // �ͷŽڵ�
            prev = current;
            current = current->next;
            free(prev);
        }
        P->next = NULL; // ��չ��ﳵ
        system("pause");
        return OK;
    */
    printf("��%d����Ʒ\n", num - 1);
    for (int i = 0; i < 124; i++)
        printf("=");
    printf("\n");
    printf("�ܼۣ�%.2lf\n", s);

    /*
    ����Ż�
    */

    double total_discount[3] = {0};
    int i;
    // ����
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
    // ���ս������
    printf("ϵͳ���Զ�ƥ�������ۿ�\n");
    if (total_discount[0] > total_discount[1])
    {
        total_discount[2] = total_discount[1];
        printf("��ʹ�û�Ա�����Żݣ�%.2lf\n", total_discount[1]);
    }
    else
    {
        total_discount[2] = total_discount[0];
        printf("��ʹ�������Żݣ�%.2lf\n", total_discount[0]);
    }
    printf("Ŀǰ����%d����\n", c->VIP_point);
    printf("Ŀǰÿ100���ֿɵ��֣�%.2lf\n", current_discount.point_discount);
    printf("�Ƿ�ʹ�û��ֵ��֣�(0:�� 1:��)��");
    int choice;
    scanf("%d", &choice);
    if (choice == 1)
    {
        int point_chioce;
        point_chioce = (c->VIP_point) / 100;
        total_discount[2] = total_discount[2] - point_chioce * current_discount.point_discount;
        printf("��ʹ��%d���ֵ���\n", point_chioce * 100);
        c->VIP_point = c->VIP_point - point_chioce * 100;
    }
    printf("���ռ۸�%.2lf\n", total_discount[2]);
    srand(time(NULL));
    int a = rand() % 1000000;
    printf("ȷ���Ƿ���ˣ���֤�룺%d����", a);
    int pay_method;
    scanf("%d", &pay_method);
    if (pay_method == a)
    {
        // ���������߼�
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
                        printf("��ϲ�����Ʒ��%s\n", l->goods.name);
                        break;
                    }
                    else
                        l = l->next;
                }
                break;
            }
        }

        printf("֧���ɹ�\n");
        money = money + (s - total_discount[2]);
        c->consume_money = c->consume_money + total_discount[2];

        // ����VIP�ȼ�
        int i = (int)c->consume_money;
        if ((i % 100) >= c->VIP_level)
            c->VIP_level++;

        // ����VIP����
        c->VIP_point = c->VIP_point + total_discount[2];

        // ������Ʒ���ۺͿ�沢�ͷŹ��ﳵ�ڴ�
        LNode_customer_cart *current = P->next;
        LNode_goods *goods_ptr;
        while (current != NULL)
        {
            // ���Ҷ�Ӧ��Ʒ���������ۺͿ��
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

            // �ͷŵ�ǰ���ﳵ�ڵ�
            LNode_customer_cart *temp = current;
            current = current->next;
            free(temp);
        }
        P->next = NULL; // ��չ��ﳵ

        system("pause");
        system("cls");
        return OK;
    }
    else
    {
        printf("֧��ʧ��,�����ϲ����\n");
        system("pause");
        system("cls");
        return ERROR;
    }
}

// �޸��Żݹ���
status modify_promotion_rule()
{
    int i;
    for (i = 0; i < 124; i++)
        printf("=");
    printf("\n");
    printCentered("�޸��Ż�", 124);
    printf("\n");
    for (i = 0; i < 124; i++)
        printf("=");
    printf("\n");
    printf("1.����\n");
    printf("2.VIP���ֹ���\n");
    printf("3.��Ա�ȼ�����\n");
    printf("4.����\n");
    printf("0.�����ϲ����\n");
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
            printf("�������������%d����(����-1����)��", i + 1);
            scanf("%lf", &current_discount.full_reduction[0][i]);
            if (current_discount.full_reduction[0][i] == -1)
                break;
            printf("\n");
            printf("�������������%lf��Ӧ���Żݽ�", current_discount.full_reduction[0][i]);
            scanf("%lf", &current_discount.full_reduction[1][i]);
        }
        printf("�޸ĳɹ�\n");
        system("pause");
        return OK;
    }
    case 3:
    {
        current_discount.VIP_discount[0][0] = 0;
        current_discount.VIP_discount[1][0] = 0;
        for (i = 0; i < 100; i++)
        {
            printf("������VIP�ȼ�%d��ȼ�(����-1����)��", i + 1);
            scanf("%lf", &current_discount.VIP_discount[0][i]);
            if (current_discount.VIP_discount[0][i] == -1)
                break;
            printf("\n");
            printf("������VIP�ȼ�%lf��Ӧ���ۿۣ�", current_discount.VIP_discount[0][i]);
            scanf("%lf", &current_discount.VIP_discount[1][i]);
        }
        printf("�޸ĳɹ�\n");
        system("pause");
        return OK;
    }
    case 4:
    {
        current_discount.full_gift[0][0] = 0;
        current_discount.full_gift[1][0] = 0;
        printf("�������������%d����(����-1����)��", i + 1);
        scanf("%lf", &current_discount.full_gift[0][i]);
        if (current_discount.full_gift[0][i] == -1)
            break;
        printf("\n");
        printf("�������������%d��Ӧ����ƷID��", current_discount.full_gift[0][i]);
        scanf("%d", &current_discount.full_gift[1][i]);
        printf("�޸ĳɹ�\n");
        system("pause");
        return OK;
    }
    case 2:
    {
        printf("���������100���ֵֵĽ�");
        scanf("%lf", &current_discount.point_discount);
        printf("�޸ĳɹ�\n");
        system("pause");
        return OK;
    }
    case 0:
        system("pause");
        return OK;
    default:
        printf("�������,������\n");
        system("pause");
        return ERROR;
    }
}

// �鿴������Ϣ
status view_supply_info(LinkList_goods &L)
{
    LNode_goods *p = L->next;
    if (p == NULL) // �������Ϊ��
    {
        printf("������Ʒ��¼\n");
        printf("�������Ʒ\n");
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
    printf("�����빩Ӧ������");
    int n;
    char name[256];
    double price;
    scanf("%d", &n);
    printf("�����빩Ӧ��1�����ƣ�");
    scanf("%s", name);
    printf("�����빩Ӧ��1�ļ۸�");
    scanf("%lf", &price);
    for (int i = 2; i <= n; i++)
    {
        char nnn[256];

        printf("�����빩Ӧ��%d������(��-1����)��", i);
        scanf("%s", nnn);
        if (strcmp(nnn, "-1") == 0)
            break;
        printf("�����빩Ӧ��%d�ļ۸�", i);
        double p;
        scanf("%lf", &p);
        if (p < price)
        {
            strcpy(name, nnn);
            price = p;
        }
    }
    printf("��Ӧ�����ƣ�%s �۸�%lf\n�����ŵĿ��Դ����н���\n", name, price);
    printf("���ع���Ա�嵥");
    system("pause");
    return OK;
}

// �һ�/������
status find_password(LinkList_customer &L, int i)
{
    char name[100];
    char c1[256], c2[256];
    printf("�������û�����");
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
        printf("�û���������,������\n");
        system("pause");
        return ERROR;
    }
    if (i == 0)
    {
        printf("������ԭ���룺");
        HiddenInput(c1);
        if (strcmp(p->customer.password, c1) == 0)
            ;
        else
        {
            printf("�������,������\n");
            system("pause");
            return ERROR;
        }
    }
    srand(time(NULL));
    int a = rand() % 1000000;
    printf("��������֤��(%d)��", a);
    int b;
    scanf("%d", &b);
    if (a == b)
    {
        printf("��֤����ȷ");
        while (1)
        {
            printf("�����������룺");
            HiddenInput(c1);
            printf("\nȷ�����룺");
            HiddenInput(c2);
            if (strcmp(c1, c2) == 0)
            {
                strcpy(p->customer.password, c1);
                printf("�����޸ĳɹ�,�����ϲ����\n");
                system("pause");
                return OK;
            }
        }
    }
    else
        printf("��֤�����,������\n");
    printf("�����ϲ����\n");
    system("pause");
    return ERROR;
}

// ����Ա�˵�
int admin_menu()
{
    while (1)
    {
        system("cls");
        printHeader("��ӭ�������Ա�˵�");
        printf("0.�������ݲ��ر�ϵͳ\n");
        printf("1.�鿴�����û�\n");
        printf("2.�鿴��Ʒ��������\n");
        printf("3.�鿴���ۼ�¼\n");
        printf("4.������Ϣ\n");
        printf("5.�鿴�Żݷ���\n");
        printf("6.�޸��Żݹ���\n");
        printf("7.�رչ���Ա�˵�\n");
        printf("��ѡ��\n");
        int c;
        scanf("%d", &c);
        switch (c)
        {
        case 1:
            if (!ListTraverse_customer(L_customer))
                printf("����������\n");
            break;
        case 2:
            if (!ListTraverse_goods(L_goods, 1))
                printf("����������\n");
            break;
        case 3:
            if (!calculate_sale_money(L_goods))
            {
                printf("���ز˵���ѡ�� ������Ʒ\n");
            }
            break;
        case 4:
            if (!view_supply_info(L_goods))
                printf("����������\n");
            break;
        case 5:
            if (!display_promotion_rules())
                printf("��ʾ�Żݷ���ʧ��\n");
            break;
        case 6:
            if (!modify_promotion_rule())
                printf("����������\n");
            break;
        case 7:
            return 0;
        case 0:
            return -1;
        default:
            printf("�������,������\n");
            break;
        }
    }
    return 0;
}

// �û��˵�
void customer_menu(customer *customer)
{
    // ��ʼ�����ﳵ
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
        printHeader("��ӭ�������˳���");
        printf("%s %d�� ���֣�%d\n", customer->name, customer->VIP_level, customer->VIP_point);
        for (int i = 0; i < 124; i++)
            printf("=");
        printf("\n");

        // ��ʾ���ﳵ����
        printf("���ﳵ��\n");
        LinkList_customer_cart p = L_customer_cart->next;
        if (p != NULL)
        {
            printCentered("���", 5);
            printCentered("��Ʒ��", 40);
            printCentered("����", 5);
            printCentered("�۸�", 15);
            printf("\n");
            while (p != NULL)
            {
                printf("%4d. ", p->goods.id);
                printf("%-40s", p->goods.name);
                printf("%-5d", p->number);
                printf("%-15.2lf\n", p->goods.price);
                p = p->next;
            }
            for (int i = 0; i < 124; i++)
                printf("=");
            printf("\n");
        }
        else
        {
            printf("���ﳵ�ǿյ�\n");
            for (int i = 0; i < 124; i++)
                printf("=");
            printf("\n");
        }

        printf("1. �����Ʒ\n");
        printf("2. ������Ʒ\n");
        printf("3. �޸Ĺ��ﳵ��Ʒ����\n");
        printf("4. ɾ�����ﳵ��Ʒ\n");
        printf("5. ����\n");
        printf("6. �޸�����\n");
        printf("0. �˳�����\n");
        printf("��ѡ��");

        int c;
        scanf("%d", &c);
        system("cls");

        switch (c)
        {
        case 1:
            ListTraverse_goods(L_goods, 0);
            if (!add_customer_cart(L_customer_cart, L_goods))
            {
                printf("�����Ʒʧ��\n");
            }
            system("cls");
            break;
        case 2:
        {
            char s[100];
            printf("��������Ʒ������Ʒ����(֧��ģ������)��");
            scanf("%s", s);
            search_goods(s, L_goods);
            if (!add_customer_cart(L_customer_cart, L_goods))
            {
                printf("�����Ʒʧ��\n");
            }
            system("cls");
            break;
        }
        case 3:
            // �޸Ĺ��ﳵ����Ʒ����
            if (L_customer_cart->next != NULL)
            {
                printf("������Ҫ�޸ĵ���ƷID��������������-1 0��������\n");
                while (1)
                {
                    int id, num;
                    scanf("%d %d", &id, &num);
                    if (id == -1)
                        break;

                    LNode_customer_cart *cart = L_customer_cart->next;
                    while (cart != NULL && cart->goods.id != id)
                    {
                        cart = cart->next;
                    }

                    if (cart == NULL)
                    {
                        printf("���ﳵ��û��IDΪ%d����Ʒ\n", id);
                        continue;
                    }

                    if (num <= 0)
                    {
                        // �ӹ��ﳵ��ɾ��
                        LNode_customer_cart *prev = L_customer_cart;
                        while (prev->next != cart)
                        {
                            prev = prev->next;
                        }
                        prev->next = cart->next;
                        free(cart);
                        printf("�Ѵӹ��ﳵ��ɾ������Ʒ\n");
                    }
                    else
                    {
                        cart->number = num;
                        printf("�Ѹ�����Ʒ����\n");
                    }
                }
            }
            else
            {
                printf("���ﳵ�ǿյ�\n");
            }
            system("pause");
            break;
        case 4:
            // �ӹ��ﳵ��ɾ����Ʒ
            if (L_customer_cart->next != NULL)
            {
                printf("������Ҫɾ������ƷID������-1��������\n");
                while (1)
                {
                    int id;
                    scanf("%d", &id);
                    if (id == -1)
                        break;
                    LNode_customer_cart *prev = L_customer_cart;
                    LNode_customer_cart *cart = L_customer_cart->next;
                    while (cart != NULL && cart->goods.id != id)
                    {
                        prev = cart;
                        cart = cart->next;
                    }

                    if (cart == NULL)
                    {
                        printf("���ﳵ��û��IDΪ%d����Ʒ\n", id);
                        continue;
                    }

                    prev->next = cart->next;
                    free(cart);
                    printf("�Ѵӹ��ﳵ��ɾ������Ʒ\n");
                }
            }
            else
            {
                printf("���ﳵ�ǿյ�\n");
            }
            system("pause");
            break;
        case 5:
            if (!settlement(L_customer_cart, customer, L_goods))
            {
                printf("����ʧ��\n");
            }
            break;
        case 6:
            find_password(L_customer, 0);
            break;
        case 0:
            // �ͷŹ��ﳵ�ڴ�
            while (L_customer_cart->next != NULL)
            {
                LNode_customer_cart *temp = L_customer_cart->next;
                L_customer_cart->next = temp->next;
                free(temp);
            }
            free(L_customer_cart);
            return;
        default:
            printf("�������,������\n");
            system("pause");
            break;
        }
    }
}

// ��ʾ��ǰ�Żݷ���
status display_promotion_rules()
{
    printHeader("��ǰ�Żݷ���");

    // 1. �����Ż�
    printf("\n�������Żݡ�\n");
    printf("������    �Żݽ��    �ۿ���\n");
    printf("--------------------------------\n");
    for (int i = 0; i < 101; i++)
    {
        if (current_discount.full_reduction[0][i] == -1.0)
            break;
        double discount_rate = (current_discount.full_reduction[1][i] / current_discount.full_reduction[0][i]) * 100;
        printf("�� %-8.2lf  �� %-8.2lf  %.1f%%\n", current_discount.full_reduction[0][i], current_discount.full_reduction[1][i], discount_rate);
    }
    printf("\n");

    // 2. ��Ա�ۿ�
    printf("\n����Ա�ۿۡ�\n");
    printf("��Ա�ȼ�    �ۿ۱���    ÿ����100Ԫ��ʡ\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < 101; i++)
    {
        if (current_discount.VIP_discount[0][i] == -1.0)
            break;
        double savings = 100 - current_discount.VIP_discount[1][i];
        printf("VIP%-8d  %.1f��      %.2lfԪ\n", (int)current_discount.VIP_discount[0][i], current_discount.VIP_discount[1][i] / 10, savings);
    }
    printf("\n");

    // 3. �����
    printf("\n���������\n");
    printf("���ѽ��    ������Ʒ\n");
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
                printf("��%-8d  %s(��ֵ%.2lfԪ)\n", current_discount.full_gift[0][i], p->goods.name, p->goods.price);
                break;
            }
            p = p->next;
        }
    }
    printf("\n");

    // 4. �����Ż�
    printf("\n�������Żݡ�\n");
    printf("----------------------------------------\n");
    printf("ÿ100���ֿɵ��֣�%.2lfԪ\n", current_discount.point_discount);
    printf("���ֻ�ȡ����ÿ����1Ԫ�ɵ�1����\n");
    printf("----------------------------------------\n");

    printf("\n");
    for (int i = 0; i < 124; i++)
        printf("=");
    system("pause");
    return OK;
}

/*-------------------------------------------
                ����ͼ�������
-------------------------------------------*/

// �������ݵ��ļ�
status save_data()
{
    FILE *fp;
    // �����û���Ϣ
    fp = fopen("customer.bin", "wb");
    if (!fp)
    {
        printf("�����û���Ϣʧ��\n");
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

    // ������Ʒ��Ϣ
    fp = fopen("goods.bin", "wb");
    if (!fp)
    {
        printf("������Ʒ��Ϣʧ��\n");
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

    // �����Żݹ����ı��ļ�
    fp = fopen("discounts.txt", "w");
    if (!fp)
    {
        printf("�����Żݹ���ʧ��\n");
        return ERROR;
    }

    // ������������
    for (int i = 0; i < 101; i++)
    {
        if (current_discount.full_reduction[0][i] == -1.0)
            break;
        fprintf(fp, "%.2lf %.2lf\n", current_discount.full_reduction[0][i], current_discount.full_reduction[1][i]);
    }
    fprintf(fp, "\n"); // ��ӿ��зָ�

    // ����VIP�ۿ۹���
    for (int i = 0; i < 101; i++)
    {
        if (current_discount.VIP_discount[0][i] == -1.0)
            break;
        fprintf(fp, "%.2lf %.2lf\n", current_discount.VIP_discount[0][i], current_discount.VIP_discount[1][i]);
    }
    fprintf(fp, "\n"); // ��ӿ��зָ�

    // ������������
    for (int i = 0; i < 101; i++)
    {
        if (current_discount.full_gift[0][i] == -1)
            break;
        fprintf(fp, "%d %d\n", current_discount.full_gift[0][i], current_discount.full_gift[1][i]);
    }
    fprintf(fp, "\n"); // ��ӿ��зָ�

    // ��������ۿ۹���
    fprintf(fp, "%.2lf", current_discount.point_discount);

    fclose(fp);
    return OK;
}

// ���ļ���������
status load_data()
{
    FILE *fp;
    // �����û���Ϣ
    fp = fopen("customer.bin", "rb");
    if (!fp)
    {
        printf("����ʧ��.1\n");
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
            printf("�ڴ����ʧ��\n");
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

    // ������Ʒ��Ϣ
    fp = fopen("goods.bin", "rb");
    if (!fp)
    {
        printf("����ʧ��.2\n");
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
            printf("�ڴ����ʧ��\n");
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

    // ��ʼ���Żݹ�������
    memset(&current_discount, 0, sizeof(current_discount));

    // ����������Ա�ۿۺ���������Ľ��������Ϊ-1
    for (int i = 0; i < 101; i++)
    {
        current_discount.full_reduction[0][i] = -1.0;
        current_discount.full_reduction[1][i] = 0.0;
        current_discount.VIP_discount[0][i] = -1.0;
        current_discount.VIP_discount[1][i] = 0.0;
        current_discount.full_gift[0][i] = -1;
        current_discount.full_gift[1][i] = 0;
    }

    // �����Żݹ���
    fp = fopen("discounts.txt", "r");
    if (!fp)
    {
        printf("�����Żݹ���ʧ�ܣ���ʹ��Ĭ��ֵ\n");
        current_discount.point_discount = 1.0; // Ĭ��100���ֵ�1Ԫ
        return OK;                             // �Żݹ������ʧ�ܲ�Ӱ��ϵͳ����
    }

    // ��ʼ��������
    int full_reduction_count = 0;
    int vip_discount_count = 0;
    int full_gift_count = 0;
    char line[256];
    int section = 0; // 0=����, 1=��Ա�ۿ�, 2=����, 3=�����ۿ�

    // ���ж�ȡ�ļ�
    while (fgets(line, sizeof(line), fp))
    {
        // �������У����������л�����һ��section
        if (line[0] == '\n' || line[0] == '\r')
        {
            section++;
            continue;
        }

        // ���ݵ�ǰsection����ͬ���͵��Żݹ���
        switch (section)
        {
        case 0: // ��������
            if (sscanf(line, "%lf %lf",
                       &current_discount.full_reduction[0][full_reduction_count],
                       &current_discount.full_reduction[1][full_reduction_count]) == 2)
            {
                full_reduction_count++;
            }
            break;

        case 1: // ��Ա�ۿ۹���
            if (sscanf(line, "%lf %lf",
                       &current_discount.VIP_discount[0][vip_discount_count],
                       &current_discount.VIP_discount[1][vip_discount_count]) == 2)
            {
                vip_discount_count++;
            }
            break;

        case 2: // ��������
            if (sscanf(line, "%d %d",
                       &current_discount.full_gift[0][full_gift_count],
                       &current_discount.full_gift[1][full_gift_count]) == 2)
            {
                full_gift_count++;
            }
            break;

        case 3: // �����ۿ�
            sscanf(line, "%lf", &current_discount.point_discount);
            break;
        }
    }

    // ���ý������
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

    // ���û�ж�ȡ�������ۿۣ�����Ĭ��ֵ
    if (current_discount.point_discount <= 0)
    {
        current_discount.point_discount = 1.0; // Ĭ��100���ֵ�1Ԫ
    }

    fclose(fp);
    return OK;
}

/*-------------------------------------------
              ����������
-------------------------------------------*/

int main()
{
    // setlocale(LC_ALL, ".936");
    /*system("chcp 936 > nul");// �л��� GBK ����
    SetConsoleOutputCP(936);
    SetConsoleCP(936);
    setlocale(LC_ALL, "Chinese");*/

    // ����̨��ʼ�������롢���⡢��ɫ�ȣ�
    system("title ��ӭ�������˳���");
    // system("color f0");
    system("mode con cols=124 lines=30");
    // ��ʼ���û�����
    while (1)
    {
        if (!InitList_customer(L_customer))
            ;
        else
            break;
    }

    // ��ʼ����Ʒ����
    while (1)
    {
        if (!InitList_goods(L_goods))
            ;
        else
            break;
    }

    // ���������ļ�
    if (!load_data())
    {
        printf("����ʧ��\n");
        printf("������1���¼���\n");
        int c;
        scanf("%d", &c);
        if (c == 1)
        {
            if (!load_data())
            {
                printf("����ʧ��\n");
                return 0;
            }
        }
    }

    // ��ѭ������ʾ��¼/ע��˵��������û�ѡ��
    while (1)
    {
        system("cls");
        printHeader("��ӭ�������˳��� v1.0.5");
        printf("1.��Ա��½\n");
        printf("2.�»�Աע��\n");
        printf("3.��Ա�һ�����\n");
        printf("4.����Ա��½\n");
        printf("��ѡ��\n");
        // �����û����룬���ö�Ӧ����ģ��
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
                printf("�û�����ʧ��,������\n");
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
                        printf("����ʧ��,���ع������\n");
                        admin_menu();
                    }
                    else
                        printf("����ɹ�,�˳�ϵͳ\n");
                    return 0;
                }
                else
                    break;
            }

        case 3:
            if (!find_password(L_customer, 1))
                printf("�һ�����ʧ��,������\n");
            break;

        case 0:
        {
            if (!save_data())
            {
                printf("����ʧ��,���ع������\n");
                admin_menu();
            }
            else
                printf("����ɹ�,�˳�ϵͳ\n");
            return 0;
        }
        default:
            printf("�������,������\n");
            break;
        }
    }
    return 0;
}
