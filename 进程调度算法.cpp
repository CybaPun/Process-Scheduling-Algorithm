#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <Windows.h>
using namespace std;

typedef struct PCB {//进程结构体
    char name[10];      //进程名
    int state;          //进程状态 0:就绪，1:等待，2:运行
    int priority;       //优先级
    int remaining_time; //运行完成耗时
    struct PCB* next;   //构造链表
} PCB;

//维护三个全局指针
PCB* ready_queue = NULL;
PCB* wait_queue = NULL;
PCB* current_process = NULL;

// 初始化进程
void init_processes() {
    srand(time(NULL));//以时间作为随机数种子，保证随机性
    printf("\n正在随机生成10个待调度的进程：\n\n进程名\t进程状态(0:就绪 1:等待)\t  优先级   需在处理机执行的时间(ms)\n");
    for (int i = 0; i < 10; i++) {
        PCB* p = new PCB;// PCB* p = (PCB*)malloc(sizeof(PCB));
        snprintf(p->name, sizeof(p->name), "P%d", i + 1); //sprintf_s(p->name, "P%d", i + 1);
        p->state = rand() % 2;
        p->priority = (rand() % 10) + 1;
        p->remaining_time = (rand() % 50) + 1;
        p->next = NULL;
        printf("  %s\t\t%d\t\t    %d\t\t    %d\n", p->name, p->state, p->priority, p->remaining_time);

        if (p->state == 0) {
            if (ready_queue == NULL) ready_queue = p;
            else {
                PCB* temp = ready_queue;
                while (temp->next) temp = temp->next;
                temp->next = p;
            }
        }
        else {
            if (wait_queue == NULL) wait_queue = p;
            else {
                PCB* temp = wait_queue;
                while (temp->next) temp = temp->next;
                temp->next = p;
            }
        }
    }
}

// 打印队列
void print_queues() {
    printf("当前执行进程: %s\n", current_process ? current_process->name : "无");
    printf("已随机分配到队列，如下所示\n");

    printf("就绪队列: ");
    PCB* temp = ready_queue;
    while (temp) {
        if (temp != current_process) printf("%s ", temp->name);
        temp = temp->next;
    }

    printf("\n等待队列: ");
    temp = wait_queue;
    while (temp) {
        printf("%s ", temp->name);
        temp = temp->next;
    }
    printf("\n\n");
}

// FIFO调度
void schedule_fifo() {
F:
    while (ready_queue != NULL) {//一直到就绪队列为空，程序执行结束
        current_process = ready_queue;
        ready_queue = ready_queue->next;
        current_process->next = NULL;
        current_process->state = 2;

        // 随机移动等待队列头到就绪队列尾
        if (rand() % 2 == 1 && wait_queue) {
            PCB* wait_head = wait_queue;
            wait_queue = wait_queue->next;
            wait_head->next = NULL;
            wait_head->state = 0;

            if (!ready_queue) ready_queue = wait_head;
            else {
                PCB* tail = ready_queue;
                while (tail->next) tail = tail->next;
                tail->next = wait_head;
            }
        }

        // 生成执行时间
        int execute_time = rand() % 50 + 1;
        printf("执行进程 %s，实际完成所需时间还要 %dms，分配时间: %dms → ", current_process->name, current_process->remaining_time, execute_time);

        if (execute_time >= current_process->remaining_time) {
            printf("完成\n");
            free(current_process);
        }
        else {
            current_process->remaining_time -= execute_time;
            printf("剩余待完成时间: %dms\n", current_process->remaining_time);

            // 随机加入就绪或等待队列
            if (rand() % 2 == 0) {
                current_process->state = 0;
                if (!ready_queue) ready_queue = current_process;
                else {
                    PCB* tail = ready_queue;
                    while (tail->next) tail = tail->next;
                    tail->next = current_process;
                }
            }
            else {
                current_process->state = 1;
                if (!wait_queue) wait_queue = current_process;
                else {
                    PCB* tail = wait_queue;
                    while (tail->next) tail = tail->next;
                    tail->next = current_process;
                }
            }
        }

        current_process = NULL;
        print_queues();
        Sleep(500);
    }
    if (wait_queue) {
        ready_queue = wait_queue;
        wait_queue = NULL;
        printf("\n此时就绪队列为空，程序执行结束，但等待队列仍有未执行完的进程，现将等待队列的进程挪至就绪队列继续运行......\n\n");
        print_queues();
        Sleep(500);
        goto F;
    }
    printf("\n\nFIFO调度算法演示结束！\n");
}

