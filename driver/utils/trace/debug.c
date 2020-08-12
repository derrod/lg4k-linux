/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * debug.c
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
 
#include "cxt_mgr.h"
#include "trace.h"
#include "debug.h"
#include "linux/kernel.h"
#include "linux/slab.h"
#include "linux/string.h"

typedef struct
{
    trace_attr_handle_t attr_handle;
}debug_cxt_t;

typedef enum
 {
     UNKNOWN_CMD=-1,
     CMD_FILE=0,
     CMD_FUNC,
     CMD_LINE,
     CMD_FLAG,
     AVAIL_CMD_COUNT,
 }debug_ctrl_cmd_e;
 
 
 typedef struct
 {
     char *filepattern;
     char *funcpattern;
     
     int start_line;
     int end_line;
     ddebug_flags_e mask;
     ddebug_flags_e flags;
 }debug_ctrl_desc_t;
 
 static debug_cxt_t debug_cxt;
 static const char *cmd_avail[AVAIL_CMD_COUNT]=
 {
     [CMD_FILE]="file",
     [CMD_FUNC]="func",
     [CMD_LINE]="line",
     [CMD_FLAG]="flag",
 };
 
 static debug_ctrl_cmd_e match_cmd(const char *str);
 static char *tokenize(char *str,const char *delim,int *max_count,char **tokens);
 static void exec_debug_ctrl_desc(debug_ctrl_desc_t *desc);
 
 static debug_ctrl_cmd_e match_cmd(const char *str)
 {
     int i;
     int ret=UNKNOWN_CMD;
     for(i=0;i<AVAIL_CMD_COUNT;i++)
     {
         if(strcmp(str,cmd_avail[i])==0)
         {
             ret=i;
             break;
         }
     }
         
    return ret;
 }

static ddebug_t ddebug_metadatas[] __attribute__((section("ddebug")))=
{ 
};

static char *tokenize(char *str,const char *delim,int *max_count,char **tokens)
{
    int i;
    char *token=NULL;
    char *substr=str;
    int count=0;
    
    for(i=0,count=0;i<*max_count;i++)
    {
        
        token=strsep(&substr,delim);
        tokens[i]=token;
        count++;
        if(substr)
        {
            substr=skip_spaces(substr);
        }else
        {
            *max_count=count;
            break;
        }
            
    }
    
    return substr;
}



static void exec_debug_ctrl_desc(debug_ctrl_desc_t *desc)
{
    ddebug_t *pddebug_data;
    
    if(!desc)
        return;
    
//    if(desc->filepattern)
//        mesg("file \"%s\" %d\n",desc->filepattern,strlen(desc->filepattern));
//    if(desc->funcpattern)
//        mesg("func %s %d\n",desc->funcpattern,strlen(desc->funcpattern));
//    if(desc->start_line>0)
//        mesg("line range %d ~ %d\n",desc->start_line,desc->end_line);
//    mesg("mask %x flags %x",desc->mask,desc->flags);
    for(pddebug_data=ddebug_metadatas;pddebug_data->filename;pddebug_data++)
    {
        
        if(desc->filepattern)
        {
            char *filematch=NULL;
            
            filematch=strstr(pddebug_data->filename,desc->filepattern);
            if(!filematch)
            {
               continue;
            }
        }
        if(desc->funcpattern)
        {
            char *funcmatch=NULL;
            funcmatch=strstr(pddebug_data->function,desc->funcpattern);
            if(!funcmatch)
            {
               continue;
            }
        }
        if(desc->start_line>0)
        {
            if(pddebug_data->lineno<desc->start_line || pddebug_data->lineno > desc->end_line)
            {
                continue;
            }
        }
        pddebug_data->flags &= ~desc->mask;
        pddebug_data->flags |= desc->flags;
        if(pddebug_data->flags & DDEBUG_FLAG_SHOW)
        {
            mesg("*filename %s  func %s lineno %d flags %x\n",pddebug_data->filename,pddebug_data->function,pddebug_data->lineno,pddebug_data->flags);
        }
        
        
    }
    
}


