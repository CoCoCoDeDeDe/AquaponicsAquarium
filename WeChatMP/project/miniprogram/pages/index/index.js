Page({

  /**
   * 页面的初始数据
   */
  data: {
    NavigatorList: [
      {
        Path: '/pages/TabBar/SmartLinkGroup/index',
        Name: '智联组页面',
        Intro: '管理你的 UniIO 和 智联组，以智联组为单位控制分布式物联网设备',
        Icon_Url: '/static/images/icons/icon_tabbar_group_active.png',
        Color_Main: '',
        Is_Forbidden: false,
      },
      {
        Path: '/pages/TabBar/smartLink/index',
        Name: '智联页面',
        Intro: '为你的智联组的 UniIO 配置自动化运行规则，赋予 UniIO 智能',
        Icon_Url: '/static/images/icons/tabBar_smartLink_active.png',
        Color_Main: '',
        Is_Forbidden: true,
      },
      {
        Path: '/pages/TabBar/product/index',
        Name: '产品和设备页面',
        Intro: '管理用户所拥有的设备，浏览我们推出的产品',
        Icon_Url: '/static/images/icons/tabBar_product_active.png',
        Color_Main: '',
        Is_Forbidden: false,
      },
      {
        Path: '/pages/TabBar/mine/index',
        Name: '我的页面',
        Intro: '管理用户账号，设置小程序功能',
        Icon_Url: '/static/images/icons/tabBar_mine_active.png',
        Color_Main: '',
        Is_Forbidden: false,
      },
    ]
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    
  },


  On_BindTap_NavigatorTo(e) {
    // console.log("e", e)
    const Rec_PageUrl = e.currentTarget.dataset.pageurl
    // console.log("Rec_PageUrl:", Rec_PageUrl)
    setTimeout(() => {
      wx.reLaunch({
        url: Rec_PageUrl,
      })
    }, 200)
  }
  
})