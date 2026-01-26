# Huaray驱动使用说明
Copyright (C) 2022, HITCRT_VISION, all rights reserved.  
Author:BG2EDG  
Time:2022-04-16  

### 结构
```
.
├── CMakeLists.txt
├── config.mvcfg         // 相机配置文件，可自行选择放置位置
├── HuarayCam.cpp        // 源文件
├── HuarayCam.h          // 头文件
└── HUARAYConfig.cmake   // 提供cmake配置，务必放置于此
```

### 环境要求
1. c++ 标准14及以上    
2. Boost  
3. OpenCV  
4. GoogleTest(暂未使用)  
5. Huaray驱动版本2.3.x

### 使用说明
1. 终端运行```unmae -r```查看内核版本，**目前2.3.2版本驱动最高只支持到5.11内核**  
2. 下载[Huaray驱动2.3.2](http://download.huaraytech.com/pub/sdk/Ver2.3.2/Linux/x86/)并安装，旧大华驱动会被自动删除替代。注意在哪个内核下使用就在哪个内核下安装。  
3. 推荐使用```git submodule```方法将**整个camera文件夹**加入git工程，最low的方法是复制整个文件夹，无法获得及时的驱动更新  
4. 最外层CMakeLists.txt要添加以下两句，其中./huaray替换为HUARAYConfig.cmake所在路径  
   ```
    set(HUARAY_DIR ./huaray)
    find_package(HUARAY REQUIRED)   
   ```
5. 程序用法参见例程demos/huaray，使用到了一些C++的高级特性，可以参考本文档[笔记](#笔记)一栏的资料。  
6. Ubuntu 20.04下不论何种内核，deviceOnLine函数无法正常实现，现象是拔掉重插后报段错误，可能是驱动自身的问题。建议不要使用驱动的retry功能，在deviceOffLine时直接退出自己的程序，交给自启动脚本进行重连操作，避免卡在此处。  
7. 相机参数中的m_id变量是从1开始计数，与设备列表的Idx对应，如果赋0,则会自动设置为第一个生产商为Dahua/Huaray的相机Idx。  
### 问题记录
| 时间       | 问题                    | 现象                                   | 作者   | 解决                                                                 |
| ---------- | ----------------------- | -------------------------------------- | ------ | -------------------------------------------------------------------- |
| 2021-04-03 | Huaray驱动cmake找不到库 | 提示确实HUARAYConfig.cmake类似格式文件 | BG2EDG | 根据DAHUAConfig.cmake写了一个                                        |
| 2021-04-13 | 无法执行断线重连函数    | 传入的函数无法被断连回调执行           | BG2EDG | 拷贝构造函数里面忘记加入两个函数的成员变量，导致拷贝失败，修改后正常 |

### TODO
| 内容                                                         | 完成度 |
| ------------------------------------------------------------ | ------ |
| 确定接口和参数设置方式                                       | 100%   |
| old色彩转换用cv::cvtColor;驱动提供了颜色转换，对比速度       | 100%   |
| 使用静态函数做回调只能指定一个相机-> boost::bind绑定成员函数 | 100%   |
| 取流策略？序列？最新？                                       | 0%     |
| 调节抓图帧率                                                 | 0%     |

### 笔记
| 内容                                                                                                                                                                                                                                                               |
| ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| 相机执行顺序：findDevices,open，startGrab,stopGrab,close.相机加载参数要在open后、startGrab前，拉流过程（grabbing)中不能设置参数                                                                                                                                    |
| 相机使用方法拉流和触发，拉流可分为回调取图和多线程取图，触发分为软触发和外部触发。林哥版本的Dahua驱动林哥的驱动本质上是从流中取图片，并不是软触发                                                                                                                  |
| 使用指向某块数据的指针进行Mat初始化时，不会给矩阵分配内存，而是只初始化matrix header。所以不发生拷贝。但外部数据可能会被其他函数销毁。                                                                                                                             |
| 当前驱动的回调函数如果没有执行完，不会阻碍抓图，相机会等待回调执行完再把最新的图片送进来。实验方法：回调函数设置cv::waitkey(500),查询相机帧率为208，处理帧率则远小于此。imshow发现图片是跳跃式显示，说明处理不会干扰抓图，但时间过长会导致下一张图片不能及时进来。 |
| make_shared或make_unique会创建新对象,而非直接使用原对象的地址                                                                                                                                                                                                      |
| [std::function的使用](https://zhuanlan.zhihu.com/p/390883475)                                                                                                                                                                                                      |
| [shared_ptr](http://c.biancheng.net/view/7898.html)/[unique_ptr](http://c.biancheng.net/view/8672.html)                                                                                                                                                            |
| [智能指针传参](https://herbsutter.com/2013/06/05/gotw-91-solution-smart-pointer-parameters/)                                                                                                                                                                       |
| [sink概念](https://www.cppstories.com/2017/02/modernize-sink-functions/)                                                                                                                                                                                           |
| [asan提示alloc-dealloc错误：含义是C和C++的allocator混用，比如malloc/delete, new/free, and new/delete[].该错误不必过分关注.](https://docs.microsoft.com/en-us/cpp/sanitizers/error-alloc-dealloc-mismatch?view=msvc-170)                                            |



