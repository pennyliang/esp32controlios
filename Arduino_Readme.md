# Arduino 环境安装与烧录教程

本文档手把手带你从零搭建 Arduino 烧录环境，并把两块 ESP32 固件烧录进去。**第一次接触 Arduino / ESP32 的读者按本文一步步做即可。**

目录:

1. [安装 Arduino IDE](#1-安装-arduino-ide)
2. [安装 ESP32 开发板支持](#2-安装-esp32-开发板支持)
3. [安装 USB 串口驱动(CH340)](#3-安装-usb-串口驱动ch340)
4. [安装依赖库](#4-安装依赖库)
5. [开发板设置](#5-开发板设置)
6. [烧录 A 板与 B 板](#6-烧录-a-板与-b-板)
7. [用串口监视器查看运行状态](#7-用串口监视器查看运行状态)
8. [常见烧录问题](#8-常见烧录问题)

\---

## 1\. 安装 Arduino IDE

1. 打开官网 [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)
2. 下载 **Arduino IDE 2.x**(对应你的系统:Windows / macOS / Linux)
3. 安装并打开。首次打开会自动下载一些组件,等它完成即可。

\---

## 2\. 安装 ESP32 开发板支持

Arduino 默认不认识 ESP32,需要加装支持包。

1. 打开 **文件(File)→ 首选项(Preferences)**
2. 找到 **附加开发板管理器网址(Additional Boards Manager URLs)**,在框里填入:

```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package\_esp32\_index.json
   ```

&#x20;   也可以尝试：https://jihulab.com/esp-mirror/espressif/arduino-esp32/-/raw/gh-pages/package\_esp32\_index\_cn.json （这个是国内加速过的版本比较快)

   (如果已有别的网址,点右边小图标,一行一个加进去)

3. 点 **OK** 保存
4. 打开 **工具(Tools)→ 开发板(Board)→ 开发板管理器(Boards Manager)**
5. 搜索框输入 `esp32`,找到 **esp32 by Espressif Systems**,点 **安装(Install)**

   * 包比较大,耐心等下载完成
   * 建议装稳定版(比如 2.x 或 3.x 的正式版),避免用 alpha/beta

> 装完后,\*\*工具 → 开发板\*\* 菜单里就会出现 "ESP32 Arduino" 一大类,里面有很多板型。

\---

## 3\. 安装 USB 串口驱动(CH340)

大多数廉价 ESP32 开发板用的是 **CH340** USB 转串口芯片,电脑要认得它才能烧录。

* **Linux**:一般免驱,插上就有 `/dev/ttyUSB0`。若没有,把当前用户加入 `dialout` 组:

```bash
  sudo usermod -aG dialout $USER
  ```

  然后**重新登录**生效。

* **Windows**:多数情况会自动装。若设备管理器里 COM 口有黄色感叹号,去搜 "CH340 驱动" 下载安装(WCH 官方:wch.cn)。
* **macOS**:新系统多数自带;不行则装 CH340 macOS 驱动。

**验证是否识别到板子**:

* Linux/macOS 终端:插板子前后各跑一次 `ls /dev/tty\*`,多出来的那个(如 `/dev/ttyUSB0`、`/dev/cu.usbserial-xxxx`)就是你的板子
* Windows:设备管理器 → 端口(COM 和 LPT),看多出来的 `USB-SERIAL CH340 (COMx)`

\---

## 4\. 安装依赖库

本项目 A 板需要异步 Web 库,B 板需要修改版蓝牙库。

### 4.1 A 板依赖:异步 Web 服务器库

需要两个库(第二个是第一个的依赖):

* **ESPAsyncWebServer** — [https://github.com/ESP32Async/ESPAsyncWebServer](https://github.com/ESP32Async/ESPAsyncWebServer)
* **AsyncTCP** — [https://github.com/ESP32Async/AsyncTCP](https://github.com/ESP32Async/AsyncTCP)

安装方式二选一:

**方式一(库管理器)**:**工具 → 管理库(Manage Libraries)**,搜索 `ESPAsyncWebServer`(选 ESP32Async 维护的版本)和 `AsyncTCP`,分别安装。

**方式二(手动 .zip)**:从上面 GitHub 页面点 **Code → Download ZIP**,然后在 Arduino 里 **项目(Sketch)→ 加载库(Include Library)→ 添加 .ZIP 库**,选下载的 zip。

### 4.2 B 板依赖:修改版蓝牙库(**必须用本项目的**)

⚠️ **重点**:B 板要用本仓库 `libraries/ESP32\_BLE\_Combo\_Keyboard\_Mouse/` 目录里的**修改版库**——它在原版基础上新增了绝对定位能力。**用原版库绝对定位、点击会失效。**

安装方法:把本仓库 `libraries/ESP32\_BLE\_Combo\_Keyboard\_Mouse/` **整个文件夹**复制到你电脑的 Arduino 库目录:

|系统|Arduino 库目录|
|-|-|
|Windows|`C:\\Users\\<你的用户名>\\Documents\\Arduino\\libraries\\`|
|macOS|`\~/Documents/Arduino/libraries/`|
|Linux|`\~/Arduino/libraries/`|

> 复制后,库目录里应能看到 `ESP32\_BLE\_Combo\_Keyboard\_Mouse/`,里面有 `BleComboKeyboard.cpp/.h`、`BleComboMouse.cpp/.h` 等文件。
>
> \*\*不确定库目录在哪?\*\* 打开 \*\*文件 → 首选项\*\*,里面 "项目文件夹位置 / Sketchbook location" 那个路径下的 `libraries` 子目录就是。

### 4.3 复制库之后重启 IDE

**每次手动改动或复制库文件后,务必彻底关闭 Arduino IDE 再重新打开**,否则 IDE 可能仍在用缓存的旧库,导致"改了没生效"。

\---

## 5\. 开发板设置

打开 **工具(Tools)** 菜单,逐项设置(一般默认值就能用):

* **开发板(Board)**:ESP32 Arduino → **ESP32 Dev Module**
* **端口(Port)**:选你板子对应的串口(见第 3 步识别到的那个)
* 其余保持默认即可(CPU Frequency 240MHz、Flash 4MB 等)

> 如果编译后提示程序太大(Sketch too big),可在 \*\*工具 → Partition Scheme\*\* 选一个 App 分区更大的方案(如 "Huge APP")。本项目一般用不到。

\---

## 6\. 烧录 A 板与 B 板

> \*\*两块板轮流烧,一次只插一块。\*\* 两块同时插会分不清哪个是哪个串口。

### 6.1 烧 A 板(WiFi 网关)

1. 打开 `esp32/` 目录里的 **A 板固件**(WiFi 网关那个 `.ino`)
2. **改 WiFi 配置**(文件顶部):

```cpp
   const char\* WIFI\_SSID = "你的WiFi名称";   // ESP32 只支持 2.4G，别填 5G
   const char\* WIFI\_PASS = "你的WiFi密码";
   ```

3. 只把 **A 板**插到电脑 USB
4. **工具 → 端口**:选 A 板的串口
5. 点左上角 **✓(Verify 编译)**,等它编译。**第一次编译 ESP32+WiFi 较慢,一两分钟正常**
6. 编译通过(底部显示 "Done compiling")后,点 **→(Upload 上传)**
7. 底部出现 `Hard resetting via RTS pin...` 即烧录成功

### 6.2 烧 B 板(BLE 键鼠)

1. **拔掉 A 板**,只插 **B 板**
2. 打开 `esp32/` 目录里的 **B 板固件**(BLE 键鼠那个 `.ino`)
3. **工具 → 端口**:重新选 B 板的串口(号可能和 A 板不同)
4. 点 **✓ 编译** → **→ 上传**
5. 烧录成功后,串口应打印 `BLE 键鼠已启动(等待 iPhone 配对)`

\---

## 7\. 用串口监视器查看运行状态

烧录后,打开 **工具 → 串口监视器(Serial Monitor)**,右下角波特率选 **115200**。

* **A 板** 会打印:连接 WiFi、分配到的 IP 地址、`\[A板存活]` 心跳
→ 记下这个 IP,手机浏览器打开它就是遥控面板
* **B 板** 会打印:`BLE 键鼠已启动`、`\[B板存活]` 心跳(含蓝牙连接状态、队列积压)

> 想看哪块板的输出,就把串口监视器连到哪块板的 USB 端口。

\---

## 8\. 常见烧录问题

|现象|解决办法|
|-|-|
|上传卡在 `Connecting......\_\_\_\_....`|按住板子上的 **BOOT** 键不放,等开始写入再松手(部分板子要手动进下载模式)|
|`Could not open /dev/ttyUSBx, the port doesn't exist`|串口掉了:重新插拔 USB;换根短粗的好数据线;别用 USB hub,直插电脑;Linux 确认加了 dialout 组|
|端口被占用 / 无法上传|**关掉串口监视器**再上传(监视器占着端口会和上传抢)|
|编译报错找不到库(如 `ESPAsyncWebServer.h: No such file`)|对应依赖库没装好,回到第 4 步;装完**重启 IDE**|
|B 板功能不全(能移动不能绝对定位/点击)|用的不是修改版库,或 IDE 缓存旧库:确认第 4.2 步的库复制正确,并**重启 IDE 重新编译**|
|改了库文件却不生效|Arduino IDE 有库缓存:彻底**关闭并重开 IDE** 后再编译|
|上传成功但板子行为像旧代码|同上,重启 IDE;或确认烧的是正确的 `.ino` 和正确的端口|
|Windows 认不到 COM 口|装 CH340 驱动(见第 3 步)|

\---

## 快速检查清单

烧录前对照一遍:

* \[ ] Arduino IDE 2.x 已安装
* \[ ] ESP32 开发板支持包已装(工具→开发板里能看到 ESP32 Dev Module)
* \[ ] CH340 驱动 OK,能在端口列表看到板子
* \[ ] A 板依赖:ESPAsyncWebServer + AsyncTCP 已装
* \[ ] B 板依赖:本项目 `libraries/ESP32\_BLE\_Combo\_Keyboard\_Mouse/` 已复制到 Arduino 库目录
* \[ ] 复制库后**重启过 IDE**
* \[ ] 开发板选 ESP32 Dev Module,端口选对
* \[ ] A 板固件里的 WiFi 名称/密码已改成自己的(2.4G)

全部打勾,就可以按第 6 步开始烧录了。

烧录完成后,回到主 [README.md](./README.md) 的"配对与使用"章节，完成 iPhone 配对和网页遥控。

