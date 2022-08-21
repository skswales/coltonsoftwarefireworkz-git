/* ev_tree.c */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Copyright (C) 1989-1998 Colton Software Limited
 * Copyright (C) 1998-2015 R W Colton */

/* Tree management routines for evaluator */

/* MRJC March 1989 / January 1991 */

#include "common/gflags.h"

#include "ob_ss/ob_ss.h"

/*
internal functions
*/

static void
tree_sort_customs(void);

static void
tree_sort_names(void);

static void
tree_sort_ranges(
    _InVal_     EV_DOCNO ev_docno,
    P_SS_DOC p_ss_doc);

static void
tree_sort_slrs(
    P_SS_DOC p_ss_doc);

/*
global use tables
*/

DEPTABLE custom_use_deptable;
DEPTABLE event_use_deptable;
DEPTABLE name_use_deptable;

/*
block size increments
*/

#define CUSTOM_USE_INC 10
#define NAME_USE_INC 10
#define SLR_USE_INC 100
#define RANGE_USE_INC 20
#define EVENT_USE_INC 2

/******************************************************************************
*
* add a custom function use
*
******************************************************************************/

_Check_return_
static STATUS
add_custom_use(
    P_EV_HANDLE p_h_custom,
    _InRef_     PC_EV_SLR p_slr)
{
    STATUS status = STATUS_OK;
    P_CUSTOM_USE p_custom_use;
    SC_ARRAY_INIT_BLOCK array_init_block = aib_init(CUSTOM_USE_INC, sizeof32(CUSTOM_USE), TRUE);

    if(NULL != (p_custom_use = al_array_extend_by(&custom_use_deptable.h_table, CUSTOM_USE, 1, &array_init_block, &status)))
    {
        /* copy in new dependency */
        p_custom_use->custom_to = *p_h_custom;
        p_custom_use->slr_by = *p_slr;

        global_flags.blown = 1;
    }

    return(status);
}

/******************************************************************************
*
* add an event use
*
******************************************************************************/

_Check_return_
static STATUS
add_event_use(
    P_EVENT_TYPE p_event_type,
    _InRef_     PC_EV_SLR p_ev_slr)
{
    STATUS status = STATUS_OK;
    P_EVENT_USE p_event_use;
    SC_ARRAY_INIT_BLOCK array_init_block = aib_init(EVENT_USE_INC, sizeof32(EVENT_USE), TRUE);

    if(NULL != (p_event_use = al_array_extend_by(&event_use_deptable.h_table, EVENT_USE, 1, &array_init_block, &status)))
    {
        /* copy in new dependency */
        p_event_use->event_type = *p_event_type;
        p_event_use->slr_by = *p_ev_slr;

        global_flags.blown = 1;
    }

    return(status);
}

/******************************************************************************
*
* add a use to the list of name uses
*
******************************************************************************/

_Check_return_
static STATUS
add_name_use(
    P_EV_NAME_REF p_ev_name_ref,
    _InRef_     PC_EV_SLR p_slr)
{
    STATUS status = STATUS_OK;
    P_NAME_USE p_name_use;
    SC_ARRAY_INIT_BLOCK array_init_block = aib_init(NAME_USE_INC, sizeof32(NAME_USE), TRUE);

    if(NULL != (p_name_use = al_array_extend_by(&name_use_deptable.h_table, NAME_USE, 1, &array_init_block, &status)))
    {
        /* copy in new dependency */
        p_name_use->name_to = *p_ev_name_ref;
        p_name_use->slr_by = *p_slr;

        global_flags.blown = 1;
    }

    return(status);
}

/******************************************************************************
*
* add a dependency to the list of range dependencies
*
******************************************************************************/

_Check_return_
static STATUS
add_range_use(
    _InRef_     PC_EV_RANGE p_ev_range,
    _InRef_     PC_EV_SLR p_ev_slr,
    _InVal_     UINT by_index)
{
    STATUS status = STATUS_OK;
    P_SS_DOC p_ss_doc;
    P_RANGE_USE p_range_use;
    SC_ARRAY_INIT_BLOCK array_init_block = aib_init(RANGE_USE_INC, sizeof32(RANGE_USE), TRUE);

    if(P_DATA_NONE == (p_ss_doc = ev_p_ss_doc_from_docno(ev_slr_docno(&p_ev_range->s))))
        return(STATUS_FAIL);

    if(NULL != (p_range_use = al_array_extend_by(&p_ss_doc->range_table.h_table, RANGE_USE, 1, &array_init_block, &status)))
    {
        /* copy in new dependency */
        p_range_use->range_to = *p_ev_range;
        p_range_use->slr_by = *p_ev_slr;
        p_range_use->by_index = (U8) by_index;

        global_flags.blown = 1;
    }

    return(status);
}

/******************************************************************************
*
* add a dependency to the list of slr dependencies
*
******************************************************************************/

