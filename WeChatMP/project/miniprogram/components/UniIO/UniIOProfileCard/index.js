// components/UniIO/UniIOProfileCard/index.js
import { isValidNonEmptyString } from '../../../utils/common'
import { requestWithLafToken, on_laf_token_Invalid, on_request_error, on_db_error, on_param_error, on_unknown_error, on_common_error } from '../../../apis/laf'
Component({

  /**
   * 组件的属性列表
   */
  properties: {
    UniIOProfile: {
      type: Object,
      value: {
        UniIO_Type: 'Type',
        UniIO_MainColor: '#ccc',
        UniIO_Name: 'UniIO_Name',
        Device_Name: 'Device_Name',
        UniIO_Id: 'UniIO_Id',
        SmartLinkGroup_Name: 'SLGroup_Name',
        Is_Selected: false,
        IsShow_SLGroup_Name: false,
      }
    },

    UniIOCardBtnList: {
      type: Array,
      value: [
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
          Img_Url: '/static/images/icons/icon_unbind_line_active.png',
          FunctionName: '删除',
          BindTapHandlerName: 'On_BindTap_Delete',
        },
      ]
    }
  },

  /**
   * 组件的初始数据
   */
  data: {
    
  },

  /**
   * 组件的方法列表
   */
  methods: {

    async On_BindTap_Delete(e) {

      wx.showModal({
        cancelColor: '#aaa',
        cancelText: '取消',
        confirmColor: '#c41a16',
        confirmText: '解绑',
        content: '请谨慎确认',
        editable: false,
        placeholderText: 'placeholderText',
        showCancel: true,
        title: '是否确认解绑设备',
        success: async (result) => {
          if(result.cancel) {
            return
          }
          if(result.confirm) {
            const Rec_UniIO_Id = await e.currentTarget.dataset.uniioid
            // console.log("Rec_UniIO_Id:", Rec_UniIO_Id)
        
            await this.RemoveUniIO(Rec_UniIO_Id)
          }
        },
        fail: (res) => {},
        complete: (res) => {},
      })
  
    },
  
    async RemoveUniIO(Rec_UniIO_Id) {
  
      // 调用 API 将 UniIO 从 SLGroup 中删除
      let ResData
      let body = {
        "uniIOList": [
          {
            "_id": Rec_UniIO_Id
          }
        ]
      }
      try{
        ResData = await requestWithLafToken('POST', '/iot2/smartLinkGroup/RemoveUnIOsFromSmartLinkGroup', {}, body)
      } catch(err) {
        // console.log("err:", err)
        switch(err.runCondition) {
          case 'laf_token error':
            on_laf_token_Invalid()
            return
          default:
            on_common_error(err)
            return
        }
      }
  
      wx.showToast({
        title: ResData.errMsg,
        duration: 1500,
        icon: 'none',
        mask: false,
      })

      // 调用双亲页面传入的刷新页面的函数即移除成功后要执行的函数
      // 重新获取 UniIOList 刷新页面

      this.triggerEvent('RefreshPage', { data: {} })

      // this.GetNewUniIOProfileList()
    },
  }
})