// 优先级调度（假设优先级1为最高）
void schedule_priority() {
P:
    while (ready_queue != NULL) {//一直到就绪队列为空，程序执行结束
        // 遍历找到优先级最高的进程
        PCB* prev = NULL, * max_prev = NULL;
        PCB* current = ready_queue, * max_pcb = ready_queue;
        int max_priority = ready_queue->priority;

        while (current) {
            if (current->priority < max_priority) {  // 优先级值越小，优先级越高
                max_priority = current->priority;
                max_pcb = current;//更新当前优先级最高的节点
                max_prev = prev;//保存当前优先级最高的节点的前一个节点以方便对其进行删除操作
            }
            prev = current;
            current = current->next;
        }

        // 从就绪队列中移除最高优先级进程
        if (max_prev == NULL) {
            ready_queue = max_pcb->next;
        }
        else {
            max_prev->next = max_pcb->next;
        }
        current_process = max_pcb;
        current_process->state = 2;
        current_process->next = NULL;

        // 随机移动等待队列头到就绪队列尾
        if (rand() % 2 == 1 && wait_queue) {
            PCB* wait_head = wait_queue;
            wait_queue = wait_queue->next;
            wait_head->next = NULL;
            wait_head->state = 0;

            if (!ready_queue) ready_queue = wait_head;
            else {
                PCB* tail = ready_queue;
                while (tail->next) tail = tail->next;
                tail->next = wait_head;
            }
        }

        // 生成执行时间
        int execute_time = rand() % 50 + 1;
        printf("执行进程 %s（优先级%d），实际完成所需时间还要 %dms，分配时间: %dms → ",
            current_process->name, current_process->priority, current_process->remaining_time, execute_time);

        if (execute_time >= current_process->remaining_time) {
            printf("完成\n");
            free(current_process);
        }
        else {
            current_process->remaining_time -= execute_time;
            printf("剩余待完成时间: %dms\n", current_process->remaining_time);

            // 随机加入就绪或等待队列
            if (rand() % 2 == 0) {
                current_process->state = 0;
                if (!ready_queue) ready_queue = current_process;
                else {
                    PCB* tail = ready_queue;
                    while (tail->next) tail = tail->next;
                    tail->next = current_process;
                }
            }
            else {
                current_process->state = 1;
                if (!wait_queue) wait_queue = current_process;
                else {
                    PCB* tail = wait_queue;
                    while (tail->next) tail = tail->next;
                    tail->next = current_process;
                }
            }
        }

        current_process = NULL;
        print_queues();
        Sleep(500);
    }
    if (wait_queue) {
        ready_queue = wait_queue;
        wait_queue = NULL;
        printf("\n此时就绪队列为空，程序执行结束，但等待队列仍有未执行完的进程，现将等待队列的进程挪至就绪队列继续运行......\n\n");
        print_queues();
        Sleep(500);
        goto P;
    }
    printf("\n\n优先级调度算法演示结束！\n");
}

