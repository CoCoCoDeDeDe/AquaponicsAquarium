// components/navigatorCard/index.js
Component({

  /**
   * 组件的属性列表
   */
  properties: {
    cardData: {
      type: Object,
      value: {
        iconUrl: '/static/images/icons/icon_programSetting_block_darkBlue@2x.png',
        title: '退出登录',
        // methodName_bindTap: 'on_logOff',
        // navigateUrl: null,
      },
      required: false,
    },
    title: {
      type: Object,
      value: {
        littleTitle: '退出登录'
      }
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

  }
})