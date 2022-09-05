#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include <sys/ipc.h>
#include <sys/shm.h>

struct share_mem_head {
    int first_id;
    int tail_id;
    int count;
};

struct share_mem_node {
    int id;
    int next_id;
    int data;
};

#define SHM_HEAD_KEY (int)'chmx'


void share_mem_add_node(struct share_mem_head *head, int data)
{
    int node_id = shmget(SHM_HEAD_KEY + head->count + 1, sizeof(struct share_mem_node), IPC_CREAT | 0666);
    struct share_mem_node *node = shmat(node_id, NULL, 0);
    node->id = node_id;
    node->next_id = -1;
    node->data = data;

    if (head->count == 0) {
        head->first_id = node_id;
        head->tail_id = node_id;
    } else {
        // count > 0
        struct share_mem_node *tail_node = shmat(head->tail_id, NULL, 0);
        tail_node->next_id = node_id;
        head->tail_id = node_id;
        shmdt(tail_node);
    }
    shmdt(node);

    head->count++;
}

void share_mem_list_node(struct share_mem_head *head)
{
    int node_id = head->first_id;
    while (node_id != -1) {
        struct share_mem_node *node = shmat(node_id, NULL, 0);
        printf("node id: %d, data: %d\n", node_id, node->data);

        node_id = node->next_id;
    }
}

int main(int argc, char *argv[])
{
    int ret = 0;

    struct share_mem_head *head = NULL;

    int head_id = shmget(SHM_HEAD_KEY, sizeof(struct share_mem_head), IPC_CREAT | 0666);
    head = (struct share_mem_head *)shmat(head_id, NULL, 0);
    memset(head, 0, sizeof(struct share_mem_head));
    head->first_id = -1;
    head->tail_id = -1;
    head->count = 0;

    int i = 0;
    for (i = 0; i < 10; i++) {
        share_mem_add_node(head, i);
    }

    share_mem_list_node(head);

    return ret;
}