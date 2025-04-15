// 引入同目录下types文件夹中的类型定义文件
/// <reference path="./types/index.d.ts" />  

interface IAppOption {  // 定义小程序App()函数的配置选项接口
  globalData: {  // 全局数据对象定义
    userInfo?: WechatMiniprogram.UserInfo,  // 可选的用户信息，使用微信小程序提供的UserInfo类型
  }
  userInfoReadyCallback?: WechatMiniprogram.GetUserInfoSuccessCallback,  // 可选的用户信息准备完成回调函数
}