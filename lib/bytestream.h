#ifndef BYTESTREAM_H
#define BYTESTREAM_H

#include <stdint.h>
#include "framework.h"

typedef struct ByteStream
{
public:
    ByteStream(int (*put)(uint8_t, ByteStream *), int (*get)(ByteStream *, bool last), int (*len)(ByteStream *));

    inline int length()
    {
        return len(this);
    }

    inline int position()
    {
        return pos;
    }

    inline int read()
    {
        pos++;
        return get(this, true);
    }

    inline int read(uint8_t *buf, int offset, int count)
    {
        return read(buf, offset, count, false);
    }

    inline int read(uint8_t *buf, int offset, int count, bool leaveOpen)
    {
        int c = 0;
        for (int i = 0; i < count; i++)
        {
            int d = get(this, !leaveOpen && i == count - 1);
            if (d >= 0)
            {
                buf[i + offset] = d;
                c++;
            }
            else
            {
                break;
            }
        }
        pos += c;
        return c;
    }

    inline int write(uint8_t data)
    {
        pos++;
        return put(data, this);
    }

    inline int write(uint8_t *data, int offset, int count)
    {
        printf("write %p\n", put);
        put(0xA4, nullptr);
        int c = 0;
        for (int i = 0; i < count; i++)
        {
            int d = put(data[i + offset], this);
            printf("i: %i\n", i);
            if (d >= 0)
            {
                c++;
            }
            else
            {
                break;
            }
        }
        printf("done %i\n", c);
        pos += c;
        return c;
    }

private:
    int pos;                             /* bytes read or written so far */
    int (*put)(uint8_t, ByteStream *);   /* function to write one byte to stream */
    int (*get)(ByteStream *, bool last); /* function to read one byte from stream */
    int (*len)(ByteStream *);            /* function to get length of stream */
} ByteStream;

#endif