_Check_return_
static STATUS
add_slr_use(
    _InRef_     PC_EV_SLR p_ev_slr,
    _InRef_     PC_EV_SLR p_ev_slr_by,
    _InVal_     UINT by_index)
{
    STATUS status = STATUS_OK;
    P_SS_DOC p_ss_doc;
    P_SLR_USE p_slr_use;
    SC_ARRAY_INIT_BLOCK array_init_block = aib_init(SLR_USE_INC, sizeof32(SLR_USE), TRUE);

    if(P_DATA_NONE == (p_ss_doc = ev_p_ss_doc_from_docno(ev_slr_docno(p_ev_slr))))
        return(STATUS_FAIL);

    if(NULL != (p_slr_use = al_array_extend_by(&p_ss_doc->slr_table.h_table, SLR_USE, 1, &array_init_block, &status)))
    {
        /* copy in new dependency */
        p_slr_use->slr_to = *p_ev_slr;
        p_slr_use->slr_by = *p_ev_slr_by;
        p_slr_use->by_index = (U8) by_index;

        global_flags.blown = 1;
    }

    return(status);
}

/******************************************************************************
*
* routine for bsearch to compare two CUSTOM_USE
*
******************************************************************************/

PROC_BSEARCH_PROTO(static, compare_custom, CUSTOM_USE, CUSTOM_USE)
{
    BSEARCH_KEY_VAR_DECL(PC_CUSTOM_USE, key);
    BSEARCH_DATUM_VAR_DECL(PC_CUSTOM_USE, datum);

    if(key->custom_to > datum->custom_to)
        return(1);

    if(key->custom_to < datum->custom_to)
        return(-1);

    return(0);
}

/******************************************************************************
*
* routine for bsearch to compare two NAME_USE
*
******************************************************************************/

PROC_BSEARCH_PROTO(static, compare_name, NAME_USE, NAME_USE)
{
    BSEARCH_KEY_VAR_DECL(PC_NAME_USE, key);
    BSEARCH_DATUM_VAR_DECL(PC_NAME_USE, datum);

    if(key->name_to.h_name > datum->name_to.h_name)
        return(1);

    if(key->name_to.h_name < datum->name_to.h_name)
        return(-1);

    return(0);
}

/******************************************************************************
*
* routine for bsearch to compare two SLR_USE
*
******************************************************************************/

PROC_BSEARCH_PROTO(static, compare_slr, SLR_USE, SLR_USE)
{
    BSEARCH_KEY_VAR_DECL(PC_SLR_USE, key);
    BSEARCH_DATUM_VAR_DECL(PC_SLR_USE, datum);

    if(key->slr_to.row > datum->slr_to.row)
        return(1);
    if(key->slr_to.row < datum->slr_to.row)
        return(-1);

    if(key->slr_to.col > datum->slr_to.col)
        return(1);
    if(key->slr_to.col < datum->slr_to.col)
        return(-1);

    return(0);
}

/******************************************************************************
*
* return flag saying whether entry is deleted for aligator garbage collection
*
******************************************************************************/

PROC_ELEMENT_DELETED_PROTO(static, deleted_custom)
{
    return(((PC_CUSTOM_USE) p_any)->flags.tobedel);
}

PROC_ELEMENT_DELETED_PROTO(static, deleted_event)
{
    return(((PC_EVENT_USE) p_any)->flags.tobedel);
}

PROC_ELEMENT_DELETED_PROTO(static, deleted_name)
{
    return(((PC_NAME_USE) p_any)->flags.tobedel);
}

PROC_ELEMENT_DELETED_PROTO(static, deleted_range)
{
    return(((PC_RANGE_USE) p_any)->flags.tobedel);
}

PROC_ELEMENT_DELETED_PROTO(static, deleted_slr)
{
    return(((PC_SLR_USE) p_any)->flags.tobedel);
}

/******************************************************************************
*
* add all the dependencies for an expression cell into the tree
*
******************************************************************************/

