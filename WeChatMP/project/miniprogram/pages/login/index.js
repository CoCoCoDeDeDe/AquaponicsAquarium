// pages/login/index.js
import { login, register, requestWithLafToken } from '../../apis/laf'
Page({

  /**
   * 页面的初始数据
   */
  data: {
    
    page_state_enum: {
      loginOrRegister: 'loginOrRegister',
      loginSucceed: 'loginSucceed',
    },
    page_state: 'loginOrRegister',
    value_ipt_username: null,
    value_ipt_pwd: null,
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad(options) {

  },
  
  async onLogin(e) {
    let tmp = {
      username: this.data.value_ipt_username,
      pwd: this.data.value_ipt_pwd
    }
    try{
      await login(tmp.username, tmp.pwd)
    } catch(err) {
      console.log("登录页面 onLogin catch 触发 err:", err)
      switch(err.runCondition) {
        case 'param error':
          wx.showToast({
            title: '用户名或密码错误',
            duration: 1500,
            icon: 'none',
            mask: true
          })
          break
        case 'storage error':
          wx.showToast({
            title: 'laf_token 存储失败',
            duration: 1500,
            icon: 'error',
            mask: true
          })
          break
        case 'request error':
          wx.showToast({
            title: '网络请求失败',
            duration: 1500,
            icon: 'error',
            mask: true
          })
          break
      }
      return
    }
    // 成功登录的继续

    // 提示
    wx.showToast({
      title: '登录成功',
      duration: 1500,
      icon: 'success',
      mask: false,
    })

    // 登录成功后跳转页面
    setTimeout(() => {
      wx.reLaunch({
        url: '/pages/TabBar/SmartLinkGroup/index'
      })
    }, 1500)

  },

  async onRegister(e) {
    // 收集参数
    let tmp = {
      username: this.data.value_ipt_username,
      pwd: this.data.value_ipt_pwd
    }
    // 调用注册 API
    try{
      await register(tmp.username, tmp.pwd)
    } catch(err) {
      console.log("注册 onRegister catch 触发 err:", err)
      switch(err.runCondition) {
        default:
          wx.showToast({
            title: err.errMsg,
            duration: 1500,
            icon: 'error',
            mask: true
          })
          break
      }
      return
    }
    // 成功注册的继续
    wx.showToast({
      title: '注册成功',
      duration: 1500,
      icon: 'success',
      mask: false,
    })

    // 登录
    this.onLogin()
  }

})