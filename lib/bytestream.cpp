#include "bytestream.h"

ByteStream::ByteStream(int (*put)(uint8_t, ByteStream *), int (*get)(ByteStream *, bool), int (*len)(ByteStream *))
{
    pos = 0;
    this->put = put;
    this->get = get;
    this->len = len;
}