_Check_return_
extern STATUS
ev_add_compiler_output_to_tree(
    P_COMPILER_OUTPUT p_compiler_output,
    _InRef_     PC_EV_SLR p_ev_slr,
    _InVal_     BOOL add_todo)
{
    S32 custom_checkuse, name_checkuse;
    STATUS status = STATUS_OK;
    U8 by_index, by_stop;

    /* push state of names and custom check use flags, then
     * clear them; we don't want to delete definitions that
     * may have uses that we are about to add
     */
    custom_checkuse = custom_def.flags.checkuse;
    custom_def.flags.checkuse = 0;
    name_checkuse = name_def.flags.checkuse;
    name_def.flags.checkuse = 0;

    for(by_index = 0, by_stop = (U8) array_elements32(&p_compiler_output->h_slrs); status_ok(status) && by_index < by_stop; ++by_index)
        status = add_slr_use(array_ptr(&p_compiler_output->h_slrs, EV_SLR, by_index), p_ev_slr, by_index);

    for(by_index = 0, by_stop = (U8) array_elements32(&p_compiler_output->h_ranges); status_ok(status) && by_index < by_stop; ++by_index)
        status = add_range_use(array_ptr(&p_compiler_output->h_ranges, EV_RANGE, by_index), p_ev_slr, by_index);

    for(by_index = 0, by_stop = (U8) array_elements32(&p_compiler_output->h_names); status_ok(status) && by_index < by_stop; ++by_index)
        status = add_name_use(array_ptr(&p_compiler_output->h_names, EV_NAME_REF, by_index), p_ev_slr);

    for(by_index = 0, by_stop = (U8) array_elements32(&p_compiler_output->h_custom_calls); status_ok(status) && by_index < by_stop; ++by_index)
        status = add_custom_use(array_ptr(&p_compiler_output->h_custom_calls, EV_HANDLE, by_index), p_ev_slr);

    for(by_index = 0, by_stop = (U8) array_elements32(&p_compiler_output->h_custom_defs); status_ok(status) && by_index < by_stop; ++by_index)
    {
        P_EV_HANDLE p_ev_handle = array_ptr(&p_compiler_output->h_custom_defs, EV_HANDLE, by_index);
        ARRAY_INDEX custom_num = custom_def_find(*p_ev_handle);

        if(custom_num >= 0)
        {
            P_EV_CUSTOM p_ev_custom = array_ptr(&custom_def.h_table, EV_CUSTOM, custom_num);
            P_SS_DOC p_ss_doc;

            p_ev_custom->owner = *p_ev_slr;
            p_ev_custom->flags.undefined = 0;

            /* mark document as a custom function document */
            if(P_DATA_NONE != (p_ss_doc = ev_p_ss_doc_from_docno(ev_slr_docno(p_ev_slr))))
                p_ss_doc->custom = 1;

            ev_todo_add_custom_dependents(*p_ev_handle);
        }
    }

    for(by_index = 0, by_stop = (U8) array_elements32(&p_compiler_output->h_events); status_ok(status) && by_index < by_stop; ++by_index)
        status = add_event_use(array_ptr(&p_compiler_output->h_events, EVENT_TYPE, by_index), p_ev_slr);

    /* restore flags */
    custom_def.flags.checkuse = (UBF) custom_checkuse;
    name_def.flags.checkuse = (UBF) name_checkuse;

    if(status_ok(status) && add_todo)
        ev_todo_add_slr(p_ev_slr);

    return(status);
}

/******************************************************************************
*
* add the dependents of a cell to the tree
* (used when a cell is duplicated etc)
*
******************************************************************************/

_Check_return_
extern STATUS
ev_add_ev_cell_to_tree(
    P_EV_CELL p_ev_cell,
    _InRef_     PC_EV_SLR p_ev_slr)
{
    STATUS status = STATUS_OK;
    UBF by_index;
    UBF by_stop;

    if(!p_ev_cell->parms.rpn_variable)
        return(STATUS_OK);

    for(by_index = 0, by_stop = p_ev_cell->parms.slr_n; status_ok(status) && by_index < by_stop; ++by_index)
        status = add_slr_use((p_ev_slr_from_ev_cell(p_ev_cell, by_index)), p_ev_slr, by_index);

    for(by_index = 0, by_stop = p_ev_cell->parms.range_n; status_ok(status) && by_index < by_stop; ++by_index)
        status = add_range_use((p_ev_range_from_ev_cell(p_ev_cell, by_index)), p_ev_slr, by_index);

    for(by_index = 0, by_stop = p_ev_cell->parms.name_n; status_ok(status) && by_index < by_stop; ++by_index)
        status = add_name_use((p_ev_name_from_ev_cell(p_ev_cell, by_index)), p_ev_slr);

    for(by_index = 0, by_stop = p_ev_cell->parms.custom_n; status_ok(status) && by_index < by_stop; ++by_index)
        status = add_custom_use((p_ev_custom_from_ev_cell(p_ev_cell, by_index)), p_ev_slr);

    for(by_index = 0, by_stop = p_ev_cell->parms.event_n; status_ok(status) && by_index < by_stop; ++by_index)
        status = add_event_use((p_ev_event_from_ev_cell(p_ev_cell, by_index)), p_ev_slr);

    if(status_ok(status))
        ev_todo_add_slr(p_ev_slr);

    return(status);
}

/******************************************************************************
*
* search the custom use table for a reference to a given custom
*
******************************************************************************/

_Check_return_
extern ARRAY_INDEX
search_for_custom_use(
    _InVal_     EV_HANDLE h_custom)
{
    P_CUSTOM_USE sp_custom_use, p_custom_use;
    ARRAY_INDEX res;
    CUSTOM_USE target;

    tree_sort_customs();

    if(!custom_use_deptable.sorted)
        return(STATUS_FAIL);

    sp_custom_use = array_base(&custom_use_deptable.h_table, CUSTOM_USE);

    target.custom_to = h_custom;

    /* search for reference */
    if(NULL != (p_custom_use = (P_CUSTOM_USE)
            bsearch(&target, sp_custom_use, (U32) custom_use_deptable.sorted, sizeof(CUSTOM_USE), compare_custom)))
    {
        /* step back to start of all names the same */
        ARRAY_INDEX custom_ix = PtrDiffElemU32(p_custom_use, sp_custom_use);

        while((custom_ix > 0) && (0 == compare_custom(p_custom_use - 1, &target)))
        {
            --custom_ix;
            --p_custom_use;
            assert(NULL != (p_custom_use - 1));
        }

        res = custom_ix;
    }
    else
        res = STATUS_FAIL;

    return(res);
}

/******************************************************************************
*
* search the name tree for a reference to a given name
*
******************************************************************************/

