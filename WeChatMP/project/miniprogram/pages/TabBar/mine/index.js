// pages/mine/index.js
import { requestWithLafToken, on_laf_token_Invalid, on_request_error } from '../../../apis/laf'

Page({

  /**
   * 页面的初始数据
   */
  data: {
    serviceNavigatorCardList: [
      {
        iconUrl: '/static/images/icons/icon_helpAndFeedback_block_darkBlue@2x.png',
        title: '帮助与反馈',
        navigateUrl: '/pages/mine/index'  // TODO 打造帮助与反馈页面
      },
      {
        iconUrl: '/static/images/icons/icon_shoppingOrder_block_darkBlue@2x.png',
        title: '购物记录',
        navigateUrl: '/pages/mine/index'  // TODO 打造购物记录页面
      },
      {
        iconUrl: '/static/images/icons/icon_afterService_block_darkBlue@2x.png',
        title: '产品售后',
        navigateUrl: '/pages/mine/index'  // TODO 打造产品售后页面
      }
    ],
    programNavigatorCardList: [
      {
        iconUrl: '/static/images/icons/icon_programSetting_block_darkBlue@2x.png',
        title: '设置',
        navigateUrl: '/pages/mine/index'  // TODO 打造程序设置页面
      }
    ],

    cardData_logIn: {
      iconUrl: '/static/images/icons/icon_login_block_active.png',
      title: '登录',
      // methodName_bindTap: 'on_logOff',
      // navigateUrl: null,
    },

    cardData_logOff: {
      iconUrl: '/static/images/icons/icon_logoff_block_active_danger.png',
      title: '退出登录',
      // methodName_bindTap: 'on_logOff',
      // navigateUrl: null,
    },
    userProfile: {
      avatar_url: '/static/images/icons/defaultAvatar.png',
      nickname: '默认昵称',
      username: 'default user_name'
    }
  },

  /**
   * 生命周期函数--监听页面加载
   */
  async onLoad(options) {
    // console.log("onLoad options:", options)
    this.reset()

    this.requestPageData()
    // const res = await wx.getStorage({key: 'laf_token'})
    // console.log("测试 res", res)
  },
  
  // 页面重置
  reset(e) {
    this.setData({
      
    })
  },

  // 获取mine页面信息
  async requestPageData(e) {
    let userProfile
    try{
      const requestRes = await requestWithLafToken('GET', `/iot2/user/getUserProfile`)
      userProfile = requestRes.userProfile
    } catch(err) {
      console.log("API getUserProfile err:", err)
      switch(err.runCondition) {
        case 'laf_token error':
          on_laf_token_Invalid()
          return
        case 'request error':
          on_request_error()
          return
        case 'db error':
          on_db_error()
          return
        case 'param error':
          on_param_error()
          return
        default:
          on_unknown_error()
          return
      }
    }

    // 成功的继续
    this.setData({
      userProfile: {...this.data.userProfile, ...userProfile} // 展开语法，后者拥有的元素(包括值为 undefined 的)会覆盖前者中相同键的元素，其他方法：遍历
    })
    console.log("this.data.userProfile", this.data.userProfile)
  },

  // 点击事件：登录
  async on_bindTap_GoToLogIn(e) {
    this.GoToLogIn()
  },

  // 登录
  async GoToLogIn(e) {
    wx.navigateTo({
      url: '/pages/login/index',
    })
  },

  // 点击事件：退出登录
  on_bindTap_LogOff(e) {
    // 为退出登录函数嵌套确认弹窗
      wx.showModal({
        cancelColor: '#aaa',
        cancelText: '取消',
        confirmColor: '#c41a16',
        confirmText: '退出登录',
        content: '请谨慎确认',
        editable: false,
        placeholderText: 'placeholderText',
        showCancel: true,
        title: '是否确认退出登录',
        success: async (result) => {
          // console.log("result", result)
          if(result.cancel) {
            return
          }
          if(result.confirm) {
            this.onLogOff()
          }
        },
        fail: (res) => {},
        complete: (res) => {},
      })
  },

  // 退出登录
  onLogOff(e) {
    wx.removeStorage({
      key: 'laf_token',
      success: (res) => {
        wx.showToast({
          title: '退出登录成功',
          duration: 2000,
          icon: 'success',
          mask: false,
        })
      },
      fail: (res) => {
        wx.showToast({
          title: '退出登录成功失败',
          duration: 15000,
          icon: 'error',
          mask: true,
        })
      },
    })

    this.reset()
  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage() {
    return {
      title: '快来使用我们的小程序吧！', // 分享标题
      path: '/pages/TabBar/SmartLinkGroup/index', // 分享路径，可带参数
      // imageUrl: '/images/share.png', // 自定义分享图片路径
      success(res) {
        // 分享成功的回调函数
        console.log('分享成功', res);
        // 可以在这里添加分享成功后的奖励逻辑
      },
      fail(res) {
        // 分享失败的回调函数
        console.log('分享失败', res);
      }
    };
  },

  /**
   * 用户点击右上角分享到朋友圈
   */
  onShareTimeline() {
    return {
      title: '分布式智能物联网平台',
      query: {
        // userId: '123', // 可携带的参数
      },
      imageUrl: '/path/to/share/image.jpg' // 必须是本地路径或临时路径
    }
  },

})