/*
 * This file implements structures for query parsing
 *

AGPL License

Copyright (c) 2011 Russell Sullivan <jaksprats AT gmail DOT com>
ALL RIGHTS RESERVED 

   This file is part of ALCHEMY_DATABASE

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __ALC_QUERY__H
#define __ALC_QUERY__H

#include "adlist.h"

#include "btreepriv.h"
#include "xdb_common.h"
#include "common.h"

typedef struct r_tbl {
    robj   *name;
    bt     *btr;
    int     col_count;
    int     vimatch;
    ulong   ainc;
    robj   *col_name [MAX_COLUMN_PER_TABLE]; //TODO make sds
    uchar   col_type [MAX_COLUMN_PER_TABLE];
    bool    col_indxd[MAX_COLUMN_PER_TABLE]; /* used in updateRow OVRWR */
    uint32  n_intr;     /* num ACCESSES in current LRU interval */
    uint32  lastts;     /* HIGH-LOAD: last timestamp of lru interval */
    uint32  nextts;     /* HIGH-LOAD: next timestamp of lru interval */
    uint32  lrud;       /* timestamp & bool */
    char    lruc;       /* column containing LRU */
    char    lrui;       /* index containing LRU */
    uchar   nmci;       /* number of MultipleColumnIndexes */
    uchar   nltrgr;     /* number of LuaTriggers */
    int     sk;         /* index of shard-key column */
    int     fk_cmatch;  /* Foreign-key local column */
    int     fk_otmatch; /* Foreign-key other table's table */
    int     fk_ocmatch; /* Foreign-key other table's column */
} r_tbl_t;

typedef struct r_ind {
    bt    *btr;     /* Btree of index                                     */
    robj  *obj;     /* Name of index                                      */
    int    table;   /* table index is ON                                  */
    int    column;  /* single column OR 1st MCI column                    */
    list  *clist;   /* MultipleColumnIndex(mci) list                      */
    int    nclist;  /* MCI: num columns                                   */
    int   *bclist;  /* MCI: array representation (for speed)              */
    bool   virt;    /* virtual                      - i.e. on primary key */
    uchar  cnstr;   /* CONSTRAINTS: [UNIQUE,,,]                           */
    bool   lru;     /* LRUINDEX                                           */
    bool   luat;    /* LUATRIGGER - call lua function per CRUD            */
} r_ind_t;

typedef struct AlsoSqlObject { /* SIZE: 32 BYTES */
    char   *s;
    uint32  len;
    uint32  i;
    ulong   l;
    float   f;
    uchar   type;
    uchar   enc;
    uchar   freeme;
    uchar   empty;
} aobj;

typedef struct update_expression {
    bool  yes;
    char  op;
    int   c1match;
    int   type;
    char *pred;
    int   plen;
} ue_t;

typedef struct join_column {
    int t;
    int c;
    int jan;
} jc_t;

typedef struct filter {
    int      jan;    /* JoinAliasNumber filter runs on (for JOINS)        */
    int      imatch; /* index  filter runs on (for JOINS)                 */
    int      tmatch; /* table  filter runs on (for JOINS)                 */
    int      cmatch; /* column filter runs on                             */
    enum OP  op;     /* operation filter applies [,=,!=]                  */

    bool     iss;    /* is string, WHERE fk = 'fk' (1st iss=0, 2nd iss=1) */
    sds      key;    /* RHS of filter (e.g. AND x < 7 ... key=7)          */
    aobj     akey;   /* value of KEY [sds="7",int=7,float=7.0]            */

    sds      low;    /* LHS of Range filter (AND x BETWEEN 3 AND 4 -> 3)  */
    aobj     alow;   /* value of LOW  [sds="3",int=3,float=3.0]           */
    sds      high;   /* RHS of Range filter (AND x BETWEEN 3 AND 4 -> 4)  */
    aobj     ahigh;  /* value of HIGH [sds="4",int=4,float=4.0]           */

    list    *inl;    /* WHERE ..... AND x IN (1,2,3)                      */
    list    *klist;  /* MCI list of matching (ordered) keys (as f_t) */
} f_t;

