#include<stdio.h>
#include"header.h"

int main(int argc, char* argv[]){
    int s;
    printf("WELCOME\n");
    do{
        printf("Which task would you like to perform?\n");
        printf("1.Encrypt the message in Image\n");
        printf("2.Decrypt the message from image.\n");
        printf("3.Quit\n");
        printf("Enter the task you would like to perform:");
        scanf("%d",&s);
        switch(s){
            case 1 :
                encrypt(argv[1],argv[2]);
                break;
            case 2:
                decrypt();
                break;
            case 3:
                break;
                
            default:
                printf("Enter correct task number!\n");
        }
    }while(s!=3);
    return 0;
}