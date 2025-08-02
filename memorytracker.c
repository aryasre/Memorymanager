#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct node
{
	void *address;
    size_t size;
	struct node *link;
}Slist;

typedef struct FreedNode {
    void *address;
    size_t size;
    struct FreedNode *link;
} FreedNode;


Slist *head = NULL;
FreedNode *freed_list = NULL;


void insert_first(void *ptr,size_t max)
{
    Slist *new = malloc(sizeof(Slist));
    new->address = ptr;
    new->size = max;
    new->link = head;
    head  = new;
}

void log_data(void *ptr,size_t size,const char *dma)
{
   FILE *fp = fopen("memorylog.txt","a");
    if(fp == NULL)
    {
        printf("Failed to open file\n");
        return;
    }
   fprintf(fp, "[%s] Address = %p, Size = %zu bytes\n",dma,ptr,size);
    fclose(fp);
}
size_t remove_node(void *ptr)
{
    Slist *temp = head;
    Slist *prev = NULL;
    size_t capacity;
    while(temp!= NULL)
    { 
        
        if(temp->address == ptr)
        {
            if(temp == head)
            {
                head = temp->link;
            }
            else
            {
          prev->link = temp->link;
            }
          capacity = temp->size;
          free(temp);
          return capacity;
        }
        prev = temp;
        temp = temp->link;
    }

    return 0;
}
void my_free(void *ptr)
{
    if(ptr)
    {
        Slist *temp = ptr;
        size_t size = remove_node(temp);
         if (size == 0) {
            printf("Warning: Attempted to free untracked or already freed memory: %p\n", temp);
            return;
        }

        
        FreedNode *fnode = malloc(sizeof(FreedNode));
        fnode->address = ptr;
        fnode->size = size;
        fnode->link = freed_list;
        freed_list = fnode;

       FILE *fp = fopen("memorylog.txt", "a");
        if(fp!=NULL)
        {
        fprintf(fp, "[FREE ] Address = %p, Size = %zu bytes\n", ptr, size);
        fclose(fp);
        
        }
         
    }
}
void *my_malloc(size_t size)
{
    void *mptr = malloc(size);
    if(mptr == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    insert_first(mptr,size);
    log_data(mptr,size,"MALLOC");
    
    return mptr;

}

void *my_calloc(size_t nmemb,size_t size)
{
    void *cptr = calloc(nmemb,size);
    if(cptr == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    log_data(cptr,nmemb*size,"CALLOC");
    insert_first(cptr,nmemb*size);

    return cptr;

}

void *my_realloc(void *ptr,size_t size)
{
    void *rptr = realloc(ptr,size);
    if(rptr == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }


   if (rptr != ptr)
    {
       my_free(ptr);
        insert_first(rptr, size);
        log_data(rptr,size,"REALLOC");
    }
    else
    {
        
        Slist *temp = head;
        while (temp)
        {
            if (temp->address == ptr)
            {
                temp->size = size;
                break;
            }
            temp = temp->link;
        }

        log_data(rptr,size,"REALLOC");
        insert_first(rptr, size);
    }

    return rptr;
}



void print_freed_memory()
{
    FreedNode *temp = freed_list;

    FILE *fp = fopen("memorylog.txt", "a");
    fprintf(fp, "\n--- Freed Memory Blocks ---\n");
    
    if (!temp)
        fprintf(fp, "None\n");

    while(temp)
    {
        fprintf(fp, "Address: %p, Size: %zu bytes\n", temp->address, temp->size);
        temp = temp->link;
    }

    fclose(fp);
}

void available_memory()
{
    FreedNode *temp = freed_list;

    FILE *fp = fopen("memorylog.txt", "r");
    printf("\n--- Available Memory Blocks ---\n");
    if(temp == NULL)
    {
        printf("No memory is available\n");
        return;
    }
    
    while(temp)
    {
        
        printf("Address: %p, Size: %zu bytes\n", temp->address, temp->size);
        temp = temp->link;
    }

    fclose(fp);
}

void check_memory_leak()
{
    FILE *fp = fopen("memorylog.txt", "a");
    if (!fp) {
        perror("Failed to open memorylog.txt");
        return;
    }

    fprintf(fp, "\n-----Memory leak------\n");

    Slist *temp1 = head;
    FreedNode *temp2; 
    int flag;
    size_t total_leaked = 0;

    while (temp1 != NULL)
    {
        flag = 0;
        temp2 = freed_list; 
        while (temp2 != NULL)
        {
            if (temp1->address == temp2->address)
            {
                flag = 1;
                break;
            }
            temp2 = temp2->link;
        }

        if (flag == 0)
        {
            fprintf(fp, "Address: %p, Size: %zu bytes\n", temp1->address, temp1->size);
            total_leaked += temp1->size;
        }

        temp1 = temp1->link;
    }

    fprintf(fp, "Total leaked memory: %zu bytes\n", total_leaked);
    fclose(fp);
}


int main()
{
    FILE *fp;
    fp = fopen("memorylog.txt","w");
   fprintf(fp,"------Memory log------\n");
    fclose(fp);

    int *a = (int *)my_malloc(10*sizeof(int));
    char *b = (char *)my_calloc(15,sizeof(char));
    double *c = (double *)my_malloc(10*sizeof(double));
     a  = my_realloc(a,20*sizeof(int));
    my_free(a);
    my_free(c);

    print_freed_memory();
    check_memory_leak();
    available_memory();
   
    
}
