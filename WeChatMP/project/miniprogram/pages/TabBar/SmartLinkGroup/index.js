// pages/SmartLinkGroup/index.js
import { isValidNonEmptyString } from '../../../utils/common'
import { requestWithLafToken, on_laf_token_Invalid, on_request_error, on_db_error, on_param_error, on_unknown_error, on_common_error, GetUniIODataList } from '../../../apis/laf'
Page({

  /**
   * 页面的初始数据
   */
  data: {
    GroupCmdBtnList: [
      {
        Img_Url: '/static/images/icons/icon_deviceDetail_line_dekBlue@2x.png',
        FunctionName: '查看',
        BindTapHandlerName: 'On_BindTap_Read',
      },
      {
        Img_Url: '/static/images/icons/icon_edit_line_darkBlue@2x.png',
        FunctionName: '修改',
        BindTapHandlerName: 'On_BindTap_Update',
      },
      {
        Img_Url: '/static/images/icons/icon_delete_line_darkBlue@2x.png',
        FunctionName: '删除',
        BindTapHandlerName: 'On_BindTap_Delete',
      },
    ],
    Is_GroupCmdCard_Show: true,

    PageOption: {
      SLGroup_Id: 'Default_SLGroup_Id',
      SLGroupProfile: {
        SLGroup_Id: 'Default_SLGroup_Id',
        SLGroup_Name: '默认智联组名称',
        SLGroup_CreateTime: '',
        SLGroup_UpdateTime: '',
      },
    },

    SLGroupProfileList: [
      {
        SLGroup_Id: 'Default_SLGroup_Id',
        SLGroup_Name: '默认智联组名称',
        SLGroup_Sequence: 0,
      },
      {
        SLGroup_Id: 'Default_SLGroup_Id1',
        SLGroup_Name: '默认智联组名称1',
        SLGroup_Sequence: 1,
      },
      {
        SLGroup_Id: 'Default_SLGroup_Id2',
        SLGroup_Name: '默认智联组名称2',
        SLGroup_Sequence: 2,
      },
      {
        SLGroup_Id: 'Default_SLGroup_Id3',
        SLGroup_Name: '默认智联组名称3默认智联组名称3默认智联组名称3默认智联组名称3',
        SLGroup_Sequence: 3,
      },
    ],

    UniIODataList: [
      {
        UniIO_Id: 'Default_UniIO_Id',
        UniIO_ExternalName: '示例 UniIO 名称',
        SmartLinkGroup_Name: '示例智联组名称',
        Device_Name: '示例设备名称',
        UniIO_Type: 'actor',
        Cmd_Config: {
          Is_Enum: true,
          Enum: [
            {
              Value: 0,
              Mean: '关闭',
              Main_Color: '#e64340',
            },
            {
              Value: 1,
              Mean: '启动',
              Main_Color: '#1bbb1b',
            },
          ],
          Value_Max: 100,
          Value_Min: 0,
        },
        UniIO_Value_Unit: 'DefaultUnit',
        UniIO_Value_Mean_Pair: {
          0: "空闲",
          1: "运行",
          2: "错误",
        },
        LateastRecord: {
          event_time: 'default_event_time',
          value: 'DefaultValue',
        },
        EChartData: {
          xAxis: {
            data: [
              '00:00',
              '00:01',
              '00:08',
              '00:11',
              '10:00',
              '15:00',
              '16:00',
              '18:01',
            ]
          },
          series: [{
            name: '负载率',
            data: [5, 20, 500, 5, 20, 500, 5, 20],
            itemStyle: { color: '#FFEC71' }, // 线条颜色
          }]
        }
      },
      {
        UniIO_Id: 'Default_UniIO_Id2',
        UniIO_ExternalName: '示例 UniIO 名称',
        SmartLinkGroup_Name: '示例智联组名称',
        Device_Name: '示例设备名称',
        UniIO_Type: 'actor',
        Cmd_Config: {
          Is_Enum: true,
          Enum: [
            {
              Value: 0,
              Mean: '关闭',
              Main_Color: '#e64340',
            },
            {
              Value: 1,
              Mean: '启动',
              Main_Color: '#1bbb1b',
            },
          ],
          Value_Max: 100,
          Value_Min: 0,
        },
        UniIO_Value_Unit: 'DefaultUnit',
        UniIO_Value_Mean_Pair: {
          0: "空闲",
          1: "运行",
          2: "错误",
        },
        LateastRecord: {
          event_time: 'default_event_time',
          value: 'DefaultValue',
        },
        EChartData: {
          xAxis: {
            data: [
              '00:00',
              '00:01',
              '00:08',
              '00:11',
              '10:00',
              '15:00',
              '16:00',
              '18:01',
            ]
          },
          series: [{
            name: '负载率',
            data: [5, 20, 500, 5, 20, 500, 5, 20],
            itemStyle: { color: '#FFEC71' }, // 线条颜色
          }]
        }
      },
    ],
  },

  /**
   * 生命周期函数--监听页面加载
   */
  async onLoad(options) {
    // console.log("options:", options)
    this.setData({  // 清空预览所用的数据
      PageOption: {},
      SLGroupProfileList: [],
      // UniIODataCardList: [],
    })

    // 当前浏览的 SLGroup 默认由页面参数指定, 若无页面参数则默认用智联组表中第一个或者权重第一的或者标为重要的智联组
    // 获取用户的智联组简介表
    await this.GetSLGroupProfileList() // Debug Delete

    // 若获取的该用户的智联组表为空，则本也为空智联组状态，很多智联组不为空时的操作和函数不执行或显示表为空的提示

    if(this.data.SLGroupProfileList.length > 0) {
      if(options.SLGroupId === undefined || options.SLGroupId === '' || options.SLGroupId === ' ') {
        console.log("无传入 options.SLGroupId")
        await this.setData({
          'PageOption.SLGroup_Id': this.data.SLGroupProfileList[0].SLGroup_Id,
        })
      } else{
        console.log("有传入 options.SLGroupId")
        await this.setData({
          'PageOption.SLGroup_Id': options. SLGroupId
        })
      }

      // 获取本 SLGroup 的 Profile
      await this.GetNewSLGroupProfile()
      // PageOption: {
      //   SLGroup_Id: 'Default_SLGroup_Id',
      //   SLGroupProfile: {
      //     SLGroup_Id: 'Default_SLGroup_Id',
      //     SLGroup_Name: '默认智联组名称',
      //     SLGroup_CreateTime: '',
      //     SLGroup_UpdateTime: '',
      //   },
      // },
      // 获取本 SLGroup 的所有的 UniIO 的 UniIODataCardList
      await GetUniIODataList(this, {smartLinkGroup_id: this.data.PageOption.SLGroup_Id})
      
      // 用定时器重复获取新 UniIODataList
      this.data.timer = setInterval(() => {
        GetUniIODataList(this, {smartLinkGroup_id: this.data.PageOption.SLGroup_Id})
      }, 10000)

    } else{ // 显示 智联组 为空时的提示

      setTimeout(() => {  // 等页面出场动画差不多加载完后显示  
        wx.showToast({
          title: '暂无智联组',
          duration: 1500,
          icon: 'none',
          mask: false,
        })
      }, 1000);

    }


    

  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady() {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow() {

  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide() {

  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload() {

  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh() {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom() {

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

  // 智联组管理卡片的显示与隐藏开关
  On_BindTap_SwitchSLGroupCmdCardShow(e) {
    let StatusTmp = this.data.Is_GroupCmdCard_Show
    let StatusNew
    StatusNew = StatusTmp ? false : true
    this.setData({
      Is_GroupCmdCard_Show: StatusNew
    })
  },

  async GetSLGroupProfileList(e) {
    // console.log("GetSLGroupProfileList")
    let ResData
    try{
      ResData = await requestWithLafToken('GET', '/iot2/smartLinkGroup/GetSLGroupProfileList')
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
    // console.log("ResData:", ResData)

    // 整理数据
    let SLGroupProfileList_ToSave = ResData.SLGroupProfileList
    
    this.setData({
      SLGroupProfileList: SLGroupProfileList_ToSave
    })
  },

  async GetNewSLGroupProfile(e) {
    // console.log("GetNewSLGroupProfile")
    let ResData
    try{
      ResData = await requestWithLafToken('GET', '/iot2/smartLinkGroup/GetSmartLinkGroupInfo', {smartLinkGroup_id: this.data.PageOption.SLGroup_Id})
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
    console.log("ResData:", ResData)

    // 整理数据
    let SLGroupProfile_ToSave = ResData.SLGroupInfo
    
    this.setData({
      'PageOption.SLGroupProfile': SLGroupProfile_ToSave
    })
    // console.log("this.data.PageOption.SLGroupProfile:", this.data.PageOption.SLGroupProfile)
  },

  On_BindTap_GoTo_UniIOListCmdPage(e) {
    this.GoTo_UniIOListCmdPage()
  },

  GoTo_UniIOListCmdPage(e) {
    const Target_SLGroup_Id = this.data.PageOption.SLGroup_Id
    // console.log("Target_SLGroup_Id:", Target_SLGroup_Id)
    const Target_Url = `/pages/SmartLinkGroup/UniIOListCmd/index?SLGroupId=${Target_SLGroup_Id}`
    console.log("Target_Url:", Target_Url)

    wx.navigateTo({
      url: Target_Url,
      events: (e) => {
        console.log("e:", e)
      },
      success: (result) => {},
      fail: (res) => {},
      complete: (res) => {},
    })
  },

  

})