_Check_return_
extern ARRAY_INDEX
search_for_name_use(
    _InVal_     EV_HANDLE h_name)
{
    P_NAME_USE sp_name_use, p_name_use;
    ARRAY_INDEX res;
    NAME_USE target;

    tree_sort_names();

    if(!name_use_deptable.sorted)
        return(STATUS_FAIL);

    target.name_to.h_name = h_name;

    sp_name_use = array_base(&name_use_deptable.h_table, NAME_USE);

    /* search for reference */
    if(NULL != (p_name_use = (P_NAME_USE)
            bsearch(&target, sp_name_use, (U32) name_use_deptable.sorted, sizeof(NAME_USE), compare_name)))
    {
        /* step back to start of all names the same */
        ARRAY_INDEX name_ix = PtrDiffElemU32(p_name_use, sp_name_use);

        while((name_ix > 0) && (0 == compare_name(p_name_use - 1, &target)))
        {
            --name_ix;
            --p_name_use;
            assert(NULL != (p_name_use - 1));
        }

        res = name_ix;
    }
    else
        res = STATUS_FAIL;

    return(res);
}

/******************************************************************************
*
* search the table of cell references for a reference to the given cell
*
******************************************************************************/

_Check_return_
extern ARRAY_INDEX
search_for_slr_use(
    _InRef_     PC_EV_SLR p_ev_slr)
{
    P_SS_DOC p_ss_doc;
    ARRAY_INDEX res;

    if(P_DATA_NONE == (p_ss_doc = ev_p_ss_doc_from_docno(ev_slr_docno(p_ev_slr))))
        return(STATUS_FAIL);

    tree_sort_slrs(p_ss_doc);

    res = STATUS_FAIL;

    if(p_ss_doc->slr_table.sorted)
    {
        P_SLR_USE sp_slr_use;
        SLR_USE target;
        P_SLR_USE p_slr_use;

        target.slr_to = *p_ev_slr;
        sp_slr_use = p_slr_use_from_p_ss_doc(p_ss_doc, 0);

        /* search for reference */
        if(NULL != (p_slr_use = (P_SLR_USE)
                bsearch(&target, sp_slr_use, (U32) p_ss_doc->slr_table.sorted, sizeof(SLR_USE), compare_slr)))
        {
            /* step back to start of all refs the same */
            ARRAY_INDEX slr_ix = PtrDiffElemU32(p_slr_use, sp_slr_use);

            while((slr_ix > 0) && (0 == compare_slr(p_slr_use - 1, &target)))
            {
                --slr_ix;
                --p_slr_use;
                assert(NULL != (p_slr_use - 1));
            }

            res = slr_ix;
        }
    }

    return(res);
}

/******************************************************************************
*
* step along all dependencies for a given name and put them in the document list
*
******************************************************************************/

static void
ensure_refs_to_name_in_list(
    P_DOCU_DEP_SUP p_docu_dep_sup,
    _InVal_     EV_HANDLE h_name)
{
    ARRAY_INDEX name_ix;

    if((name_ix = search_for_name_use(h_name)) >= 0)
    {
        P_NAME_USE p_name_use = array_ptr(&name_use_deptable.h_table, NAME_USE, name_ix);
        NAME_USE key;
        const ARRAY_INDEX n_name_uses = array_elements(&name_use_deptable.h_table);

        key.name_to.h_name = h_name;

        while((name_ix < n_name_uses) && (0 == compare_name(p_name_use, &key)))
        {
            (*p_docu_dep_sup->p_proc_ensure_docno) (p_docu_dep_sup, ev_slr_docno(&p_name_use->slr_by));
            ++p_name_use;
            ++name_ix;
        }
    }
}

/******************************************************************************
*
* add dependent documents to the list
*
******************************************************************************/

