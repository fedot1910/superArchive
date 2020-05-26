#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>

#define Size 4096
#define SizeM 5
#define SizeMg 6
#define SizeB 10
#define SizeOff 10
#define SizeRoot 255

struct FileWr
{
    unsigned long int id_Parent;
    unsigned long int id;
    unsigned long int info_Bites;
    char name[Size];
    char path[Size];
};

struct Counter
{
    unsigned long int id;
};

void LongToStr(char* place,unsigned long int id)
{
    char Conv[SizeB+1];
    for(int j=0;j<SizeB;j++) Conv[j]='0';
    int i=0;
    while(id>0)
    {
        Conv[9-i]+=id%10;
        id=id/10;
        i++;
    }
    Conv[SizeB]='\0';
    for(int j=0;j<SizeB+1;j++) place[j]=Conv[j];
    return;
}

void AddChar(char* place,char* Name,int n)
{
 char Conv[n+3];
 for(int j=0;j<n+3;j++) Conv[j]='/';
 for(int i=1;i<n+1;i++)
    {
        Conv[i]=Name[i-1];
    }
 Conv[n+2]='\0';
 for(int j=0;j<n+3;j++) place[j]=Conv[j];
 return;
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


void MyCat(char* place,char* Name,int n,char* Name2,int n2)
{
 char buff[n+n2+1];
 for(int j=0;j<n+n2+1;j++) buff[j]='/';
 for(int i=0;i<n;i++)
    {
        buff[i]=Name[i];
    }
 for(int i=0;i<n2;i++)
 {
     buff[n+i]=Name2[i];
 }
 buff[n+n2]='\0';
 for(int j=0;j<n+n2+1;j++) place[j]=buff[j];
 return;
}

void MyCatChar(char* place,char* Name,int n,char Name2)
{
 char Conv[n+2];
 for(int j=0;j<n+2;j++) Conv[j]='/';
 for(int i=0;i<n;i++)
    {
        Conv[i]=Name[i];
    }
 Conv[n]=Name2;
 Conv[n+1]='\0';
 for(int j=0;j<n+2;j++) place[j]=Conv[j];
 return;
}
void Copy(char* place,char* Name,int Length)
{
    for(int i=0;i<Length;i++) place[i]=Name[i];
}

int printdir(char *dir, int depth,int out,struct Counter* coun,unsigned long int Par,char* path, int outbuff) 
{
    char block[Size];
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
            int wr;
            char fullpath[Size]="";
            char Mode[SizeMg]="";
            char pips[Size];
            sprintf(Mode,"%i",statbuf.st_mode);
            MyCat(fullpath,path,strlen(path),"/",1);
            MyCat(fullpath,fullpath,strlen(fullpath),entry->d_name,strlen(entry->d_name));
            unsigned int AllInf=0;
            int fread=open(fullpath,O_RDONLY);
            coun->id++;
            if(fread>0)
                while((nread=read(fread,block,sizeof(block)))>0)
                {
                    write(outbuff,block,nread);
                    AllInf+=nread;
                }
            else 
            {
                printf("Can't open %s file\n",entry->d_name);
                closedir(dp);
                return -1;
            }
            AddChar(pips,entry->d_name,strlen(entry->d_name));
            write(out,pips,strlen(entry->d_name)+2);
            LongToStr(pips,coun->id);
            wr=write(out,pips,SizeB);//свой id
            if(wr<0) 
            {
                printf("Can't write id\n");
                close(fread);
                closedir(dp);
                return -1;
            }
            LongToStr(pips,Par);
            wr=write(out,pips,SizeB);//id предка
            if(wr<0) 
            {
                printf("Can't write id parent\n");
                close(fread);
                closedir(dp);
                return -1;
            }
            wr=write(out,Mode,SizeM);
            if(wr<0)
            {
                printf("Can't write file mode\n");
                close(fread);
                closedir(dp);
                return -1;
            } 
            LongToStr(pips,AllInf);
            wr=write(out,pips,SizeB);
            if(wr<0) 
            {
                printf("Can't write file bite length\n");
                close(fread);
                closedir(dp);
                return -1;
            }
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
            int wr;
            char pips[Size];
            coun->id++;
            char fullpath[Size]="";
            char Mode[SizeMg]="";
            sprintf(Mode,"%i",statbuf.st_mode);
            AddChar(pips,entry->d_name,strlen(entry->d_name));
            wr=write(out,pips,strlen(entry->d_name)+2);
            if(wr<0)
            {
                printf("Can't write dir name\n");
                closedir(dp);
                return -2;
            }
            LongToStr(pips,coun->id);
            wr=write(out,pips,SizeB);
            if(wr<0)
            {
                printf("Can't write dir id\n");
                closedir(dp);
                return -2;
            }
            LongToStr(pips,Par);
            wr=write(out,pips,SizeB);
            if(wr<0)
            {
                printf("Can't write dir id parent\n");
                closedir(dp);
                return -2;
            }
            wr=write(out,"Dir",strlen("Dir"));            
            if(wr<0)
            {
                printf("Can't write dir word\n");
                closedir(dp);
                return -2;
            }
            wr=write(out,Mode,SizeM);
            if(wr<0)
            {
                printf("Can't write mode\n");
                closedir(dp);
                return -2;
            }
            MyCat(fullpath,path,strlen(path),"/",1);
            MyCat(fullpath,fullpath,strlen(fullpath),entry->d_name,strlen(entry->d_name));
            printdir(entry->d_name, depth+4,out,coun,coun->id,fullpath,outbuff);  
        }
    } 
    chdir(".."); 
    closedir(dp);
    return 0;
}   

