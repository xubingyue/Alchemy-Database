/*
 *
 * This file implements "CREATE TABLE x AS redis_datastructure"
 *

MIT License

Copyright (c) 2010 Russell Sullivan <jaksprats AT gmail DOT com>
ALL RIGHTS RESERVED 

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */

#ifndef __DENORM__H
#define __DENORM__H

#include "common.h"
#include "redis.h"

#define NUM_ACCESS_TYPES 14
#define ACCESS_SELECT_COMMAND_NUM 0
void createTableAsObject(redisClient *c);


bool emptyNoop(redisClient *c);
#define PIPE_NONE_FLAG      0
#define PIPE_ERR_FLAG       1
#define PIPE_ONE_LINER_FLAG 2
#define PIPE_EMPTY_SET_FLAG 3
long fakeClientPipe(redisClient *c,
                    redisClient *rfc,
                    void        *wfc,
                    int          is_ins,
                    flag        *flg,
                    bool (* adder)
                    (redisClient *c, void *x, robj *key, long *l, int b, int n),
                    bool (* emptyer)(redisClient *c));
#endif /* __DENORM__H */