extern void
tree_get_dependent_docs(
    _InVal_     EV_DOCNO ev_docno,
    P_DOCU_DEP_SUP p_docu_dep_sup)
{
    P_SS_DOC p_ss_doc;

    (*p_docu_dep_sup->p_proc_ensure_docno) (p_docu_dep_sup, ev_docno);

    if(P_DATA_NONE == (p_ss_doc = ev_p_ss_doc_from_docno(ev_docno)))
    {
        assert0();
        return;
    }

    {
    const ARRAY_INDEX n_slrs = array_elements(&p_ss_doc->slr_table.h_table);
    ARRAY_INDEX i;
    P_SLR_USE p_slr_use = array_range(&p_ss_doc->slr_table.h_table, SLR_USE, 0, n_slrs);

    for(i = 0; i < n_slrs; ++i, ++p_slr_use)
    {
        if(!p_slr_use->flags.tobedel)
            (*p_docu_dep_sup->p_proc_ensure_docno) (p_docu_dep_sup, ev_slr_docno(&p_slr_use->slr_by));
    }
    } /*block*/

    {
    const ARRAY_INDEX n_ranges = array_elements(&p_ss_doc->range_table.h_table);
    ARRAY_INDEX i;
    P_RANGE_USE p_range_use = array_range(&p_ss_doc->range_table.h_table, RANGE_USE, 0, n_ranges);

    for(i = 0; i < n_ranges; ++i, ++p_range_use)
    {
        if(!p_range_use->flags.tobedel)
            (*p_docu_dep_sup->p_proc_ensure_docno) (p_docu_dep_sup, ev_slr_docno(&p_range_use->slr_by));
    }
    } /*block*/

    {
    const ARRAY_INDEX n_names = array_elements(&name_def.h_table);

    if(p_ss_doc->nam_ref_count && n_names)
    {
        P_EV_NAME p_ev_name = array_range(&name_def.h_table, EV_NAME, 0, n_names);
        ARRAY_INDEX i;

        for(i = 0; i < n_names; ++i, ++p_ev_name)
        {
            if(!p_ev_name->flags.tobedel)
            {
                switch(p_ev_name->def_data.did_num)
                {
                case RPN_DAT_SLR:
                    /* if name refers to this document */
                    if(ev_slr_docno(&p_ev_name->def_data.arg.slr) == ev_docno)
                        ensure_refs_to_name_in_list(p_docu_dep_sup, p_ev_name->handle);
                    break;

                case RPN_DAT_RANGE:
                    /* if name refers to this document */
                    if(ev_slr_docno(&p_ev_name->def_data.arg.range.s) == ev_docno)
                        ensure_refs_to_name_in_list(p_docu_dep_sup, p_ev_name->handle);
                    break;
                }
            }
        }
    }
    } /*block*/

    {
    const ARRAY_INDEX n_customs = array_elements(&custom_use_deptable.h_table);

    /* find all the sheets which use one of our custom functions */
    if(0 != n_customs)
    {
        ARRAY_INDEX i;
        P_CUSTOM_USE p_custom_use = array_range(&custom_use_deptable.h_table, CUSTOM_USE, 0, n_customs);

        for(i = 0; i < n_customs; ++i, ++p_custom_use)
        {
            if(!p_custom_use->flags.tobedel)
            {
                ARRAY_INDEX custom_num = custom_def_find(p_custom_use->custom_to);

                if(custom_num >= 0)
                {
                    P_EV_CUSTOM p_ev_custom = array_ptr(&custom_def.h_table, EV_CUSTOM, custom_num);

                    if(ev_slr_docno(&p_ev_custom->owner) == ev_docno)
                        (*p_docu_dep_sup->p_proc_ensure_docno) (p_docu_dep_sup, ev_slr_docno(&p_custom_use->slr_by));
                }
            }
        }
    }
    } /*block*/
}

/******************************************************************************
*
* add to list of supporting documents
*
******************************************************************************/

extern void
tree_get_supporting_docs(
    _InVal_     EV_DOCNO ev_docno_in,
    P_DOCU_DEP_SUP p_docu_dep_sup)
{
    (*p_docu_dep_sup->p_proc_ensure_docno) (p_docu_dep_sup, ev_docno_in);

    {
    DOCNO docno = DOCNO_NONE;

    while(DOCNO_NONE != (docno = docno_enum_thunks(docno)))
    {
        P_SS_DOC p_ss_doc = ev_p_ss_doc_from_docno(docno);

        if(P_DATA_NONE != p_ss_doc)
        {
            U8 this_docno = 0;

            {
            const ARRAY_INDEX n_slrs = array_elements(&p_ss_doc->slr_table.h_table);
            ARRAY_INDEX i;
            P_SLR_USE p_slr_use = array_range(&p_ss_doc->slr_table.h_table, SLR_USE, 0, n_slrs);

            for(i = 0; i < n_slrs; ++i, ++p_slr_use)
            {
                if(p_slr_use->flags.tobedel)
                    continue;

                if(ev_slr_docno(&p_slr_use->slr_by) == ev_docno_in)
                {
                    (*p_docu_dep_sup->p_proc_ensure_docno) (p_docu_dep_sup, docno);
                    this_docno = 1;
                    break;
                }
            }
            } /*block*/

            if(!this_docno)
            {
                const ARRAY_INDEX n_ranges = array_elements(&p_ss_doc->range_table.h_table);
                ARRAY_INDEX i;
                P_RANGE_USE p_range_use = array_range(&p_ss_doc->range_table.h_table, RANGE_USE, 0, n_ranges);

                for(i = 0; i < n_ranges; ++i, ++p_range_use)
                {
                    if(p_range_use->flags.tobedel)
                        continue;

                    if(ev_slr_docno(&p_range_use->slr_by) == ev_docno_in)
                    {
                        (*p_docu_dep_sup->p_proc_ensure_docno) (p_docu_dep_sup, docno);
                        break;
                    }
                }
            }
        }
        else
        {
            PTR_ASSERT(p_ss_doc);
        }
    }
    } /*block*/

    {
    const ARRAY_INDEX n_names = array_elements(&name_use_deptable.h_table);

    if(n_names)
    {
        ARRAY_INDEX i;
        P_NAME_USE p_name_use = array_range(&name_use_deptable.h_table, NAME_USE, 0, n_names);

        for(i = 0; i < n_names; ++i, ++p_name_use)
        {
            if(p_name_use->flags.tobedel)
                continue;

            if(ev_slr_docno(&p_name_use->slr_by) == ev_docno_in)
            {
                EV_HANDLE name_num = name_def_find(p_name_use->name_to.h_name);

                if(name_num >= 0)
                {
                    P_EV_NAME p_ev_name = array_ptr(&name_def.h_table, EV_NAME, name_num);

                    switch(p_ev_name->def_data.did_num)
                    {
                    case RPN_DAT_SLR:
                        (*p_docu_dep_sup->p_proc_ensure_docno) (p_docu_dep_sup, ev_slr_docno(&p_ev_name->def_data.arg.slr));
                        break;

                    case RPN_DAT_RANGE:
                        (*p_docu_dep_sup->p_proc_ensure_docno) (p_docu_dep_sup, ev_slr_docno(&p_ev_name->def_data.arg.range.s));
                        break;
                    }

                    (*p_docu_dep_sup->p_proc_ensure_docno) (p_docu_dep_sup, ev_slr_docno(&p_ev_name->owner));
                }
            }
        }
    }
    } /*block*/

    { /* find all the custom function sheets to which we refer */
    const ARRAY_INDEX n_customs = array_elements(&custom_use_deptable.h_table);

    if(0 != n_customs)
    {
        ARRAY_INDEX i;
        P_CUSTOM_USE p_custom_use = array_range(&custom_use_deptable.h_table, CUSTOM_USE, 0, n_customs);

        for(i = 0; i < n_customs; ++i, ++p_custom_use)
        {
            if(p_custom_use->flags.tobedel)
                continue;

            if(ev_slr_docno(&p_custom_use->slr_by) == ev_docno_in)
            {
                ARRAY_INDEX custom_num = custom_def_find(p_custom_use->custom_to);

                if(custom_num >= 0)
                {
                    P_EV_CUSTOM p_ev_custom = array_ptr(&custom_def.h_table, EV_CUSTOM, custom_num);

                    if(DOCNO_NONE != ev_slr_docno(&p_ev_custom->owner))
                        (*p_docu_dep_sup->p_proc_ensure_docno) (p_docu_dep_sup, ev_slr_docno(&p_ev_custom->owner));
                }
            }
        }
    }
    } /*block*/
}

