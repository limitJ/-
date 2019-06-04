/*
 * 课程： 操作系统原理
 * 程序： 首次适应内存分配算法
 * 学号： 1817249038
 * 姓名： 朱斌
 * 教师： 苏州大学 王辉
 * 编译器： LLVM clang v8.0 / gcc v8.3.0
 * 运行平台: manjaro linux dde v18.02
 * 编辑器：neovim v0.4
 * 日期: 2019-06-05 02:04
 * 作者水平有限 Bug在所难免！如有错误 还请指正！
 * github: 
 *
 * */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <memory.h>
typedef unsigned int uint;
#define throwException(e) fputs(e,stderr);
char memory_image[4096];//位镜像
typedef struct tag_mem_block {
    int pid;//进程的Pid
    uint start_address;//进程的起始地址
    uint size;//进程的大小
    struct tag_mem_block* next;//链表 指向的下一个节点
}MemBlock ;
MemBlock* list = NULL;//头指针
MemBlock* tail = NULL;//自添加部分 本程序链表使用尾插法 需要维护尾指针
MemBlock* getMemBlock(int pid,uint mem_size){
    if (mem_size > 4096) {
        throwException("分配失败，空间不足\n");
        return NULL;
    }
    if (mem_size%4 != 0) {
        throwException("错误，分配内存必须是4KB的倍数\n");
        return NULL;
    }
    if(list == NULL){
        //说明程序是第一次分配
        list = (MemBlock*)malloc(sizeof(MemBlock));
        if(list == NULL){
            throwException("系统分配失败！\n");
            return NULL;
        }
        list->start_address = 0;
        list->size = mem_size;
        list->next = NULL;
        tail = list;
        return list;
    }else{
        //找到一块空闲块 并且足够分配的区域
        MemBlock* new_MemBlock = (MemBlock*)malloc(sizeof(MemBlock));
        if (new_MemBlock == NULL) {
            throwException("系统分配失败！\n");
            return NULL;
        }
        MemBlock* temp = list;
        MemBlock* old = list;
        while(temp!=NULL){
            if(temp->next != NULL){
                //说明该节点是空闲的
                if(temp->next->start_address - (temp->start_address + temp->size) >= mem_size){
                    //说明还有空间可以分配
                    new_MemBlock->next = temp->next;
                    new_MemBlock->size = mem_size;
                    new_MemBlock->pid = pid;
                    new_MemBlock->start_address = temp->start_address+temp->size;
                    temp->next = new_MemBlock;
                    return new_MemBlock;
                }else{
                    temp = temp->next;
                }
            }else if(temp->next==NULL){
                if(temp->start_address + temp->size + mem_size >4096){
                    throwException("分配失败，空间不足\n");
                    return NULL;
                }
                new_MemBlock->next = NULL;
                new_MemBlock->pid = pid;
                new_MemBlock->start_address = tail->start_address+tail->size;
                new_MemBlock->size = mem_size;
                tail->next = new_MemBlock;
                tail =  new_MemBlock;
                return new_MemBlock;
            }
        }
    }
    return NULL;
}
bool freeMemblock(MemBlock* mb){
    if (mb == NULL || list == NULL) {
        throwException("空指针，释放失败\n");
        return false;
    }
    MemBlock* temp = list;
    if(list == mb){
        list = list->next;
        if(list == NULL){
            tail = NULL;
        }
        free(mb);
        return true;
    }else{
        while(temp->next!=mb){
            temp = temp->next;
        }
        temp->next = mb->next;
        if (temp->next == NULL) {
            tail = temp;
        }
        free(mb);
        return true;
    }
    return false;
}
void showMemImage(){
    //一行64个字符 *代表占用4KB -代表空闲4KB 每个4KB 一行256KB
    //一共4096KB 共16行数据
    memset(memory_image, '-', sizeof(memory_image));
    for (MemBlock* temp = list; temp!=NULL; temp = temp->next) {
        for (int i = temp->start_address;i < temp->start_address+temp->size; i++) {
            memory_image[i] = '*';
        }
    }
    for (int i = 0; i < 4096; i+=4) {
        if (i%256 == 0) {
            printf("\n");
        }
        printf("%c",memory_image[i]);
    }
    printf("\n");
}
int main(void)
{
    MemBlock* mb1 = getMemBlock(1, 8);
    MemBlock* mb2 = getMemBlock(2, 32);
    MemBlock* mb3 = getMemBlock(3, 64);
    MemBlock* mb4 = getMemBlock(4, 1024);
    freeMemblock(mb3);
    MemBlock* mb5 = getMemBlock(5,124);
    MemBlock* mb6 = getMemBlock(6, 1024);
    freeMemblock(mb1);
    freeMemblock(mb5);
    showMemImage();
    printf("程序结束!\n");
    return 0;
}
