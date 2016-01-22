/* $Id: ex05.mc,v 2.1 2005/06/14 22:16:47 jls Exp $ */

/*
 * Copyright 2005 SRC Computers, Inc.  All Rights Reserved.
 *
 *	Manufactured in the United States of America.
 *
 * SRC Computers, Inc.
 * 4240 N Nevada Avenue
 * Colorado Springs, CO 80907
 * (v) (719) 262-0213
 * (f) (719) 262-0223
 *
 * No permission has been granted to distribute this software
 * without the express permission of SRC Computers, Inc.
 *
 * This program is distributed WITHOUT ANY WARRANTY OF ANY KIND.
 */

#include <libmap.h>


void subr (int64_t A[], int64_t B[], int64_t Out[], int32_t Counts[], int nvec, int nspin, int64_t *time, int mapnum) {

    OBM_BANK_A (AL,      int64_t, MAX_OBM_SIZE)
    OBM_BANK_B (BL,      int64_t, MAX_OBM_SIZE)
    OBM_BANK_C (CountsL, int64_t, MAX_OBM_SIZE)

    int64_t t0, t1, t2;
    int i,n,total_nsamp,istart,cnt;
    int total_nsampA;
    int total_nsampB;
    
    Stream_64 SB,SA,SC,SOut;
    Stream_32 SAC,SBC;
    Vec_Stream_64 VSA,VSB,VSM;
    Vec_Stream_64 VSA_op,VSB_op;

    read_timer (&t0);

#pragma src parallel sections
{
#pragma src section
{
    streamed_dma_cpu_64 (&SC, PORT_TO_STREAM, Counts, nvec*sizeof(int64_t));
}
#pragma src section
{
    int i,cnta,cntb;
    int64_t i64;

    for (i=0;i<nvec;i++)  {
       get_stream_64 (&SC, &i64);
       CountsL[i] = i64;
       split_64to32 (i64, &cntb, &cnta);
       cg_accum_add_32 (cnta, 1, 0, i==0, &total_nsampA);
       cg_accum_add_32 (cntb, 1, 0, i==0, &total_nsampB);
    }
 
 printf ("   nsampA %i\n",total_nsampA);
 printf ("   nsampB %i\n",total_nsampB);
 total_nsamp = total_nsampA + total_nsampB;
 printf ("   total %i\n",total_nsamp);
}
}

#pragma src parallel sections
{
#pragma src section
{
    streamed_dma_cpu_64 (&SA, PORT_TO_STREAM, A, total_nsampA*sizeof(int64_t));
}
#pragma src section
{
    int i;
    int64_t i64;

    for (i=0;i<total_nsampA;i++)  {
       get_stream_64 (&SA, &i64);
       AL[i] = i64;
    }
}
}

#pragma src parallel sections
{
#pragma src section
{
    streamed_dma_cpu_64 (&SB, PORT_TO_STREAM, B, total_nsampB*sizeof(int64_t));
}
#pragma src section
{
    int i;
    int64_t i64;

    for (i=0;i<total_nsampB;i++)  {
       get_stream_64 (&SB, &i64);
       BL[i] = i64;
    }
}
}

#pragma src parallel sections
{
#pragma src section
{
    int n,i,cnta,cntb;
    int64_t i64;

    for (n=0;n<nvec;n++)  {
      i64 = CountsL[n];
      split_64to32 (i64, &cntb, &cnta);

      put_stream_32 (&SAC, cnta, 1);
      put_stream_32 (&SBC, cntb, 1);
   }
}
    

// **************************************************
// add code here
//  Create vector stream VSA
//  Create vector stream VSB
//  Merge output of computations on VSA and VSB
//  Output of body of merge vector (same as v1_solution
// **************************************************

#pragma src section
{
    streamed_dma_cpu_64 (&SOut, STREAM_TO_PORT, Out, total_nsamp*sizeof(int64_t));
}
}
    read_timer (&t1);
    *time = t1 - t0;
    }
