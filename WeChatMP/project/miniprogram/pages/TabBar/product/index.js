// pages/product/index.js
import { requestWithLafToken, on_laf_token_Invalid, on_request_error, on_db_error, on_param_error, on_unknown_error, on_common_error } from '../../../apis/laf'
import { GetBase64ImgSrc } from '../../../utils/common'

const PUSH_COMMODITYCARD_NUM = 1

Page({

  /**
   * 页面的初始数据
   */
  data: {

    deviceList: [
    ],

    AdverSwiperGroup: {
      _id: String,
      CommonInfo: {
        
      },
      AdverSwiperItemList: [
        {
          _id: String,
          NavOptions: {
            NavType: String,
            TargetURL: String
          },
          Img: {
            _id: String,
            FileName: String,
            MimeType: String,
            Data: String
          }
        }
      ]
    }

  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad(options) {
    this.reset()
    this.requestPageData()
  },

  // 生命周期函数--监听下拉刷新
  async onPullDownRefresh(e) {
    await this.reset()

    // 刷新商品卡片组
    const CommodityCardGroup = this.selectComponent("#CommodityCardGroup")
    await CommodityCardGroup.Refresh()

    await this.requestPageData()
    wx.stopPullDownRefresh()
  },

  onReachBottom: async function (e) {
    console.log("onReachBottom e:", e)

    const CommodityCardGroup = this.selectComponent("#CommodityCardGroup")
    CommodityCardGroup.PushCommodityCardByLimit(PUSH_COMMODITYCARD_NUM)
  },
  
  // 页面重置
  reset(e) {
    this.setData({
      requestDeviceListConfig: {
        limit: 2
      },  //每次离开页面重置 limit 使设备列表折叠
      deviceListProfile: {
      },
      deviceList: [
      ],
    })

  },

  // 第 1 次获取和追加通用
  async requestPageData(e) {
    let resData
    try{
     resData = await requestWithLafToken('GET', `/iot2/device/getDeviceListOfUser?limit=${this.data.requestDeviceListConfig.limit}&skip=${this.data.deviceList.length}`)
    } catch(err) {
      console.log("err", err)
      switch(err.runCondition) {
        case 'laf_token error':
          on_laf_token_Invalid()
          return
        default:
          on_common_error(err)
          return
      }
    }
    // console.log("resData.deviceList", resData.deviceList)
    
    let mainList = this.data.deviceList

    // 讲每个 device 的 previewImg 的 Data 改为 Base64 image src 可用格式
    for(let i = 0; i < resData.deviceList.length; i ++) {
      resData.deviceList[i].previewImg.Data = GetBase64ImgSrc(resData.deviceList[i].previewImg)
    }

    // mainList = [...mainList, ...resData.deviceList]
    mainList.push(...resData.deviceList)  // push 的参数是数组的 item, 传入数组要展开为 item
    this.setData({
      deviceList: mainList,
      'deviceListProfile.total': resData.total,
    })


  },

  onShowMoreDevices(e) {
    this.setData({
      'requestDeviceListConfig.limit': 3  // 不限制获取设备的数量，获取全部设备
    })

    this.requestPageData()
  },

  onAddDevice(e) {
    console.log("onAddDevice e:", e)

    wx.navigateTo({
      url: '/pages/device/productList/index',
    })
  },

  onToCommodityDetailPage(e) {
    console.log("onToCommodityDetailPage e:", e)
  },

  onToAdvertisementPage(e) {
    console.log("onToAdvertisementPage e:", e)
    console.log("onToAdvertisementPage")
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