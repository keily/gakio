/*
 * HashTable，采用开放地址解决 Hash 冲突,  Hash 函数
 * 和冲突解决方法参考自 Python dictobject 的实现
 * 
 * 初始化大小是 512，当负载因子达到 2/3 时进行 resize 操作，
 * 每次增加一倍
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "numdict.h"
#include "gakio.h"

#define DICT_INIT_SIZE (1 << 9)

typedef struct {
    char *key;                      
    void *value;                    
} dictitem;


struct numdict {
    unsigned long size;
    unsigned long use;
    unsigned long lookup;  /* for test */
    dictitem *items;
};


static unsigned long string_hash(const char *str);
static numdict *_numdict_new(unsigned long size);
static void numdict_resize(numdict *dict);


numdict *numdict_new()
{
    return _numdict_new(DICT_INIT_SIZE);
}


void numdict_delete(numdict *dict)
{
    unsigned long size, use, i = 0;
    dictitem *item;

    if (dict == NULL) {
        return;
    }
    size = dict->size;
    use = dict->use;
    
    for (i = (size - 1); (use > 0) && (i > 0); i--) {
        item = &(dict->items[i]);
        if (item->key) {
            free(item->key);
            if (item->value) {
                free((void *)GET_VARIABLE(item->value));
            }
            use--;
        }
    }

    free(dict->items);    /* free all dictitem */
    free(dict);           /* free dict node */
}


int numdict_put(numdict *dict, const char *key, void * const value)
{
    int key_len;
    key_len = strlen(key);
    
    dictitem *item;

    if (dict == NULL) {
        //printf("dict is NULL!\n");
        return 0;
    }
    if (key == NULL || value == NULL) {
        //printf("key is NULL or value is NULL\n");
        return 0;
    }

    /* 当 use/size >= 2/3 时，扩大表 */
    if (dict->use * 3 >= dict->size * 2) {
        numdict_resize(dict);
        printf("resize to: %lu\n", dict->size);
    }

    
    unsigned long hash = string_hash(key);
    unsigned long index = hash;
    unsigned long perturb = hash;
    
    while (1) {
        index = index & (dict->size - 1);
        item = &(dict->items[index]);
        
        dict->lookup++;  /* for test */

        /* 找到相同的 key, 替换值 */
        if ((item->key != NULL) && (!strcmp(item->key, key))) {
            item->value = value;
            //printf("key in, repce!\n");
            return 1;
        }

        if (item->key == NULL) {
            item->key = (char *)malloc((key_len + 1) * sizeof(char));
            if (item->key == NULL) {
                return 0;
            }
            strcpy(item->key, key);
            item->value = value;
            dict->use++;
            //printf("append new key index is %lu!\n", index);
            return 1;
        }

        perturb >>= 5;
        index = (index << 2) + index + perturb + 1;
    }
}


void *numdict_get(const numdict *dict, const char *key)
{
    dictitem *item;

    if (dict == NULL) {
        return NULL;
    }
    if (key == NULL) {
        return NULL;
    }

    unsigned long hash = string_hash(key);
    unsigned long index = hash;
    unsigned long perturb = hash;

    while (1) {
        index = index & (dict->size - 1);
        item = &(dict->items[index]);
        
        if (item->key == NULL) {
            return NULL;
        }

        /* key 相同，查找成功 */
        if ((item->key != NULL) && (!strcmp(item->key, key))) {
            return item->value;
        }

        perturb >>= 5;
        index = (index << 2) + index + perturb + 1;
    }

}

unsigned long get_lookup(numdict *dict)
{
    return dict->lookup;
}


static numdict *_numdict_new(unsigned long size)
{
    numdict *dict;
    
    dict = (numdict *)malloc(sizeof(numdict));
    printf("new dict malloc\n");
    if (dict == NULL) {
        return NULL;
    }
    dict->size = size;
    dict->use = 0;
    dict->lookup = 0;
    dict->items = (dictitem *)malloc(dict->size * sizeof(dictitem));
    printf("new dict items malloc\n");
    if (dict->items == NULL) {
        free(dict);
        return NULL;
    }
    memset(dict->items, 0, dict->size * sizeof(dictitem)); 
    return dict;
}


static void numdict_resize(numdict *dict)
{
    unsigned long oldsize, newsize, olduse, i;
    dictitem *item, *temp;
    numdict *newdict;

    oldsize = dict->size;
    olduse = dict->use;

    /* 这里没有检测 newsize 是否溢出，因为 unsigned long 长度和指针一样 */
    newsize = oldsize << 1;

    //items = (dictitem *)malloc(sizeof(dictitem) * newsize);
    newdict = _numdict_new(newsize);
    if (newdict == NULL) {
        return;
    }

    /* 
     * 本来 i >= 0 时才结束循环，不过 unsigned long 必然大于等于 0，
     * 考虑到 use/size <= 2/3，所以 i > 0 即可
     */
    for (i = (oldsize - 1); (olduse > 0) && (i > 0); i--) {
        item = &(dict->items[i]);
        if (item->key) {
            if (numdict_put(newdict, item->key, item->value)) {
                olduse--;
            } 
        }
    }
    
    /* 如果 olduse == 0, 则重新散列成功，交换 hashtable */
    if (!olduse) {
        temp = dict->items;
        dict->items = newdict->items;
        dict->size = newsize;
        newdict->items = temp;
        newdict->size = oldsize;
    }
    numdict_delete(newdict);
    //printf("rehash ok\n");
}


/* 这个 Hash 函数来自于 Python */
static unsigned long string_hash(const char *str) {
    unsigned long x;
    unsigned long len = 0;
 
    x = *str << 7;
    while (*str) {
        x = (1000003 * x) ^ *str++;
        len++;
    }
    x ^= len;
    return x;
}
