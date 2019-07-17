/** dos_lib.c implements some functions of glibc.c */

#include <dos_lib.h>


dos_void* memset(dos_void* dst,dos_int32 val, dos_size count)
{
    void* ret = dst;
    while(count--)
    {
        *(char*)dst = (char)val;
        dst = (char*)dst + 1; 
    }
    return ret;
}


dos_void *memcpy(dos_void *dst, const dos_void *src, dos_size len) 
{  
    if(DOS_NULL == dst || DOS_NULL == src){  
        return DOS_NULL;  
    }  
      
    void *ret = dst;  
      
    if(dst <= src || (char *)dst >= (char *)src + len){  
        while(len--){  
            *(char *)dst = *(char *)src;  
            dst = (char *)dst + 1;  
            src = (char *)src + 1;  
        }  
    }else{   
        src = (char *)src + len - 1;  
        dst = (char *)dst + len - 1;  
        while(len--){  
            *(char *)dst = *(char *)src;  
            dst = (char *)dst - 1;  
            src = (char *)src - 1;  
        }  
    }  
    return ret;  
}  




