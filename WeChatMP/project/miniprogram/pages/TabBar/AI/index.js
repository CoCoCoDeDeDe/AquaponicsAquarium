// pages/TabBar/AI/index.js
Page({

  // 页面的初始数据
  data: {
    scroll_view_refresher_triggered: false
  },

  // 生命周期函数--监听页面加载
  onLoad(options) {
  },

  // 生命周期函数--监听页面初次渲染完成
  onReady() {
  },

  // 生命周期函数--监听页面显示
  onShow() {
  },

  // 生命周期函数--监听页面隐藏
  onHide() {
  },

  // 生命周期函数--监听页面卸载
  onUnload() {
  },

  // 页面相关事件处理函数--监听用户下拉动作
  onPullDownRefresh() {
  },

  // 页面上拉触底事件的处理函数
  onReachBottom() {
  },

  // 用户点击右上角分享
  onShareAppMessage() {
  },

  // scroll-view 下拉刷新触发
  onScrollViewRefresherRefresh: async function(event) {
    try{
      // 获取目标组件
      const bot_panel = await this.selectComponent("#bot_panel")
      // console.log("bot_panel:", bot_panel.data)
      if (bot_panel) {
        await bot_panel.refreshComponent()
      } else {
        console.log("未找到 bot_panel", bot_panel)
      }
      // 设置 scroll-view 属性结束下拉刷新
      await this.setData(
        {
          scroll_view_refresher_triggered: false
        }
      )

    } catch(err) {
      console.log("onScrollViewRefresherRefresh() err:", err)
    }
  }

})