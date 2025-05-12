// pages/smartLink/index.js
Page({

  /**
   * 页面的初始数据
   */
  data: {
    // 功能页面枚举
    pageFunction_enum: {
      crontab: 'crontab',
      dataAnalysis: 'dataAnalysis'
    }
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad(options) {
    this.pageInit()

    // 根据presentPageFunction选择性初始化，这种初始化每次切换presentDevice时都要进行
  },

  // 根页面初始化
  pageInit(e) {
    this.setData({
      pageState: {
        presentPageFunction: this.data.pageFunction_enum.crontab, // 配置默认首先进入的功能页面
        
        // 定时任务页面装他
        crontabPageState: {
          deviceGroupList: [
            {
              id: '1',
              title: '所有设备'
            },
            {
              id: '2',
              title: ''
            }
          ]
        }
      }
    })
    
  },

  // crontab每日定时任务表 功能子页面 初始化
  crontabInit(e) {
    // 问题：定时计划的逻辑
    // 获取该用户的场所
    // 获取该场所的设备
    // 获取该
  }



})