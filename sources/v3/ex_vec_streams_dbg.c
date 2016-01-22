/* file ex_vec_streams.mc  */
/* version carte54 5.4-201603.6 */
#include <omp.h>



















#include <user_debug.h>


 
void subr ( int64_t A [ ] , int64_t B [ ] , int64_t Out [ ] , int32_t Counts [ ] , int nvec , int nspin , 
           int * nout , int64_t * time , int mapnum  ){
    cg_accum_add_32_Struct cg_accum_add_32_00002;
    cg_accum_add_32_Struct cg_accum_add_32_00003;
    cg_accum_add_32_Struct cg_accum_add_32_00004;
    cg_accum_add_32_Struct cg_accum_add_32_00005;

    OBM_BANK_A (AL,      int64_t, MAX_OBM_SIZE, mapnum)
    OBM_BANK_B (BL,      int64_t, MAX_OBM_SIZE, mapnum)
    OBM_BANK_C (CountsL, int64_t, MAX_OBM_SIZE, mapnum)

    int64_t t0 , t1 , t2 ;
    int i , n , total_nsamp , istart , cnt ;
    int total_nsampA ;
    int total_nsampB ;
    int nputa , nputb ;
 
    Stream_64 SB , SA , SC , SOut ;
    Stream_32 SAC , SBC ;
    Vec_Stream_64 VSA,VSB;
    Vec_Stream_256 VSA_op,VSB_op,VSM;

    read_timer__dbg ( & t0 ) ;

    nputa = 0 ;
    nputb = 0 ;

	reset_stream_64_dbg(&SC);
	SC.name = "SC";
	omp_set_num_threads(16);
#pragma omp parallel sections
{
#pragma omp section
{
    streamed_dma_cpu_64_in__dbg (&SC, PORT_TO_STREAM, Counts, nvec*sizeof(int64_t), mapnum);
}
#pragma omp section
{
    int i , cnta , cntb , na , nb ;
    int64_t i64 ;

    for ( i = 0 ; i < nvec ; i ++ ) {
       get_stream_64__dbg ( & SC , & i64 ) ;
       CountsL [ i ] = i64 ;
       split_64to32__dbg ( i64 , & cntb , & cnta ) ;
       cg_accum_add_32__dbg ( cnta , 1 , 0 , i == 0 , & total_nsampA , &cg_accum_add_32_00002) ;
       cg_accum_add_32__dbg ( cntb , 1 , 0 , i == 0 , & total_nsampB , &cg_accum_add_32_00003) ;

       na = ( cnta + 3 ) / 4 ; nb = ( cntb + 3 ) / 4 ;
       cg_accum_add_32__dbg ( na , 1 , 0 , i == 0 , & nputa , &cg_accum_add_32_00004) ;
       cg_accum_add_32__dbg ( nb , 1 , 0 , i == 0 , & nputb , &cg_accum_add_32_00005) ;
    }
 
 printf ( "nsampA %i\n" , total_nsampA ) ;
 printf ( "nsampB %i\n" , total_nsampB ) ;
 total_nsamp = total_nsampA + total_nsampB ;
 printf ( "total %i\n" , total_nsamp ) ;
}
}

	reset_stream_64_dbg(&SA);
	SA.name = "SA";
	omp_set_num_threads(16);
#pragma omp parallel sections
{
#pragma omp section
{
    streamed_dma_cpu_64_in__dbg (&SA, PORT_TO_STREAM, A, total_nsampA*sizeof(int64_t), mapnum);
}
#pragma omp section
{
    int i ;
    int64_t i64 ;

    for ( i = 0 ; i < total_nsampA ; i ++ ) {
       get_stream_64__dbg ( & SA , & i64 ) ;
       AL [ i ] = i64 ;
    }
}
}

	reset_stream_64_dbg(&SB);
	SB.name = "SB";
	omp_set_num_threads(16);
#pragma omp parallel sections
{
#pragma omp section
{
    streamed_dma_cpu_64_in__dbg (&SB, PORT_TO_STREAM, B, total_nsampB*sizeof(int64_t), mapnum);
}
#pragma omp section
{
    int i ;
    int64_t i64 ;

    for ( i = 0 ; i < total_nsampB ; i ++ ) {
       get_stream_64__dbg ( & SB , & i64 ) ;
       BL [ i ] = i64 ;
    }
}
}

	reset_stream_32_dbg(&SAC);
	SAC.name = "SAC";
	reset_stream_32_dbg(&SBC);
	SBC.name = "SBC";
	reset_vec_stream_64_dbg(&VSA);
	VSA.name = "VSA";
	reset_vec_stream_64_dbg(&VSB);
	VSB.name = "VSB";
	reset_vec_stream_256_dbg(&VSA_op);
	VSA_op.name = "VSA_op";
	reset_vec_stream_256_dbg(&VSB_op);
	VSB_op.name = "VSB_op";
	reset_vec_stream_256_dbg(&VSM);
	VSM.name = "VSM";
	reset_stream_64_dbg(&SOut);
	SOut.name = "SOut";
	omp_set_num_threads(16);
#pragma omp parallel sections
{
#pragma omp section
{
    int n , i , cnta , cntb ;
    int64_t i64 ;

    for ( n = 0 ; n < nvec ; n ++ ) {
      i64 = CountsL [ n ] ;
      split_64to32__dbg ( i64 , & cntb , & cnta ) ;

      put_stream_32__dbg ( & SAC , cnta , 1 , mapnum) ;
      put_stream_32__dbg ( & SBC , cntb , 1 , mapnum) ;
   }
}
 
#pragma omp section
{
    int n , nn , i , cnt , istart ;
    int64_t i64 ;

    istart = 0 ;
    for ( n = 0 ; n < nvec ; n ++ ) {
      get_stream_32__dbg ( & SAC , & cnt ) ;

   if ( n == 0 ) spin_wait__dbg ( nspin ) ;

      nn = n + 1 ;
      comb_32to64__dbg ( nn , cnt , & i64 ) ;
      put_vec_stream_64_header__dbg ( & VSA , i64 , mapnum) ;

      for ( i = 0 ; i < cnt ; i ++ ) {
        put_vec_stream_64__dbg ( & VSA , AL [ i + istart ] , 1 , mapnum) ;
      }
      istart = istart + cnt ;

      put_vec_stream_64_tail__dbg ( & VSA , 1234 , mapnum) ;
    }
    vec_stream_64_term__dbg ( & VSA ) ;
}
#pragma omp section
{
    int n , nn , i , cnt , istart ;
    int64_t i64 ;

    istart = 0 ;
    for ( n = 0 ; n < nvec ; n ++ ) {
      get_stream_32__dbg ( & SBC , & cnt ) ;

   if ( n == 0 ) spin_wait__dbg ( 1 ) ;

      nn = n + 1 ;
      comb_32to64__dbg ( nn , cnt , & i64 ) ;
      put_vec_stream_64_header__dbg ( & VSB , i64 , mapnum) ;

      for ( i = 0 ; i < cnt ; i ++ ) {
        put_vec_stream_64__dbg ( & VSB , BL [ i + istart ] , 1 , mapnum) ;
      }
      istart = istart + cnt ;

      put_vec_stream_64_tail__dbg ( & VSB , 1234 , mapnum) ;
    }
    vec_stream_64_term__dbg ( & VSB ) ;
}

 
 
 
 
#pragma omp section
{
    int i , n , cnt , iput ;
    int64_t v0 , v1 , i64 ;
    int64_t t0 , t1 , t2 , t3 ;
    int64_t s0 , s1 , s2 , s3 ;

    while ( is_vec_stream_64_active__dbg ( & VSA ) ) {
      get_vec_stream_64_header__dbg ( & VSA , & i64 ) ;
      split_64to32__dbg ( i64 , & n , & cnt ) ;

 printf ( "VSA cnt %i\n" , cnt ) ;

      put_vec_stream_256_header__dbg ( & VSA_op , i64 , 0 , 0 , 0 , mapnum) ;

      for ( i = 0 ; i < cnt ; i ++ ) {
        get_vec_stream_64__dbg ( & VSA , & v0 ) ;

        v1 = v0 + n * 10000 ;
 
        s0 = s1 ;
        s1 = s2 ;
        s2 = s3 ;
        s3 = v1 ;

        iput = ( ( i + 1 ) % 4 == 0 ) ? 1 : 0 ;
        if ( i == cnt - 1 ) iput = 1 ;

        if ( iput & ( i % 4 ) == 0 ) { t0 = s3 ; t1 = 0 ; t2 = 0 ; t3 = 0 ; }
        else if ( iput & ( i % 4 ) == 1 ) { t0 = s2 ; t1 = s3 ; t2 = 0 ; t3 = 0 ; }
        else if ( iput & ( i % 4 ) == 2 ) { t0 = s1 ; t1 = s2 ; t2 = s3 ; t3 = 0 ; }
        else if ( iput & ( i % 4 ) == 3 ) { t0 = s0 ; t1 = s1 ; t2 = s2 ; t3 = s3 ; }

        put_vec_stream_256__dbg ( & VSA_op , t0 , t1 , t2 , t3 , iput , mapnum) ;
      }

      get_vec_stream_64_tail__dbg ( & VSA , & i64 ) ;
      put_vec_stream_256_tail__dbg ( & VSA_op , 0 , 0 , 0 , 0 , mapnum) ;
    }
    vec_stream_256_term__dbg ( & VSA_op ) ;
}

 
 
 
 
#pragma omp section
{
    int i , n , cnt , iput ;
    int64_t v0 , v1 , i64 ;
    int64_t t0 , t1 , t2 , t3 ;
    int64_t s0 , s1 , s2 , s3 ;

    while ( is_vec_stream_64_active__dbg ( & VSB ) ) {
      get_vec_stream_64_header__dbg ( & VSB , & i64 ) ;
      split_64to32__dbg ( i64 , & n , & cnt ) ;

 
      n = n + 1000 ;
      comb_32to64__dbg ( n , cnt , & i64 ) ;

 printf ( "VSB cnt %i\n" , cnt ) ;

      put_vec_stream_256_header__dbg ( & VSB_op , i64 , 0 , 0 , 0 , mapnum) ;

      for ( i = 0 ; i < cnt ; i ++ ) {
        get_vec_stream_64__dbg ( & VSB , & v0 ) ;

        v1 = v0 + n * 1000000 ;
 
        s0 = s1 ;
        s1 = s2 ;
        s2 = s3 ;
        s3 = v1 ;

 
        iput = ( ( i + 1 ) % 4 == 0 ) ? 1 : 0 ;
        if ( i == cnt - 1 ) iput = 1 ;

        if ( iput & ( i % 4 ) == 0 ) { t0 = s3 ; t1 = 0 ; t2 = 0 ; t3 = 0 ; }
        else if ( iput & ( i % 4 ) == 1 ) { t0 = s2 ; t1 = s3 ; t2 = 0 ; t3 = 0 ; }
        else if ( iput & ( i % 4 ) == 2 ) { t0 = s1 ; t1 = s2 ; t2 = s3 ; t3 = 0 ; }
        else if ( iput & ( i % 4 ) == 3 ) { t0 = s0 ; t1 = s1 ; t2 = s2 ; t3 = s3 ; }

        put_vec_stream_256__dbg ( & VSB_op , t0 , t1 , t2 , t3 , iput , mapnum) ;
      }

      get_vec_stream_64_tail__dbg ( & VSB , & i64 ) ;
      put_vec_stream_256_tail__dbg ( & VSB_op , 0 , 0 , 0 , 0 , mapnum) ;
    }
    vec_stream_256_term__dbg ( & VSB_op ) ;
}

 
 
 
#pragma omp section
{


 printf ( "debug yes\n" ) ;
    vec_stream_merge_2_256_term__dbg ( & VSA_op , & VSB_op , & VSM , mapnum) ;




}

#pragma omp section
{
    int i , n , cnt ;
    int64_t h0 , h1 , h2 , h3 ;
    int64_t v0 , v1 , v2 , v3 ;

    istart = 0 ;
    while ( is_vec_stream_256_active__dbg ( & VSM ) ) {
      get_vec_stream_256_header__dbg ( & VSM , & h0 , & h1 , & h2 , & h3 ) ;
      split_64to32__dbg ( h0 , & n , & cnt ) ;
 
 printf ( "get VSM n %i cnt %i\n" , n , cnt ) ;

 
 
 
 
 
 
      cnt = ( cnt + 3 ) / 4 ;
      for ( i = 0 ; i < cnt ; i ++ ) {
        get_vec_stream_256__dbg ( & VSM , & v0 , & v1 , & v2 , & v3 ) ;

        put_stream_64__dbg ( & SOut , v0 , 1 , mapnum) ;
 printf ( "VSM v0 %lld\n" , v0 ) ;
      }

      get_vec_stream_256_tail__dbg ( & VSM , & h0 , & h1 , & h2 , & h3 ) ;

    }
}
#pragma omp section
{
    * nout = nputa + nputb ;
    streamed_dma_cpu_64_out__dbg (&SOut, STREAM_TO_PORT, Out, (nputa+nputb)*sizeof(int64_t), mapnum);
}
}
    read_timer__dbg ( & t1 ) ;
    * time = t1 - t0 ;
    }
