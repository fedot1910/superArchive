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
    char name[4096];
    char path[4096];
};

struct Counter
{
    unsigned long int id;
};

void LongToStr(char* place,unsigned long int id)//  куда что
{
    char Conv[11];
    for(int j=0;j<10;j++) Conv[j]='0';
    int i=0;
    while(id>0)
    {
        Conv[9-i]+=id%10;
        id=id/10;
        i++;
    }
    Conv[10]='\0';
    for(int j=0;j<11;j++) place[j]=Conv[j];
    return;
}

void AddChar(char* place,char* Name,int n) // добавить слэши по бокам       /слово/
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

int StrToLong(char* Name,int n)//  куда что
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
{ //конкатенация строк
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
{ //плюс символ в конец
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
{ //out вывод куда всю инфу для метаданных
	//coun счет для id
	// path путь до файла
	//Par счетчик родителя
	// outbuf просто инфа из файлов
	// т к рекурсия, то надо чтобы счетик менялся динамически поэтому к структуре
	// /имя/id_id родителя_режим чтнеия_кол-во байт (dir режим чтения на самом деле)
	
    char block[1024];
    int nread; 
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf; 
    if ((dp = opendir(dir)) == NULL) //не открылась
    {  
        fprintf(stderr, "cannot open directory: %s\n", dir);  
        return -1; 
    } 
    chdir(dir); 
    while((entry = readdir(dp)) != NULL) // пока по всем вложенным элемнтам в паке
    {  
        lstat(entry->d_name, &statbuf);  
        if (!S_ISDIR(statbuf.st_mode)) 
        {   /* Находит каталог, но игнорирует . и .. */   
            char fullpath[4096]="";
            char Mode[6]=""; //режим чтения
            char pips[4096]; 
            sprintf(Mode,"%i",statbuf.st_mode);
            MyCat(fullpath,path,strlen(path),"/",1); //добавить слэш в путь
            MyCat(fullpath,fullpath,strlen(fullpath),entry->d_name,strlen(entry->d_name)); //поный путь до файла
             printf("%s ",fullpath);
            unsigned int AllInf=0;
            int fread=open(fullpath,O_RDONLY);
            coun->id++;
            if(fread>0)
                while((nread=read(fread,block,sizeof(block)))>0) //пока можно считать инфу
                {
                write(outbuff,block,nread); //читать инфу и посчитать сколько считали
                AllInf+=nread;
                }
            else printf("Can't open %s file\n",entry->d_name); //неудача
            AddChar(pips,entry->d_name,strlen(entry->d_name)); // слэшы
            printf("%s ",pips);
            write(out,pips,strlen(entry->d_name)+2); //имя со лешами в пипсе
            LongToStr(pips,coun->id); //  куда что
            write(out,pips,10);//свой id
            printf("%s ",pips);
            LongToStr(pips,Par);
            write(out,pips,10);//id предка
            printf("%s\n",pips);
            write(out,Mode,5);
            LongToStr(pips,AllInf);
            write(out,pips,10);
            close(fread);
        }
    } 
    seekdir(dp,0); // вернуться в начало папки как буд-то ее не обходили
    while((entry = readdir(dp)) != NULL) // тот же цикл только для папок
    {  
        lstat(entry->d_name, &statbuf);  
        if (S_ISDIR(statbuf.st_mode)) 
        {   /* Находит каталог, но игнорирует . и .. */   
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)    
            continue;
            char pips[4096];
            coun->id++;
            char fullpath[4096]=""; // id idродителя дир мод
            char Mode[6]="";		
            sprintf(Mode,"%i",statbuf.st_mode);
            AddChar(pips,entry->d_name,strlen(entry->d_name));
            write(out,pips,strlen(entry->d_name)+2);
            LongToStr(pips,coun->id);
            write(out,pips,10);
            LongToStr(pips,Par);
            write(out,pips,10);
            write(out,"Dir",3);
            write(out,Mode,5);
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
    if(args[2][0]=='p')
    {
    char pips[4096];
    char Mode[6];
    int nread;
    struct stat buff;
    int out;
    struct Counter Cou;
    Cou.id=0;
    struct Counter Mov;
    Mov.id=0; 
    struct Counter Root;
    Root.id=1;
    char* name=&(*(strrchr(directory,'/')+1)); // имя папки для открытия
    char arhname[4096];
    MyCat(arhname,directory,strlen(directory),".MEGARAR",strlen(".MEGARAR"));
    char filebuff1[4096];
    MyCat(filebuff1,directory,strlen(directory),"FileArch",strlen("FileArch"));
    char filebuff2[4096];
    MyCat(filebuff2,directory,strlen(directory),"FileArchBuff",strlen("FileArchBuff"));
    out = open(filebuff1,  O_RDWR|O_CREAT| O_APPEND, S_IRUSR|S_IWUSR); //мета
    int outbuff = open(filebuff2, O_RDWR|O_CREAT| O_APPEND, S_IRUSR|S_IWUSR); // данные
    int fin = open(arhname, O_RDWR|O_CREAT| O_APPEND, S_IRUSR|S_IWUSR); // финальный файл
    int result=printdir(directory, 1,out, &Cou,0,directory,outbuff); 
    if(result<0)
    {
        return -1;
    }
    char blocks[4096];
    unsigned long int Sdvig=0;
    lseek(outbuff,0,SEEK_SET);
    lseek(out,0,SEEK_SET);
    while((nread=read(out,blocks,sizeof(blocks)))>0) //считаем длину метаданных
        {
           Sdvig+=nread;
       }
    lstat(directory, &buff);  
    sprintf(Mode,"%i",buff.st_mode);
    Sdvig+=40+strlen(name); 		// 10 id+ 10 id+ род 3 'dir'+ 5 режим чтения +2 имя файла со слэшами + 10 сдвиг относительно метадаными
    LongToStr(pips,Sdvig);
    write(fin,pips,10);
    AddChar(pips,name,strlen(name));
    printf("%s",pips);
    write(fin,pips,strlen(pips));
    LongToStr(pips,0);
    write(fin,pips,10);
    write(fin,pips,10);
    write(fin,"Dir",3);
    write(fin,Mode,5);
    lseek(out,0,SEEK_SET); //в начало
    while((nread=read(out,blocks,sizeof(blocks)))>0) //записываем метаданные
        {
            write(fin,blocks,nread);
        }
    while((nread=read(outbuff,blocks,sizeof(blocks)))>0) //запись данных
        {
            write(fin,blocks,nread);
        }
    printf("%i",nread);
    close(out);
    close(outbuff);
    close(fin);
    unlink(filebuff1); //удалить лишние файлы
    unlink(filebuff2);
    }



 if(args[2][0]=='u') // распаковка
    {
    char pips[4096];
    char* unname=strrchr(directory,'/'); //куда директория откуда был вызван метод разархивации
    char full[4096];
    MyCat(full,directory,strlen(directory)-strlen(unname),"",0); 
    char antihype[4096]; //для разархива
    struct FileWr RootList[255];
    unsigned int Numbr=0;
    directory=full;
    int anti=open(args[1],O_RDONLY);//файл на распак
    if(anti<0)
    {
        printf("No such file");
        return -1;
    }
    unsigned long Move=0;	//куда надо уйти чтобы читать данные
    unsigned long RightNow=10; //где щас
    read(anti,antihype,10);
    Move=StrToLong(antihype,10);
    unsigned long stop=Move;
    short int name=2; //в любой момент все 0 кроме одного
    short int id=0;	//относительно того параметра выбирается что прямо сейчас считывается из файла
    short int parent=0;
    short int dir=0;
    short int mode=0;
    short int file=0;
    short int dirbuff=0;
    char namebuff[4096]="";
    char parbuff[11]="";
    char idbuff[11]="";
    char modebuff[6]="";
    char filbuff[11]="";
    while(read(anti,antihype,4096)>0) //читаем с файла
    {
        for(int i=0;i<sizeof(antihype);i++) //обрабатываем каждый бит
        {
            if(name)  //когда прочитаем имя то идем дальше
            {
                MyCatChar(namebuff,namebuff,strlen(namebuff),antihype[i]);
                if(antihype[i]=='/') //полное имя между жвумя слэшами 
                {
                    name--;
                    if(name==0){
                        printf("%s ",namebuff);
                        id=10;}
                }
            }
            else if(id) 
            {
                MyCatChar(idbuff,idbuff,strlen(idbuff),antihype[i]);
                id--;
                if(id==0) 
                {
                    printf("%s ",idbuff);
                    parent=10; 
                }

            }
            else if(parent)
            {
                MyCatChar(parbuff,parbuff,strlen(parbuff),antihype[i]);
                parent--;
                if(parent==0)
                {
                    printf("%s  ",parbuff);
                    dir=1;
                    mode=5;
                }

            } 
            else if(file) //file это сколько бит в файле
            {
                MyCatChar(filbuff,filbuff,strlen(filbuff),antihype[i]); //куда откуда длина символ
                file--;
                if(file==0) //определение папки где лежит файл
                {
                    printf("%s \n",filbuff);
                    char place[4096]="";
                    for(int j=0;j<Numbr;j++)
                       {
                         if(RootList[j].id==StrToLong(parbuff,10))
                        {
                            Copy(place,RootList[j].path,strlen(RootList[j].path));
                        }
                       }
                    MyCat(place,place,strlen(place),namebuff,strlen(namebuff)-1);
                    int fil=open(place,O_CREAT|O_WRONLY,StrToLong(modebuff,5)); //создаем файл по данному пути
                    unsigned long CopyItem=StrToLong(filbuff,10);
                    char Buf[2048];
                    lseek(anti,Move,SEEK_SET);
                    Move+=CopyItem; //где следущие данные
                    while(1) //запись данных в файл
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
                    modebuff[0]='\0';
                    namebuff[0]='\0';
                    parbuff[0]='\0';
                    idbuff[0]='\0';
                    filbuff[0]='\0';
                }
            }
            else if(antihype[i]=='D'&& dir) // D  есть переменная дир = 1
            { //считываем папку 
                dir=0;
                dirbuff=1;
                i+=2;
                RightNow+=2;
            }
            else if(mode) // если был дир то скипает три символа и читае 
            { // а если это файл как ТУТ то Считаем за файл
                MyCatChar(modebuff,modebuff,strlen(modebuff),antihype[i]);             
                mode--;
                if(mode==0)
                {
                    printf("%s  ",modebuff);
                    if(dirbuff) //если это папка то создаем папку
                    {
                        dirbuff=0;
                        RootList[Numbr].id=StrToLong(idbuff,10); //создаем новую папку
                        RootList[Numbr].id_Parent=StrToLong(parbuff,10);
                        if(RootList[Numbr].id!=RootList[Numbr].id_Parent) //сли ид равен идродителя
                        {
                            for(int j=0;j<Numbr;j++) //поиск родителя
                            if(RootList[j].id==RootList[Numbr].id_Parent) // если найден родиетль
                                { // a/b - родитель а наша c то бдет a/b/c
                                    Copy(RootList[Numbr].path,RootList[j].path,strlen(RootList[j].path));
                                    MyCat(RootList[Numbr].path,RootList[Numbr].path,strlen(RootList[Numbr].path),namebuff,strlen(namebuff)-1);
                                    break;
                                }
                        }
                        else if(RootList[Numbr].id==RootList[Numbr].id_Parent) //для стартовой папки
                        { // a + c -> a/c
                            MyCatChar(RootList[Numbr].path,directory,strlen(directory),'/');
                            MyCat(RootList[Numbr].path,directory,strlen(directory),namebuff,strlen(namebuff)-1);
                        }
                        mkdir(RootList[Numbr].path,StrToLong(modebuff,5)); //создание папки
                        printf("\n");
                        name=2;
                        modebuff[0]='\0';
                        namebuff[0]='\0';
                        parbuff[0]='\0';
                        idbuff[0]='\0';
                        Numbr++;
                        
                    }
                    else
                    { // это файл есои это не папка
                       file=10; 
                    }
                    
                }
            }
            RightNow++;
            if(RightNow==stop) // если дошли до конца то все
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
