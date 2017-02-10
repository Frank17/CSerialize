//
//  entry.c
//  CLanguage
//
//  Created by 邹智鹏 on 16/12/8.
//  Copyright © 2016年 邹智鹏. All rights reserved.
//

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "serial.h"

// 定义学生结构
typedef struct Student {
    char *name;
    char *stu_code;
    time_t birthday;
}Student;
void displayInfo(Student *stu);// 打印学生信息
// 定义序列化函数
serial_list_ptr packStudent(empty_ptr stu_ptr);

// 定义反序列化函数
empty_ptr unpackStudent(serial_list_ptr* bytes);

int main()
{
    size_t size = 0;
    struct tm date = { 0 };
    serial_list_ptr head = NULL;
    FILE *output = NULL, *input = NULL;
    serial_list_ptr unserialize_list = NULL;
    empty_ptr stuElem = NULL;
    empty_ptr buffer = NULL;
    size_t size_of_file_buffer = 0;
    byte_array bytes_in_file = NULL;
    
    
    Student *stu = convert(Student *, malloc(sizeof(Student))); // + 1
    stu->name = convert(str_ptr, malloc(5)); // + 1
    strcpy(stu->name, "1111");
    stu->stu_code = convert(str_ptr, malloc(9)); // + 1
    strcpy(stu->stu_code, "0144090");
    date.tm_year = 1995;
    date.tm_mon = 11;
    date.tm_mday = 10;
    stu->birthday = time(NULL);
    
    appendStructureNode(&head, stu, packStudent); // + 3
    
    // free
    free(stu); // -1
    stu = NULL;
    stu = convert(Student *, malloc(sizeof(Student))); // + 1
    stu->name = convert(str_ptr, malloc(5)); // + 1
    strcpy(stu->name, "2222");
    stu->stu_code = convert(str_ptr, malloc(9)); // + 1
    strcpy(stu->stu_code, "0144091");
    date.tm_year = 1996;
    date.tm_mon = 3;
    date.tm_mday = 24;
    stu->birthday = time(NULL);
    
    appendStructureNode(&head, stu, packStudent); // + 3
    free(stu); // -1
	stu = NULL;
    
    buffer = serialize(head, &size); // + 7
    
    output = fopen("file.ser", "wb");
    fwrite(buffer, size, 1, output);
    fclose(output);
    output = NULL;
    
    destory_serial_list(head);
    head = NULL;
	free(buffer);
	buffer = NULL;
    
    input = fopen("file.ser", "rb");
    fseek(input, -(long)sizeof(size_t), SEEK_END);
    fread(&size, sizeof(size_t), 1, input);
    size_of_file_buffer = ftell(input);
    rewind(input);
    bytes_in_file = convert(byte_array, malloc(size_of_file_buffer));
    fread(convert(empty_ptr, bytes_in_file), size_of_file_buffer, 1, input);
    fclose(input);
    
    head = unserialize_list = unserialize(convert(empty_ptr, bytes_in_file), size);
    stuElem = unserializeStructure(&unserialize_list, unpackStudent);
    displayInfo(convert(Student *, stuElem));
    free(stuElem);
    stuElem = NULL;
    
    stuElem = unserializeStructure(&unserialize_list, unpackStudent);
    displayInfo(convert(Student *, stuElem));
    free(stuElem);
    stuElem = NULL;
    destory_serial_list(head);
    unserialize_list = NULL;
	free(bytes_in_file);
	bytes_in_file = NULL;
    return 0;
}

serial_list_ptr packStudent(empty_ptr stu_ptr) {
    
    Student *stu = convert(Student *, stu_ptr);
    serial_list_ptr list = NULL;
    // create node to serialize name
    type_node_ptr node = createTypeNodeWithData(stu->name, TRUE);
    node->str_block = TRUE;
    node->str_len = strlen(stu->name);
    node->type = SERIAL_TYPE_STR_ADDR;
    
    
    list = createSerialListWithHead(node);
    
    // create node to serialize code
    node = createTypeNodeWithData(stu->stu_code, TRUE);
    node->str_block = TRUE;
    node->str_len = strlen(stu->stu_code);
    node->type = SERIAL_TYPE_STR_ADDR;
    
    list = appendNode(list, node);
    
    // create node to serialize birthday
    time_t *birthday = convert(time_t *, malloc(sizeof(time_t)));
    *birthday = stu->birthday;
    node = createTypeNodeWithData(birthday, TRUE);
    node->type = SERIAL_TYPE_TIME_T;
    
    list = appendNode(list, node);
    
    return list;
}

empty_ptr unpackStudent(serial_list_ptr* bytes) {
    
    // create object to store values
    Student *stu = convert(Student *, malloc(sizeof(Student)));
    
    // read there nodes
    // name
    type_node_ptr node = (*bytes)->dataNode;
//    stu->name = convert(str_ptr, malloc(node->str_len + 1));
//    strcpy(stu->name, (str_ptr)node->data);
    stu->name = convert(str_ptr, node->data);
    *bytes = (*bytes)->nextnode;
    
    // code
    node = (*bytes)->dataNode;
//    stu->stu_code = convert(str_ptr, malloc(node->str_len + 1));
//    strcpy(stu->stu_code, convert(str_ptr, node->data));
    stu->stu_code = convert(str_ptr, node->data);
    *bytes = (*bytes)->nextnode;
    
    // birthday
    node = (*bytes)->dataNode;
//    stu->birthday = *convert(time_t *, node->data);
    memmove(&stu->birthday, node->data, sizeof(time_t));
    *bytes = (*bytes)->nextnode;
    
    return stu;
}

void displayInfo(Student *stu) {
    struct tm * local = localtime(&stu->birthday);
    printf("%s\t%s\t%4d/%02d/%02d\n", stu->name, stu->stu_code, local->tm_year, local->tm_mon, local->tm_mday);
//    free(local);
    //local = NULL;
}
