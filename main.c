#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>


struct FileWr
{
    unsigned long int id_Parent;
    unsigned long int id;
    unsigned long int info_Bites;
    char* name;
    char* path;
};

struct Counter
{
    unsigned long int id;
};

char* LongToStr(unsigned long int id)
{
    char* Conv=(char*)malloc(10*sizeof(char));
    for(int j=0;j<10;j++) Conv[j]='0';
    int i=0;
    while(id>0)
    {
        Conv[9-i]+=id%10;
        id=id/10;
        i++;
    }
    return Conv;
}

char* AddChar(char* Name,int n)
{
 char* Conv=(char*)malloc((n+2)*sizeof(char));
 for(int j=0;j<n+2;j++) Conv[j]='/';
 for(int i=1;i<n+1;i++)
    {
        Conv[i]=Name[i-1];
    }
 return Conv;
}

int StrToLong(char* Name,int n)
{
 int Summ=0;
 for(int i=0;i<n;i++)
    {
        Summ*=10;
        Summ+=Name[i]-'0'; 
    }
 return Summ;
}


char* MyCat(char* Name,int n,char* Name2,int n2)
{
 char* Conv=(char*)malloc((n+n2+1)*sizeof(char));
 for(int j=0;j<n+n2;j++) Conv[j]='/';
 for(int i=0;i<n;i++)
    {
        Conv[i]=Name[i];
    }
 for(int i=0;i<n2;i++)
 {
     Conv[n+i]=Name2[i];
 }
 Conv[n+n2]='\0';
 return Conv;
}

char* MyCatChar(char* Name,int n,char Name2)
{
 char* Conv=(char*)malloc((n+1)*sizeof(char));
 for(int j=0;j<n+1;j++) Conv[j]='/';
 for(int i=0;i<n;i++)
    {
        Conv[i]=Name[i];
    }
 Conv[n]=Name2;
 return Conv;
}

int printdir(char *dir, int depth,int out,struct Counter* coun,unsigned long int Par,char* path, int outbuff) 
{
    char block[1024];
    int nread;
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf; 
    if ((dp = opendir(dir)) == NULL) 
    {  
        fprintf(stderr, "cannot open directory: %s\n", dir);  
        return -1; 
    } 
    chdir(dir); 
    while((entry = readdir(dp)) != NULL) 
    {  
        lstat(entry->d_name, &statbuf);  
        if (!S_ISDIR(statbuf.st_mode)) 
        {   /* Находит каталог, но игнорирует . и .. */   
            char* fullpath="";
            char Mode[5]="";
            sprintf(Mode,"%i",statbuf.st_mode);
            fullpath=MyCat(path,strlen(path),"/",1);
            fullpath=MyCat(fullpath,strlen(fullpath),entry->d_name,strlen(entry->d_name));
            unsigned int AllInf=0;
            int fread=open(fullpath,O_RDONLY);
            coun->id++;
            if(fread>0)
                while((nread=read(fread,block,sizeof(block)))>0)
                {
                write(outbuff,block,nread);
                AllInf+=nread;
                }
            else printf("Can't open %s file\n",entry->d_name);
            write(out,AddChar(entry->d_name,strlen(entry->d_name)),strlen(entry->d_name)+2);
            write(out,LongToStr(coun->id),10);//свой id
            write(out,LongToStr(Par),10);//id предка
            write(out,Mode,5);
            write(out,LongToStr(AllInf),10);
            close(fread);
        }
    } 
    seekdir(dp,0);
    while((entry = readdir(dp)) != NULL) 
    {  
        lstat(entry->d_name, &statbuf);  
        if (S_ISDIR(statbuf.st_mode)) 
        {   /* Находит каталог, но игнорирует . и .. */   
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)    
            continue;
            coun->id++;
            char* fullpath="";
            char Mode[5]="";
            sprintf(Mode,"%i",statbuf.st_mode);
            write(out,AddChar(entry->d_name,strlen(entry->d_name)),strlen(entry->d_name)+2);
            write(out,LongToStr(coun->id),10);
            write(out,LongToStr(Par),10);
            write(out,"Dir",3);
            write(out,Mode,5);
            fullpath=MyCat(path,strlen(path),"/",1);
            fullpath=MyCat(fullpath,strlen(fullpath),entry->d_name,strlen(entry->d_name));
            printdir(entry->d_name, depth+4,out,coun,coun->id,fullpath,outbuff);  
        }
    } 
    chdir(".."); 
    closedir(dp);
    return 0;
}   

