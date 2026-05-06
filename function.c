#include<stdio.h>
#include<string.h>
#include"header.h"
char password[12]="hash123swati";
int readheight(FILE*);
int readwidth(FILE*);
unsigned int readbmpfilesize(FILE* );
unsigned int readsize(FILE*);
void copyheader(FILE*,FILE*);
void encodemagicstring(FILE*,FILE*,FILE*);
void encodeextension(FILE* fp1,FILE* fp2,FILE* fp3);
void encodesize(FILE* fp1,FILE*fp3,unsigned int size);
void encodedata(FILE*,FILE*,FILE*);
void remainingdata(FILE* ,FILE*,unsigned int);

int decodemagicstring(FILE*);
int decodesize(FILE*);
void decodeextension(FILE*,FILE*);
void decodedata(FILE*,FILE*,unsigned int);



void encrypt(){
    printf("INFO: Opening the required file\n");

    FILE* fp1=fopen("sampleimage.bmp","rb");
    FILE* fp2=fopen("msg.txt","rb");

    printf("INFO: checking for the Image file capacity to store data.\n");
    int bmpsize=readbmpfilesize(fp1);
    int height=readheight(fp1);
    int width=readwidth(fp1);
    int capacity=height*width*3;
    printf("INFO: Height = %d\n",height);
    printf("INFO: Width = %d\n",width);
    printf("INFO: checking for the secret.txt file size.\n");
    unsigned int size=readsize(fp2);
    if(size == 0){
        printf("secret message file is empty!\n");
        printf("Encrption not possible!\n");
        fclose(fp1);
        fclose(fp2);
        return; 
    }
    if(size>(capacity-strlen(password))){
        printf("INFO: secret data file size is larger than capacity of image!\n");
        printf("Encryption Not Possible.\n");
        return;
    }
    printf("INFO: File not empty.\n");
    printf("INFO: Done. found ok.\n");

    FILE* fp3=fopen("encryptedimage.bmp","wb");

    printf("INFO: Copying Image Header.\n");
    copyheader(fp1,fp3);
    printf("INFO: Done.\n");

    printf("INFO: Encoding magic string signature.\n");
    encodemagicstring(fp1,fp2,fp3);
    printf("INFO: DONE.\n");
    
    printf("INFO: Encoding secret.txt extension.\n");
    encodeextension(fp1,fp2,fp3);
    printf("INFO: DONE.\n");

    printf("INFO: Encoding secret.txt size.\n");
    encodesize(fp1,fp3,size);
    printf("INFO: DONE.\n");

    printf("INFO: Encoding secret data\n");
    encodedata(fp1,fp2,fp3);
    printf("INFO: DONE.\n");

    printf("INFO: Copying remaining data\n");
    remainingdata(fp1,fp3,bmpsize);
    printf("INFO: DONE.\n");

    printf("INFO: Encryption was successfull...\n");

    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
}

int readheight(FILE* fp){
    fseek(fp,22,SEEK_SET);
    unsigned char ch[4];
    fread(&ch,1,4,fp);
    int height=ch[3]<<24|ch[2]<<16|ch[1]<<8|ch[0];
    return height;
}

int readwidth(FILE* fp){
    fseek(fp,18,SEEK_SET);
    unsigned char ch[4];
    fread(&ch,1,4,fp);
    int width=ch[3]<<24|ch[2]<<16|ch[1]<<8|ch[0];
    return width;
}
unsigned int readbmpfilesize(FILE* fp1){
    fseek(fp1,2,SEEK_SET);
    unsigned char s[4];
    fread(&s,1,4,fp1);
    unsigned int size=s[3]<<24|s[2]<<16|s[1]<<8|s[0];
    return size;
}

unsigned int readsize(FILE* fp){
    fseek(fp,0,SEEK_END);
    unsigned int size=ftell(fp);
    return size;
}

void copyheader(FILE* fp1,FILE* fp3){
    char bmpheader[54];
    fseek(fp1,0,SEEK_SET);
    fseek(fp3,0,SEEK_SET);
    fread(bmpheader,1,54,fp1);
    fwrite(bmpheader,1,54,fp3);
}

void encodemagicstring(FILE* fp1,FILE* fp2,FILE* fp3){
    fseek(fp1,54,SEEK_SET);
    fseek(fp3,54,SEEK_SET);
    unsigned char ch;
    int len=strlen(password);
    int k=0;
    while(k<len){
        for(int i=7;i>=0;i--){
            fread(&ch,1,1,fp1);
            unsigned char mask=(password[k]>>i)&1;
            ch=ch&(0xFE)|mask;
            fwrite(&ch,1,1,fp3);
        }
        k++;
    }   
}

