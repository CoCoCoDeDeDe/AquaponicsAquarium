// components/Commerce/CommodityCard/index.js
Component({

  /**
   * 组件的属性列表
   */
  properties: {
    CommodityData: {
      type: Object,
      value: {
        _id: String,
        Image: {
            _id: String,
            FileName: String,
            MimeType: String,
            Data: String
        },
        TextInfo: {
            Title: "商品主标题",
            Intro: String
        },
        Promotions: [
            {
                _id: String,
                Title_Short: String, 
            }
        ],
        Price: {
            Raw: Number,
            Current: Number
        }
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