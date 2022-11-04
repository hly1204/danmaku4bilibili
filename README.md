# 弹幕机(bilibili 版)

测试中...

## 构建

构建环境 Windows 10, Visual Studio 2019, CMake, Qt6.4.

```sh
cmake -DQt6_DIR="path/to/Qt6_DIR" -B build .
cmake --build build --config Release
```

## 许可证

### Qt6

[LGPLv3](https://doc.qt.io/qt-6/lgpl.html).

## 参考资料

- [获取bilibili直播弹幕的WebSocket协议 - 炒鸡嗨客协管徐](https://blog.csdn.net/xfgryujk/article/details/80306776).
- [通过WebSocket获取bilibili直播弹幕 - yy祝](https://blog.csdn.net/yyznm/article/details/116543107).
