// ./代表当前目录，../代表双亲目录，/代表根目录
import { register, login, verify_laf_token } from "./apis/laf"

App({
  globalData: {
    laf_token_validity: false
  },

  onLaunch: async function () {
    if (!wx.cloud) {
      console.error("请使用 2.2.3 或以上的基础库以使用云能力");
    } else {
      wx.cloud.init({
        // env 参数说明：
        //   env 参数决定接下来小程序发起的云开发调用（wx.cloud.xxx）会默认请求到哪个云环境的资源
        //   此处请填入环境 ID, 环境 ID 可打开云控制台查看
        //   如不填则使用默认环境（第一个创建的环境）
        env: "cloud1-9g4n5oib233e646c",
        traceUser: true,
      });
    }

    wx.loadFontFace({
      family: 'webfont',
      source: 'url("//at.alicdn.com/t/webfont_1f7b3qbimiv.eot")',
      success: function (res) {
          console.log(res.status) //  loaded
      },
      fail: function (res) {
          console.log(res.status) //  error
      },
      complete: function (res) {
          console.log(res.status);
      }
    });

    this.reset()


    // 本地缓存调试
    // await wx.setStorage({
    //   key: 'laf_token',
    //   data: 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1aWQiOiI2ODEwNjc4MjZiNDhkMzMzNGJhMDA4YzgiLCJleHAiOjE3NDY1MTA0MTksImlhdCI6MTc0NTkwNTYxOX0.s8jrxxhrX6bDUrHXF05u1iCNRJu2t4oFcakrpLDpVS4',
    //   encrypt: false,
    // }).then(res => {
    //   console.log("存储laf_token到本地缓存测试成功，res:", res)
    // })
    // wx.getStorageInfo()
    //   .then((res) => {
    //     console.log("获取本地缓存信息成功，res:", res)
    //   })

    // 测试API
    // register('testuser', '123456').catch(console.error)
    // login('testuser', '123456').catch(console.error)
  },

  // 重置 app
  reset(e) {
    this.globalData = {
      laf_token_validity: false
    }
  },

});
