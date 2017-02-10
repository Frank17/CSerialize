//
//  serial.h
//  CLanguage
//
//  Created by 邹智鹏 on 16/12/8.
//  Copyright © 2016年 邹智鹏. All rights reserved.
//

#ifndef serial_h
#define serial_h

#include "stdint.h"
#include "inttypes.h"

#ifdef	__cplusplus
extern	"C"	{
#endif
    
    /* 类型宏定义 */
#define SERIAL_TYPE_INT32 0 // 整型 int
#define SERIAL_TYPE_UINT32 1 // 无符号整型 unsigned int
#define SERIAL_TYPE_UINT8 2 // 无符号字符型 unsigned char
#define SERIAL_TYPE_CHAR_U 2
#define SERIAL_TYPE_INT8 3 // 有符号字符型 char
#define SERIAL_TYPE_CHAR 3
#define SERIAL_TYPE_INT16 4 // 短整型 short
#define SERIAL_TYPE_SHORT 4
#define SERIAL_TYPE_UINT16 5 // 无符号短整型 unsigned short
#define SERIAL_TYPE_SHORT_U 5
#define SERIAL_TYPE_INT64 6 // long long
#define SERIAL_TYPE_UINT64 7 // unsigned long long
#define SERIAL_TYPE_LONGLONG 6
#define SERIAL_TYPE_LONGLONG_U 7
#define SERIAL_TYPE_LONG 8 // long
#define SERIAL_TYPE_LONG_U 9 // unsinged long
#define SERIAL_TYPE_STR_ADDR 10 // const char * / char *
#define SERIAL_TYPE_BYTE_ARR 11 // const char * / char *
#define SERIAL_TYPE_EMPTY_PTR 12 // void *
#define SERIAL_TYPE_DOUBLE 13 // double
#define SERIAL_TYPE_FLOAT 14 // float
#define SERIAL_TYPE_DOUBLE_U 15 // unsigned double
#define SERIAL_TYPE_FLOAT_U 16 // unsigned float
#define SERIAL_TYPE_TIME_T 17
#define SERIAL_TYPE_UNSUPPORTED -1
#define FALSE 0
#define TRUE 1
    
    /* 宏方法定义 */
#define convert(type, srcType) ((type)(srcType)) // 强制类型转换
    
    /* 类型定义 */
    typedef unsigned char byte; // 字节
    typedef unsigned char * byte_array; // 字节流
    typedef const unsigned char * const_byte_array; // 常量字节流
    typedef char bool; // 布尔型
    typedef char * str_ptr; // 字符串指针
    typedef const char * const_str_ptr; // 常量字符串指针
    typedef void * empty_ptr; // 空指针
    typedef int8_t serial_type;
    
    
    /* 结构定义 */
    typedef struct type_node {  // 类型节点
        serial_type type; // 该节点的类型
        empty_ptr data; // 该节点的数据
        bool mem_block; // 是否该节点为一个内存块
        size_t block_size; // 若是一个内存块，则需定义其大小
        bool str_block; // 指示该节点是否为一个字符串
        size_t str_len; // 若是字符串，则表示字符串长度
        bool heap_allocated; // 若是内存块，指示该内存块是否经过堆区分配
    } type_node; // 类型节点
    
    typedef type_node * type_node_ptr; // 指针别名
    
    typedef struct serial_list { // 类型链
        
        type_node_ptr dataNode; // 当前节点
        
        struct serial_list * nextnode; // 下一个节点
        
        struct serial_list * prenode; // 上一个节点
        
    } serial_list;
    typedef serial_list * serial_list_ptr; // 指针别名
    
    
    
    /* 函数定义 */
    
    typedef serial_list_ptr (*packStructure)(empty_ptr); // 函数指针，将复杂类型打包成为一个序列化字节流
    
    typedef empty_ptr (*unpackTypeNode)(serial_list_ptr*); // 函数指针，将序列化字节流反序列化为复杂类型
    
    /**
     Create a type node with empty value. The pointer default is 0, and value is 0.
     
     @return the pointer to the type node
     */
    type_node_ptr createTypeNode();// 创建一个空节点
    
    /**
     Init a node with empty value.
     
     @param node the node to be init
     */
    void initEmptyTypeNodePtr(type_node_ptr node); // 初始化一个空的节点
    
    /**
     Create a type node with given data. If the data is a memory block in the heap, the second parameter will be true.
     
     @param data the data stored in the type node
     @param allocated whether the memory for the data is in the heap
     @return the pointer to the type node
     */
    type_node_ptr createTypeNodeWithData(empty_ptr data, bool allocated); // 根据数据创建一个节点
    
    /**
     Create a serial list with given node.
     
     @param node the node keeping the data
     @return the pointer to the head of the serial list, if the list is empty, it will be NULL
     */
    serial_list_ptr createSerialListWithHead(type_node_ptr node);// 根据节点创建一个序列化表
    
    /**
     Append a type node to tail of the serial list.
     
     @param head the pointer to the serial list head
     @param node type node to be appended
     @return the pointer to the head of the serial list, if the list is empty, it will be NULL
     */
    serial_list_ptr appendNode(serial_list_ptr head, type_node_ptr node); // 追加一个节点到序列化表中
    
    /**
     Update a type node in the serial list.
     
     @param head pointer to the head of the serial list, to identify the list
     @param rank position to be updated, started from 0
     @param node the new node will be concatenated
     @return pointer to the head of the serial list, to identify the list, if the list is empty, it will be NULL
     */
    serial_list_ptr updateNode(serial_list_ptr head, uint32_t rank, type_node_ptr node); // 更新某个位置的节点为node
    
    /**
     Remove a node in the serial list.
     
     @param head pointer to the head of the serial list, to identify the list
     @param rank position to be removed
     @return pointer to the head of the serial list, to identify the list, if the list is empty, it will be NULL
     */
    serial_list_ptr removeNode(serial_list_ptr head, uint32_t rank); // 移除某个位置的节点
    
    /**
     Remove the last node in the serial list.
     
     @param head pointer to the head of the serial list, to identify the list
     @return pointer to the head of the serial list, to identify the list, if the list is empty, it will be NULL
     */
    serial_list_ptr removeLastNode(serial_list_ptr head); // 移除最后一个节点
    
    /**
     Serialize a list to a memory block stored data bytes.
     
     @param head pointer to the head of the serial list, to identify the list
     @param outSize the size of the memory block
     @return the pointer to the memory block
     */
    empty_ptr serialize(serial_list_ptr head, size_t *outSize); // 对现有的序列化表进行序列化，生成一个缓冲区
    
    /**
     Unserialize a byte array read from memory to a serialize list.
     
     @param block byte array
     @param count bytes of this block
     @return pointer to serialize list
     */
    serial_list_ptr unserialize(empty_ptr block, size_t count); // 将已有的字节流反序列化
    
    /**
     Deallocate the memory for the type node.
     
     @param node the pointer to the type node
     @return the size deallocated
     */
    size_t serial_free_node(type_node_ptr node); // 释放某个类型节点
    
    /**
     Deallocate the memory for the serial list
     
     @param list pointer to the head of the serial list, to identify the list
     @return the size deallocated
     */
    size_t serial_free_list(serial_list_ptr list); // 释放序列化表
    
    
    /**
     Destory the serial list and recover memories.

     @param head the pointer to the head of the list
     */
    void destory_serial_list(serial_list_ptr head);// 销毁序列化表
    
    /**
     Append node from a structure object
     
     @param structure object
     @param pack the function to pack an object
     */
    void appendStructureNode(serial_list_ptr *head, empty_ptr structure, packStructure pack); // append node from structure object
    
    /**
     Unserialize a list to an object.
     
     @param list the list pointer
     @param unpack the function to unpack serialize llist
     @return pointer to object
     */
    empty_ptr unserializeStructure(serial_list_ptr *list, unpackTypeNode unpack); // unseialize list to an object
    
#ifdef	__cplusplus
}
#endif

#endif /* serial_h */
