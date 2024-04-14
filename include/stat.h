/*
 * 定义了 逻辑文件层与物理文件层交互的数据
*/

#ifndef __STAT_H
#define __STAT_H

#define T_DIR       1  /* Directory */
#define T_FILE      2  /* File */
#define T_DEVICE    3  /* Device */

#define STAT_MAX_NAME  32

struct stat{
    char name[STAT_MAX_NAME + 1];
    int dev;       /* file system's disk device*/
    short type;    /* Type */
    uint64 size;   /* size of file */
};

#endif