#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#include <time.h>

#define DEFAULT "\x1B[0m"
#define M "\x1B[1;95m"
#define G "\x1B[1;32m"
#define B "\x1B[1;34m"



void print_permission(int mode)
{
    char permission_string[12];
    //file type
    if(S_ISREG(mode)) permission_string[0] = '-';
    if(S_ISDIR(mode)) permission_string[0] = 'd';
    if(S_ISLNK(mode)) permission_string[0] = 'l';
    //permission owner
    (mode & S_IRUSR) ? (permission_string[1] = 'r') : (permission_string[1] = '-');
    (mode & S_IWUSR) ? (permission_string[2] = 'w') : (permission_string[2] = '-');
    (mode & S_IXUSR) ? (permission_string[3] = 'x') : (permission_string[3] = '-');
    //permission group
    (mode & S_IRGRP) ? (permission_string[4] = 'r') : (permission_string[4] = '-');
    (mode & S_IWGRP) ? (permission_string[5] = 'w') : (permission_string[5] = '-');
    (mode & S_IXGRP) ? (permission_string[6] = 'x') : (permission_string[6] = '-');
    //permission world
    (mode & S_IROTH) ? (permission_string[7] = 'r') : (permission_string[7] = '-');
    (mode & S_IWOTH) ? (permission_string[8] = 'w') : (permission_string[8] = '-');
    (mode & S_IXOTH) ? (permission_string[9] = 'x') : (permission_string[9] = '-');
    permission_string[10] = '\0';
    printf("%s",permission_string);
}
void print_Name(int mode,char*name)
{
    //file type
    
    if(S_ISDIR(mode)) //if dir
    {
        printf(B "%s",name); 
        printf(DEFAULT);
    }
    if(S_ISLNK(mode)) //if symbolic link
    {
        printf(M "%s",name);
        char bufer[512];
        ssize_t bufsiz=readlink(name,bufer,512);
        bufer[bufsiz] = '\0';
        if(bufsiz!=-1)
            printf(DEFAULT " -> %s",bufer);
    }
    if(S_ISREG(mode)) //if is file
    {
        if((mode & S_IXOTH)||(mode & S_IXGRP)||(mode & S_IXUSR))
        {
            printf(G "%s",name); printf(DEFAULT);
        }
        else
        {
            printf(DEFAULT "%s",name);
        }
    }
}
void file(char *d_name)
{
    if(open(d_name,O_RDONLY)<0)
    {
        printf("cannot access '%s': No such file or directory\n",d_name);
        return;
    }
    struct stat mystat_f;
    struct tm * mytime;
    if(lstat(d_name, &mystat_f)==-1)
    {
        perror("cannot get file status");
        return;
    }
    print_permission(mystat_f.st_mode);
    printf(" %d", (int)mystat_f.st_nlink);
    printf(" %s", getpwuid(mystat_f.st_uid)->pw_name);
    printf(" %s", getgrgid(mystat_f.st_gid)->gr_name);
    printf(" %ld", (long)mystat_f.st_size);
    //print time
    char buf[100];
    mytime = localtime(&mystat_f.st_atime);
    strftime(buf,100," %h %e %Y ",mytime);
    printf("%s",buf);
    print_Name(mystat_f.st_mode,d_name);
    printf("\n"); 
}
void ls_la(char *d_name,int num_of_param)
{
    DIR * dir=opendir(d_name);
    if(dir==NULL)//its a file
    {
        file(d_name);
    }
    else//its a dir
    {   
        struct dirent * dp;
        struct stat mystat_d;
        struct tm * mytime;
        if(num_of_param>2)
           {
               printf("%s:\n",d_name);
           }
        while((dp = readdir(dir)) != NULL)
        {
    	    char path[512];
    	    sprintf(path, "%s/%s", d_name, dp->d_name);
            if(lstat(path, &mystat_d)==-1)
            {
                perror("cannot get file status");
                return;
            }
            print_permission(mystat_d.st_mode);
            printf(" %d", (int)mystat_d.st_nlink);
            printf(" %s", getpwuid(mystat_d.st_uid)->pw_name);
            printf(" %s", getgrgid(mystat_d.st_gid)->gr_name);
            printf(" %ld", (long)mystat_d.st_size);
            //print time
            char buf[100];
            mytime = localtime(&mystat_d.st_atime);
            strftime(buf,100," %h %e %Y ",mytime);
            printf("%s",buf);
            print_Name(mystat_d.st_mode, dp->d_name);
            printf("\n");
        }
        printf("\n");
        closedir(dir);
    }
}


int main(int argc,char *argv[])
{	
    if(argc==1)
    {
        ls_la(".",argc);
    }
    else
    {
        for(int i=1;i<argc;i++)
        {
            ls_la(argv[i],argc);
        }
    }        
    return 0;
}
