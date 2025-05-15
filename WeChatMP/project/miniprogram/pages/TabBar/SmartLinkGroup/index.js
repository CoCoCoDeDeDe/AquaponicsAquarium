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
        BindTapHandlerName: 'On_BindTap_Read_SLGroup',
      },
      {
        Img_Url: '/static/images/icons/icon_edit_line_darkBlue@2x.png',
        FunctionName: '修改',
        BindTapHandlerName: 'On_BindTap_Update_SLGroup',
      },
      {
        // /static/images/icons/icon_unbind_line_active.png
        Img_Url: '/static/images/icons/icon_unbind_line_active.png',
        FunctionName: '删除',
        BindTapHandlerName: 'On_BindTap_Delete_SLGroup',
      },
    ],

    Is_GroupCmdCard_Show: true,

    PageOption: {
      SLGroupProfile: {
        SLGroup_Id: 'Default_SLGroup_Id',
        SLGroup_Name: '默认智联组名称',
        SLGroup_CreateTime: '',
        SLGroup_UpdateTime: '',
      },
    },

    SLG_EditPop_Options: {  // 双向绑定
      IsShow_Component: false,
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
      UniIODataList: [],
    })
    // console.log("options:", options)

    // 当前浏览的 SLGroup 默认由页面参数指定, 若无页面参数则默认用智联组表中第一个或者权重第一的或者标为重要的智联组
    // 获取用户的智联组简介表
    await this.GetSLGroupProfileList() // Debug Delete

    // 指定当前在浏览的智联组的页面参数
    // console.log("options.SLGroup_Id:", options.SLGroup_Id)
    if(this.data.SLGroupProfileList.length > 0) {

      let Is_QueryValid = false

      if(options.SLGroup_Id === undefined || options.SLGroup_Id === '' || options.SLGroup_Id === ' ' || options.SLGroup_Id === 'undefined') {
        // console.log("无传入 options.SLGroup_Id:", options.SLGroup_Id)
        // Is_QueryValid = false  // 默认 false
      } else{
        // console.log("有传入 options.SLGroup_Id:", options.SLGroup_Id)

        // 校验传入的 SLGroup_Id 是否存在于刚刚获得的该用户的 SLGroup 列表中
        let Mattched_SLGroup_Id_Count = 0
        // console.log("传入的 SLGroup_Id", options.SLGroup_Id)
        for(let i = 1; i < this.data.SLGroupProfileList.length; i++) {
          // console.log(i, "轮遍历中", this.data.SLGroupProfileList[i].SLGroup_Id)
          if(options.SLGroup_Id == this.data.SLGroupProfileList[i].SLGroup_Id) {
            Mattched_SLGroup_Id_Count++
          }
        }
        console.log("页面 Query 参数的匹配情况 Mattched_SLGroup_Id_Count:", Mattched_SLGroup_Id_Count)
        if(Mattched_SLGroup_Id_Count == 1) {
          Is_QueryValid = true
        } else{
          // Is_QueryValid = false  // 默认 false
        }
      }

      if(Is_QueryValid) {
        console.log("Query 的 SLGroup_Id 有效 options.SLGroup_Id:", options.SLGroup_Id)
        await this.setData({
          'PageOption.SLGroupProfile.SLGroup_Id': options.SLGroup_Id
        })
      } else{
        console.log("Query 的 SLGroup_Id 无效 options.SLGroup_Id:", options.SLGroup_Id)
        await this.setData({
          'PageOption.SLGroupProfile.SLGroup_Id': this.data.SLGroupProfileList[0].SLGroup_Id,
        })
      }
      // 至此，当前页面已有确定的正在浏览的 SLGroup_Id
      this.RefreshPage()

    } else{ // 显示 智联组 为空时的提示
      // 若获取的该用户的智联组表为空，则本也为空智联组状态，很多智联组不为空时的操作和函数不执行或显示表为空的提示

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

  async RefreshPage(e) {
    // 获取用户的智联组简介表
    await this.GetSLGroupProfileList()

    if(this.data.SLGroupProfileList.length > 0) {
      // 用户有智联组才执行

      // 清除旧定时器（没有就没有）
      await clearInterval(this.data.timer);

      // 获取本 SLGroup 的 Profile
      await this.GetNewSLGroupProfile()

      // 获取本 SLGroup 的所有的 UniIO 的 UniIODataList
      await GetUniIODataList(this, {smartLinkGroup_id: this.data.PageOption.SLGroupProfile.SLGroup_Id})
      
      // 用定时器重复获取新 UniIODataList
      this.data.timer = setInterval(() => {
        GetUniIODataList(this, {smartLinkGroup_id: this.data.PageOption.SLGroupProfile.SLGroup_Id})
      }, 10000)
    
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

  // 获取智联组简介列表
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

    // 整理数据
    let SLGroupProfileList_ToSave = ResData.SLGroupProfileList
    
    this.setData({
      SLGroupProfileList: SLGroupProfileList_ToSave
    })
  },

  // 获取本也智联组简介
  async GetNewSLGroupProfile(e) {
    let ResData
    try{
      ResData = await requestWithLafToken('GET', '/iot2/smartLinkGroup/GetSmartLinkGroupInfo', {smartLinkGroup_id: this.data.PageOption.SLGroupProfile.SLGroup_Id})
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

    // 整理数据
    let SLGroupProfile_ToSave = ResData.SLGroupInfo
    
    this.setData({
      'PageOption.SLGroupProfile': SLGroupProfile_ToSave
    })
    // console.log("this.data.PageOption.SLGroupProfile:", this.data.PageOption.SLGroupProfile)
  },

  // 事件：点击去往管理 UniIO 列表的页面
  On_BindTap_GoTo_UniIOListCmdPage(e) {
    this.GoTo_UniIOListCmdPage()
  },

  // 去往管理 UniIO 列表的页面
  GoTo_UniIOListCmdPage(e) {
    const Target_SLGroup_Id = this.data.PageOption.SLGroupProfile.SLGroup_Id
    // console.log("Target_SLGroup_Id:", Target_SLGroup_Id)
    const Target_Url = `/pages/SmartLinkGroup/UniIOListCmd/index?SLGroup_Id=${Target_SLGroup_Id}`
    // console.log("Target_Url:", Target_Url)

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

  // 事件：点击新建智联组
  async On_BindTap_Create_SLGroup(e) {
    wx.showModal({
      cancelColor: '#aaa',
      cancelText: '取消',
      confirmColor: '#9cd6f1',
      confirmText: '新增',
      content: '',
      editable: true,
      placeholderText: '请输入新智联组的名称',
      showCancel: true,
      title: `新增智联组`,
      success: async (result) => {
        if(result.cancel) {
          return
        }
        if(result.confirm) {
          // console.log("result:", result)
          this.Create_SLGroup(result.content)
        }
      },
      fail: (res) => {},
      complete: (res) => {},
    })
  },

  // 新建智联组
  async Create_SLGroup(Para_SLGroup_Name) {
    // console.log("新智联组名称参数 Para_SLGroup_Name:", Para_SLGroup_Name)
    
    let Target_SLGroup_Name

    // 校验参数
    if(VerifyIptString(Para_SLGroup_Name)) {
      // console.log("新智联组名称有效 Target_SLGroup_Name:", Target_SLGroup_Name)
      Target_SLGroup_Name = Para_SLGroup_Name
    } else{
      // console.log("新智联组名称无效 Target_SLGroup_Name:", Target_SLGroup_Name)
      // 提示
      wx.showToast({
        title: `新智联组名称无效: ${Target_SLGroup_Name}`,
        duration: 1500,
        icon: 'none',
        mask: false,
      })
      // 结束
      return
    }

    // 请求 API
    let Res_Create_SLGroup
    try{
      Res_Create_SLGroup = await requestWithLafToken('GET', '/iot2/smartLinkGroup/CreateSmartLinkGroup', 
        {
          name: Target_SLGroup_Name,
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
    console.log("Res_Create_SLGroup:", Res_Create_SLGroup)
    const New_SLGroup_Id = Res_Create_SLGroup.SLGroup_Id

    // 新增成功后刷新页面，现在默认进入到新建的智联组的页面
    // 设置本页面在浏览的智联组id为新建的智联组的id
    this.setData({
      'PageOption.SLGroupProfile.SLGroup_Id': New_SLGroup_Id,
    })
    // 刷新本页面全部
    this.RefreshPage()

  },

  // 事件：点击 查看点击的智联组
  On_BindTap_Read_SLGroup(e) {
    // console.log("e:", e)
    const Target_SLGroup_Id = e.currentTarget.dataset.slgroupprofile.SLGroup_Id

    this.GoTO_SLGroup(Target_SLGroup_Id)
    
  },

  // 查看点击的智联组
  GoTO_SLGroup(Target_SLGroup_Id) {

    const Target_Url = `/pages/TabBar/SmartLinkGroup/index?SLGroup_Id=${Target_SLGroup_Id}`

    wx.reLaunch({
      url: Target_Url,
      success: (res) => {},
      fail: (res) => {},
      complete: (res) => {},
    })

  },

  // 事件：点击 修改智联组
  async On_BindTap_Update_SLGroup(e) {
    const Target_SLGroupProfile = e.currentTarget.dataset.slgroupprofile
    // console.log("Target_SLGroupProfile:", Target_SLGroupProfile)
    // show SLG-EditPop
    // 暂时废弃
    // this.setData({
    //   'SLG_EditPop_Options.IsShow_Component': true,
    // })

    // 显示输入弹窗
    wx.showModal({
      cancelColor: '#aaa',
      cancelText: '取消',
      confirmColor: '#9dd7f1',
      confirmText: '确认',
      content: Target_SLGroupProfile.SLGroup_Name,
      editable: true,
      placeholderText: '请输入新名称',
      showCancel: true,
      title: `为 ${Target_SLGroupProfile.SLGroup_Name} 更改名称`,
      success: async (result) => {
        if(result.cancel) {
          return
        }
        if(result.confirm) {
          this.Update_SLGroup(Target_SLGroupProfile, result.content)
        }
      },
      fail: (res) => {},
      complete: (res) => {},
    })

  },

  // 修改智联组
  async Update_SLGroup(Target_SLGroupProfile, SLGroup_NewName) {
    // 校验参数
    if(VerifyIptString(SLGroup_NewName)) {
      // console.log("新智联组名称有效 Target_SLGroup_Name:", Target_SLGroup_Name)
      // 继续
    } else{
      // console.log("新智联组名称无效 Target_SLGroup_Name:", Target_SLGroup_Name)
      // 提示
      wx.showToast({
        title: `新智联组名称无效: ${Target_SLGroup_Name}`,
        duration: 1500,
        icon: 'none',
        mask: false,
      })
      // 结束
      return
    }

    // 请求 API
    console.log("SLGroup_NewName:", SLGroup_NewName)
    let ResData
    try{
      ResData = await requestWithLafToken('POST', '/iot2/smartLinkGroup/EditSLGroup', 
      {
        SLGroup_Id: Target_SLGroupProfile.SLGroup_Id
      }, 
      {
        SLGroup_Name: SLGroup_NewName,
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

    // 刷新页面
    if(this.data.PageOption.SLGroupProfile.SLGroup_Id == ResData.Target_SLGroup_Id) {
      // 情况1: 改名的是当前页面浏览的智联组, 刷新整页
      // 重新进入TabBar智联组页面，传如入原智联组参数
      const Target_Url = `/pages/TabBar/SmartLinkGroup/index?SLGroup_Id=${this.data.PageOption.SLGroupProfile.SLGroup_Id}`
      wx.reLaunch({
        url: Target_Url,
        success: (res) => {},
        fail: (res) => {},
        complete: (res) => {},
      })
    } else{
      // 重新获取用户的智联组简介表
      await this.GetSLGroupProfileList() // Debug Delete
    }
  },

  // 事件：点击 删除智联组
  async On_BindTap_Delete_SLGroup(e) {
    // console.log("e:", e)
    const Target_SLGroupProfile = e.currentTarget.dataset.slgroupprofile
    // console.log("Target_SLGroupProfile:", Target_SLGroupProfile)
    const Target_IsCurrentSLGroup = Target_SLGroupProfile.SLGroup_Id === this.data.PageOption.SLGroupProfile.SLGroup_Id
    // modal
    wx.showModal({
      cancelColor: '#aaa',
      cancelText: '取消',
      confirmColor: '#c41a16',
      confirmText: '删除',
      content: '其下所有 UniIO 将不再绑定该智联组，并且智联组下所有配置将被删除',
      editable: false,
      placeholderText: 'placeholderText',
      showCancel: true,
      title: `是否确认删除智联组 ${Target_SLGroupProfile.SLGroup_Name}`,
      success: async (result) => {
        if(result.cancel) {
          return
        }
        if(result.confirm) {
          this.Delete_SLGroup(Target_SLGroupProfile, Target_IsCurrentSLGroup)
        }
      },
      fail: (res) => {},
      complete: (res) => {},
    })
    
  },

  // 删除智联组
  async Delete_SLGroup(Target_SLGroupProfile, Target_IsCurrentSLGroup) {
    // console.log("Delete_SLGroup Target_SLGroupProfile:", Target_SLGroupProfile)
    // console.log("Delete_SLGroup Target_IsCurrentSLGroup:", Target_IsCurrentSLGroup)

    // 请求删除智联组的 API
    let ResData
    try{
      ResData = await requestWithLafToken('GET', '/iot2/smartLinkGroup/DeleteSmartLinkGroup', {SLGroup_Id: Target_SLGroupProfile.SLGroup_Id})
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

    // 显示删除成功的提示
    // Bug: 参数当前浏览的智联组时弹窗不显示
    wx.showToast({
      title: `删除 ${Target_SLGroupProfile.SLGroup_Name} 成功`,
      duration: 1500,
      icon: 'none',
      mask: false,
      success: (res) => {},
      fail: (res) => {},
      complete: (res) => {},
    })

    // 删除后刷新页面
    // 刷新页面全部
    // 分为两种删除情况：1. 删除本页面在浏览的智联组，2.删除不是本页面在浏览的智联组
    if(Target_IsCurrentSLGroup) { // 1. 删除本页面在浏览的智联组
      // 重新进入TabBar智联组页面，不传入目标智联组参数
      // const Target_Url = `/pages/TabBar/SmartLinkGroup/index`
      // wx.reLaunch({
      //   url: Target_Url,
      //   success: (res) => {},
      //   fail: (res) => {},
      //   complete: (res) => {},
      // })

      this.onLoad()

    } else{ // 2.删除不是本页面在浏览的智联组

      // 重新获取用户的智联组简介表
      await this.GetSLGroupProfileList() // Debug Delete
    }

  }
  

})


// 校验输入框输入的字符串是否有效
function VerifyIptString(Target_Str) {
    let Tmp_Str
    // console.log("typeof Para_SLGroup_Name:", typeof Para_SLGroup_Name)
    if(typeof Target_Str === 'number') {
      Tmp_Str = Target_Str.toString()
    } else{
      Tmp_Str = Target_Str
    }
    // 校验字符串
    if(Tmp_Str === undefined || Tmp_Str === null || typeof Tmp_Str !== 'string' || Tmp_Str.trim() === '') {
      return false
    } else{
      return true
    }
}