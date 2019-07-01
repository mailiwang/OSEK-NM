#ifndef __MALLOC_H__
#define __MALLOC_H__

#include "stm32f10x.h"

#ifndef NULL
#define NULL 0
#endif


//内存池参数设定
#define MEM_BLOCK_SIZE				32															//内存池的内存块大小:32字节
#define MEM_MAX_SIZE				12*1024													//内存池大小32KB
#define MEM_ALLOC_TABLE_SIZE	MEM_MAX_SIZE/MEM_BLOCK_SIZE 		//内存管理表大小


//内存管理控制器结构体声明
typedef struct
{
		u8 	*mem_pool;				//内存池指针
		u16 *mem_map; 				//内存管理表指针
		u8  mem_ready; 				//内存池就绪标志
}SRAM_Malloc_Controller_TypeDef;


/***********************************内部函数*********************************************/

void MEM_Copy(void *des,void *src,u32 n);//复制内存内容
void MEM_Set(void *s,u8 c,u32 count);//设置内存内容
u32  _MEM_Malloc(u32 size);//内存分配
u8   _MEM_Free(u32 offset);//内存释放

/***********************************用户调用函数*********************************************/

void  MEM_Init(void);//内存管理初始化
u8    MEM_Usage_Rate(void);//获取内存使用率
void  MEM_Free(void *ptr);//内存释放
void *MEM_Malloc(u32 size);//内存分配

#endif