int main(int argc,char* args[]) {
    char* directory=args[1];
    if(args[2][0]=='p')
    {
    char Mode[5];
    int nread;
    struct stat buff;
    int out;
    struct Counter Cou;
    Cou.id=0;
    struct Counter Mov;
    Mov.id=0; 
    struct Counter Root;
    Root.id=1;
    char* name=&(*(strrchr(directory,'/')+1));
    char* arhname=MyCat(directory,strlen(directory),".MEGARAR",strlen(".MEGARAR"));
    char* filebuff1=MyCat(directory,strlen(directory),"FileArch",strlen("FileArch"));
    char* filebuff2=MyCat(directory,strlen(directory),"FileArchBuff",strlen("FileArchBuff"));
    out = open(filebuff1,  O_RDWR|O_CREAT| O_APPEND, S_IRUSR|S_IWUSR);
    int outbuff = open(filebuff2, O_RDWR|O_CREAT| O_APPEND, S_IRUSR|S_IWUSR);
    int fin = open(arhname, O_RDWR|O_CREAT| O_APPEND, S_IRUSR|S_IWUSR);
    int result=printdir(directory, 1,out, &Cou,0,directory,outbuff); 
    if(result<0)
    {
        return -1;
    }
    char blocks[4096];
    unsigned long int Sdvig=0;
    lseek(outbuff,0,SEEK_SET);
    lseek(out,0,SEEK_SET);
    while((nread=read(out,blocks,sizeof(blocks)))>0)
        {
           Sdvig+=nread;
       }
    lstat(directory, &buff);  
    sprintf(Mode,"%i",buff.st_mode);
    Sdvig+=40+strlen(name);
    write(fin,LongToStr(Sdvig),10);
    write(fin,AddChar(name,strlen(name)),strlen(name)+2);
    write(fin,LongToStr(0),10);
    write(fin,LongToStr(0),10);
    write(fin,"Dir",3);
    write(fin,Mode,5);
    lseek(out,0,SEEK_SET);
    while((nread=read(out,blocks,sizeof(blocks)))>0)
        {
            write(fin,blocks,nread);
        }
    while((nread=read(outbuff,blocks,sizeof(blocks)))>0)
        {
            write(fin,blocks,nread);
        }
    printf("%i",nread);
    close(out);
    close(outbuff);
    close(fin);
    unlink(filebuff1);
    unlink(filebuff2);
    }
    if(args[2][0]=='u')
    {
    char* unname=strrchr(directory,'/');
    directory=MyCat(directory,strlen(directory)-strlen(unname),"",0);
    char antihype[4096];
    struct FileWr RootList[255];
    unsigned int Numbr=0;
    int anti=open(args[1],O_RDONLY);
    if(anti<0)
    {
        printf("No such file");
        return -1;
    }
    unsigned long Move=0;
    unsigned long RightNow=10;
    read(anti,antihype,10);
    Move=StrToLong(antihype,10);
    unsigned long stop=Move;
    short int name=2;
    short int id=0;
    short int parent=0;
    short int dir=0;
    short int mode=0;
    short int file=0;
    short int dirbuff=0;
    char* namebuff="";
    char* parbuff="";
    char* idbuff="";
    char* modebuff="";
    char* filbuff="";
    while(read(anti,antihype,4096)>0)
    {
        for(int i=0;i<sizeof(antihype);i++)
        {
            if(name) 
            {
                namebuff=MyCatChar(namebuff,strlen(namebuff),antihype[i]);
                if(antihype[i]=='/') 
                {
                    name--;
                    if(name==0)
                        id=10;
                }
            }
            else if(id) 
            {
                idbuff=MyCatChar(idbuff,strlen(idbuff),antihype[i]);
                id--;
                if(id==0)
                {
                    parent=10;
                }

            }
            else if(parent)
            {
                parbuff=MyCatChar(parbuff,strlen(parbuff),antihype[i]);
                parent--;
                if(parent==0)
                {
                    dir=1;
                    mode=5;
                }

            } 
            else if(file)
            {
                filbuff=MyCatChar(filbuff,strlen(filbuff),antihype[i]);
                file--;
                if(file==0)
                {
                    char* place="";
                    for(int j=0;j<Numbr;j++)
                       {
                         if(RootList[j].id==StrToLong(parbuff,10))
                        {
                            place=RootList[j].path;
                        }
                       }
                    place=MyCat(place,strlen(place),namebuff,strlen(namebuff)-1);
                    int fil=open(place,O_CREAT|O_WRONLY,StrToLong(modebuff,5));
                    unsigned long CopyItem=StrToLong(filbuff,10);
                    char Buf[2048];
                    lseek(anti,Move,SEEK_SET);
                    Move+=CopyItem;
                    while(1)
                    {
                        if(CopyItem>2048)
                        {
                            read(anti,Buf,2048);
                            write(fil,Buf,2048);
                            CopyItem-=2048;
                        }
                        else
                        {
                            read(anti,Buf,CopyItem);
                            write(fil,Buf,CopyItem);
                            break;
                        }
                    }
                    lseek(anti,RightNow,SEEK_SET);
                    close(fil);
                    name=2;
                    modebuff="";
                    namebuff="";
                    parbuff="";
                    idbuff="";
                    filbuff="";
                }
            }
            else if(antihype[i]=='D'&& dir) 
            {
                dir=0;
                dirbuff=1;
                i+=2;
                RightNow+=2;
            }
            else if(mode) 
            {
                modebuff=MyCatChar(modebuff,strlen(modebuff),antihype[i]);
                mode--;
                if(mode==0)
                {
                    if(dirbuff)
                    {
                        dirbuff=0;
                        RootList[Numbr].id=StrToLong(idbuff,10);
                        RootList[Numbr].id_Parent=StrToLong(parbuff,10);
                        if(RootList[Numbr].id!=RootList[Numbr].id_Parent)
                        {
                            for(int j=0;j<Numbr;j++)
                            if(RootList[j].id==RootList[Numbr].id_Parent)
                                {
                                    RootList[Numbr].path=RootList[j].path;
                                    RootList[Numbr].path=MyCat(RootList[Numbr].path,strlen(RootList[Numbr].path),namebuff,strlen(namebuff)-1);
                                    break;
                                }
                        }
                        else if(RootList[Numbr].id==RootList[Numbr].id_Parent)
                        {
                            RootList[Numbr].path=MyCatChar(directory,strlen(directory),'/');
                            RootList[Numbr].path=MyCat(directory,strlen(directory),namebuff,strlen(namebuff)-1);
                        }
                        mkdir(RootList[Numbr].path,StrToLong(modebuff,5));
                        name=2;
                        modebuff="";
                        namebuff="";
                        parbuff="";
                        idbuff="";
                        Numbr++;
                        
                    }
                    else
                    {
                       file=10;
                    }
                    
                }
            }
            RightNow++;
            if(RightNow==stop)
            {
                break;
            }
        }
        if(RightNow==stop)
                break;
      }
    }
    exit(0);
}