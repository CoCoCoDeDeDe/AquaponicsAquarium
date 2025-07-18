// pages/ChatAI/Conversation/index.js

import { formatUnixTime_Type1, formatUnixTime_Type2, convertObjToArray, remainInArray } from "../../../utils/common"
import { requestWithLafToken, on_laf_token_Invalid, on_common_error, requestBotInfo, requestConversationList, requestConversationMessageList } from "../../../apis/laf"

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
          chat_id: "7527602655481593866",
          content: "哎呀，我没办法直接获取你现在位置的经纬度呢😅 不过要是你在基元智联平台上关联了能定位的设备，你把平台token给我，我就能用相关功能找找看设备信息里有没有位置数据，说不定就能知道你的位置经纬度啦。快把token给我，咱们一起试试看呀！",
          content_type: "text",
          conversation_id: "7527585244585803817",
          created_at: 1752656573,
          id: "7527602662280544265",
          meta_data: {
          },
          reasoning_content: "",
          role: "assistant",
          section_id: "7527585244585803817",
          type: "answer",
          updated_at: 1752656575
        },
        {
          bot_id: "",
          chat_id: "7527602655481593866",
          content: "我现在位置的经纬度是多少",
          content_type: "text",
          conversation_id: "7527585244585803817",
          created_at: 1752656572,
          id: "7527602657704280073",
          meta_data: {
            Key_1: ""
          },
          reasoning_content: "",
          role: "user",
          section_id: "7527585244585803817",
          type: "question",
          updated_at: 1752656572
        },
        {
          bot_id: "7525815471376465929",
          chat_id: "7527602431849611306",
          content: "我没办法查看对话请求里extra_params的数据哟😔 你快把基元智联平台的token给我，我这就带着它去调用合适的功能，好好帮你管理平台上的设备和资源，就像超级管家一样把一切安排得妥妥当当！ ",
          content_type: "text",
          conversation_id: "7527585244585803817",
          created_at: 1752656521,
          id: "7527602436631216170",
          meta_data: {
          },
          reasoning_content: "",
          role: "assistant",
          section_id: "7527585244585803817",
          type: "answer",
          updated_at: 1752656525
        },
        {
          bot_id: "7525815471376465929",
          chat_id: "7527602431849611306",
          content: "你能查看对话请求中extra_params的数据吗",
          content_type: "text",
          conversation_id: "7527585244585803817",
          created_at: 1752656520,
          id: "7527602431849644074",
          meta_data: {
            Key_1: ""
          },
          reasoning_content: "",
          role: "user",
          section_id: "7527585244585803817",
          type: "question",
          updated_at: 1752656520
        },
        {
          bot_id: "7525815471376465929",
          chat_id: "7527602200395202612",
          content: "我没办法直接查看对话请求中parameters的数据呢🤔 你直接把基元智联平台的token告诉我就好啦。有了这个关键的token，我就能访问相关的插件和API，帮你管理设备和资源啦，就像拿到了宝藏大门的钥匙一样！",
          content_type: "text",
          conversation_id: "7527585244585803817",
          created_at: 1752656468,
          id: "7527602209526284340",
          meta_data: {
          },
          reasoning_content: "",
          role: "assistant",
          section_id: "7527585244585803817",
          type: "answer",
          updated_at: 1752656470
        },
        {
          bot_id: "",
          chat_id: "7527602200395202612",
          content: "你能查看对话请求中parameters的数据吗",
          content_type: "text",
          conversation_id: "7527585244585803817",
          created_at: 1752656466,
          id: "7527602200395235380",
          meta_data: {
            Key_1: ""
          },
          reasoning_content: "",
          role: "user",
          section_id: "7527585244585803817",
          type: "question",
          updated_at: 1752656466
        },
        {
          bot_id: "7525815471376465929",
          chat_id: "7527601953019576371",
          content: "我还不知道你的秘密呢😜 不过要是你愿意把基元智联平台的token给我，我就能通过它开启一些“小调查”，去获取你在平台上的用户信息、设备信息啥的。说不定在这个过程中，能发现一些关于你在平台上的“小秘密”哦。快把token分享给我吧，咱们一起去探索探索！",
          content_type: "text",
          conversation_id: "7527585244585803817",
          created_at: 1752656411,
          id: "7527601963467374618",
          meta_data: {
          },
          reasoning_content: "",
          role: "assistant",
          section_id: "7527585244585803817",
          type: "answer",
          updated_at: 1752656414
        },
        {
          bot_id: "",
          chat_id: "7527601953019576371",
          content: "你知道我的秘密吗",
          content_type: "text",
          conversation_id: "7527585244585803817",
          created_at: 1752656409,
          id: "7527601954919301170",
          meta_data: {
            Authorization: "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1aWQiOiI2ODE0NzBhYzY1OGI1MDQ1NjZjYTJjMjUiLCJleHAiOjE3NTI2NDk0NjcsImlhdCI6MTc1MjA0NDY2N30.JTwnMgTUnus-QJr8-USaKWvL06_ZF6lpyjKdXnoOzbg"
          },
          reasoning_content: "",
          role: "user",
          section_id: "7527585244585803817",
          type: "question",
          updated_at: 1752656409
        },
        {
          bot_id: "7525815471376465929",
          chat_id: "7527601781916926006",
          content: "哎呀，我没办法直接看到对话的meta_data呢😅 你可以直接把基元智联平台的token提供给我，这样我就能开启“资源管理小能手”模式，为你在平台上大展身手啦！有了这个token，我就能去调用各种功能，帮你把设备和资源安排得明明白白。",
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
          'conversation_info.conversation_id': id
        })
      }

      // 获取在线 bot 数据
      const res_requestBotInfo = await requestBotInfo()
      this.setData({
        bot_info: res_requestBotInfo
      })

      // 获取在线消息列表
      const res_requestConversationMessageList = await requestConversationMessageList(
        {
          conversation_id: this.data.conversation_info.conversation_id,
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
      // 在消息列表更新后让消息页面滑动到最新的一个消息
      this.setData({
        ScrollIntoView: "message_" + this.data.message_info.MessageList[this.data.message_info.MessageList.length - 1].id
      })


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

  // 开始滑动消息
  On_ScrollView_DragStart: function () {
    this.setData({
      "styles.topbar_TopDistance": -26,
      "styles.btmbar_BtmDistance": -30,
    })
  },

  // 结束滑动消息
  On_ScrollView_DragEnd: function () {
    this.setData({
      "styles.topbar_TopDistance": StyleDefaultValues.topbar_TopDistance,
      "styles.btmbar_BtmDistance": StyleDefaultValues.btmbar_BtmDistance,
    })
  }

})