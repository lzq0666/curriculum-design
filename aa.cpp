#include "stdio.h"
#include "math.h"
#include "string.h"
#include "stdlib.h"
#include "malloc.h"
#include "conio.h"
#include "windows.h"
#include "time.h"
#include "locale.h"  
//����gbk�����

#define ERROR 0
#define OK 1
#define ElemType int
#define LIST_INIT_SIZE 100
#define status int
// �����û��ṹ��
typedef struct customer
{
    int id;
    char name[100];
    char password[256];
    int VIP_level;
    int VIP_point;
    double consume_money;
} customer;

// ������Ʒ�ṹ��
typedef struct goods
{
    int id;
    char name[100];
    double price;
    int state;      // 0:���� 1:���� 2:ȱ�� 3:����
    int stock;      // ���
    int type;       // 0:ˮ�� 1:�߲� 2:���� 3:���� 4:��ʳ 5:���� 6: �ľ� 7: ����
    int sale_count; // ��������
} goods;

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

// ȫ�ֱ���
LinkList_customer L_customer;
LinkList_goods L_goods;

// �û�������ʼ��
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
        L->customer.id = 0;
        return OK;
    }
}

// ��Ʒ������ʼ��
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

// �����û�����
status ListTraverse_customer(LinkList_customer L)
{
    LNode_customer *p = L->next;
    while (p != NULL)
    {
        for (int i = 0; i < 124; i++)
            printf("=");
        printf("%d. ", p->customer.id);
        printf("�û�����%s", p->customer.name);
        printf("��Ա�ȼ���%d", p->customer.VIP_level);
        printf("��Ա���֣�%d", p->customer.VIP_point);
        printf("���ѽ�%lf", p->customer.consume_money);
        for (int i = 0; i < 124; i++)
            printf("=");
        p = p->next;
    }
    system("pause");
    return OK;
}

// ������Ʒ����
status ListTraverse_goods(LinkList_goods L)
{
    LNode_goods *p = L->next;
    for (int i = 0; i < 124; i++)
        printf("=");
    while (p != NULL)
    {
        printf("��� ��Ʒ�� ���� �۸� ״̬ ���\n");
        printf("%d. ", p->goods.id);
        printf("%-47s", p->goods.name);
        switch (p->goods.type)
        {
        case 0:
            printf("ˮ��    ");
            break;
        case 1:
            printf("�߲�    ");
            break;
        case 2:
            printf("����    ");
            break;
        case 3:
            printf("����    ");
            break;
        case 4:
            printf("��ʳ    ");
            break;
        case 5:
            printf("����    ");
            break;
        case 6:
            printf("�ľ�    ");
            break;
        case 7:
            printf("����    ");
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

// �ļ�����
// �������ݵ��ļ�
status save_data()
{
    FILE *fp;
    // �����û���Ϣ
    fp = fopen("customer.dll", "wb");
    if (!fp)
    {
        printf("�����û���Ϣʧ��\n");
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

    // ������Ʒ��Ϣ
    fp = fopen("goods.dll", "wb");
    if (!fp)
    {
        printf("������Ʒ��Ϣʧ��\n");
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
// ���ļ���������
status load_data()
{
    FILE *fp;
    // �����û���Ϣ
    fp = fopen("user.txt", "r");
    if (!fp)
    {
        printf("����ʧ��\n");
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

    // ������Ʒ��Ϣ
    fp = fopen("pro1.txt", "r");
    if (!fp)
    {
        printf("����ʧ��\n");
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
    printf("���سɹ�\n");
    if (!ListTraverse_goods(L_goods))
        printf("����������\n");
    printf("׼��д��\n");
    system("pause");
    if (!save_data())
    {
        printf("����ʧ��,���ع�������\n");
    }
    else
        printf("����ɹ�,�˳�ϵͳ\n");
    system("pause");
    return 0;
}