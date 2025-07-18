import { requestWithLafToken, on_common_error } from "../../../apis/laf"

const FIRST_REQUEST_COMMODITYCARD_NUM = 4

// components/Commerce/CommodityCardGroup/index.js
Component({

  lifetimes: {
    attached: function () {

      // 每次挂载时获取固定数量的新商品卡片
      this.PushCommodityCard(0, FIRST_REQUEST_COMMODITYCARD_NUM)

    }
  },

  /**
   * 组件的属性列表
   */
  properties: {

  },

  /**
   * 组件的初始数据
   */
  data: {

    CommodityCardList_Total: null,

    CommodityCardList: [
      // {
      //   _id: String,
      //   Image: {
      //       _id: String,
      //       FileName: String,
      //       MimeType: String,
      //       Data: String
      //   },
      //   TextInfo: {
      //       Title: "商品主标题",
      //       Intro: String
      //   },
      //   Promotions: [
      //       {
      //           _id: String,
      //           Title_Short: "促销短名", 
      //       }
      //   ],
      //   Price: {
      //       Raw: 30,
      //       Current: 20
      //   }
      // }
    ]
  },

  /**
   * 组件的方法列表
   */
  methods: {
    // 按 skip 和 limit 获取指定段落的商品卡片记录，当没有更多时提示
    PushCommodityCard: async function (Skip, Limit) {
      try {

        // 当现加载的卡片数量已经达到已知的总可获取的卡片数量，则提示并不在追加
        if(typeof this.data.CommodityCardList_Total == 'number' && this.data.CommodityCardList.length >= this.data.CommodityCardList_Total) {
          wx.showToast({
            title: '没有更多了',
            duration: 1500,
            icon: 'error',
            mask: false
          })

          return  // exit
        }

        const RequestCommodityCardDataRes = await requestWithLafToken(
          'GET',
          '/iot2/Commerce/Commodity/GetCommodity',
          {
            skip: Skip,
            limit: Limit
          }
        )

        let NewCommodityCardList = RequestCommodityCardDataRes.Results.CommodityCardList

        const OldCommodityCardList = this.data.CommodityCardList

        const CommodityCardList_Main = OldCommodityCardList.concat(
          ...NewCommodityCardList
        )

        this.setData(
          {
            CommodityCardList_Total: RequestCommodityCardDataRes.Results.CommodityCardList_Total,
            CommodityCardList: CommodityCardList_Main
          }
        )

        // console.log("PushCommodityCard() this.data:", this.data)

      } catch(err) {
        switch(err.runCondition) {
          case 'laf_token error':
            on_laf_token_Invalid()
            return
          default:
            on_common_error(err)
            return
        }
      }
    },

    // 自动检测已加载的 CommodityCard 数量并跳过对应数量的 CommodityCard 记录，获取后续的 Limit 个记录
    PushCommodityCardByLimit: async function(Limit) {
      const Skip = this.data.CommodityCardList.length
      console.log("PushCommodityCardByLimit Skip:", Skip)
      this.PushCommodityCard(Skip, Limit)
    },

    // 刷新
    Refresh: async function() {
      // 1. 清除所有已经加载的卡片以及相关信息
      this.setData({
        CommodityCardList_Total: null,
        CommodityCardList: []
      })

      console.log("CommodityCardGroup Refresh() this.data", this.data)

      // 2. 重新获取固定数量的卡片
      this.PushCommodityCard(0, FIRST_REQUEST_COMMODITYCARD_NUM)

    }

  }
})