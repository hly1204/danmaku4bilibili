#ifndef ZLIB_HELPER_H
#define ZLIB_HELPER_H

#include <QByteArray>
#include <QByteArrayView>

/// \brief 标准 ZLIB 的压缩
/// \param[in] source 源
/// \param[out] ok 是否成功
/// \returns 压缩后的二进制
QByteArray zlib_compress(QByteArrayView source, bool *ok = nullptr);

/// \brief 标准 ZLIB 的解压缩
/// \param[in] source 源
/// \param[out] ok 是否成功
/// \returns 解压缩后的二进制
QByteArray zlib_uncompress(QByteArrayView source, bool *ok = nullptr);

#endif
