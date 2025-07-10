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
      {
        Path: '/pages/Admin/ImagesBase64Cmd/index',
        Name: '管理员管理图片',
        Intro: '管理员管理图片',
        Icon_Url: '/static/images/icons/tabBar_mine_active.png',
        Color_Main: '',
        Is_Forbidden: false,
      },
      {
        Path: '/pages/Admin/DB/ManageCenter/index',
        Name: '数据库管理中心',
        Intro: '管理数据库',
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
      // imageUrl: '/path/to/share/image.jpg' // 必须是本地路径或临时路径
    }
  },
  
})