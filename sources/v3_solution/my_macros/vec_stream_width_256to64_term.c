#include <user_debug.h>
#include <ctype.h>

extern MAP_Trace_Control_t *MAP_Trace;

void vec_stream_width_256to64_term__dbg (Vec_Stream_256 *S0, Vec_Stream_64 *S1, int mapnum) {
    int64_t data0;
    int64_t data1;
    int64_t data2;
    int64_t data3;
    Vec_Stream_256 *p0 = S0;

    while (active_vec_stream(S0)) {
        get_vec_stream_256_header__dbg (S0, &data0, &data1, &data2, &data3);
        put_vec_stream_64_header__dbg  (S1, data0, mapnum);

 printf ("vec_width af get head d0-3 %llx %llx %llx %llx\n",data0,data1,data2,data3);

        while (!is_vec_done(p0)) {
           get_vec_stream_256__dbg (S0, &data0, &data1, &data2, &data3);
 printf ("vec_width af get body d0-3 %lld %lld %lld %lld\n",data0,data1,data2,data3);

           put_vec_stream_64__dbg (S1, data0, 1, mapnum);
           put_vec_stream_64__dbg (S1, data1, 1, mapnum);
           put_vec_stream_64__dbg (S1, data2, 1, mapnum);
           put_vec_stream_64__dbg (S1, data3, 1, mapnum);
        }

        get_vec_stream_256_tail__dbg (S0, &data0, &data1, &data2, &data3);
        put_vec_stream_64_tail__dbg  (S1, data0, mapnum);
 printf ("vec_width af get tail d0-3 %lld %lld %lld %lld\n",data0,data1,data2,data3);
        }

    vec_stream_64_term__dbg (S1);
    }
