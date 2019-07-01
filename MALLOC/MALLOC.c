#include "stm32f10x.h"
#include "MALLOC.h"

//�����ڴ��
__align(32) u8 MEM_Pool[MEM_MAX_SIZE];

//�����ڴ�����
u16 MEM_Map[MEM_ALLOC_TABLE_SIZE];

//�����ڴ�������
const u32 MEM_TBL_Size=MEM_ALLOC_TABLE_SIZE;    //�ڴ������С
const u32 MEM_BLK_Size=MEM_BLOCK_SIZE;          //�ڴ�ֿ��С
const u32 MEM_MAX_Size=MEM_MAX_SIZE;            //�ڴ�صĿռ��С

//�����ڴ����������ṹ�岢��ʼ��
SRAM_Malloc_Controller_TypeDef Malloc_Controller_Struct=
{
    MEM_Pool,           //�ڴ���׵�ַ
    MEM_Map,            //�ڴ������׵�ַ
    0                   //�ڴ��Ĭ��δ����
};

//�����ڴ�����
//*des:Ŀ�ĵ�ַ
//*src:Դ��ַ
//n:��Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
void MEM_Copy(void *des,void *src,u32 n)
{
		u8 *xdes=des;
		u8 *xsrc=src;
		while(n--)*xdes++=*xsrc++;
}
//�����ڴ�����
//*s:�ڴ��׵�ַ
//c :Ҫ���õ�ֵ
//count:��Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
void MEM_Set(void *s,u8 c,u32 count)
{
		u8 *xs=s;
		while(count--)*xs++=c;
}
//�ڴ�����ʼ��
//1:ʹ����Ӧ�ڴ��,0:��ʹ����Ӧ�ڴ��
void MEM_Init(void)
{
		MEM_Set(Malloc_Controller_Struct.mem_map,0,MEM_TBL_Size*2);//�ڴ�������������
		MEM_Set(Malloc_Controller_Struct.mem_pool,0,MEM_MAX_Size);//�ڴ����������
		Malloc_Controller_Struct.mem_ready=1;//�ڴ�����ʼ�����
}
//��ȡ�ڴ�ʹ����
//memx:�����ڴ��
//����ֵ:ʹ����(0~100)
u8 MEM_Usage_Rate(void)
{
		u32 i,used=0;

    for(i=0;i<MEM_TBL_Size;i++)
    {
        if(Malloc_Controller_Struct.mem_map[i])used++;
    }
    return (used*100)/(MEM_TBL_Size);
}
//�ڴ����(�ڲ�����)
//memx:�����ڴ��
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:0XFFFFFFFF,�������;����,�ڴ�ƫ�Ƶ�ַ
u32 _MEM_Malloc(u32 size)
{
    signed long offset=0;
    u32 i,nmemb,cmemb=0;

    if(size==0)return 0XFFFFFFFF;//����Ҫ����
    nmemb=size/MEM_BLK_Size;
    if(size%MEM_BLK_Size)nmemb++;
    for(offset=MEM_TBL_Size-1;offset>=0;offset--)
    {
				if(!Malloc_Controller_Struct.mem_map[offset])cmemb++;
				else cmemb=0;
				if(cmemb==nmemb)
				{
						for(i=0;i<nmemb;i++)
						{
								Malloc_Controller_Struct.mem_map[offset+i]=nmemb;
						}
						return (offset*MEM_BLK_Size);//����ƫ�Ƶ�ַ
				}
    }
    return 0XFFFFFFFF;//δ�ҵ����Ϸ����������ڴ��
}
//�ڴ��ͷ�(�ڲ�����)
//memx:�����ڴ��
//offset:�ڴ��ַƫ��
//����ֵ:0,�ͷųɹ�;1,�ͷ�ʧ��
u8 _MEM_Free(u32 offset)
{
    int i;

    if(offset<MEM_MAX_Size)
    {
        int index=offset/MEM_BLK_Size;
        int nmemb=Malloc_Controller_Struct.mem_map[index];
        for(i=0;i<nmemb;i++)
        {
            Malloc_Controller_Struct.mem_map[index+i]=0;
        }
        return 0;
    }else return 1;//ƫ�Ƴ�����
}
//�ڴ��ͷ�(�ⲿ����)
//memx:�����ڴ��
//ptr:�ڴ��׵�ַ
void MEM_Free(void *ptr)
{
		u32 offset;
		if(ptr==NULL)return;//��ַΪ0,�Ͳ������κβ���
		offset=(u32)ptr-(u32)Malloc_Controller_Struct.mem_pool;
		_MEM_Free(offset);//�ͷ��ڴ�
}
//�ڴ����(�ⲿ����)
//memx:�����ڴ��
//size:�ڴ��С(�ֽ�)
//����ֵ:���䵽���ڴ��׵�ַ
void *MEM_Malloc(u32 size)
{
		u32 offset;
		offset=_MEM_Malloc(size);
		if(offset==0XFFFFFFFF)return NULL;
		else return (void*)((u32)Malloc_Controller_Struct.mem_pool+offset);
}
