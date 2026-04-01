#include "header.h"

//linked list for backgroung job management

void insert_at_first(pid_t pid,const char* cmd) // insert a new job at the front of the list
{
    Slist* node = malloc(sizeof(Slist));
    if(node == NULL)
    {
        perror("malloc");
        return;
    }
    node->pid = pid;
    strncpy(node->command,cmd,MAX_INPUT-1);
    node->command [MAX_INPUT - 1] = '\0';
    node->next = head;
    head = node;
}

void delete_node(pid_t pid) // delete the node matching given data
{
    Slist* curr = head;
    Slist* prev = NULL;

    while(curr != NULL)
    {
            if(curr->pid == pid)
            {
                if(prev == NULL)
                {
                    head = curr->next;
                }
                else
                    prev->next = curr->next;
                free(curr);
                return;
            }
            prev = curr;
            curr = curr->next;
        
    }
}

void delete_first(void) //delete first node (most recently added job)
{
    if(head == NULL)
    return;
    Slist* tmp = head;
    head = head->next;
    free(tmp);
}

void print_list(Slist* node)
{
    int index = 1;
    if(node == NULL)
    {
        printf("No background jobd\n");
        return;
    }
    while(node != NULL)
    {
        printf("[%d] PID: %d Command: %s\n",index,node->pid,node->command);
        node = node->next;
        index++;
    }
}