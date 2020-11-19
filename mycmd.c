#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<dirent.h>
#include<stddef.h>
#include<fcntl.h>
#include<time.h>
#include<sys/time.h>
#include<pwd.h>
#include"mycmd.h"
#define buffsize 1024
void pwd_()
{
	char *path=NULL;
	path=getcwd(NULL,0);	//返回当前工作目录的地址//NULL参数动态分配内存
	puts(path);		//输出当前目录
	free(path);			//释放缓冲以防止内存泄漏
}

void makedir_()
{
	char command1[30];
	scanf("%s",command1);
	fflush(stdin);
	if(mkdir(command1,0755)!=0)  //成功则创建一个command1文件，否则返回-1打印创建失败  
		printf("make dir default!\n");
}


void exit_()
{
	exit(0);
}

void chadir_()
{
	char command3[50];
	scanf("%s",command3);
	fflush(stdin);
	if(chdir(command3)!=0)	//成功则切换致command3目录，否则返回-1改变目录失败
	{
		printf("change dir path default!\n");
	}
}

void list_()
{
	char command4[50];
	struct dirent *entry;//目录结构体指针
	DIR *olist=NULL;	//目录流指针
	scanf("%s",command4);
	fflush(stdin);			
	if((olist=opendir(command4))==NULL)	//打开目录command，成功则返回指向dir结构体的指针
	{
		printf("opendir  default!\n");
	}
	while(entry=readdir(olist))			//readdir（plist）读取目录commmand下所有文件（包括目录），返回指向目录command下所有文件的dirent结构体d
	{
		printf("%s\t",entry->d_name);	//打印名字
	}
	printf("\n");
	if(closedir(olist)!=0)
		printf("closedir  default!\n");
}


void copy_()
{
	char cp1[30],cp2[30],buf[buffsize];
	int fd1,fd2;
	int n;
	scanf("%s",cp1);
    	fflush(stdin);
	scanf("%s",cp2);
   	if((fd1=open(cp1,O_RDWR|O_CREAT,0664))==-1)	//open是文件表述符中的函数
		printf("open the file false!\n");
	if((n=read(fd1,buf,buffsize))==-1)
		printf("read the file false!\n");
	if((fd2=open(cp2,O_RDWR|O_CREAT,0664))==-1)
		printf("open the file false!\n");
	if(write(fd2,buf,n)==-1)
		printf("write the file false!\n");
	if(close(fd1)==-1)
		printf("close false!\n");
	if(close(fd2)==-1)
		printf("close false!\n");
}
void find_()
{
	char str1[30],str2[30];
	scanf("%s",str1);
	fflush(stdin);
	scanf("%s",str2);
	fflush(stdin);
	List(str1,str2);
}
void List(char *path,char *name)
{
	struct dirent* ent = NULL;	//传入文件保存基本信息
	DIR *pDir;					//返回一个目录索引
	char dir[512];				//用来存入文件信息
	struct stat statbuf;   		//获取目录的全面信息
	if((pDir=opendir(path))==NULL)//opendir()用来打开参数name 指定的目录, 并返回DIR*形态的目录流,返回位空则直接return
	{
		return;
	}//打开目录
	while((ent=readdir(pDir))!=NULL)//调用readdir()会读到下一个文件，readdir是依次读出目录中的所有文件，每次只能读一个
	{//统计当前文件夹下有多少文件（不包括文件夹）
	//得到读取文件的绝对路径名
		snprintf( dir, 512,"%s/%s", path, ent->d_name );  	 //得到文件信息   
		lstat(dir, &statbuf);								 //返回文件相关信息
		if(S_ISDIR(statbuf.st_mode))						//判断是不是一个目录
		{
			if(strncmp(ent->d_name,".",1) == 0 )  			//判断是不是当前目录
				continue;
			List(dir,name);               					//如果是目录，递归  
		}
		else
		{
			if(strcmp(name,ent->d_name) == 0 )  	//将获取到的文件名传给name
				printf("%s\n",dir);					//打印文件
		}       
	}
	closedir(pDir);		//关闭目录，对应之前由opendir()打开的目录
}
void touch_()
{
	char str1[30];
	scanf("%s",str1);
	fflush(stdin);
	if(access(str1,F_OK)==0){
		struct timespec newtime[2];
		newtime[0].tv_nsec=UTIME_NOW;
		newtime[1].tv_nsec=UTIME_NOW;
		utimensat(AT_FDCWD,str1,newtime,0);
	}else{
		open(str1,O_CREAT,0644);
	}
}
void whoami_()
{
	uid_t id;					//uid_t是Linux中专门定义ID的数据类型，是整型
	struct passwd * pa;
	id=geteuid();				//获得有效UID值
	pa=getpwuid(id);			//通过UID值获取password数据
	printf("%s\n",pa->pw_name);	//输出pa结构体中的pw_name
	
}
void remove_()
{
	char str1[30];
	scanf("%s",str1);
	fflush(stdin);	//清空缓存区
	remove_dir(str1);
	
}
int remove_dir(const char *dir)
{
	char cur_dir[] = ".";	//当前目录
	char up_dir[] = "..";	//父目录
	char dir_name[512];		//存入文件信息
	DIR *dirp;				//要删除的目录的绝对路径    
	struct dirent *dp;		//传入文件保存基本信息
	struct stat dir_stat;	//获取目录的全面信息
	if ( 0 != access(dir, F_OK) ) {	//access()判断是否有存取文件的权限，传入F_OK目的是检查文件是否存在
		return 0; 		// 参数传递进来的目录不存在，直接返回
	}
	if ( 0 > stat(dir, &dir_stat) ) {	//stat()获取文件信息，成功为0，失败为-1
		return -1;		// 获取目录属性失败，返回错误
	}
	if ( S_ISREG(dir_stat.st_mode) ) {		// 这里S_ISREG判断是否为普通文件，普通文件直接删除
		remove(dir);
	} else if ( S_ISDIR(dir_stat.st_mode) ) { 	// S_ISDIR()判断是否目录文件
		dirp = opendir(dir);
		while ( (dp=readdir(dirp)) != NULL ) {	//循环读取
			if ( (0 == strcmp(cur_dir, dp->d_name)) || (0 == strcmp(up_dir, dp->d_name)) ) {
				continue;	// 忽略 .文件 和 ..文件
			}
			sprintf(dir_name, "%s/%s", dir, dp->d_name);//存入信息
			remove_dir(dir_name);// 递归调用递归结束的条件是遍历到的文件为普通文件（此时会将普通文件删除）
       	}
        	closedir(dirp);	//关闭目录流
        	rmdir(dir);		// 当一个目录中的内容全部被删除之后，就可以删除该目录（空目录）
	} else {
		perror("unknow file type!");    
	}
}
void error_()
{
	printf("command not found !\n");
}
void date(){
	time_t t;
	time(&t);//获取从1970至今过了多少秒，存入time_t类型的t;
	printf("%s", ctime(&t));//用ctime将秒数转化成字符串格式，输出：Thu Feb 28 14:14:17 2019
}
void clear(){
	printf("\x1b[H\x1b[2J");
}

