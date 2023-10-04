# 弹幕机(bilibili 版)

未经许可的客户端接口已经不能用啦, 所以不会再更新了. 仅供参考吧.

## 构建

构建环境 Windows 10, Visual Studio 2019/2022, CMake, Qt6.4.

配置参考 [configure.bat](./configure.bat).

使用 `-T v142` 指定使用工具集版本, 若使用 Visual Studio 16 2019 则自动选择 v142 工具集, 否则需要单独安装, 具体见 Qt 的二进制对应的工具集版本.

构建:

```sh
cmake --build build --config Release
```

## 许可证

### Qt6

[GPLv3](https://doc.qt.io/qt-6/gpl.html).

## 参考资料

1. [Bilibili-Live-API - lovelyyoshino (GitHub)](https://github.com/lovelyyoshino/Bilibili-Live-API).
2. [获取bilibili直播弹幕的WebSocket协议 - 炒鸡嗨客协管徐](https://blog.csdn.net/xfgryujk/article/details/80306776).
3. [通过WebSocket获取bilibili直播弹幕 - yy祝](https://blog.csdn.net/yyznm/article/details/116543107).
