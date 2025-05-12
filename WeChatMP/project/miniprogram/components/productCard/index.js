import { requestWithLafToken, on_laf_token_Invalid, on_request_error, on_request_kit } from '../../apis/laf'
Component({

  /**
   * 组件的属性列表
   */
  properties: {
    cardData: {
      type: Object,
      value: {
        previewImg_url: 'https://mp-1b9cd3c8-d666-4006-b791-11d5ce02e1be.cdn.bspapp.com/IoT1/test/previewImg_aqaq.png',
        name: '默认产品名字',
        intro: '默认产品简介',
      },
      required: false,
    }
  },

  /**
   * 组件的初始数据
   */
  data: {
    
  },

  async attached() {
    // this.onGetProductList()
  },

  /**
   * 组件的方法列表
   */
  methods: {
  }
})