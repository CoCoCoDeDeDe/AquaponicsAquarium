// pages/SmartLinkGroup/UniIOListCmd.js
import { isValidNonEmptyString } from '../../../utils/common'
import { requestWithLafToken, on_laf_token_Invalid, on_request_error, on_db_error, on_param_error, on_unknown_error, on_common_error } from '../../../apis/laf'
Page({

  /**
   * 页面的初始数据
   */
  data: {
    PageOption: {
      SLGroup_Id: 'Default_SLGroup_Id',
      SLGroup_Name: '默认智联组名称',
      SLGroup_CreateTime: '',
      SLGroup_UpdateTime: '',
    },
    
    UniIOCardBtnList: [
      // {
      //   Img_Url: '/static/images/icons/icon_deviceDetail_line_dekBlue@2x.png',
      //   FunctionName: '查看',
      //   BindTapHandlerName: 'On_BindTap_Read',
      // },
      // {
      //   Img_Url: '/static/images/icons/icon_edit_line_darkBlue@2x.png',
      //   FunctionName: '修改',
      //   BindTapHandlerName: 'On_BindTap_Update',
      // },
      {
        Img_Url: '/static/images/icons/icon_delete_line_darkBlue@2x.png',
        FunctionName: '删除',
        BindTapHandlerName: 'On_BindTap_Delete',
      },
    ],
    UniIOProfileList: [
      {
        UniIO_Id: 'Default_UniIO_Id1',
        UniIO_Name: '默认的长的UniIO对外名称',
        Device_Id: 'Default_Device_Id1',
        Device_Name: '默认的长的设备名称',
        UniIO_Main_Color: '#1bbb1b',
        UniIOType: 'actor',
      },
      {
        UniIO_Id: 'Default_UniIO_Id2',
        UniIO_Name: '短的UniIO对外名称',
        Device_Id: 'Default_Device_Id2',
        Device_Name: '默认的很长很长的设备名称',
        UniIO_Main_Color: '#42a5f5',
        UniIOType: 'sensor',
      },
      {
        UniIO_Id: 'Default_UniIO_Id3',
        UniIO_Name: '默认的长的UniIO对外名称',
        Device_Id: 'Default_Device_Id3',
        Device_Name: '默认的长的设备名称',
        UniIO_Main_Color: '#f08705',
        UniIOType: 'actor',
      },
      {
        UniIO_Id: 'Default_UniIO_Id4',
        UniIO_Name: '默认的长的UniIO对外名称',
        Device_Id: 'Default_Device_Id4',
        Device_Name: '默认的长的设备名称',
        UniIO_Main_Color: '#f8d714',
        UniIOType: 'sensor',
      },
    ],
  },

  /**
   * 生命周期函数--监听页面加载
   */
  async onLoad(options) {
    console.log("options:", options)
    await this.setData({
      'PageOption.SLGroup_Id': options. SLGroup_Id
    })

    
    // 获取本 SLGroup 的 Profile
    const Res_GetNewSLGroupProfile = await this.GetNewSLGroupProfile()
    switch(Res_GetNewSLGroupProfile.errMsg) {
      case 'succeed':

        this.RefreshPage()
        break
      default:
        const Duration = 1500

        // 显示提示
        wx.showToast({
          title: Res_GetNewSLGroupProfile.errMsg,
          duration: Duration,
          icon: 'none',
          mask: false,
        })
        
        // showToast 结束后转主页
        setTimeout(() => {
          wx.reLaunch({
            url: '/pages/index/index',
          })
        }, Duration);
        break
    }

    
  },

  async RefreshPage() {
    // 获取本 SLGroup 的 UniIOProfileList
    await this.GetNewUniIOProfileList()

    // 设置导航栏标题
    await wx.setNavigationBarTitle({
      title: this.data.PageOption.SLGroup_Name,
    })
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
  async onPullDownRefresh() {
    
    const Res_GetNewSLGroupProfile = await this.GetNewSLGroupProfile()
    switch(Res_GetNewSLGroupProfile.errMsg) {
      case 'succeed':
        await this.GetNewUniIOProfileList()
        break
      default:
        const Duration = 1500
        wx.showToast({
          title: Res_GetNewSLGroupProfile.errMsg,
          duration: Duration,
          icon: 'none',
          mask: false,
        })
        // showToast 结束后转主页
        setTimeout(() => {
          wx.reLaunch({
            url: '/pages/index/index',
          })
        }, Duration);
        break
    }
    wx.stopPullDownRefresh()
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
  },

  // 事件：点击点添加UniIo按钮
  On_BindTap_Add(e) {
    this.GoTo_BindUniIOWithSLGroup()
  },

  // 携带本页智联组信息跳转到绑定 UniIO 与 智联组 的页面
  GoTo_BindUniIOWithSLGroup() {
    const Target_Url = `/pages/SmartLinkGroup/BindUniIOWithSLGroup/index?SLGroup_Id=${this.data.PageOption.SLGroup_Id}`
    wx.navigateTo({
      url: Target_Url,
    })
    console.log("navigateTo:", Target_Url)
  },

  async GetNewSLGroupProfile(e) {
    let ResData
    try{
      ResData = await requestWithLafToken('GET', '/iot2/smartLinkGroup/GetSmartLinkGroupInfo', {smartLinkGroup_id: this.data.PageOption.SLGroup_Id})
    } catch(err) {
      switch(err.runCondition) {
        case 'laf_token error':
          on_laf_token_Invalid()
          break
        default:
          on_common_error(err)
          break
      }
      return err
    }
    console.log("ResData:", ResData)

    let PageOption_ToSave = this.data.PageOption

    PageOption_ToSave.SLGroup_Name = ResData.SLGroupInfo.SLGroup_Name
    PageOption_ToSave.SLGroup_UpdateTime = ResData.SLGroupInfo.SLGroup_CreateTime
    PageOption_ToSave.SLGroup_UpdateTime = ResData.SLGroupInfo.SLGroup_UpdateTime

    this.setData({
      PageOption: PageOption_ToSave,
    })
    return {
      runCondition: 'succeed',
      errMsg: 'succeed',
    }
  },

  async GetNewUniIOProfileList(e) {
    
    let ResData
    try{
      ResData = await requestWithLafToken('GET', '/iot2/uniIO/GetUniIOCardProfileList', {smartLinkGroup_id: this.data.PageOption.SLGroup_Id})
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

    if(ResData.UniIODataList.length <= 0) {
      wx.showToast({
        title: '本智联组暂无 UniIO',
        duration: 1500,
        icon: 'none',
        mask: false,
      })
    }

    // 转换数据格式
    let UniIOProfileList_ForSave = []
    for(let i = 0; i < ResData.UniIODataList.length; i++) {
      UniIOProfileList_ForSave[i] = {
        UniIO_Id: ResData.UniIODataList[i].UniIO_Id,
        UniIO_Name: ResData.UniIODataList[i].UniIO_Name,
        Device_Id: ResData.UniIODataList[i].Device_Id,
        Device_Name: ResData.UniIODataList[i].Device_Name,
        UniIO_MainColor: ResData.UniIODataList[i].UniIO_MainColor,
        UniIO_TemplateName: ResData.UniIODataList[i].UniIO_TemplateName,
        UniIO_Type: ResData.UniIODataList[i].UniIO_Type,
      }
    }
    // console.log("UniIOList_ForSave:", UniIOList_ForSave)

    await this.setData({
      UniIOProfileList: UniIOProfileList_ForSave
    })

    wx.showToast({
      title: '获取 UniIO 列表成功',
      duration: 900,
      icon: 'none',
      mask: false,
    })

  }

})