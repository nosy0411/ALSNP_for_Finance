#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// ������ �Լ� 
// 1�ʸ� ��ٸ��� �ƱԸ�Ʈ^2 �� �����Ѵ�. 
void *t_function(void *data)
{
    int num = *((int *)data);
    printf("num %d\n", num);
    sleep(1);
    return (void *)(num*num);
}

int main()
{
    pthread_t p_thread;
    int thr_id;
    int status;
    int a = 100;

    thr_id = pthread_create(&p_thread, NULL, t_function, (void *)&a);
    if (thr_id < 0)
    {
        perror("thread create error : ");
        exit(0);
    }
    // ������ �ĺ��� p_thread �� ����Ǳ� ��ٷȴٰ� 
    // ���Ḯ�ϰ��� �����´�. 
    pthread_join(p_thread, (void *)&status);
    printf("thread join : %d\n", status);

    return 0;
}