typedef struct lua_trigger_command {
    sds fname;
    int ncols;
    int cmatchs[MAX_COLUMN_PER_TABLE];
} ltc_t;
typedef struct lua_trigger {
    ltc_t     add;
    ltc_t     del;
    ushort16  num; /* Index[][num] */
} luat_t;

typedef struct where_clause_order_by {
    uint32  nob;                       /* number ORDER BY columns             */
    int     obc[MAX_ORDER_BY_COLS];    /* ORDER BY col                        */
    int     obt[MAX_ORDER_BY_COLS];    /* ORDER BY tbl -> JOINS               */
    bool    asc[MAX_ORDER_BY_COLS];    /* ORDER BY ASC/DESC                   */
    long    lim;                       /* ORDER BY LIMIT                      */
    long    ofst;                      /* ORDER BY OFFSET                     */
    sds     ovar;                      /* OFFSET varname - used by cursors    */
} wob_t;

typedef struct check_sql_where_clause {
    uchar   wtype;
    sds     token;
    sds     lvr;     /* Leftover AFTER parse                    */
    f_t     wf;      /* WhereClause Filter (i.e. i,c,t,low,inl) */
    list   *flist;   /* FILTER list (nonindexed cols in WC)     */
} cswc_t;

typedef struct order_by_sort_element {
    void   *row;
    void  **keys;
    aobj   *apk;
    uchar  *lruc;
} obsl_t;

typedef struct index_join_pair {
    enum OP  op;
    f_t      lhs;     /* LeftHandSide  table,column,index */
    f_t      rhs;     /* RIGHTHandSide table,column,index */
    list    *flist;   /* Filter-lists are per JOIN LEVEL */
    uint32   nrows;   /* number of rows */
    int      kimatch; /* MCI imatch */
} ijp_t;
typedef struct join_block {
    bool    cstar;
    int     qcols;
    jc_t    js[MAX_JOIN_INDXS];

    sds     lvr;                /* Leftover AFTER parse                       */

    uint32  n_jind;             /* num 2ndary Join-Indexes                    */
    int     hw;                 /* "highwater" line JIndexes become Filters   */
    ijp_t   ij[MAX_JOIN_INDXS]; /* list of Join-Indexes                       */
    list   *mciflist;           /* missing MCI indexes as FILTERS             */

    wob_t   wb;                 /* ORDER BY [c1,c2 DESC] LIMIT x OFFSET y     */

    list   *fflist;             /* Deepest Join-Level FILTER list on RHS      */
    list   *fklist;             /* Deepest Join-Level Keylist on RHS          */
    int     fkimatch;           /* Deepest Join-Level MCI imatch              */
    uint32  fnrows;             /* Deepest Join-Level FILTER's number of rows */

    obsl_t *ob;                 /* ORDER BY values                            */
} jb_t;

void init_wob(wob_t *wb);
void destroy_wob(wob_t *wb);
void init_check_sql_where_clause(cswc_t *w, int tmatch, sds token);
void destroyINLlist(list **inl);
void releaseFlist(list **flist);
void destroyFlist(list **flist);
void destroy_check_sql_where_clause(cswc_t *w);

void init_join_block(jb_t *jb);
void destroy_join_block(cli *c, jb_t *jb);

typedef struct string_and_length {
    char *s;
    int   len;
    bool  freeme;
    uchar type;
} sl_t;
void release_sl(sl_t sl);

/* DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG */
void explainRQ(cli *c, cswc_t *w, wob_t *wb);

void initQueueOutput();
int  queueOutput(const char *format, ...);
void dumpQueueOutput(cli *c);

void dumpWB(printer *prn,   wob_t *wb);
void dumpW(printer *prn,    cswc_t *w);
void dumpSds(printer *prn,  sds s, char *smsg);
void dumpRobj(printer *prn, robj *r, char *smsg, char *dmsg);
void dumpFL(printer *prn,   char *prfx, char *title, list *flist);
void dumpSL(sl_t sl);

#endif /*__ALC_QUERY__H */ 