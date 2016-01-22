void vec_stream_width_256to64_term__dbg (Vec_Stream_256 *S0, Vec_Stream_64 *S1, int mapnum) {

    int64_t data0;
    int64_t data1;
    int64_t data2;
    int64_t data3;

    while (is_vec_stream_256_active(S0)) {
        get_vec_stream_256_header__dbg (S0, &data0, &data1, &data2, &data3);
        put_vec_stream_64_header__dbg  (S1, data0);

        while (all_vec_streams_active()) {
           get_vec_stream_256__dbg (S0, &data0, &data1, &data2, &data3);

           put_vec_stream_64__dbg (S1, data0, 1, mapnum);
           put_vec_stream_64__dbg (S1, data1, 1, mapnum);
           put_vec_stream_64__dbg (S1, data2, 1, mapnum);
           put_vec_stream_64__dbg (S1, data3, 1, mapnum);
        }

        get_vec_stream_256_tail__dbg (S0, &data0, &data1, &data2, &data3);
        put_vec_stream_64_tail__dbg  (S1, data0);
        }

    stream_term_64__dbg (S1);
    }
