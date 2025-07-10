// pages/productList/index.js
import { isValidNonEmptyString, GetBase64ImgSrc } from '../../../utils/common'
import { requestWithLafToken, on_laf_token_Invalid, on_request_error, on_db_error, on_param_error, on_unknown_error, on_common_error } from '../../../apis/laf'
Page({

  /**
   * 页面的初始数据
   */
  data: {

    productList: [
      {
        createdAt: "20250508T034732Z",
        name: "默认名字1",
        updateAt: "20250508T034732Z",
        _id: "1",
        PreviewImg_Base64URL: "",
      },
      {
        createdAt: "20250508T034732Z",
        name: "默认名字2",
        updateAt: "20250508T034732Z",
        _id: "2",
        PreviewImg_Base64URL: "",
      },
      {
        createdAt: "20250508T034732Z",
        name: "默认名字3",
        updateAt: "20250508T034732Z",
        _id: "3",
        PreviewImg_Base64URL: "",
      },
      {
        createdAt: "20250508T034732Z",
        name: "默认名字4",
        updateAt: "20250508T034732Z",
        _id: "4",
        PreviewImg_Base64URL: "",
      }
    ]

  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad(options) {
    this.reset()
    this.onGetProductList()
  },

  // 每次触底时 skip 要成为已经获取到的产品的数量, 按新 skip 获取新的 limit 个产品后将新获取的数组追尾到旧数组后面
  // 可优化：在此判断本地产品数量与数据库产品数量时获取实时的数据库产品数量，方式数据库更新了但已经在本页面就看不到新的，还要改获取的排序逻辑
  async onReachBottom(options) {
    const { requestOptions } = this.data

    if(this.data.dbInfo.total != undefined && this.data.productList.length >= this.data.dbInfo.total ) {
      if(this.data.counters.noMoreTip_showToast < 1) {
        wx.showToast({
          title: '没有更多咯',
          duration: 1000,
          icon: 'none',
          mask: false,
        })
        const { counters } = this.data
        this.setData({
          'counters.noMoreTip_showToast': counters.noMoreTip_showToast + 1
        })

      }
      return  // 已经将目标数据库中符合要求的产品获取完了
    }

    await this.setData({
      'requestOptions.skip': requestOptions.skip + requestOptions.limit,
    })
    
    // 请求追加产品
    let newProductList
    try{
      let resData = await requestWithLafToken('GET', '/iot2/product/getProductList', { limit: this.data.requestOptions.limit, skip: this.data.requestOptions.skip })

      // 将 resData.productList 中每一子项的 previewImg 的 Data 改写为前端标签可用的格式
      await resData.productList.forEach( (CurrentValue, Index, Array) => {
        console.log("CurrentValue:", CurrentValue)
        CurrentValue.previewImg.Data = GetBase64ImgSrc(CurrentValue.previewImg)
      })

      // console.log("resData", resData)
      newProductList = resData.productList

      this.setData({
        'dbInfo.total': resData.total
      })
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
    // 成功请求的后续
    let oldProductList = this.data.productList
    await this.setData({
      productList: [...oldProductList, ...newProductList]
    })
  },

  // 重置页面
  reset() {
    this.setData({
      requestOptions: {
        limit: 7,
        skip: 0,
      },
      dbInfo: {
        total: undefined,
      },
      counters: {
        noMoreTip_showToast: 0,
      },
    })
  },

  
  async onGetProductList(e) {
    let productList
    try{
      const resData = await requestWithLafToken('GET', '/iot2/product/getProductList', { limit: this.data.requestOptions.limit, skip: this.data.requestOptions.skip })

      // 将 resData.productList 中每一子项的 previewImg 的 Data 改写为前端标签可用的格式
      await resData.productList.forEach( (CurrentValue, Index, Array) => {
        console.log("CurrentValue:", CurrentValue)
        CurrentValue.previewImg.Data = GetBase64ImgSrc(CurrentValue.previewImg)
      })

      productList = resData.productList

      this.setData({
        'dbInfo.total': resData.total
      })
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
    // 成功请求的后续
    // console.log("productList:", productList)
    this.setData({
      productList: productList
    })
  },

  async onToBindPageWithProductId(e) {
    // console.log("onToBindPage 携带参数 e.currentTarget.dataset:", e.currentTarget.dataset)
    const { product_id } = e.currentTarget.dataset
    // console.log("onToBindPage 携带参数 product_id:", product_id)
    wx.navigateTo({
      url: `/pages/device/bindUserWithDevice/index?product_id=${product_id}`,
    })
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
      imageUrl: '/path/to/share/image.jpg' // 必须是本地路径或临时路径
    }
  },

})