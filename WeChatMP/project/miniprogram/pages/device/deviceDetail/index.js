// pages/deviceDetail/index.js
import { isValidNonEmptyString } from '../../../utils/common'
import { requestWithLafToken, on_laf_token_Invalid, on_request_error, on_db_error, on_param_error, on_unknown_error, on_common_error } from '../../../apis/laf'
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
      this.GetUniIODataList(this.data.common_info.device_info.huawei_device_id)
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
    await this.GetUniIODataList(this.data.common_info.device_info.huawei_device_id)

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
  
  /* 通过 huawei_device_id 获取 uniIOCardDataList。数据由 html 的 wx:for 用于渲染卡片 */
  async GetUniIODataList(huawei_device_id) {
    let resData
    try{
      resData = await requestWithLafToken('GET', '/iot2/uniIO/GetUniIOCardDataList', { huawei_device_id : huawei_device_id })
      // console.log("resData:", resData)
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
    
    // 处理每个 UniIOData
    const UniIODataList = await Promise.all( resData.UniIODataList.map( async (item, idx, arr) => {
      // console.log("item 1:", item)

      let NewItem = {}

      // 遍历每个 UniIOData 项，找到其 Records 中最近的一项 Record 并单独存放
      // 校验 Records
      let LateastRecord = null
      // console.log("item.Records.length:", item.Records.length)
      if(item.Records.length > 0) { // 获取的记录为空时不 reduce()
        LateastRecord = item.Records.reduce( (prev, current) => {
          const prevTime = new Date(prev.event_time).getTime()
          const currentTime = new Date(current.event_time).getTime()
          return currentTime > prevTime ? current : prev
        } )
        Object.assign(NewItem, {LateastRecord})
        // console.log("NewItem 1:", NewItem)
      } else{
        Object.assign(NewItem, {LateastRecord})
        // console.log("NewItem 2:", NewItem)
      }

      // 存储已经可用的数据到新介质
      Object.assign(NewItem,
        {
          UniIO_Id: item.UniIO_Id,
          UniIO_Type: item.UniIO_Type,
          UniIO_TemplateName: item.UniIO_TemplateName,
          UniIO_ExternalName: item.UniIO_ExternalName,
          SmartLinkGroup_Name: item.SmartLinkGroup_Name,
          Device_Name: item.Device_Name,
          UniIO_Value_Mean_Pair: item.UniIO_Value_Mean_Pair,  // 没有则为 undefined
          UniIO_Value_Unit: item.UniIO_Value_Unit,
          Cmd_Config: item.Cmd_Config,
        })
        // console.log("NewItem 3:", NewItem)
        // console.log("NewItem.UniIO_Type:", NewItem.UniIO_Type)
        // console.log("NewItem.UniIO_Type:", NewItem.UniIO_Type)
        // console.log("NewItem.Cmd_Config:", NewItem.Cmd_Config)

      // 将部分数据改为适合 echart 的格式
      let DataX = [], DataY = []
      if(item.Records.length > 0) {
        for (let i = 0; i < item.Records.length; i++) {

          // 把20250510T120031Z字符串格式时间转化为 06:00 格式的字符串
          DataX[i] = TimeStrConvert_ISO8601_To_HHmm(item.Records[item.Records.length - 1 - i].event_time)
          
          // 转化单独 Y 轴数据到数组
          DataY[i] = item.Records[item.Records.length - 1 - i].value
          // 如果 UniIO_Value_Mean_Pair 有值则匹配为对应意义 string
          // 本方案已放弃，echart 折线图的 y 周数据只可为数值
          // if(NewItem.UniIO_Value_Mean_Pair !== undefined) {
          //   DataY[i] = NewItem.UniIO_Value_Mean_Pair[DataY[i].toString()]
          // }   
        }
      } else{
        // 无 Records 不处理
      }
      // console.log("DataX:", DataX)
      // console.log("DataY:", DataY)
      Object.assign(NewItem, {
        EChartData: {
          xAxis: {
            data: DataX,
          },
          series: [{  // 可一个表显示多条数据
            data: DataY,
          }]
        }
      })
      // console.log("NewItem 4:", NewItem)

      // 赋值默认值给暂时没有网络数据的 echart 配置项
      NewItem.EChartData.series[0].name = NewItem.UniIO_ExternalName
      NewItem.EChartData.series[0].itemStyle = { color: item.MainColor }

      return NewItem
    } ) )

    // return

    this.setData({
      UniIODataList: UniIODataList,
    })

  }


})

function TimeStrConvert_ISO8601_To_HHmm(isoString) {

  const [datePart, timePart] = isoString.split("T");
  // console.log("timePart:", timePart)
  let result = 'TT:SS'
  result = timePart[0] + timePart[1] + ':' + timePart[2] + timePart[3]
  // console.log("result:", result)
  return result
}