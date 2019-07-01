#include "stm32f10x.h"
#include "MALLOC.h"

//定义内存池
__align(32) u8 MEM_Pool[MEM_MAX_SIZE];

//定义内存管理表
u16 MEM_Map[MEM_ALLOC_TABLE_SIZE];

//设置内存管理参数
const u32 MEM_TBL_Size=MEM_ALLOC_TABLE_SIZE;    //内存管理表大小
const u32 MEM_BLK_Size=MEM_BLOCK_SIZE;          //内存分块大小
const u32 MEM_MAX_Size=MEM_MAX_SIZE;            //内存池的空间大小

//定义内存管理控制器结构体并初始化
SRAM_Malloc_Controller_TypeDef Malloc_Controller_Struct=
{
    MEM_Pool,           //内存池首地址
    MEM_Map,            //内存管理表首地址
    0                   //内存池默认未就绪
};

//复制内存内容
//*des:目的地址
//*src:源地址
//n:需要复制的内存长度(字节为单位)
void MEM_Copy(void *des,void *src,u32 n)
{
		u8 *xdes=des;
		u8 *xsrc=src;
		while(n--)*xdes++=*xsrc++;
}
//设置内存内容
//*s:内存首地址
//c :要设置的值
//count:需要设置的内存大小(字节为单位)
void MEM_Set(void *s,u8 c,u32 count)
{
		u8 *xs=s;
		while(count--)*xs++=c;
}
//内存管理初始化
//1:使能相应内存池,0:不使能相应内存池
void MEM_Init(void)
{
		MEM_Set(Malloc_Controller_Struct.mem_map,0,MEM_TBL_Size*2);//内存管理表数据清零
		MEM_Set(Malloc_Controller_Struct.mem_pool,0,MEM_MAX_Size);//内存池数据清零
		Malloc_Controller_Struct.mem_ready=1;//内存管理初始化完成
}
//获取内存使用率
//memx:所属内存块
//返回值:使用率(0~100)
u8 MEM_Usage_Rate(void)
{
		u32 i,used=0;

    for(i=0;i<MEM_TBL_Size;i++)
    {
        if(Malloc_Controller_Struct.mem_map[i])used++;
    }
    return (used*100)/(MEM_TBL_Size);
}
//内存分配(内部调用)
//memx:所属内存块
//size:要分配的内存大小(字节)
//返回值:0XFFFFFFFF,代表错误;其他,内存偏移地址
u32 _MEM_Malloc(u32 size)
{
    signed long offset=0;
    u32 i,nmemb,cmemb=0;

    if(size==0)return 0XFFFFFFFF;//不需要分配
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
						return (offset*MEM_BLK_Size);//返回偏移地址
				}
    }
    return 0XFFFFFFFF;//未找到符合分配条件的内存块
}
//内存释放(内部调用)
//memx:所属内存块
//offset:内存地址偏移
//返回值:0,释放成功;1,释放失败
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
    }else return 1;//偏移超区了
}
//内存释放(外部调用)
//memx:所属内存块
//ptr:内存首地址
void MEM_Free(void *ptr)
{
		u32 offset;
		if(ptr==NULL)return;//地址为0,就不进行任何操作
		offset=(u32)ptr-(u32)Malloc_Controller_Struct.mem_pool;
		_MEM_Free(offset);//释放内存
}
//内存分配(外部调用)
//memx:所属内存块
//size:内存大小(字节)
//返回值:分配到的内存首地址
void *MEM_Malloc(u32 size)
{
		u32 offset;
		offset=_MEM_Malloc(size);
		if(offset==0XFFFFFFFF)return NULL;
		else return (void*)((u32)Malloc_Controller_Struct.mem_pool+offset);
}
