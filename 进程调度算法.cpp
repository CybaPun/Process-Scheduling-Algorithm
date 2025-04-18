#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <Windows.h>
using namespace std;

typedef struct PCB {//���̽ṹ��
    char name[10];      //������
    int state;          //����״̬ 0:������1:�ȴ���2:����
    int priority;       //���ȼ�
    int remaining_time; //������ɺ�ʱ
    struct PCB* next;   //��������
} PCB;

//ά������ȫ��ָ��
PCB* ready_queue = NULL;
PCB* wait_queue = NULL;
PCB* current_process = NULL;

// ��ʼ������
void init_processes() {
    srand(time(NULL));//��ʱ����Ϊ��������ӣ���֤�����
    printf("\n�����������10�������ȵĽ��̣�\n\n������\t����״̬(0:���� 1:�ȴ�)\t  ���ȼ�   ���ڴ����ִ�е�ʱ��(ms)\n");
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

// ��ӡ����
void print_queues() {
    printf("��ǰִ�н���: %s\n", current_process ? current_process->name : "��");
    printf("��������䵽���У�������ʾ\n");

    printf("��������: ");
    PCB* temp = ready_queue;
    while (temp) {
        if (temp != current_process) printf("%s ", temp->name);
        temp = temp->next;
    }

    printf("\n�ȴ�����: ");
    temp = wait_queue;
    while (temp) {
        printf("%s ", temp->name);
        temp = temp->next;
    }
    printf("\n\n");
}

// FIFO����
void schedule_fifo() {
F:
    while (ready_queue != NULL) {//һֱ����������Ϊ�գ�����ִ�н���
        current_process = ready_queue;
        ready_queue = ready_queue->next;
        current_process->next = NULL;
        current_process->state = 2;

        // ����ƶ��ȴ�����ͷ����������β
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

        // ����ִ��ʱ��
        int execute_time = rand() % 50 + 1;
        printf("ִ�н��� %s��ʵ���������ʱ�仹Ҫ %dms������ʱ��: %dms �� ", current_process->name, current_process->remaining_time, execute_time);

        if (execute_time >= current_process->remaining_time) {
            printf("���\n");
            free(current_process);
        }
        else {
            current_process->remaining_time -= execute_time;
            printf("ʣ������ʱ��: %dms\n", current_process->remaining_time);

            // ������������ȴ�����
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
        printf("\n��ʱ��������Ϊ�գ�����ִ�н��������ȴ���������δִ����Ľ��̣��ֽ��ȴ����еĽ���Ų���������м�������......\n\n");
        print_queues();
        Sleep(500);
        goto F;
    }
    printf("\n\nFIFO�����㷨��ʾ������\n");
}

// ���ȼ����ȣ��������ȼ�1Ϊ��ߣ�
void schedule_priority() {
P:
    while (ready_queue != NULL) {//һֱ����������Ϊ�գ�����ִ�н���
        // �����ҵ����ȼ���ߵĽ���
        PCB* prev = NULL, * max_prev = NULL;
        PCB* current = ready_queue, * max_pcb = ready_queue;
        int max_priority = ready_queue->priority;

        while (current) {
            if (current->priority < max_priority) {  // ���ȼ�ֵԽС�����ȼ�Խ��
                max_priority = current->priority;
                max_pcb = current;//���µ�ǰ���ȼ���ߵĽڵ�
                max_prev = prev;//���浱ǰ���ȼ���ߵĽڵ��ǰһ���ڵ��Է���������ɾ������
            }
            prev = current;
            current = current->next;
        }

        // �Ӿ����������Ƴ�������ȼ�����
        if (max_prev == NULL) {
            ready_queue = max_pcb->next;
        }
        else {
            max_prev->next = max_pcb->next;
        }
        current_process = max_pcb;
        current_process->state = 2;
        current_process->next = NULL;

        // ����ƶ��ȴ�����ͷ����������β
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

        // ����ִ��ʱ��
        int execute_time = rand() % 50 + 1;
        printf("ִ�н��� %s�����ȼ�%d����ʵ���������ʱ�仹Ҫ %dms������ʱ��: %dms �� ",
            current_process->name, current_process->priority, current_process->remaining_time, execute_time);

        if (execute_time >= current_process->remaining_time) {
            printf("���\n");
            free(current_process);
        }
        else {
            current_process->remaining_time -= execute_time;
            printf("ʣ������ʱ��: %dms\n", current_process->remaining_time);

            // ������������ȴ�����
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
        printf("\n��ʱ��������Ϊ�գ�����ִ�н��������ȴ���������δִ����Ľ��̣��ֽ��ȴ����еĽ���Ų���������м�������......\n\n");
        print_queues();
        Sleep(500);
        goto P;
    }
    printf("\n\n���ȼ������㷨��ʾ������\n");
}

// SJF����
void schedule_sjf() {
S:
    while (ready_queue != NULL) {//һֱ����������Ϊ�գ�����ִ�н���
        // �����ҵ���ʱ��̵Ľ���
        PCB* prev = NULL, * min_prev = NULL;
        PCB* current = ready_queue, * min_pcb = ready_queue;
        int min_time = ready_queue->remaining_time;

        while (current) {
            if (current->remaining_time < min_time) {
                min_time = current->remaining_time;
                min_pcb = current;//���µ�ǰ��ʱ��̵Ľڵ�
                min_prev = prev;//���浱ǰ��ʱ��̵Ľڵ��ǰһ���ڵ��Է���������ɾ������
            }
            prev = current;
            current = current->next;
        }

        // �Ӿ����������Ƴ���̽���
        if (min_prev == NULL) {
            ready_queue = min_pcb->next;
        }
        else {
            min_prev->next = min_pcb->next;
        }
        current_process = min_pcb;
        current_process->state = 2;
        current_process->next = NULL;

        // ����ƶ��ȴ�����ͷ����������β
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

        // ����ִ��ʱ��
        int execute_time = rand() % 50 + 1;
        printf("ִ�н��� %s����ǰ����ʱ�䣺 %dms��������ʱ��: %dms �� ",
            current_process->name, current_process->remaining_time, execute_time);

        if (execute_time >= current_process->remaining_time) {
            printf("���\n");
            free(current_process);
        }
        else {
            current_process->remaining_time -= execute_time;
            printf("ʣ������ʱ��: %dms\n", current_process->remaining_time);

            // ������������ȴ�����
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
        printf("\n��ʱ��������Ϊ�գ�����ִ�н��������ȴ���������δִ����Ľ��̣��ֽ��ȴ����еĽ���Ų���������м�������......\n\n");
        print_queues();
        Sleep(500);
        goto S;
    }
    printf("\n\nSJF�����㷨��ʾ������\n");
}

// �ͷ����н����ڴ�
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
        free_processes();    // �ͷ�֮ǰ�Ľ����ڴ�
        init_processes();    // ��ʼ���½���
        printf("\n\n��ʼ����״̬:\n");
        print_queues();
        // ��ʾ�˵�
        printf("\n---------------------------------------------------------------------\n\n");
        printf("��ѡ����̵����㷨������������ţ�1 - FIFO, 2 - ���ȼ�, 3 - SJF�������� q ���˳�����\n");
        M:
        string input = " ";
        cin >> input;
        switch (input[0]) {
            case '1':
                printf("\n����ִ��FIFO�����㷨��\n\n");
                schedule_fifo(); break;
            case '2':
                printf("\n����ִ�����ȼ������㷨���������ȼ�1Ϊ��ߣ���\n\n");
                schedule_priority(); break;
            case '3':
                printf("\n����ִ��SJF�����㷨��\n\n");
                schedule_sjf(); break;
            case 'q':
                break;
            default:
                cout << "\n��Ч���룬�����ԣ�\n";
                goto M; break;
        }
        if (input[0] == 'q')break;
        cout << "\n���س�������......";
        cin.get(); cin.get();
        system("cls");
    }
    free_processes(); // ��������
    printf("\n\n");
    system("pause");
    return 0;
}