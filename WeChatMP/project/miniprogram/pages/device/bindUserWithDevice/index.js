// pages/device/BindUserToDevice/index.js
import { isValidNonEmptyString } from '../../../utils/common'
import { requestWithLafToken, on_laf_token_Invalid, on_request_error, on_db_error, on_param_error, on_unknown_error, on_common_error } from '../../../apis/laf'

Page({

  /**
   * 页面的初始数据
   */
  data: {
    ipt_huawei_device_id: undefined,
    cardData: {
      previewImg_url: 'https://mp-1b9cd3c8-d666-4006-b791-11d5ce02e1be.cdn.bspapp.com/IoT1/test/previewImg_aqaq.png',
      name: '默认产品名字',
      intro: '默认产品简介',
    }
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad(options) {
    // console.log("options", options)
    const { product_id } = options
    // console.log("product_id", product_id)

    // 页面参数校验
    if(!isValidNonEmptyString(product_id)) {
      wx.showToast({
        title: '页面参数无效',
        duration: 1500,
        icon: 'error',
        mask: false,
      })
      wx.navigateBack({
        delta: 0,
      })
    }

    this.onGetProductInfo(product_id)

  },

  async onGetProductInfo(product_id) {
    try{
      const resData = await requestWithLafToken('GET', '/iot2/product/getProductInfo', { product_id: product_id })
      this.setData({
        cardData: resData.productProfile
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

  on_confirmBtn_bindTap(e) {
    this.bindUserWithDevice()
  },

  bindUserWithDevice(e) {
    // 参数获取
    const { ipt_huawei_device_id } = this.data
    console.log("获取的要匹配的设备的 ipt_huawei_device_id:", ipt_huawei_device_id)

    // 参数校验
    if(ipt_huawei_device_id == undefined || ipt_huawei_device_id.trim().length < 1) {
      wx.showToast({
        title: '无效的设备 ID',
        duration: 1500,
        icon: 'error',
        mask: false,
      })
      return
    }

    // 发起绑定请求
    this.onBindRequest(ipt_huawei_device_id)
  },

  async onBindRequest(huawei_device_id) {
    // 开始请求
    let bindResData
    try{
      const resData = await requestWithLafToken('GET', '/iot2/device/bindUserWithDevice', {huawei_device_id: huawei_device_id})
      // 继续
      bindResData = resData
    } catch(err) {
      switch(err.runCondition) {
        case 'laf_token error':
          on_laf_token_Invalid()
          return
        case 'request error':
          on_request_error()
          return
        case 'db error':
          on_db_error()
          return
        case 'param error':
          on_param_error()
          return
        default:
          on_common_error(err)
          return
      }
    }
    // 匹配成功后返回上一页
    console.log("绑定成功")
    await wx.showToast({
      title: bindResData.errMsg,
      duration: 1500,
      icon: 'success',
      mask: true,
    })
    setTimeout(() => {
      wx.navigateBack()
    }, 2000)
  }



})