// SJF调度
void schedule_sjf() {
S:
    while (ready_queue != NULL) {//一直到就绪队列为空，程序执行结束
        // 遍历找到耗时最短的进程
        PCB* prev = NULL, * min_prev = NULL;
        PCB* current = ready_queue, * min_pcb = ready_queue;
        int min_time = ready_queue->remaining_time;

        while (current) {
            if (current->remaining_time < min_time) {
                min_time = current->remaining_time;
                min_pcb = current;//更新当前耗时最短的节点
                min_prev = prev;//保存当前耗时最短的节点的前一个节点以方便对其进行删除操作
            }
            prev = current;
            current = current->next;
        }

        // 从就绪队列中移除最短进程
        if (min_prev == NULL) {
            ready_queue = min_pcb->next;
        }
        else {
            min_prev->next = min_pcb->next;
        }
        current_process = min_pcb;
        current_process->state = 2;
        current_process->next = NULL;

        // 随机移动等待队列头到就绪队列尾
        if (rand() % 2 == 1 && wait_queue) {
            PCB* wait_head = wait_queue;
            wait_queue = wait_queue->next;
            wait_head->next = NULL;
            wait_head->state = 0;

            if (!ready_queue) ready_queue = wait_head;
            else {
                PCB* tail = ready_queue;
                while (tail->next) tail = tail->next;
                tail->next = wait_head;
            }
        }

        // 生成执行时间
        int execute_time = rand() % 50 + 1;
        printf("执行进程 %s（当前所需时间： %dms），分配时间: %dms → ",
            current_process->name, current_process->remaining_time, execute_time);

        if (execute_time >= current_process->remaining_time) {
            printf("完成\n");
            free(current_process);
        }
        else {
            current_process->remaining_time -= execute_time;
            printf("剩余待完成时间: %dms\n", current_process->remaining_time);

            // 随机加入就绪或等待队列
            if (rand() % 2 == 0) {
                current_process->state = 0;
                if (!ready_queue) ready_queue = current_process;
                else {
                    PCB* tail = ready_queue;
                    while (tail->next) tail = tail->next;
                    tail->next = current_process;
                }
            }
            else {
                current_process->state = 1;
                if (!wait_queue) wait_queue = current_process;
                else {
                    PCB* tail = wait_queue;
                    while (tail->next) tail = tail->next;
                    tail->next = current_process;
                }
            }
        }

        current_process = NULL;
        print_queues();
        Sleep(500);
    }
    if (wait_queue) {
        ready_queue = wait_queue;
        wait_queue = NULL;
        printf("\n此时就绪队列为空，程序执行结束，但等待队列仍有未执行完的进程，现将等待队列的进程挪至就绪队列继续运行......\n\n");
        print_queues();
        Sleep(500);
        goto S;
    }
    printf("\n\nSJF调度算法演示结束！\n");
}

// 释放所有进程内存
void free_processes() {
    PCB* temp = ready_queue;
    while (temp) {
        PCB* next = temp->next;
        free(temp);
        temp = next;
    }
    ready_queue = NULL;

    temp = wait_queue;
    while (temp) {
        PCB* next = temp->next;
        free(temp);
        temp = next;
    }
    wait_queue = NULL;
}

int main() {
    while (1) {
        free_processes();    // 释放之前的进程内存
        init_processes();    // 初始化新进程
        printf("\n\n初始队列状态:\n");
        print_queues();
        // 显示菜单
        printf("\n---------------------------------------------------------------------\n\n");
        printf("请选择进程调度算法（输入数字序号：1 - FIFO, 2 - 优先级, 3 - SJF），输入 q 可退出程序：\n");
        M:
        string input = " ";
        cin >> input;
        switch (input[0]) {
            case '1':
                printf("\n正在执行FIFO调度算法：\n\n");
                schedule_fifo(); break;
            case '2':
                printf("\n正在执行优先级调度算法（假设优先级1为最高）：\n\n");
                schedule_priority(); break;
            case '3':
                printf("\n正在执行SJF调度算法：\n\n");
                schedule_sjf(); break;
            case 'q':
                break;
            default:
                cout << "\n无效输入，请重试：\n";
                goto M; break;
        }
        if (input[0] == 'q')break;
        cout << "\n按回车键继续......";
        cin.get(); cin.get();
        system("cls");
    }
    free_processes(); // 最终清理
    printf("\n\n");
    system("pause");
    return 0;
}