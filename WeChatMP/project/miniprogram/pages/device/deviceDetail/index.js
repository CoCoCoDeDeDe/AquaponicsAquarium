// pages/deviceDetail/index.js
import { isValidNonEmptyString } from '../../../utils/common'
import { requestWithLafToken, on_laf_token_Invalid, on_request_error, on_db_error, on_param_error, on_unknown_error, on_common_error, GetUniIODataList } from '../../../apis/laf'
Page({

  /**
   * 页面的初始数据
   */
  data: {

    "common_info": {
      "device_info": {
        "_id": "default_device_id",
        "product_id": "default_product_id",
        "huawei_device_id": "default_huawei_device_id",
        "name": "默认设备名称",
        "createdAt": "20250502T064449Z",
        "updateAt": "20250502T064449Z"
      },
      "product_info": {
        "_id": "DefaultDeviceId",
        "name": "默认产品名称",
        "previewImg_url": "https://mp-1b9cd3c8-d666-4006-b791-11d5ce02e1be.cdn.bspapp.com/IoT1/test/previewImg_aqaq.png",
        "detailPoster_url": "https://mp-1b9cd3c8-d666-4006-b791-11d5ce02e1be.cdn.bspapp.com/IoT1/test/devicePoster1x1_22213.png",
        "intro": "默认产品简介",
        "updateAt": "20250502T062058Z"
      }
    },

    UniIODataList: [

    ],

  },

  /**
   * 生命周期函数--监听页面加载
   */
  async onLoad(options) {
    console.log("options", options)
    await this.setData({
      'common_info.device_info.huawei_device_id': options.huawei_device_id,
    })

    await this.reset()

    // return

    // 用定时器重复获取新 UniIODataList
    this.data.timer = setInterval(() => {
      // console.log("setInterval Callback")
      GetUniIODataList(this, {huawei_device_id: this.data.common_info.device_info.huawei_device_id})
    }, 10000)
  },

  onUnload(e) {
    if(this.data.timer) {
      clearInterval(this.data.timer)
    }
  },

  // 根据 uniIO 数组获取 uniIO 数据、生成 uniIO 卡片的程序要独立于 通过 device 获取 uniIO 数组的程序，以便根据 smartLinkGroup 等其他方式获取不同的 uniIO 数组
  async reset() {
    // 初始化各种配置
    await this.setData({

    })

    /* 获取设备和产品信息( product_profile, device_profile, ) */
    await this.GetCommonInfo(this.data.common_info.device_info.huawei_device_id)

    /* 通过 huawei_device_id 获取 uniIOCardDataList。数据由 html 的 wx:for 用于渲染卡片 */
    await GetUniIODataList(this, {huawei_device_id: this.data.common_info.device_info.huawei_device_id})

    // 设置页面导航栏标题
    await wx.setNavigationBarTitle({
      title: `${this.data.common_info.device_info.name}`,
    })
  },

  /* 获取设备和产品信息( product_profile, device_profile, ) */
  async GetCommonInfo(huawei_device_id) {
    try{
      const resData = await requestWithLafToken('GET', '/iot2/device/getDeviceInfo', { huawei_device_id : huawei_device_id })
      // console.log("resData:", resData)
      this.setData({
        common_info: resData.common_info
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
