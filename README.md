# 弹幕机(bilibili 版)

测试中...

## 构建

构建环境 Windows 10, Visual Studio 2019/2022, CMake, Qt6.4.

配置:

```sh
cmake -DQt6_DIR="C:/Qt/6.4.0/msvc2019_64/lib/cmake/Qt6" -T v142,host=x64 -G "Visual Studio 17 2022" -A x64 -B build .
```

使用 `-T v142` 指定使用工具集版本, 若使用 Visual Studio 16 2019 则自动选择 v142 工具集, 否则需要单独安装, 具体见 Qt 的二进制对应的工具集版本.

构建:

```sh
cmake --build build --config Release
```

## 许可证

### Qt6

[LGPLv3](https://doc.qt.io/qt-6/lgpl.html).

## 参考资料

- [获取bilibili直播弹幕的WebSocket协议 - 炒鸡嗨客协管徐](https://blog.csdn.net/xfgryujk/article/details/80306776).
- [通过WebSocket获取bilibili直播弹幕 - yy祝](https://blog.csdn.net/yyznm/article/details/116543107).
