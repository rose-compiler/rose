// RC-66:  Tristan

    typedef struct z_stream_s {

        unsigned state;

    } z_stream;

    typedef z_stream * z_streamp;

    void foo (strm, pending)

        unsigned *pending;

        z_streamp strm;

    {

        *pending = strm->state;

    }