void encodeextension(FILE* fp1,FILE* fp2,FILE* fp3){
    char str[4]=".txt";
    unsigned char ch;
   encodesize(fp1,fp3,strlen(str));
   int len=strlen(str);
    int k=0;
    while(k<len){
        for(int i=7;i>=0;i--){
            fread(&ch,1,1,fp1);
            unsigned char mask=(str[k]>>i)&1;
            ch=(ch&(~(1)))|mask;
            fwrite(&ch,1,1,fp3);
        }
        k++;
    }   
}

void encodesize(FILE* fp1,FILE*fp3,unsigned int size){
    unsigned char ch;
    int i=31;
    while(i>=0){
        fread(&ch,1,1,fp1);
        char mask=size>>i&1;
        ch=(ch&(0xFE))|mask;
        fwrite(&ch,1,1,fp3);
        i--;
    }
}

void encodedata(FILE* fp1,FILE* fp2,FILE* fp3){
    unsigned char ch;
    int size=readsize(fp2);
    fseek(fp2,0,SEEK_SET);
    for(int i=0;i<size;i++){
        char c;
        fread(&c,1,1,fp2);
        for(int j=7;j>=0;j--){
            fread(&ch,1,1,fp1);
            char mask=(c>>j)&1;
            ch=(ch&(0xFE))|mask;
            fwrite(&ch,1,1,fp3);
        }
    }
}
void remainingdata(FILE* fp1,FILE* fp3,unsigned int bmpsize){
    unsigned int curr=ftell(fp1);
    // printf("%u\n",bmpsize);
    // printf("%u\n",curr);
    while(curr<bmpsize){
        char ch;
        fread(&ch,1,1,fp1);
        fwrite(&ch,1,1,fp3);
        curr++;
        // printf("%u\n",curr);
    }
}

void decrypt(){
    printf("INFO: Opening required file\n");

    FILE* fp1=fopen("encryptedimage.bmp","rb");
    FILE* fp2=fopen("decode_msg.txt","wb");
    printf("INFO: Opened file successfully\n");

    printf("INFO: Decoding magic string\n");
    int v=decodemagicstring(fp1);
    if(v){
        printf("Not Encrypted file\n");
        fclose(fp1);
        fclose(fp2);
        return ;
    }
    printf("INFO: Done\n");

    printf("INFO: Decoding secret file extension\n");
    decodeextension(fp1,fp2);
    printf("INFO: DONE\n");

    printf("INFO: Decoding the secret data size\n");
    unsigned int size=decodesize(fp1);
    printf("INFO: DONE\n");

    printf("INFO: Decoding the data\n");
    decodedata(fp1,fp2,size);
    printf("INFO:DONE\n");
    printf("INFO: Decryption was successful...\n");
    fclose(fp1);
    fclose(fp2);

}

int decodemagicstring(FILE* fp1){
    fseek(fp1,54,SEEK_SET);
    int len=strlen(password);
    char str[len+1];
    int pixels=8*len;
    str[len]='\0';
    for(int i=0;i<len;i++){
        str[i]=0;
        for(int j=7;j>=0;j--){
            char ch;
            fread(&ch,1,1,fp1);
            char extract_bit=ch&1;
            str[i]=str[i]|(extract_bit<<j);
        }
    }
    if(strcmp(str,password)!=0){
        return 1;
    }
    return 0;
}

int decodesize(FILE* fp1){
   int size=0;
   for(int i=31;i>=0;i--){
    char ch;
        fread(&ch,1,1,fp1);
        int extract_bit=ch&1;
        size=size|(extract_bit<<i);
   }
    return size;
}

void decodeextension(FILE* fp1,FILE* fp2){
    int size=decodesize(fp1);
    char ch[size];
    for(int i=0;i<size;i++){
        ch[i]=0;
        for(int j=7;j>=0;j--){
            char c;
            fread(&c,1,1,fp1);
            char extract_bit=c&1;
            ch[i]=ch[i]|(extract_bit<<j);
        }
    }
    fprintf(fp2,"File extension: ");
    fwrite(ch,size,1,fp2);
    fprintf(fp2,"\n");
}

void decodedata(FILE* fp1,FILE* fp2,unsigned int size){
    fprintf(fp2,"Data: ");
    for(int i=0;i<size;i++){
        char ch=0;
        for(int j=7;j>=0;j--){
            char c;
            fread(&c,1,1,fp1);
            char extract_bit=c&1;
            ch=ch|(extract_bit<<j);
        }
        fwrite(&ch,1,1,fp2);
    }
}