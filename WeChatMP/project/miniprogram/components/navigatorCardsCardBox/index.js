// components/navigatorCardsCardBox/index.js
Component({

  /**
   * 组件的属性列表
   */
  properties: {
    // simpleProp: Number,
    navigatorCardList: {
      type: Array,
      value: 
      [
        {
          iconUrl: '/static/images/icons/icon_helpAndFeedback_block_darkBlue@2x.png',
          title: '帮助与反馈',
          navigateUrl: '/pages/mine/index'
        },
        {
          iconUrl: '/static/images/icons/icon_shoppingOrder_block_darkBlue@2x.png',
          title: '购物记录',
          navigateUrl: '/pages/mine/index'
        },
        {
          iconUrl: '/static/images/icons/icon_afterService_block_darkBlue@2x.png',
          title: '产品售后',
          navigateUrl: '/pages/mine/index'
        },
        {
          iconUrl: '/static/images/icons/tabBar_product_active.png',
          title: '我的产品',
          navigateUrl: '/pages/product/index'
        }
      ],
      required: false,
      observer: function(newVal, oldVal, changedPath) {}
    }
  },

  /**
   * 组件的初始数据
   */
  data: {

  },

  /**
   * 组件的方法列表
   */
  methods: {
    onNavigate(e) {
      console.log("onNavigate e:", e)
      wx.reLaunch({
        url: e.currentTarget.dataset.navigateurl
      })
      .then(res => {
        console.log("res", res)
      })
      .catch(err => {
        console.log("err", err)
      })
    }
  }
})