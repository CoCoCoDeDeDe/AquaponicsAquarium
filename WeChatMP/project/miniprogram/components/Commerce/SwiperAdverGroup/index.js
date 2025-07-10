// components/Commerce/SwiperAdverGroup/index.js
import { requestWithLafToken, on_laf_token_Invalid, on_request_error, on_db_error, on_param_error, on_unknown_error, on_common_error } from '../../../apis/laf'
import { GetBase64ImgSrc } from '../../../utils/common'

Component({

  lifetimes: {
    attached: function() {
      
      // 获取横滚广告信息
      this.GetSwiperData()

    }
  },

  /**
   * 组件的属性列表
   */
  properties: {
    
  },

  /**
   * 组件的初始数据
   */
  data: {
    AdverSwiperGroup: {
      _id: String,
      CommonInfo: {
        
      },
      AdverSwiperItemList: [
        {
          _id: String,
          NavOptions: {
            NavType: String,
            TargetURL: String
          },
          Img: {
            _id: String,
            FileName: String,
            MimeType: String,
            Data: String
          }
        }
      ]
    }
  },

  /**
   * 组件的方法列表
   */
  methods: {
    // 获取横滚广告数据
    GetSwiperData: async function () {
      try {

        const RequestSwiperDataRes = await requestWithLafToken(
          'GET',
          '/iot2/Commerce/Adver/GetSwiperAdverGroupData'
        )

        let NewSwiperData = RequestSwiperDataRes.Results.AdverSwiperGroup

        this.setData(
          {
            AdverSwiperGroup: NewSwiperData
          }
        )

        console.log("this.data:", this.data)

      } catch(err) {
        console.log("err:", err)

        switch(err.runCondition) {
          case 'laf_token error':
            on_laf_token_Invalid()
            return
          default:
            on_common_error(err)
            return
        }
      }
    }

  }
})