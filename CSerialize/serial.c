//
//  serial.c
//  CLanguage
//
//  Created by 邹智鹏 on 16/12/8.
//  Copyright © 2016年 邹智鹏. All rights reserved.
//

#include "serial.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static const size_t type_node_size = sizeof(type_node);

static const size_t serial_node_size = sizeof(serial_list);

/**
 Init the empty list.
 
 @param head the pointer to the serial list
 @return the pointer to the head of the serial list
 */
static serial_list_ptr initEmptySerialList(serial_list_ptr head); // 初始化头指针

/**
 According to the type of the value, revieve the byte of this kind of type.
 
 @param type value type
 @return the byte of this kind of type
 */
static size_t byteForType(serial_type type); // get size of the type

// create node
type_node_ptr createTypeNode() {
    
    type_node_ptr node_ptr = (type_node_ptr)malloc(type_node_size); // to allocate memory in the heap
    
    // Init the node
    initEmptyTypeNodePtr(node_ptr);
    
    return node_ptr;
}

// init function
void initEmptyTypeNodePtr(type_node_ptr node) {
    node->type = 0;
    node->data = 0;
    node->mem_block = 0;
    node->block_size = 0;
    node->str_block = 0;
    node->str_len = 0;
    node->heap_allocated = 0;
}

// create a node with data
type_node_ptr createTypeNodeWithData(empty_ptr data, bool allocated) {
    type_node_ptr node_ptr = (type_node_ptr)malloc(type_node_size); // to allocate memory in the heap
    
    // Init the node
    initEmptyTypeNodePtr(node_ptr);
    node_ptr->data = data;
    node_ptr->heap_allocated = allocated;
    
    return node_ptr;
}

// Create a serial list with given node.
serial_list_ptr createSerialListWithHead(type_node_ptr node) {
    
    serial_list_ptr head = (serial_list_ptr)malloc(serial_node_size); // create
    
    head = initEmptySerialList(head); // init a empty serial list
    
    head->dataNode = node;
    
    return head;
}

serial_list_ptr appendNode(serial_list_ptr head, type_node_ptr node) {
    serial_list_ptr newNode = NULL;
    serial_list_ptr tmp_ptr = head;
    while (tmp_ptr->nextnode) { // run at the last node
        tmp_ptr = tmp_ptr->nextnode;
    }
    // create a serial list node
    newNode = createSerialListWithHead(node);
    // concatenate two nodes
    tmp_ptr->nextnode = newNode;
    newNode->prenode = tmp_ptr;
    return head;
}

serial_list_ptr updateNode(serial_list_ptr head, uint32_t rank, type_node_ptr node) {
    
    // get the head
    serial_list_ptr tmp_ptr = head;
    while (rank-- > 0) {
        tmp_ptr = tmp_ptr->nextnode;
    }
    
    // the same as data block
    if (tmp_ptr->dataNode->data != node->data) {
        // if the old block is in the heap
        if (tmp_ptr->dataNode->heap_allocated) {
            // free it
            serial_free_node(tmp_ptr->dataNode);
        }
    }
    tmp_ptr->dataNode = node; // change the datanode
    return head;
}

serial_list_ptr removeNode(serial_list_ptr head, uint32_t rank) {
    
    // get head of the serial list
    serial_list_ptr tmp_head = head;
    serial_list_ptr picked_ptr = NULL;
    while (rank-- > 0) {
        tmp_head = tmp_head->nextnode;
    }
    
    picked_ptr = tmp_head; // the node to be removed
    
    if (picked_ptr->nextnode && picked_ptr->prenode) { // if the next node is not null and the previous node is not null
        // break association
        picked_ptr->prenode->nextnode = picked_ptr->nextnode;
        picked_ptr->nextnode->prenode = picked_ptr->prenode;
        picked_ptr->nextnode = NULL;
        picked_ptr->prenode = NULL;
    } else if(picked_ptr->nextnode) { // remove the first node
        head = picked_ptr->nextnode; // reset list head
        head->prenode = NULL;
        picked_ptr->nextnode = NULL;
        picked_ptr->prenode = NULL;
    } else if(NULL == picked_ptr->prenode) { // remove the last node
        head = picked_ptr->nextnode;
        picked_ptr->prenode = NULL;
        picked_ptr->nextnode = NULL;
    }
    
    // to free if necessary
    // annotion it, avoid double free
//    if (picked_ptr->dataNode->heap_allocated && (picked_ptr->dataNode->mem_block || picked_ptr->dataNode->str_block)) {
//        serial_free_node(picked_ptr->dataNode); // free memory block
//    }
    serial_free_list(picked_ptr); // free list node
    
    return head;
}

serial_list_ptr removeLastNode(serial_list_ptr head) {
    
    serial_list_ptr picked_ptr = head;
    while (picked_ptr->nextnode) {// run till the last one
        picked_ptr = picked_ptr->nextnode;
    }
    
    // if the last one is the first one
    if (NULL == picked_ptr->prenode) {
        head = NULL; // reset head as NULL
    } else {
        picked_ptr->prenode->nextnode = NULL;
        picked_ptr->prenode = NULL;
        picked_ptr->nextnode = NULL;
    }
    
    // to free if necessary
//    if (picked_ptr->dataNode->heap_allocated && (picked_ptr->dataNode->mem_block || picked_ptr->dataNode->str_block)) {
//        serial_free_node(picked_ptr->dataNode); // free memory block
//    }
    serial_free_list(picked_ptr); // free list node
    
    return head;
}

