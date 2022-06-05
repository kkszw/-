#include<stdio.h>
#include<stdlib.h>
typedef struct PCB{//PCB结构体
    char name[8];
    int begin;
    int size;
    struct PCB *left;
    struct PCB *right;
}Mypcb;
typedef struct memory{//内存空间结构体
    int usage;
    int begin;
    int size;
    struct memory *left;
    struct memory *right;
}Memory;

void processCreate();
void add(struct PCB *head, struct PCB *process);
int addM(Memory *head,Memory *q);
void check();
void processTime();
void processEnd();
void reclaimSpace(Mypcb *head);
void processWake();
void processBlock();
void showState();
void show(Mypcb *head);
void showSize();

Mypcb *ready,*running,*block;//就绪，执行和阻塞
Memory *memoryHead;//内存空间

void main()
{
    memoryHead=(Memory *)malloc(sizeof(Memory));
    Memory *m=(Memory *)malloc(sizeof(Memory));
    ready=(Mypcb *)malloc(sizeof(Mypcb));
    running=(Mypcb *)malloc(sizeof(Mypcb));
    block=(Mypcb *)malloc(sizeof(Mypcb));
    memoryHead->usage=1;
    memoryHead->right=m;
    m->left=memoryHead;
    m->usage=0;
    m->right=NULL;
    ready->right=NULL;
    running->right=NULL;
    block->right=NULL;

    char ch;
    printf("Please enter the base address and length of the space:");
    scanf("%d%d",&m->begin,&m->size);
    printf("Please enter your command:");
    scanf(" %c",&ch);
    while(ch!='#')
    {

        switch(ch)
        {
        case 'c':
            processCreate();//创建新进程
            break;
        case 't':
            processTime();//时间片到
            break;
        case 'e':
            processEnd();//进程结束
            break;
        case 'b':
            processBlock();//进程阻塞
            break;
        case 'w':
            processWake();//进程唤醒
            break;
        case 's':
            showState();//展示三种状态
            break;
        case 'z':
            showSize();//显示剩余空间
            break;
        default:
            printf("Unknow Command\n");
        }
        printf("Please enter your command:");
        scanf(" %c",&ch);
    }
}
void processCreate()
{
    int flag=0;
    Mypcb *p=(Mypcb *)malloc(sizeof(Mypcb));
    scanf("%s",p->name);
    scanf("%d",&p->size);

    Memory *q=(Memory *)malloc(sizeof(Memory));
    q->usage=1;
    q->size=p->size;
    flag=addM(memoryHead,q);
    if(flag==1)
        return;
    p->size=q->size;
    p->begin=q->begin;
    add(ready,p);
    printf("Process %s was created successfully\n",p->name);
    check();
}
int addM(Memory *head,Memory *q)//最佳适应分配空间
{
    int minnum=999999;
    Memory *temp=head->right,*min=NULL;
    while(temp!=NULL)
    {
        if(temp->usage==0&&temp->size>=q->size&&temp->size-q->size<minnum)//查找最适合的空间
        {
            minnum=temp->size-q->size;
            min=temp;
        }
        temp=temp->right;
    }
    if(min==NULL)
    {
        printf("No enough space!\n");
        return 1;
    }
    else
    {
        if(min->size-q->size<=2)//碎片空间过小时直接送了
            q->size=min->size;
        min->left->right=q;
        q->left=min->left;
        q->right=min;
        min->left=q;
        q->begin=min->begin;
        min->begin=q->begin+q->size;
        min->size-=q->size;
        if(min->size==0)//是否还有剩余
            min->usage=1;
    }
}
void add(struct PCB *head, struct PCB *process)
{
   struct PCB *tmp=head;
   while(tmp->right!=NULL)
       tmp=tmp->right;
   tmp->right=process;
   process->left=tmp;
   process->right=NULL;
}
void check()//检测执行态是否为空
{
    Mypcb *head=running->right,*s1_head=ready->right;
    if(head==NULL&&s1_head!=NULL)
    {
        s1_head->left->right=s1_head->right;
        if(s1_head->right!=NULL)
        {
            s1_head->right->left=s1_head->left;
        }
        add(running,s1_head);
    }
}
void processTime()
{
    Mypcb *s2=running->right;
    if(s2==NULL)
    {
        printf("running is empty!\n");
    }
    else
    {
        s2->left->right=NULL;
        add(ready,s2);
    }
    check();
}
void processEnd()
{
    Mypcb *s2=running->right;
    if(s2==NULL)
    {
        printf("running is empty!\n");
    }
    else
    {
        running->right=NULL;
        reclaimSpace(s2);
    }
    check();
}
void reclaimSpace(Mypcb *head)
{
    Memory *temp=memoryHead->right;
    while(temp!=NULL)
    {
        if(temp->begin==head->begin&&temp->size==head->size&&temp->usage==1)//在空间链表中找到对应的节点
            break;
        temp=temp->right;
    }
    if(temp==NULL)
        printf("Wrong!!Not find the point\n");
    else
    {
        temp->usage=0;
        if(temp->left!=NULL&&temp->left->usage==0)//回收的节点前一个没有使用，合并
        {
            temp->left->size+=temp->size;
            temp->left->right=temp->right;
            if(temp->right!=NULL)
                temp->right->left=temp->left;
            temp=temp->left;
        }
        if(temp->right!=NULL&&temp->right->usage==0)//回收节点的后一个没有使用，合并回收
        {
            temp->size+=temp->right->size;
            temp->right=temp->right->right;
            if(temp->right->right!=NULL)
            {
                temp->right->right->left=temp;
            }
        }
    }
}
void processWake()
{
    Mypcb *s3=block->right;
    if(s3==NULL)
    {
        printf("block is empty!\n");
    }
    else
    {
        s3->left->right=s3->right;
        if(s3->right!=NULL)
        {
            s3->right->left=s3->left;
        }
        add(ready,s3);
    }
    check();
}
void processBlock()
{
    Mypcb *s2=running->right;
    Mypcb *head=block->right;
    if(s2==NULL)
    {
        printf("running is empty!\n");
    }
    else
    {
        add(block,s2);
        running->right=NULL;
    }
    check();
}
void showState()
{
    Mypcb *s1=ready->right,*s2=running->right,*s3=block->right;
    printf("Ready:");
    show(s1);
    printf("Running:");
    show(s2);
    printf("Block:");
    show(s3);
}
void show(Mypcb *head)
{
    Mypcb *temp=head;
    while(temp!=NULL)
    {
        printf("%s(%d,%d) ",temp->name,temp->begin,temp->size);
        temp=temp->right;
    }
    printf("\n");
}
void showSize()
{
    Memory *temp=memoryHead->right;
    printf("The left memory:\n");
    while(temp!=NULL)
    {
        if(temp->usage==0)
        {
            printf("Start:%d,Size:%d\n",temp->begin,temp->size);
        }
        temp=temp->right;
    }
}