static unsigned int debug_ctl_store(void *cxt, const char *buf, unsigned int count)
{
    int cnt=count;
    char *dupstr=kstrndup(buf,count,GFP_KERNEL);
    char *sepstr=dupstr;
    char *cmdline=NULL;
    const char *cmdline_delim=";\n";
    
    
    if(dupstr)
    {
        do
        {
            
            cmdline=strsep(&sepstr,cmdline_delim);
            if(cmdline)
            {
                const char *cmd_delim=" ";
                debug_ctrl_cmd_e cmd_index;
                char *tokens[2]={0};
                int token_count;
                debug_ctrl_desc_t debug_ctrl={0};
                BOOL_T valid=FALSE;
                
                
                do
                {
                    token_count=sizeof(tokens)/sizeof(char *);
                    cmdline=tokenize(cmdline,cmd_delim,&token_count,&tokens[0]);
                    cmd_index=match_cmd(tokens[0]);
                    switch(cmd_index)
                    {
                    case CMD_FILE:
                        if(token_count==2)
                        {
                            valid=TRUE;
                            debug_ctrl.filepattern=tokens[1];
                        }
                        break;
                    case CMD_FUNC:
                        if(token_count==2)
                        {
                            valid=TRUE;
                            debug_ctrl.funcpattern=tokens[1];
                        }
                        break;
                    case CMD_LINE:
                        if(token_count==2)
                        {
                            int lineno;
                            char *start=tokens[1];
                            char *end=NULL;
                            valid=TRUE;
                            if((end=strchr(tokens[1],'-'))!=NULL)
                            {
                                *end=0;
                                end++;
                                if(start[0])
                                    kstrtoint(start,10,&lineno);
                                else
                                    lineno=1;
                                debug_ctrl.start_line=lineno;
                                kstrtoint(end,10,&lineno);
                                debug_ctrl.end_line=lineno;
                            }else
                            {
                                kstrtoint(start,10,&lineno);
                                debug_ctrl.start_line=lineno;
                                debug_ctrl.end_line=lineno;
                            }
                        }
                        break;
                    case CMD_FLAG:
                        if(token_count==2)
                        {
                            char *flags=tokens[1];
                            BOOL_T on=TRUE;
                            int i;
                            
                            for(i=0;i<strlen(flags);i++)
                            {
                                switch(flags[i])
                                {
                                case '+':
                                    on=TRUE;
                                    break;
                                case '-':
                                    on=FALSE;
                                    break;
                                case 'f':
                                    debug_ctrl.mask |=DDEBUG_FLAG_INC_FUNC;
                                    if(on)
                                        debug_ctrl.flags |=DDEBUG_FLAG_INC_FUNC;
                                    break;
                                case 'F':
                                    debug_ctrl.mask |=DDEBUG_FLAG_INC_FILE;
                                    if(on)
                                        debug_ctrl.flags |=DDEBUG_FLAG_INC_FILE;
                                    break;
                                case 'l':
                                    debug_ctrl.mask |=DDEBUG_FLAG_INC_LINENO;
                                    if(on)
                                        debug_ctrl.flags |=DDEBUG_FLAG_INC_LINENO;
                                    break;
                                case 'p':
                                    debug_ctrl.mask |=DDEBUG_FLAG_SHOW;
                                    if(on)
                                        debug_ctrl.flags |=DDEBUG_FLAG_SHOW;
                                    break;
                                default:
                                    break;
                                }   
                            }
                            
                        }    
                        break;
                    
                    default:
                        break;
                    }
                }while(cmdline); 
                if(valid)
                {
                    exec_debug_ctrl_desc(&debug_ctrl);
                }
            }
        }while(sepstr);
    
         kfree(dupstr);
    }
    return cnt;
}

                


void init_debug(handle_t trace_handle)
{
    trace_attr_setup_t setup;
    trace_attr_handle_t attr_handle;
    
    setup.name = "debug_ctrl";
    setup.store = debug_ctl_store;
    setup.cxt = NULL;
    attr_handle=trace_attr_create(&setup);
    if(attr_handle)
    {
        debug_cxt.attr_handle=attr_handle;
	trace_model_add_attr(trace_handle,attr_handle);
    }
}

void uninit_debug()
{
    if(debug_cxt.attr_handle)
        trace_attr_free(debug_cxt.attr_handle);
}

int mesg(const char *fmt,...)
{
    va_list args;
    int r;
    
    va_start(args, fmt);

    r=vprintk(fmt,args);
    va_end(args);
    
    return r;
}

