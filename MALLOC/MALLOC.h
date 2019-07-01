#ifndef __MALLOC_H__
#define __MALLOC_H__

#include "stm32f10x.h"

#ifndef NULL
#define NULL 0
#endif


//�ڴ�ز����趨
#define MEM_BLOCK_SIZE				32															//�ڴ�ص��ڴ���С:32�ֽ�
#define MEM_MAX_SIZE				12*1024													//�ڴ�ش�С32KB
#define MEM_ALLOC_TABLE_SIZE	MEM_MAX_SIZE/MEM_BLOCK_SIZE 		//�ڴ������С


//�ڴ����������ṹ������
typedef struct
{
		u8 	*mem_pool;				//�ڴ��ָ��
		u16 *mem_map; 				//�ڴ�����ָ��
		u8  mem_ready; 				//�ڴ�ؾ�����־
}SRAM_Malloc_Controller_TypeDef;


/***********************************�ڲ�����*********************************************/

void MEM_Copy(void *des,void *src,u32 n);//�����ڴ�����
void MEM_Set(void *s,u8 c,u32 count);//�����ڴ�����
u32  _MEM_Malloc(u32 size);//�ڴ����
u8   _MEM_Free(u32 offset);//�ڴ��ͷ�

/***********************************�û����ú���*********************************************/

void  MEM_Init(void);//�ڴ�����ʼ��
u8    MEM_Usage_Rate(void);//��ȡ�ڴ�ʹ����
void  MEM_Free(void *ptr);//�ڴ��ͷ�
void *MEM_Malloc(u32 size);//�ڴ����

#endif