empty_ptr serialize(serial_list_ptr head, size_t *outSize) {
    
    // default
    size_t mem_size = 0;
    empty_ptr buffer = NULL;
    size_t node_count = 0;
    empty_ptr tmp = NULL;
    
    *outSize = 0;
    while (head) { // iterator every list node
        type_node_ptr type_node = head->dataNode;
        //        int i;
        if (type_node->mem_block || type_node->str_block) {
            // filter pointer to blocks
            size_t oldSize = mem_size;
            size_t block_size = 0; // the size of the block
            byte_array newBuffer = NULL;
            
            mem_size += sizeof(size_t);
            if (type_node->mem_block) {
                block_size = type_node->block_size;
                mem_size += type_node->block_size;
            } else {
                mem_size += type_node->str_len + 1;
                block_size = type_node->str_len + 1;
            }
            
            buffer = realloc(buffer, mem_size); // realloc to expand memory block
            newBuffer = convert(byte_array, buffer) + oldSize; // make pointer offset
            memmove(newBuffer, &block_size, sizeof(size_t)); // move memory
            newBuffer += sizeof(size_t);
            memmove(newBuffer, type_node->data, block_size);
            newBuffer = NULL;
        } else {
            // basic value type
            byte_array newBuffer = convert(byte_array, buffer);
            size_t type_size = byteForType(type_node->type);
            buffer = realloc(buffer, sizeof(size_t) + type_size + mem_size);
            newBuffer = convert(byte_array, buffer);
            newBuffer += mem_size;
            mem_size = mem_size + sizeof(size_t) + type_size;
            memmove(newBuffer, &type_size, sizeof(size_t));
            newBuffer += sizeof(size_t);
            memmove(newBuffer, type_node->data, type_size);
            newBuffer = NULL;
        }
        node_count++;
        head = head->nextnode;
        type_node = NULL;
    }
    *outSize = mem_size;
    head = NULL;
    buffer = realloc(buffer, mem_size + sizeof(size_t));
    // write count
    tmp = convert(str_ptr, buffer) + mem_size;
    memmove(tmp, &node_count, sizeof(mem_size));
    tmp = NULL;
    *outSize += sizeof(mem_size);
    return buffer;
}

serial_list_ptr unserialize(empty_ptr block, size_t count) {
    unsigned int i;
    serial_list_ptr list = NULL;
    byte_array tmp_block = convert(byte_array, block);
    for (i = 0; i < count; i++) {
        type_node_ptr node = NULL;
        size_t size = 0;
        memmove(&size, tmp_block, sizeof(size_t));
        tmp_block += sizeof(size_t);
        node = createTypeNode();
        node->data = malloc(size);
        memmove(node->data, tmp_block, size);
        node->heap_allocated = TRUE;
        node->block_size = size;
        node->mem_block = TRUE;
        node->type = SERIAL_TYPE_EMPTY_PTR;
        tmp_block += size;
        if (list) {
            appendNode(list, node);
        } else {
            list = createSerialListWithHead(node);
        }
        node = NULL;
    }
    return list;
}

size_t serial_free_node(type_node_ptr node) {
    size_t count = type_node_size; // the memory size
    if (node->heap_allocated) { // if it is in the heap
        count += type_node_size;
        free(node->data); // free block
        node->heap_allocated = FALSE;
        node->data = NULL;
    }
    free(node); // free parent node
    node = NULL;
    return count; // the free size
}

size_t serial_free_list(serial_list_ptr list) {
    size_t count = serial_node_size + serial_free_node(list->dataNode);
    free(list);
    list = NULL;
    return count;
}

static serial_list_ptr initEmptySerialList(serial_list_ptr head) {
    head->dataNode = NULL;
    head->prenode = NULL;
    head->nextnode = NULL;
    return head;
}

static size_t byteForType(serial_type type) {
    switch (type) {
        case 0:
        case 1:
            return sizeof(int32_t);
        case 2:
        case 3:
            return sizeof(int8_t);
        case 4:
        case 5:
            return sizeof(int16_t);
        case 6:
        case 7:
            return sizeof(int64_t);
        case 8:
        case 9:
            return sizeof(long);
        case 10:
        case 11:
        case 12:
            return sizeof(empty_ptr);
        case 13:
        case 15:
            return sizeof(double);
        case 14:
        case 16:
            return sizeof(float);
        case 17:
            return sizeof(time_t);
        default:
            return 0;
    }
    return 0;
}

void appendStructureNode(serial_list_ptr *head, empty_ptr structure, packStructure pack) {
    serial_list_ptr tmp = NULL;
    if (NULL == *head) { // not exist list
        serial_list_ptr list_node = pack(structure);
        *head = list_node;
        return;
    }
    tmp = *head;
    
    while (tmp->nextnode) {
        tmp = tmp->nextnode;
    }
    
    tmp->nextnode = pack(structure);
    tmp->nextnode->prenode = tmp->nextnode;
    
}

empty_ptr unserializeStructure(serial_list_ptr *list, unpackTypeNode unpack) {
    return (*unpack)(list);
}

void destory_serial_list(serial_list_ptr head) {
    while (head) {
        // free datanode
        head = removeNode(head, 0);
    }
}