/******************************************************************************
*
* remove any entries that are to be deleted from a dependency table
*
******************************************************************************/

static S32 /* we delete any ? */
tree_remove_tobedel(
    P_DEPTABLE p_deptable,
    _InRef_     P_PROC_ELEMENT_DELETED p_proc_element_deleted)
{
    ARRAY_INDEX undeleted_elements;

    if(p_deptable->flags.tobedel && (0 != (undeleted_elements = array_elements(&p_deptable->h_table))))
    {
        /* garbage collect deptable */
        AL_GARBAGE_FLAGS al_garbage_flags;
        AL_GARBAGE_FLAGS_CLEAR(al_garbage_flags);
        al_garbage_flags.remove_deleted = 1;
        al_garbage_flags.shrink = 1;
        al_garbage_flags.may_dispose = 1;
        al_array_auto_compact_set(&p_deptable->h_table);
        consume(S32, al_array_garbage_collect(&p_deptable->h_table, p_deptable->mindel, p_proc_element_deleted, al_garbage_flags));

        p_deptable->mindel = array_elements(&p_deptable->h_table);
        p_deptable->sorted -= undeleted_elements - array_elements(&p_deptable->h_table);

        p_deptable->flags.tobedel = 0;
        return(1);
    }

    return(0);
}

/******************************************************************************
*
* generic routine to sort trees
*
******************************************************************************/

/*ncr*/
extern S32 /* table was altered */
tree_sort(
    P_DEPTABLE p_deptable,
    _InRef_     P_PROC_ELEMENT_DELETED p_proc_element_deleted,
    _InRef_opt_ P_PROC_BSEARCH p_proc_bsearch)
{
    S32 blown = tree_remove_tobedel(p_deptable, p_proc_element_deleted);

    if(p_deptable->sorted < array_elements(&p_deptable->h_table))
    {
        if(NULL != p_proc_bsearch)
        {
            trace_0(TRACE_MODULE_EVAL, TEXT("** sort **"));
            al_array_qsort(&p_deptable->h_table, p_proc_bsearch);
        }

        p_deptable->sorted = array_elements(&p_deptable->h_table);

        blown = 1;
    }

    return(blown);
}

/******************************************************************************
*
* sort all the trees
*
******************************************************************************/

extern void
tree_sort_all(void)
{
    DOCNO docno = DOCNO_NONE;

    while(DOCNO_NONE != (docno = docno_enum_thunks(docno)))
    {
        P_SS_DOC p_ss_doc;

        if(P_DATA_NONE != (p_ss_doc = ev_p_ss_doc_from_docno(docno)))
        {
            tree_sort_ranges(docno, p_ss_doc);
            tree_sort_slrs(p_ss_doc);
        }
    }

    tree_sort_customs();
    tree_sort_events();
    tree_sort_names();

    custom_list_sort();
    name_list_sort();
}

/******************************************************************************
*
* sort custom use table
*
******************************************************************************/

static void
tree_sort_customs(void)
{
    if(global_flags.lock)
        return;

    if(tree_sort(&custom_use_deptable, deleted_custom, compare_custom))
        global_flags.blown = 1;

    trace_2(TRACE_APP_MEMORY_USE,
            TEXT("custom use table is: ") U32_TFMT TEXT(" elements") U32_TFMT TEXT(" bytes"),
            array_elements(&custom_use_deptable.h_table), array_size32(&custom_use_deptable.h_table) * sizeof32(CUSTOM_USE));
}

