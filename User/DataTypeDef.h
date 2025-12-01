#ifndef __DATATYPE_DEF_H__
#define __DATATYPE_DEF_H__

#ifndef        NULL
    #define        NULL                (0)
#endif

#ifndef        NULL_PTR
    #define        NULL_PTR            (void*)0
#endif

#ifndef        BOOL_DEFINE
    typedef        unsigned char    	BOOL;
    #define        BOOL_DEFINE
#endif

#ifndef        CHAR_DEFINE
    typedef        char                 CHAR;
    #define        CHAR_DEFINE
#endif

#ifndef        uint8_t_DEFINE
    typedef        unsigned char    	uint8_t;
    #define        uint8_t_DEFINE
#endif

#ifndef        S8_DEFINE
    typedef        signed char      	S8;
    #define        S8_DEFINE
#endif

#ifndef        U16_DEFINE
    typedef        unsigned short   	U16;
    #define        U16_DEFINE
#endif

#ifndef        S16_DEFINE
    typedef        signed short     	S16;
    #define        S16_DEFINE
#endif

#ifndef        uint32_t_DEFINE
    typedef        unsigned int    		uint32_t;
    #define        uint32_t_DEFINE
#endif

#ifndef        S32_DEFINE
    typedef        signed int       	S32;
    #define        S32_DEFINE
#endif

#ifndef        U64_DEFINE
    typedef        unsigned long long 	U64;
    #define        U64_DEFINE
#endif

#ifndef        S64_DEFINE
    typedef        signed long long    	S64;
    #define        S64_DEFINE
#endif

#ifndef        F32_DEFINE
    typedef        float                F32;
    #define        F32_DEFINE
#endif

#ifndef        F64_DEFINE
    typedef        double               F64;
    #define        F64_DEFINE
#endif

#endif