int main(int argc,char* args[]) {
 char* directory=args[1];
 char* where=args[2];
 if(args[3][0]=='p')
    {
        char pips[Size];
        char arhname[Size];
        char filebuff1[Size];    
        char filebuff2[Size];
        char Mode[SizeMg];
        unsigned long int Sdvig=0;
        int nread;
        int wr;
        struct stat buff;
        int out;
        struct Counter Cou;
        Cou.id=0;
        struct Counter Mov;
        Mov.id=0; 
        struct Counter Root;
        Root.id=1;
        char* name=&(*(strrchr(directory,'/')+1));
        MyCatChar(arhname,where,strlen(where),'/');
        MyCat(arhname,arhname,strlen(arhname),name,strlen(name-1));
        MyCat(arhname,arhname,strlen(arhname),".MEGARAR",strlen(".MEGARAR"));
        printf("%s",arhname);
        MyCat(filebuff1,directory,strlen(directory),"FileArch",strlen("FileArch"));
        MyCat(filebuff2,directory,strlen(directory),"FileArchBuff",strlen("FileArchBuff"));
        out = open(filebuff1,  O_RDWR|O_CREAT| O_APPEND, S_IRUSR|S_IWUSR);
        if(out<0)
            {
                printf("Can't create metafile\n");
                return -1;
            }
        int outbuff = open(filebuff2, O_RDWR|O_CREAT| O_APPEND, S_IRUSR|S_IWUSR);
        if(outbuff<0)
            {
                printf("Can't create datafile\n");
                return -1;
            }
        int fin = open(arhname, O_RDWR|O_CREAT| O_APPEND, S_IRUSR|S_IWUSR);
        if(fin<0)
            {   
                printf("Can't create archfile\n");
                return -1;
            }
        int result=printdir(directory, 1,out, &Cou,0,directory,outbuff); 
        if(result<0)
            {
                return -1;
            }   
        lseek(outbuff,0,SEEK_SET);
        lseek(out,0,SEEK_SET);
        while((nread=read(out,pips,sizeof(pips)))>0)
            {
                Sdvig+=nread;
            }
        lstat(directory, &buff);  
        sprintf(Mode,"%i",buff.st_mode);
        Sdvig+=SizeM+SizeB+SizeB+2+strlen(name)+strlen("Dir")+SizeOff;
        LongToStr(pips,Sdvig);
        wr=write(fin,pips,SizeOff);
        if(wr<0)
            {
                printf("Can't write offset\n");
                return -1;
            }
        AddChar(pips,name,strlen(name));
        wr=write(fin,pips,strlen(pips));
        if(wr<0)
            {
                printf("Can't write first dir name\n");
                return -1;
            }
        LongToStr(pips,0);
        wr=write(fin,pips,SizeB);
        if(wr<0)
            {
                printf("Can't write first dir id\n");
                return -1;
            }
        wr=write(fin,pips,SizeB);
        if(wr<0)
            {
                printf("Can't write first dir id parent\n");
                return -1;
            }
        wr=write(fin,"Dir",strlen("Dir"));
        if(wr<0)
            {   
                printf("Can't write first dir word\n");
                return -1;
            }
        wr=write(fin,Mode,SizeM);
        if(wr<0)
            {
                printf("Can't write first dir mode\n");
                return -1;
            }
        lseek(out,0,SEEK_SET);
        while((nread=read(out,pips,sizeof(pips)))>0)
            {
                write(fin,pips,nread);
            }
        while((nread=read(outbuff,pips,sizeof(pips)))>0)
            {
                write(fin,pips,nread);
            }
        close(out);
        close(outbuff);
        close(fin);
        unlink(filebuff1);
        unlink(filebuff2);
    }
 if(args[3][0]=='u')
    {
        char antihype[Size];
        struct FileWr RootList[SizeRoot];
        unsigned int Numbr=0;
        directory=where;
        int anti=open(args[1],O_RDONLY);
        if(anti<0)
        {
            printf("No such file");
            return -1;
        }
        unsigned long Move=0;
        unsigned long RightNow=SizeOff;
        read(anti,antihype,SizeOff);
        Move=StrToLong(antihype,SizeOff);
        unsigned long stop=Move;
        short int name=2;
        short int id=0;
        short int parent=0;
        short int dir=0;
        short int mode=0;
        short int file=0;
        short int dirbuff=0;
        char namebuff[Size]="";
        char parbuff[SizeB+1]="";
        char idbuff[SizeB+1]="";
        char modebuff[SizeMg]="";
        char filbuff[SizeB+1]="";
        while(read(anti,antihype,Size)>0)
        {
            for(int i=0;i<sizeof(antihype);i++)
            {
                if(name) 
                {
                    MyCatChar(namebuff,namebuff,strlen(namebuff),antihype[i]);
                    if(antihype[i]=='/') 
                    {
                        name--;
                        if(name==0)
                        {
                       // printf("%s ",namebuff);
                            id=SizeB;
                        }
                    }
                }
                else if(id) 
                {
                    MyCatChar(idbuff,idbuff,strlen(idbuff),antihype[i]);
                    id--;
                    if(id==0)
                    {
                    //printf("%s ",idbuff);
                        parent=SizeB;
                    }

                }
                else if(parent)
                {
                    MyCatChar(parbuff,parbuff,strlen(parbuff),antihype[i]);
                    parent--;
                    if(parent==0)
                    {
                    //printf("%s  ",parbuff);
                        dir=1;
                        mode=SizeM;
                    }

                } 
                else if(file)
                {
                    MyCatChar(filbuff,filbuff,strlen(filbuff),antihype[i]);
                    file--;
                    if(file==0)
                    {
                    //printf("%s \n",filbuff);
                        char place[Size]="";
                        for(int j=0;j<Numbr;j++)
                        {
                         if(RootList[j].id==StrToLong(parbuff,SizeB))
                            {
                                Copy(place,RootList[j].path,strlen(RootList[j].path));
                            }
                        }
                        MyCat(place,place,strlen(place),namebuff,strlen(namebuff)-1);
                        int fil=open(place,O_CREAT|O_WRONLY,StrToLong(modebuff,SizeM));
                        if (fil<0)
                        {
                            printf("Can't create file\n");
                            return -1;
                        }
                        unsigned long CopyItem=StrToLong(filbuff,SizeB);
                        char Buf[Size];
                        lseek(anti,Move,SEEK_SET);
                        Move+=CopyItem;
                        while(1)
                        {
                            if(CopyItem>Size)
                            {
                                read(anti,Buf,Size);
                                write(fil,Buf,Size);
                                CopyItem-=Size;
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
                        modebuff[0]='\0';
                        namebuff[0]='\0';
                        parbuff[0]='\0';
                        idbuff[0]='\0';
                        filbuff[0]='\0';
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
                    MyCatChar(modebuff,modebuff,strlen(modebuff),antihype[i]);             
                    mode--;
                    if(mode==0)
                    {
                        if(dirbuff)
                        {
                            dirbuff=0;
                            RootList[Numbr].id=StrToLong(idbuff,SizeB);
                            RootList[Numbr].id_Parent=StrToLong(parbuff,SizeB);
                            if(RootList[Numbr].id!=RootList[Numbr].id_Parent)
                            {
                                for(int j=0;j<Numbr;j++)
                                if(RootList[j].id==RootList[Numbr].id_Parent)
                                    {
                                        Copy(RootList[Numbr].path,RootList[j].path,strlen(RootList[j].path));
                                        MyCat(RootList[Numbr].path,RootList[Numbr].path,strlen(RootList[Numbr].path),namebuff,strlen(namebuff)-1);
                                        break;
                                    }
                            }
                            else if(RootList[Numbr].id==RootList[Numbr].id_Parent)
                            {
                                MyCatChar(RootList[Numbr].path,directory,strlen(directory),'/');
                                MyCat(RootList[Numbr].path,directory,strlen(directory),namebuff,strlen(namebuff)-1);
                            }
                            mkdir(RootList[Numbr].path,StrToLong(modebuff,SizeM));
                            name=2;
                            modebuff[0]='\0';
                            namebuff[0]='\0';
                            parbuff[0]='\0';
                            idbuff[0]='\0';
                            Numbr++;
                        
                        }
                        else
                        {
                            file=SizeB;
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