/******************************************************************************
*
* sort event use table
*
******************************************************************************/

extern void
tree_sort_events(void)
{
    if(global_flags.lock)
        return;

    if(tree_sort(&event_use_deptable, deleted_event, NULL))
        global_flags.blown = 1;

    trace_2(TRACE_APP_MEMORY_USE,
            TEXT("event use table is: ") U32_TFMT TEXT(" elements") U32_TFMT TEXT(" bytes"),
            array_elements(&event_use_deptable.h_table), array_size32(&event_use_deptable.h_table) * sizeof32(EVENT_USE));
}

/******************************************************************************
*
* sort name use tree
*
******************************************************************************/

static void
tree_sort_names(void)
{
    if(global_flags.lock)
        return;

    if(tree_sort(&name_use_deptable, deleted_name, compare_name))
        global_flags.blown = 1;

    trace_2(TRACE_APP_MEMORY_USE,
            TEXT("name use table is: ") U32_TFMT TEXT(" elements") U32_TFMT TEXT(" bytes"),
            array_elements(&name_use_deptable.h_table), array_size32(&name_use_deptable.h_table) * sizeof32(NAME_USE));
}

/******************************************************************************
*
* sort range tree and build range lookup indexes
*
******************************************************************************/

PROC_BSEARCH_PROTO(extern, compare_range_index, RANGE_INDEX, RANGE_INDEX)
{
    BSEARCH_KEY_VAR_DECL(PC_RANGE_INDEX, key);
    BSEARCH_DATUM_VAR_DECL(PC_RANGE_INDEX, datum);

    if(*key > *datum)
        return(1);

    if(*key < *datum)
        return(-1);

    return(0);
}

#if TRACE_ALLOWED

static void
range_lookup_tables_size(
    P_SS_DOC p_ss_doc,
    _OutRef_    P_U32 p_col_size,
    _OutRef_    P_U32 p_row_size)
{
    ARRAY_INDEX n;
    ARRAY_INDEX i;
    P_ARRAY_HANDLE p_array_handle;

    *p_col_size = array_size32(&p_ss_doc->h_range_cols) * sizeof32(ARRAY_HANDLE);

    for(i = 0, n = array_elements(&p_ss_doc->h_range_cols), p_array_handle = array_range(&p_ss_doc->h_range_cols, ARRAY_HANDLE, 0, n);
        i < n;
        ++i, ++p_array_handle)
    {
        *p_col_size += array_size32(p_array_handle) * sizeof32(RANGE_INDEX);
    }

    *p_row_size = array_size32(&p_ss_doc->h_range_rows) * sizeof32(ARRAY_HANDLE);

    for(i = 0, n = array_elements(&p_ss_doc->h_range_rows), p_array_handle = array_range(&p_ss_doc->h_range_rows, ARRAY_HANDLE, 0, n);
        i < n;
        ++i, ++p_array_handle)
    {
        *p_row_size += array_size32(p_array_handle) * sizeof32(RANGE_INDEX);
    }
}

#endif

/*
 * clear existing range tables
 */

static void
range_lookup_tables_dispose(
    P_SS_DOC p_ss_doc)
{
    ARRAY_INDEX n;
    ARRAY_INDEX i;
    P_ARRAY_HANDLE p_array_handle;

    for(i = 0, n = array_elements(&p_ss_doc->h_range_cols), p_array_handle = array_range(&p_ss_doc->h_range_cols, ARRAY_HANDLE, 0, n);
        i < n;
        ++i, ++p_array_handle)
    {
        al_array_dispose(p_array_handle);
    }
    al_array_dispose(&p_ss_doc->h_range_cols);

    for(i = 0, n = array_elements(&p_ss_doc->h_range_rows), p_array_handle = array_range(&p_ss_doc->h_range_rows, ARRAY_HANDLE, 0, n);
        i < n;
        ++i, ++p_array_handle)
    {
        al_array_dispose(p_array_handle);
    }
    al_array_dispose(&p_ss_doc->h_range_rows);
}

