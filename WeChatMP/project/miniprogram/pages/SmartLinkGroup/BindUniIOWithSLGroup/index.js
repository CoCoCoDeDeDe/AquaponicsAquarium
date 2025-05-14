// pages/SmartLinkGroup/BindUniIOWithSLGroup/index.js
import { isValidNonEmptyString } from '../../../utils/common'
import { requestWithLafToken, on_laf_token_Invalid, on_request_error, on_db_error, on_param_error, on_unknown_error, on_common_error } from '../../../apis/laf'
Page({

  /**
   * 页面的初始数据
   */
  data: {
    CtrlBarData: {
      BtnDataList: [
        {
          Title: '全选',
          Event_BindTap: 'On_BindTap_SelectAll',
          ColorSet: {
            Board_Bg: '#98d9f7',
          }
        },
        {
          Title: '全不选',
          Event_BindTap: 'On_BindTap_SelectAllNot',
          ColorSet: {
            Board_Bg: '#a5a5a5',
          }
        },
        {
          Title: '过滤已绑定',
          Event_BindTap: 'On_BindTap_FilterBound',
          ColorSet: {
            Board_Bg: '#bbbbbb',
          }
        },
        {
          Title: '刷新',
          Event_BindTap: 'On_BindTap_Refresh',
          ColorSet: {
            Board_Bg: '#ffec73',
          }
        },
      ]
    },

    ViewOption: {
      Scroll_Into_View_Target: '',
      Can_Submit: false,
      Is_Dragging: false,
    },

    SLGroupProfile:{
      SLGroup_Id: "Default_SLGroup_Id",
    },

    SLGroupProfile_Pairs: [
      {
        key: '目标智联组',
        value: 'Defult_SLGroup_Name',
      },
      {
        key: '已选数量',
        value: 9,
      },
      {
        key: '总数',
        value: 29,
      }
    ],

    ToTopBtnOption:{
      UniIO_Type: 'Top',
      UniIO_MainColor: '#ccc',
      UniIO_Name: '回到顶部',
      Device_Name: '',
      UniIO_Id: 'UniIO_Id',
      SmartLinkGroup_Name: '',
      Is_Selected: false,
      IsShow_SLGroup_Name: false,
    },

    UniIOCardBtnList: [],

    UniIOProfileList: [],

  },

  /**
   * 生命周期函数--监听页面加载
   */
  async onLoad(options) {

    // 无参错误处理
    if(!options.SLGroup_Id) {
      const Duration = 1500
      wx.showToast({
        title: '缺少页面参数',
        duration: Duration,
        icon: 'error',
        mask: true,
      })
      setTimeout(() => {
        wx.navigateBack({
          fail: (res) => {
            wx.reLaunch({
              url: '/pages/index/index',
            })
          },
        })
      }, Duration);
    }

    const { SLGroup_Id } = options
    this.setData({
      'SLGroupProfile.SLGroup_Id': SLGroup_Id,
    })

    // 获取 SLGroupProfile
    try{
      await this.GetSLGroupProfile({
        SLGroup_Id,
      })
    } catch(err) {
      return
    }

    this.PageInit()
  },

  async PageInit(e) {
    await this.GetUniIOProfileList()

    await this.RefreshOverView()
  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  async onReady() {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  async onShow() {

  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  async onHide() {

  },

  /**
   * 生命周期函数--监听页面卸载
   */
  async onUnload() {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  async onReachBottom() {

  },

  /**
   * 用户点击右上角分享
   */
  async onShareAppMessage() {

  },

  // 获取本页面目标智联组的信息
  async GetSLGroupProfile(e) {
    const { SLGroup_Id } = e
    // console.log("GetSLGroupProfile SLGroup_Id:", SLGroup_Id)

    return new Promise(async (resolve, reject) => {
      let Res_RequestUniIOProfile
    try{
      Res_RequestUniIOProfile = await requestWithLafToken('GET', '/iot2/smartLinkGroup/GetSmartLinkGroupInfo', {
        smartLinkGroup_id: SLGroup_Id,
      })
    } catch(err) {
      switch(err.runCondition) {
        case 'laf_token error':
          on_laf_token_Invalid()
          break
        case 'not found':
          // 页面参数无效 错误处理
          const Duration = 1500
          wx.showToast({
            title: '页面参数无效',
            duration: Duration,
            icon: 'error',
            mask: true,
          })
          setTimeout(() => {
            wx.navigateBack({
              fail: (res) => {
                wx.reLaunch({
                  url: '/pages/index/index',
                })
              },
            })
          }, Duration);
          break
        default:
          on_common_error(err)
          break
      }
      reject(err)
      return
    }

    // 处理参数
    try{
      const {
        SLGroup_Id,
        SLGroup_Name,
        SLGroup_CreateTime,
        SLGroup_UpdateTime,
      } = Res_RequestUniIOProfile.SLGroupInfo

      this.setData({
        'SLGroupProfile_Pairs[0].value': SLGroup_Name,
      })

    } catch(err) {
      console.log("处理参数 错误 err:", err)
      reject()
    }

    resolve()
    })
  },

  // 获取UniIOProfile列表
  async GetUniIOProfileList(e) {

    let Res_RequestUniIOProfileList
    try{

      Res_RequestUniIOProfileList = await requestWithLafToken('GET', '/iot2/uniIO/GetUniIOCardProfileList')

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

    // console.log("Res_RequestUniIOProfileList:", Res_RequestUniIOProfileList)

    // 处理数据
    for(let i = 0; i < Res_RequestUniIOProfileList.UniIODataList.length; i++) {
      Res_RequestUniIOProfileList.UniIODataList[i].Is_Selected = false
      Res_RequestUniIOProfileList.UniIODataList[i].IsShow_SLGroup_Name = true
    }
    // console.log("Res_RequestUniIOProfileList:", Res_RequestUniIOProfileList)

    this.setData({
      UniIOProfileList: Res_RequestUniIOProfileList.UniIODataList
    })

  },

// 点击卡片的事件
  async On_BindTap_UniIOProfileCard(e) {
    const { UniIO_Id } = e.currentTarget.dataset.uniioprofile;
    
    // 查找目标项索引
    const index = this.data.UniIOProfileList.findIndex(item => item.UniIO_Id === UniIO_Id);
    
    if (index !== -1) {
      // 获取当前选中状态
      const isSelected = this.data.UniIOProfileList[index].Is_Selected;
      
      // 直接更新特定路径的属性（触发 observers）
      this.setData({
        [`UniIOProfileList[${index}].Is_Selected`]: !isSelected,
      });
      
      // console.log(`UniIO ${UniIO_Id} 的选中状态已更新为:`, !isSelected);
    } else {
      // console.warn('未找到匹配的 UniIO_Id:', UniIO_Id);
      wx.showToast({
        title: 'internal error 未找到匹配的 UniIO_Id',
        duration: 1500,
        icon: 'none',
      })
      return
    }

    // 更新总览数据
    this.RefreshOverView()
  },

  // 刷新总览（包括控制按钮状态）
  async RefreshOverView() {
    // 计算页面 UniIO 总数
    const Count_UniIO_Total = this.data.UniIOProfileList.length
    // 计算已选 UniIO 总数
    const Count_UniIO_Selected = this.data.UniIOProfileList.reduce((count, item) => {
      return item.Is_Selected ? count + 1 : count
    }, 0)
    // console.log("Count_UniIO_Selected:", Count_UniIO_Selected)

    // 存储到响应式数据
    this.setData({
      'SLGroupProfile_Pairs[1].value': Count_UniIO_Selected,
      'SLGroupProfile_Pairs[2].value': Count_UniIO_Total,
    })

    // 刷新提交按钮状态
    let Can_Submit
    if(Count_UniIO_Selected > 0) {
      Can_Submit = true
    } else{
      Can_Submit = false
    }
    this.setData({
      'ViewOption.Can_Submit': Can_Submit,
    })

  },

  // 获取已选的UniIOProfile列表
  async GetSelectedUniIOProfileList() {
    const Result = this.data.UniIOProfileList.filter((item, idx, arr) => {
      return item.Is_Selected === true
    }, this)
    // console.log("GetSelectedUniIOProfileList:", Result)
    return Result
  },

  // CtrlBar 表单控制
  async On_BindTap_SelectAll() {
    const UniIOProfileList_SelectAdd = this.data.UniIOProfileList.map(((item, idx, arr) => {
      let NewItem = item
      NewItem.Is_Selected = true
      return NewItem
    }), this)
    this.setData({
      UniIOProfileList: UniIOProfileList_SelectAdd
    })
    this.RefreshOverView()
  },

  // CtrlBar 表单控制
  async On_BindTap_SelectAllNot() {
    const UniIOProfileList_SelectAddNot = this.data.UniIOProfileList.map(((item, idx, arr) => {
      let NewItem = item
      NewItem.Is_Selected = false
      return NewItem
    }), this)
    this.setData({
      UniIOProfileList: UniIOProfileList_SelectAddNot
    })
    this.RefreshOverView()
  },
  
  // CtrlBar 表单控制
  async On_BindTap_FilterBound() {
    // 获取本页目标智联组已绑定的UniIO列表
    let Res_RequestUniIOListBound
    try{
      Res_RequestUniIOListBound = await requestWithLafToken('GET', '/iot2/uniIO/GetUniIOList', {
        smartLinkGroup_id: this.data.SLGroupProfile.SLGroup_Id
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
    const { 
      uniIOList = [],
     } = Res_RequestUniIOListBound

    // 遍历本页展示的所有UniIO，如果有存在于本页目标智联组已绑定的UniIO列表中的则将其删除（或者隐藏（可能要加新属性））
    // 如果该UniIO与已绑定的UniIO列表中有匹配则过滤 true，反之 false
    const UniIOProfileList_Filtered = this.data.UniIOProfileList.filter((item1, idx, arr) => {
      // Count_Mattched 代表当前遍历的页面中的某个 UniIO 与已经和目标智联组绑定了的 UniIO 列表的匹配情况，0 是无匹配，其他是匹配
      const Count_Mattched = uniIOList.reduce((count, item2)=> {
        // 匹配则计数+1，不匹配则计数不变
        return item1.UniIO_Id == item2._id ? count + 1 : count
      }, 0)
      // 匹配了则 false保留，无匹配(0)则 true保留
      return Count_Mattched == 0 ? true : false
    }, this)

    // 保存与应用过滤
    // 修改UniIOProfileList应与this.RefreshOverView()绑定
    this.setData({
      UniIOProfileList: UniIOProfileList_Filtered,
    })
    this.RefreshOverView()
  },

  // CtrlBar 表单控制
  async On_BindTap_Refresh() {
    this.PageInit()
  },

  // 按下解禁的submit按钮
  async On_BindTap_Submit() {
    console.log("On_BindTap_Submit")
    this.SubmitSelectedUniIOProfileList()
  },
  
  // 提交已选UniIO列表，请求绑定
  async SubmitSelectedUniIOProfileList() {
    // 获取已选表
    const SelectedUniIOProfileList = await this.GetSelectedUniIOProfileList()
    // console.log("SelectedUniIOProfileList:", SelectedUniIOProfileList)

    // 校验已选表，逻辑上不会为空，因为为空时不绑定点击事件
    if(SelectedUniIOProfileList.length <= 0) {
      wx.showToast({
        title: '未选择 UniIO',
        duration: 1500,
        icon: 'none',
        mask: false,
      })
      return  // 退出
    }

    // 整理数据格式
    const UniIOList_ToPost = SelectedUniIOProfileList.map((item, idx, arr) => {
      return {
        _id: item.UniIO_Id
      }
    }, this)
    // console.log("UniIOList_ToPost:", UniIOList_ToPost)

    // 请求
    try{
      // 漏掉 await 会导致 catch 无法捕捉错误
      await requestWithLafToken('POST', '/iot2/smartLinkGroup/BindUniIOWithSLGroup', {
        SLGroup_Id: this.data.SLGroupProfile.SLGroup_Id,
      },
      {
        UniIOList: UniIOList_ToPost,
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

    // 刷新页面。包括已选数量、每个UniIOPRofileCard的绑定的智联组信息
    this.PageInit()
  },

  // 事件：点击回到顶部按钮
  async On_BindTap_ToTop(e) {

    this.setData({
      'ViewOption.Scroll_Into_View_Target': 'top',
    }, () => {
      console.log("this.data.ViewOption.Scroll_Into_View_Target", this.data.ViewOption.Scroll_Into_View_Target)
      
      // 使用createSelectorQuery确认元素是否存在
      const query = wx.createSelectorQuery()
      query.select('#top').boundingClientRect()
      query.select('.ScrollView').boundingClientRect()
      query.exec(res => {
        if (res[0]) {
          // console.log('找到目标元素，位置:', res[0])
          // console.log('scroll-view位置:', res[1])
        } else {
          console.log('未找到目标元素')
        }
      })
    })
    
  },

  // 事件：开始拖拽页面
  async OnScrollView_DragStart(e) {
    // console.log("OnScrollView_DragStart")
    this.setData({
      'ViewOption.Is_Dragging': true,
    })
  },

  // 事件：拖拽页面结束
  async OnScrollView_DragEnd(e) {
    // console.log("OnScrollView_DragEnd")
    this.setData({
      'ViewOption.Is_Dragging': false,
    })
  },

})