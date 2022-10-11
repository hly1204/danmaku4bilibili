#include "zlibhelper.h"
#include <QDebug>
#include <QtGlobal>
#include <zlib.h>

QByteArray zlib_compress(QByteArrayView source, bool *ok)
{
    const ulong n     = source.size();
    ulong       bound = compressBound(n);
    QByteArray  res(bound, '\0');
    if (::compress(reinterpret_cast<uchar *>(res.data()), &bound, reinterpret_cast<const uchar *>(source.constData()), n) != Z_OK) {
        if (ok != nullptr) *ok = false;
        qWarning() << __func__ << "ERROR";
        return QByteArray();
    }
    if (ok != nullptr) *ok = true;
    res.resize(bound);
    return res;
}

QByteArray zlib_uncompress(QByteArrayView source, bool *ok)
{
    const ulong n     = source.size();
    ulong       bound = 1;
    while (bound < n) bound <<= 1;
    QByteArray res;
    for (;;) {
        res.resize(bound);
        switch (::uncompress(reinterpret_cast<uchar *>(res.data()), &bound, reinterpret_cast<const uchar *>(source.constData()), n)) {
        case Z_OK:
            res.resize(bound);
            if (ok != nullptr) *ok = true;
            return res;
        case Z_DATA_ERROR: [[fallthrough]];
        case Z_MEM_ERROR:
            if (ok != nullptr) *ok = false;
            qWarning() << __func__ << "ERROR";
            return QByteArray();
        case Z_BUF_ERROR: bound <<= 1; continue;
        }
    }
}