static void
tree_sort_ranges(
    _InVal_     EV_DOCNO ev_docno,
    P_SS_DOC p_ss_doc)
{
    if(!global_flags.lock)
    {
        EV_COL numcol = ev_numcol(ev_docno);
        EV_ROW numrow = ev_numrow(ev_docno);

        if(tree_sort(&p_ss_doc->range_table, deleted_range, NULL)
           ||
           numcol != array_elements(&p_ss_doc->h_range_cols)
           ||
           numrow != array_elements(&p_ss_doc->h_range_rows))
        {
            STATUS status = STATUS_OK;

            global_flags.blown = 1;

            range_lookup_tables_dispose(p_ss_doc);

            /* allocate new range tables */
            if(array_elements(&p_ss_doc->range_table.h_table))
            {
                SC_ARRAY_INIT_BLOCK array_init_block = aib_init(1, sizeof32(ARRAY_HANDLE), TRUE);
                consume_ptr(al_array_alloc_ARRAY_HANDLE(&p_ss_doc->h_range_cols, numcol, &array_init_block, &status));
                consume_ptr(al_array_alloc_ARRAY_HANDLE(&p_ss_doc->h_range_rows, numrow, &array_init_block, &status));
                status_consume(status);
            }

            if(p_ss_doc->h_range_cols && p_ss_doc->h_range_rows)
            {
                const ARRAY_INDEX n_range_use = array_elements(&p_ss_doc->range_table.h_table);
                ARRAY_INDEX range_ix;
                P_RANGE_USE p_range_use;
                SC_ARRAY_INIT_BLOCK array_init_block = aib_init(10, sizeof32(RANGE_INDEX), FALSE);

                assert(n_range_use <= U16_MAX);

                /* look thru range dependencies */
                for(range_ix = 0, p_range_use = p_range_use_from_p_ss_doc(p_ss_doc, range_ix);
                    range_ix < n_range_use && status_ok(status);
                    ++range_ix, ++p_range_use)
                {
                    if(status_ok(status))
                    {
                        EV_COL col_stop = ev_slr_col(&p_range_use->range_to.e);
                        EV_COL ev_col;

                        if(col_stop > numcol) col_stop = numcol;

                        for(ev_col = ev_slr_col(&p_range_use->range_to.s); ev_col < col_stop; ++ev_col)
                        {
                            P_RANGE_INDEX p_range_index;

                            if(NULL == (p_range_index = al_array_extend_by(array_ptr(&p_ss_doc->h_range_cols, ARRAY_HANDLE, ev_col), RANGE_INDEX, 1, &array_init_block, &status)))
                                break;

                            *p_range_index = (RANGE_INDEX) range_ix;
                        }
                    }

                    if(status_ok(status))
                    {
                        const EV_ROW row_stop = MIN(ev_slr_row(&p_range_use->range_to.e), numrow);
                        EV_ROW ev_row;

                        for(ev_row = ev_slr_row(&p_range_use->range_to.s); ev_row < row_stop; ++ev_row)
                        {
                            P_RANGE_INDEX p_range_index;

                            if(NULL == (p_range_index = al_array_extend_by(array_ptr(&p_ss_doc->h_range_rows, ARRAY_HANDLE, ev_row), RANGE_INDEX, 1, &array_init_block, &status)))
                                break;

                            *p_range_index = (RANGE_INDEX) range_ix;
                        }
                    }
                }

                /* sort range indexes */
                if(status_ok(status))
                {
                    ARRAY_INDEX n = array_elements(&p_ss_doc->h_range_cols);
                    ARRAY_INDEX i;
                    P_ARRAY_HANDLE p_array_handle = array_range(&p_ss_doc->h_range_cols, ARRAY_HANDLE, 0, n);

                    for(i = 0; i < n; ++i, ++p_array_handle)
                    {
                        if(*p_array_handle)
                            al_array_qsort(p_array_handle, compare_range_index);
                    }
 
                    n = array_elements(&p_ss_doc->h_range_rows);
                    p_array_handle = array_range(&p_ss_doc->h_range_rows, ARRAY_HANDLE, 0, n);
                    for(i = 0; i < n; ++i, ++p_array_handle)
                    {
                        if(*p_array_handle)
                            al_array_qsort(p_array_handle, compare_range_index);
                    }
                }
                else
                    range_lookup_tables_dispose(p_ss_doc);
            }
        }

#if TRACE_ALLOWED
        if_constant(tracing(TRACE_APP_MEMORY_USE))
        {
            U32 col_size, row_size;
            trace_1(TRACE_APP_MEMORY_USE,
                    TEXT("range use table is: ") U32_TFMT TEXT(" bytes"),
                    array_size32(&p_ss_doc->range_table.h_table) * sizeof32(RANGE_USE));
            range_lookup_tables_size(p_ss_doc, &col_size, &row_size);
            trace_1(TRACE_APP_MEMORY_USE, TEXT("range col table is: ") U32_TFMT TEXT(" bytes"), col_size);
            trace_1(TRACE_APP_MEMORY_USE, TEXT("range row table is: ") U32_TFMT TEXT(" bytes"), row_size);
        }
#endif
    }

    /* clear out node distance checks */
    if(p_ss_doc && global_flags.blown)
    {
        const ARRAY_INDEX n_ranges = array_elements(&p_ss_doc->range_table.h_table);
        ARRAY_INDEX range_ix;
        P_RANGE_USE p_range_use = array_range(&p_ss_doc->range_table.h_table, RANGE_USE, 0, n_ranges);

        /* look thru range dependencies */
        for(range_ix = 0; range_ix < n_ranges; ++range_ix, ++p_range_use)
        {
            p_range_use->node_distance = -1;
        }
    }
}

/******************************************************************************
*
* sort an slr tree
*
******************************************************************************/

static void
tree_sort_slrs(
    P_SS_DOC p_ss_doc)
{
    if(!global_flags.lock)
    {
        if(tree_sort(&p_ss_doc->slr_table, deleted_slr, compare_slr))
            global_flags.blown = 1;

        trace_1(TRACE_APP_MEMORY_USE,
                TEXT("slr use table is: ") U32_TFMT TEXT(" bytes"),
                array_size32(&p_ss_doc->slr_table.h_table) * sizeof32(SLR_USE));
    }
}

/* end of ev_tree.c */
