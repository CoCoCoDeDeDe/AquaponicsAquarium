// components/deviceCard/index.js
import { isValidNonEmptyString } from '../../utils/common'
import { requestWithLafToken, on_laf_token_Invalid, on_request_error, on_db_error, on_param_error, on_unknown_error, on_common_error } from '../../apis/laf'
Component({

  /**
   * 组件的属性列表
   */
  properties: {
    deviceProfile: {
      type: Object,
      value: {
        _id: 'default_id',
        product_id: 'default_product_id',
        huawei_device_id: 'default_huawei_device_id',
        previewImg_url: 'https://mp-1b9cd3c8-d666-4006-b791-11d5ce02e1be.cdn.bspapp.com/IoT1/test/previewImg_aqaq.png', // 来自产品集合
        name: '默认设备名称', // 用户自定的，来自设备集合
      }
      ,
      required: false,
      observer: function(newVal, oldVal, changedPath) {}
    }
  },

  /**
   * 组件的初始数据
   */
  data: {
    columnBtnsArr: [
      {
        imageSrc: '/static/images/icons/icon_deviceDetail_line_dekBlue@2x.png',
        bindtap: 'onBindtapNavigateToDeviceDetailPage'
      },
      {
        imageSrc: '/static/images/icons/icon_edit_line_darkBlue@2x.png',
        bindtap: 'onBindtapEditDevice'
      },
      {
        imageSrc: '/static/images/icons/icon_delete_line_darkBlue@2x.png',
        bindtap: 'onBindtapUnbindDevice'
      }
    ]
  },

  /**
   * 组件的方法列表
   */
  methods: {

    onBindtapDeviceDetail(e) {
      const { deviceprofile } = e.currentTarget.dataset // data- 属性传递的数据的键名全小写
      console.log("onBindtapDeviceDetail deviceprofile:", deviceprofile)
    },

    onBindtapEditDevice(e) {
      const { deviceprofile } = e.currentTarget.dataset // data- 属性传递的数据的键名全小写
      console.log("onBindtapEditDevice deviceprofile:", deviceprofile)
      
      wx.showModal({
        cancelColor: '#aaa',
        cancelText: '取消',
        confirmColor: '#54c17d',
        confirmText: '提交',
        content: this.data.deviceProfile.name,
        editable: true,
        placeholderText: 'new name',
        showCancel: true,
        title: '请宝宝输入新米即',
        success: async (result) => {
          console.log("result", result)
          if(result.cancel) {
            return
          }
          if(result.confirm) {
            const { deviceprofile } = e.currentTarget.dataset // data- 属性传递的数据的键名全小写
            this.onEditDevice(deviceprofile, result.content)
          }
        },
        fail: (res) => {},
        complete: (res) => {},
      })
    },

    async onEditDevice(deviceprofile, newName) {
      // newName 校验
      if(typeof newName !== 'string' || newName.trim().length <= 0) {
        wx.showToast({
          title: `新名称 ${newName} 不合格`,
          duration: 1500,
          icon: 'error',
          mask: false,
        })
        return
      }

      try{
        const resData = await requestWithLafToken('GET', '/iot2/device/editDeviceName', {huawei_device_id: deviceprofile.huawei_device_id, name: newName})
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

      wx.showToast({
        title: `改名为 ${newName} 成功`,
        duration: 1500,
        icon: 'none',
        mask: true,
      })

      // TODO 子组件控制双亲页面刷新页面

    },

    onBindtapUnbindDevice(e) {
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
          // console.log("result", result)
          if(result.cancel) {
            return
          }
          if(result.confirm) {
            const { deviceprofile } = e.currentTarget.dataset // data- 属性传递的数据的键名全小写
            this.onUnbindDevice(deviceprofile)
          }
        },
        fail: (res) => {},
        complete: (res) => {},
      })

    },

    async onUnbindDevice(deviceprofile) {
      console.log("onUnbindDevice deviceprofile:", deviceprofile)
      try{
        const resData = await requestWithLafToken('GET', '/iot2/device/unbindUserWithDevice', {huawei_device_id: deviceprofile.huawei_device_id})
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

      wx.showToast({
        title: '解绑成功',
        duration: 1500,
        icon: 'success',
        mask: true,
      })

      // TODO 子组件控制双亲页面刷新页面
    },

    async onBindtapNavigateToDeviceDetailPage(e) {
      console.log("this.data.deviceProfile.huawei_device_id:", this.data.deviceProfile.huawei_device_id)
      this.onNavigateToDeviceDetailPage(this.data.deviceProfile.huawei_device_id)
    },

    async onNavigateToDeviceDetailPage(huawei_device_id) {
      wx.navigateTo({
        url: `/pages/device/deviceDetail/index?huawei_device_id=${huawei_device_id}`,
      })
    }

  }
})