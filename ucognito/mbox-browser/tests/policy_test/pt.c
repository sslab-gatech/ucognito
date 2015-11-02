#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    printf("My PID: %d\n", getpid());
    system("cat a_subdir/a");
    system("cat a_subdir/b");
    system("cat a_subdir/c");
    system("cat a_subdir/d");
    system("cat a_subdir/e");
    system("cat a_subdir/numdir/1");
    system("cat a_subdir/numdir/2");
    system("cat a_subdir/numdir/3");
    system("cat a_subdir/capdir/A");
    system("cat a_subdir/capdir/B");
    system("cat a_subdir/capdir/C");
    system("cat b_subdir/b_content");
}
