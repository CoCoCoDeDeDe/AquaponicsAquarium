// pages/ChatAI/Conversation/index.js

import { formatUnixTime_Type1, formatUnixTime_Type2, convertObjToArray, remainInArray } from "../../../utils/common"
import { requestWithLafToken, on_laf_token_Invalid, on_common_error, requestBotInfo, requestConversationList, requestConversationMessageList, requestConversationRetrive, requestDeleteMessage } from "../../../apis/laf"

const StyleDefaultValues = {
  topbar_TopDistance: 4,
  btmbar_BtmDistance: 16
}

Page({

  /**
   * 页面的初始数据
   */
  data: {
    conversation_info: {

    },
    ScrollIntoView: "",
    styles: {
      topbar_TopDistance: StyleDefaultValues.topbar_TopDistance,
      btmbar_BtmDistance: StyleDefaultValues.btmbar_BtmDistance,
    },
    input_info: {
      Value: ''
    },
    role_map: {
      "assistant": {
        avatar_url: "https://lf9-appstore-sign.oceancloudapi.com/ocean-cloud-tos/FileBizType.BIZ_BOT_ICON/2094992681874336_1752332578980342463_pSvshsBxqi.png?lk3s=50ccb0c5&x-expires=1753280953&x-signature=GOGfa0EIm3Q1TeNW5a9ArzQfz88%3D"
      },
      "user": {
        avatar_url: "/static/images/icons/defaultAvatar_violet.png"
      }
    },
    message_info: {
      MessageList: [
        {
          bot_id: "7525815471376465929",
          chat_id: "7527601781916926006",
          content: "",
          content_type: "text",
          conversation_id: "7527585244585803817",
          created_at: 1752656372,
          id: "7527601796865343551",
          meta_data: {
          },
          reasoning_content: "",
          role: "assistant",
          section_id: "7527585244585803817",
          type: "answer",
          updated_at: 1752656374
        }
      ]
    }
  },

  /**
   * 生命周期函数--监听页面加载
   */
  async onLoad(options) {
    try{
      // 获取和校验页面 query 参数 id
      const { id } = options
      if(!id) {
        // 参数缺失，退出本页面
        console.log("参数缺失，退出本页面")
        wx.navigateBack(
          {
            delta: 1
          }
        )
        return
      } else {
        this.setData({
          'conversation_info.id': id
        })
      }

      // 获取在线 bot 数据
      const res_requestBotInfo = await requestBotInfo()
      this.setData({
        bot_info: res_requestBotInfo
      })

      // 获取在线会话消息
      const res_requestConversationRetrive = await requestConversationRetrive({conversation_id: this.data.conversation_info.id})
      console.log("res_requestConversationRetrive:", res_requestConversationRetrive)
      await this.setData({
        conversation_info: {
          ...this.data.conversation_info,
          ...res_requestConversationRetrive
        }
      })
      console.log("this.data.conversation_info:", this.data.conversation_info)
      
      // 获取在线消息列表
      await this.refreshConversationMessageList()
      
      // 在消息列表更新后让消息页面滑动到最新的一个消息
      this.viewIntoMessage({ message_id: this.data.message_info.MessageList[this.data.message_info.MessageList.length - 1].id })


    } catch(err) {
      console.log("onLoad(options) err:", err)
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

  },

  // 开始滑动
  On_ScrollView_DragStart: function () {
    this.setData({
      "styles.topbar_TopDistance": -26,
      "styles.btmbar_BtmDistance": -30,
    })
  },

  // 结束滑动
  On_ScrollView_DragEnd: function () {
    this.setData({
      "styles.topbar_TopDistance": StyleDefaultValues.topbar_TopDistance,
      "styles.btmbar_BtmDistance": StyleDefaultValues.btmbar_BtmDistance,
    })
  },

  // 点击返回上一页按钮
  onTapNavigateBackBtn: function() {
    try{
      wx.navigateBack({
        delta: 1,
        success: (res) => {
          // console.log("navigateBack success() res", res)
        },
        fail: (res) => {
          console.log("navigateBack fail() res", res)
          wx.showToast({
            title: res.errMsg,
            duration: 1000,
            icon: 'none',
            mask: false,
          })
        },
        complete: (res) => {
          // console.log("navigateBack complete() res", res)
        },
      })
    } catch(err) {
      console.log("onTapNavigateBackBtn() err:", err)
    }
  },

  // 刷新消息列表
  refreshConversationMessageList: async function() {
    const res_requestConversationMessageList = await requestConversationMessageList(
      {
        conversation_id: this.data.conversation_info.id,
        order: 'asc',
        before_id: undefined,
        after_id: undefined,
        limit: 50
      }
    )
    console.log("res_requestConversationMessageList:", res_requestConversationMessageList)
    this.setData({
      'message_info.MessageList': res_requestConversationMessageList
    })
  },

  // 让视野滑动到指定消息
  viewIntoMessage: async function( options = { message_id: '' } ) {
    try{
      // 1. 校验参数：确保 message_id 是有效的非空字符串
      if (!options || typeof options !== 'object') {
        console.warn('viewIntoMessage: 无效的参数格式，需传入对象');
        return;
      }
      const { message_id } = options;
      if (typeof message_id !== 'string' || message_id.trim().length === 0) {
        console.warn('viewIntoMessage: message_id 必须是非空字符串');
        return;
      }

      this.setData({
        ScrollIntoView: "message_" + options.message_id
      })
    } catch(err) {
      console.log("viewIntoMessage() err:", err)
    }
  },
  
  // 点击指定消息消息板
  onTapMessageBoard: function(options) {
    try{
      // 获取点击数据
      const { message_id } = options.currentTarget.dataset

    } catch(err) {
      console.log("onTapMessageBoard() err:", err)
    }
  },

  // 长按指定消息消息板
  onLongPressMessageBoard: function(options) {
    try{
      // console.log("onLongPressMessageBoard() options:", options)
      // 获取长按数据
      const { message_id, message_idx } = options.currentTarget.dataset

      // 弹出是否删除消息框
      wx.showModal({
        title: '是否删除该消息',
        content: '删除后将无法恢复',
        showCancel: true,
        cancelText: '取消',
        cancelColor: '#181441',
        confirmText: '删除',
        confirmColor: '#e75247',
        editable: false,
        placeholderText: '',
        complete: (res) => {
          if (res.cancel) {
            return
          }
      
          if (res.confirm) {
            // 执行删除目标消息
            this.deleteMessage( { message_id, next_message_id: this.data.message_info.MessageList[message_idx + 1].id } )
          }
        }
      })  // showModal

    } catch(err) {
      console.log("onLongPressMessageBoard() err:", err)
    }
  },

  // 删除指定消息
  deleteMessage: async function( options = { message_id, next_message_id } ) {
    try{
      // 获取目标 id
      const { message_id, next_message_id } = options

      // 发出删除请求
      const res_requestDeleteMessage = await requestDeleteMessage(
        {
          conversation_id: this.data.conversation_info.id,
          message_id
        }
      )
      console.log("res_requestDeleteMessage:", res_requestDeleteMessage)

      // 提示用户已经成功删除
      wx.showToast({
        title: `成功删除消息 ${message_id}`,
        icon: 'success',
        duration: 1000,
        mask: false,
      })

      // 刷新本地消息列表
      this.refreshConversationMessageList()

      // 让视野滑动到上次删除的消息的下一个消息
      this.viewIntoMessage( { message_id: next_message_id } )

    } catch(err) {
      console.log("onLongPressMessageBoard() err:", err)
    }
  }

})