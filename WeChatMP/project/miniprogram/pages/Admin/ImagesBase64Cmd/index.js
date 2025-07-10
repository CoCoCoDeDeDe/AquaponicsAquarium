// pages/Admin/ImagesCmd/index.js
import { isValidNonEmptyString } from '../../../utils/common'
import { requestWithLafToken, UploadFile, on_laf_token_Invalid, on_common_error, QueryObjectToString } from '../../../apis/laf'

const PAGE_LIMIT = 20
const MODE_CAN_STR_ID = true

Page({

  /**
   * 页面的初始数据
   */
  data: {
    FloatingBtn: {
      IsHiding: false,
      ConfigList: [
        {
          EasyName: '刷新',
          EventName_BindTap: 'On_BindTap_RefreshPage',
        }
      ],
      buttonLeft: 20, // 初始位置x坐标
      buttonTop: 200, // 初始位置y坐标
      startX: 0,
      startY: 0,
      moveX: 0,
      moveY: 0,
      diffX: 0,
      diffY: 0,
      isMoving: false, // 是否正在移动
      clickable: true // 点击是否生效
    },

    ImgPreviewBase64List_Options: {
      Limit: PAGE_LIMIT,
      Total: undefined,
    },

    ImgBase64List: [],

    UploadImgBtn_Options: {
      CanUpload: true,
      IsHiding: false,
    },

    ImgPopup_Options: {
      IsShowing: false,
      ShowingImg_Idx: undefined,
      ShowingImg_Url: undefined,
    },
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad(options) {

    // 获取预览图列表数据
      // 获取参数
    const { Limit } = this.data.ImgPreviewBase64List_Options
      // 发起请求
    this.PushNewImgPreviewBase64List({
      Limit,
      Skip: 0,
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
  async onShow() {
    
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

  // 页面内容刷新
  async PageRefresh(e) {
    // 重置
    this.PageInit()
    
    // 重获
    this.onLoad()
  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  async onPullDownRefresh() {
    this.PageRefresh()
  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom() {

  },

  /**
   * 用户点击右上角分享
   */
  async onShareAppMessage() {

    this.PageInit()
  },

  // 页面初始化
  async PageInit(e) {
    this.setData({
      ImgPreviewBase64List_Options: {
        Limit: PAGE_LIMIT,
        Skip: 0,
        Total: undefined,
      },
      ImgBase64List: []
    })
  },

  // 点击事件：点击获取更多图片列表的按钮
  async On_BindTap_ShowMore(e) {

    // 获取参数
    const {
      Limit = PAGE_LIMIT,
      Total = 0,
    } = this.data.ImgPreviewBase64List_Options

    // 获取参数
    const Length = this.data.ImgBase64List.length

    if(Length >= Total) {
      // 本地获取的预览图总数已经>=上次获取时获取的Total数量了
      console.log("本地获取的预览图总数已经>=上次获取时获取的Total数量了")
      return
    }

    // 默认页面启动时已经获取了第一次，第一次skip为0，其后的获取都要增加skip
    const NewSkip = Length

    this.PushNewImgPreviewBase64List({
      Limit,
      Skip: NewSkip
    })

  },

  // 获取新预览图图片列表到本地预览图列表
  async PushNewImgPreviewBase64List(Options) {
    // 获取本地的获取预览图列表的参数
    const { Limit, Skip } = Options
    console.log("Options:", Options)

    // 获取新列表
    let Res_RequestNewList
    try{
      Res_RequestNewList = await requestWithLafToken('GET', '/iot2/images/GetImgPreviewBase64List',
      {
        limit: Limit,
        skip: Skip,
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

    let {
      ImgPreviewBase64List = [],
      Total = 0,
    } = Res_RequestNewList
    
    // 遍历获取的预览图 Base64 格式数据数组，将 Base64 格式的数据改为前端可读取的格式
    for(let i = 0; i < ImgPreviewBase64List.length; i++) {

      // 获取所需的当前遍历项的数据
      const { MimeType, PreviewData } = ImgPreviewBase64List[i]

      // 转换 Base64 数据
      const Base64ImgUrl = `data:${MimeType};base64,${PreviewData}`

      // 更新为转换后的数据
      ImgPreviewBase64List[i].PreviewData = Base64ImgUrl
    }

    let Main_List = this.data.ImgBase64List
    // arr.push() 会改变 arr, 且返回的是新数组的长度
    Main_List.push(...ImgPreviewBase64List)
    this.setData({
      ImgBase64List: Main_List,
      'ImgPreviewBase64List_Options.Total': Total,
    })
    // 每次更新 Total 时更新 wx 顶部导航栏
    wx.setNavigationBarTitle({
      title: `图片库 (${this.data.ImgBase64List.length}/${Total})`,
    })

  },

  // 点击事件: 点击上传图片按钮
  async On_BindTap_UploadImgBtn() {

    // 选择图片的结果
    let Res_ChooseMedia
    try{
      Res_ChooseMedia = await wx.chooseMedia({
          count: 9,
          mediaType: ['image'],
          sourceType: ['album', 'camera'],
          maxDuration: 10,
          sizeType: ['original', 'compressed'],
          camera: 'back',
        })
    } catch(err) {
      console.log("选择图片失败或未选择 err:", err)
    }
    console.log("Res_ChooseMedia:", Res_ChooseMedia)
    
    if(Res_ChooseMedia) {
      // 如果有选择要上传的文件
      // 挨个上传文件
      for(let i = 0; i < Res_ChooseMedia.tempFiles.length; i++) {
        const {
          fileType,
          size,
          tempFilePath,
        } = Res_ChooseMedia.tempFiles[i]

        // 上传文件
        let Res_UploadImg
        try{
          Res_UploadImg = await UploadFile({
            Last_Url: '/iot2/images/UploadImgListBase64',
            FilePath: tempFilePath,
            FormData: undefined,  // 即 body
          })
        } catch(err) {
          console.log("文件上传错误 err:", err)
          switch(err.runCondition) {
            case 'laf_token error':
              on_laf_token_Invalid()
              return
            default:
              on_common_error(err)
              return
          }
        }
        console.log("Res_UploadImg:", Res_UploadImg)
      }
      
      // 有要上传的文件且文件都上传完成后刷新预览图列表
      // 可优化：显示原已经显示的预览图加上新上传的预览图
      this.PageRefresh()

    }
    

  },

  // 拖拽事件：主滑动页面拖拽开始
  async OnScrollView_DragStart(e) {

    // 隐藏上传图片按钮
    this.setData({
      'UploadImgBtn_Options.IsHiding': true,
      'FloatingBtn.IsHiding': true,
    })
  },

  // 拖拽事件：主滑动页面拖拽结束
  async OnScrollView_DragEnd(e) {
    
    // 取消隐藏上传图片按钮
    this.setData({
      'UploadImgBtn_Options.IsHiding': false,
      'FloatingBtn.IsHiding': false,
    })
  },

  // 点击事件：点击某个预览图，要展示图片大图
  async On_BindTap_CheckImg(e) {

    // 获取点击参数
    const { _id } = e.currentTarget.dataset.imginfo
    const imgidx = e.currentTarget.dataset.imgidx

    console.log("imgidx", imgidx)
    
    // 获取要大图展示的内容
      // 检查是否已经有对应内容
    const MatchedImgCount = this.data.ImgBase64List.reduce((counter, item) => {
      if(item._id == _id && item.Data) {
        // 对应图片已经有原图数据
        return counter + 1
      } else{
        return counter
      }
    }, 0)
    
    if(MatchedImgCount == 0) {
      // 对应图片没有原图数据
      // 获取指定图片的原图数据
      let Res_GetImgData
      try {
        Res_GetImgData = await this.GetImgData({
          Image_Id: _id,
          Mode_Can_Str_Id: MODE_CAN_STR_ID,
        })
      } catch (err) {
        console.log("获取图片原图数据失败 err:", err)
        wx.showToast({
          title: '获取图片原图数据失败',
          duration: 1500,
          icon: 'none',
          mask: false,
        })
        return  // 退出
      }

      // 将 Data 嵌入到本地图片数组中对应项中
      const { MimeType, Data } = Res_GetImgData.Res_GetImgData
      // 遍历已加载的图片列表
      for(let i = 0; i < this.data.ImgBase64List.length; i++) { // 或用 .find()
        if(this.data.ImgBase64List[i]._id == _id) {
          // 找到本次点击指定的图片项，为其插入原图数据
          let NewItem = this.data.ImgBase64List[i]
          NewItem.Data = `data:${MimeType};base64,${Data}`
            // 插入原图数据
          this.setData({
            [`this.data.ImgBase64List[${i}].Data`]: Data
          })
          // 但凡匹配到一个则结束匹配
          i = this.data.ImgBase64List.length
        }
        // 若不匹配则自然匹配下一个或结束
      } // 嵌入完毕
    } // 指定图片已获得原图数据

    // 获取指定图片的 idx

    // 指定大图弹窗要显示的图片并显示大图弹窗
    await this.ImgPopup_ShowImg({
      Image_Idx: imgidx,
    })

  },

  // 获取指定图片的原图数据
  async GetImgData(Options) {
    return new Promise( async (resolve, reject) => {
      // 获取本函数的参数
      const {
        Image_Id,
        Mode_Can_Str_Id = false,
      } = Options

      // 校验本函数的参数
      if(Image_Id == undefined) {
        console.log("参数无效 Image_Id:", Image_Id)
        return reject({
          runCondition: 'para error',
          errMsg: `参数无效 Image_Id: ${Image_Id}`,
        })
      }

      // 发起请求
      let Res_GetImgData
      try{
        Res_GetImgData = await requestWithLafToken('POST', '/iot2/images/GetImgBase64', {
          Image_Id: Image_Id
        },
        {
          Mode_Can_Str_Id: Mode_Can_Str_Id,
        })
      } catch(err) {
        console.log("获取图片失败 err:", err)
        reject(err)
      }

      return resolve({
        Res_GetImgData,
      })
    } )
  },

  // 显示大图弹窗并显示指定的图片
  async ImgPopup_ShowImg(Options) {
    return new Promise( async (resolve, reject) => {
      try{
        // 获取本函数参数
        const {
          Image_Idx,
        } = Options

        // 校验本函数参数
        if(Image_Idx == undefined) {
          return reject({
            runCondition: 'para error',
            errMsg: 'ImgPopup_ShowImg 本地函数参数无效',
          })
        }

        await this.setData({
          'ImgPopup_Options.IsShowing': true,
          'ImgPopup_Options.ShowingImg_Idx': Image_Idx,
          'ImgPopup_Options.ShowingImg_Url': this.data.ImgBase64List[Image_Idx].Data,
        })

      } catch(err) {
        console.log("ImgPopup_ShowImg err:", err)
        return reject(err)
      }
    } )
  },

  async On_BindTap_ImgPopupImgBox(e) {
    await this.ImgPopup_Hide()
  },

  // 隐藏大图弹窗
  async ImgPopup_Hide(Options) {
    return new Promise( (resolve, reject) => {
      try{

        this.setData({
          'ImgPopup_Options.IsShowing': false
        })

      } catch(err) {
        console.log("ImgPopup_Hide err:", err)
        return reject(err)
      }
    } )
  },

  // scroll-view 事件: 滑到顶
  async OnScrollView_ScrollToUpper(e) {
    console.log("e:", e)
  },

  // scroll-view 事件: 触摸开始
  async OnScrollView_TouchStart(e) {
    console.log("e:", e)
  },

  // scroll-view 事件: 触摸移动
  async OnScrollView_TouchMove(e) {
    console.log("e:", e)
  },

  // scroll-view 事件: 触摸结束
  async OnScrollView_TouchEnd(e) {
    console.log("e:", e)
  },

  // FloatingBtnsBox 事件
  async FloatingBtnsBox_handleTouchStart(e) {
    this.setData({
      'FloatingBtn.startX': e.touches[0].clientX,
      'FloatingBtn.startY': e.touches[0].clientY,
      'FloatingBtn.isMoving': false,
      'FloatingBtn.clickable': true
    });
  },

  // FloatingBtnsBox 事件
  async FloatingBtnsBox_handleTouchMove(e) {
    // const { windowWidth, windowHeight } = wx.getSystemInfoSync();
    const { startX, startY, buttonLeft, buttonTop } = this.data.FloatingBtn;
    
    // 计算移动距离
    const moveX = e.touches[0].clientX;
    const moveY = e.touches[0].clientY;
    const diffX = moveX - startX;
    const diffY = moveY - startY;
    
    // 判断是否移动了一定距离才视为拖动
    if (Math.abs(diffX) > 5 || Math.abs(diffY) > 5) {
      this.setData({
        isMoving: true,
        clickable: false
      });
    }
    
    // 计算新位置
    let newLeft = buttonLeft + diffX;
    let newTop = buttonTop + diffY;
    
    // 边界限制
    // if (newLeft < 0) newLeft = 0;
    // if (newLeft > windowWidth - 50) newLeft = windowWidth - 50;
    // if (newTop < 0) newTop = 0;
    // if (newTop > windowHeight - 50) newTop = windowHeight - 50;
    
    this.setData({
      'FloatingBtn.buttonLeft': newLeft,
      'FloatingBtn.buttonTop': newTop,
      'FloatingBtn.startX': moveX,
      'FloatingBtn.startY': moveY
    });
  },

  // FloatingBtnsBox 事件
  async FloatingBtnsBox_handleTouchEnd(e) {
    // // 吸附到边缘效果
    // const { windowWidth } = wx.getSystemInfoSync();
    // const { buttonLeft, buttonTop } = this.data.FloatingBtn;
    
    // // 自动吸附到最左侧或最右侧
    // if (buttonLeft < windowWidth / 2) {
    //   this.setData({
    //     'FloatingBtn.buttonLeft': 10
    //   });
    // } else {
    //   this.setData({
    //     'FloatingBtn.buttonLeft': windowWidth - 60
    //   });
    // }
  },

  // Flating Btn 事件: 点击刷新按钮
  async On_BindTap_RefreshPage(e) {
    this.PageRefresh()
  },

  // 大图弹窗按钮事件: 点击更改图片数据按钮
  async On_BindTap_ChangeImgData(e) {

    // 选择图片的结果
    let Res_ChooseMedia
    try{
      Res_ChooseMedia = await wx.chooseMedia({
          count: 1,
          mediaType: ['image'],
          sourceType: ['album', 'camera'],
          maxDuration: 10,
          sizeType: ['original', 'compressed'],
          camera: 'back',
        })
    } catch(err) {
      console.log("选择图片失败或未选择 err:", err)
    }
    console.log("Res_ChooseMedia:", Res_ChooseMedia)
    
    if(Res_ChooseMedia) {
      const { ShowingImg_Idx } = this.data.ImgPopup_Options
  
      const Target_Img_Id = this.data.ImgBase64List[ShowingImg_Idx]._id

      console.log("Res_ChooseMedia:", Res_ChooseMedia)
  
      let Res_ChangeImgData = await this.ChangeImgData({
        Image_Id: Target_Img_Id,
        FilePath: Res_ChooseMedia.tempFiles[0].tempFilePath,
        Mode_Can_Str_Id: MODE_CAN_STR_ID,
      })

      // Res_ChangeImgData.UpdateField 与 ImgBase64List 中的项相比只差 _id
      // 为 Res_ChangeImgData.UpdateField 添加 _id 字段
      Res_ChangeImgData.UpdateField = Object.assign(Res_ChangeImgData.UpdateField, {
        _id: Target_Img_Id,
      })

      // 为 Base64 数据字符串添加前缀
      Res_ChangeImgData.UpdateField.Data = `data:${Res_ChangeImgData.UpdateField.MimeType};base64,${Res_ChangeImgData.UpdateField.Data}`
      Res_ChangeImgData.UpdateField.PreviewData = `data:${Res_ChangeImgData.UpdateField.MimeType};base64,${Res_ChangeImgData.UpdateField.PreviewData}`

      // 更新 Target_Img_Id 或 ShowingImg_Idx 指定的图片更新其在 ImgBase64List 中的数据, 包括原图 Data, 同时更新 ShowingImg_Url
      this.setData({
        [`ImgBase64List[${ShowingImg_Idx}]`]: Res_ChangeImgData.UpdateField,  // 更新 ImgBase64List
      })
      this.setData({
        'ImgPopup_Options.ShowingImg_Url': Res_ChangeImgData.UpdateField.Data,  // 更新大图显示弹窗的图片数据
      })
    }
  },

  // 更新图片数据
  async ChangeImgData(Options) {
    return new Promise( async (resolve, reject) => {
      try{
        // 获取参数
        const {
          Image_Id,
          FilePath,
          Mode_Can_Str_Id,
        } = Options

        // 校验参数
        if(Image_Id == undefined || FilePath == undefined) {
          throw new Error('ChangeImgData 本地函数参数无效')
        }

        // 发起请求
        const Res_ChangeImgData = await UploadFile({
          Last_Url: `/iot2/images/UpdateImgBase64`,
          FilePath: FilePath,
          FormData: {
            Mode_Can_Str_Id,
          },
          Query: {
            Image_Id: Image_Id,
          }
        })

        resolve(Res_ChangeImgData)

      } catch(err) {
        switch(err.runCondition) {
          case 'laf_token error':
            on_laf_token_Invalid()
            break
          default:
            on_common_error()
            break
        }
        console.log(`ChangeImgData 错误 err: ${err}`)
        return reject(err)
      }
    } )
  },
  
  // 大图弹窗事件: 点击删除图片按钮
  async On_BindTap_DeleteImg(e) {

    const { ShowingImg_Idx } = this.data.ImgPopup_Options
    const ShowingImg_Id = this.data.ImgBase64List[ShowingImg_Idx]._id

    wx.showModal({
      cancelColor: '#aaa',
      cancelText: '取消',
      confirmColor: '#de0308',
      confirmText: '确认',
      content: '图片 _id: ' + ShowingImg_Id,
      editable: false,
      placeholderText: '',
      showCancel: true,
      title: '是否确认删除图片',
      success: async (result) => {
        console.log("result:", result)
        if(result.confirm) {
          try{
            await this.DeleteImg({
              Image_Id: ShowingImg_Id,
              Mode_Can_Str_Id: MODE_CAN_STR_ID,
            })
          } catch(err) {
            console.log("删除失败 err:", err)
            wx.showToast({
              title: '删除失败',
              duration: 1500,
              icon: 'none',
              mask: false,
            })
            return
          }
          wx.showToast({
            title: '删除成功',
            duration: 1500,
            icon: 'none',
            mask: false,
          })

          // 刷新页面（隐藏大图窗口，刷新预览图列表长度为原数量-1）
          this.ImgPopup_Hide()
          const Original_ImgBase64List_Length = this.data.ImgBase64List.length
          const Res_RequestNewList = await this.RequestImgPreviewBase64List({
            Length: Original_ImgBase64List_Length
          })

          // 结构请求结果
          let {
            ImgPreviewBase64List = [],
            Total = 0,
          } = Res_RequestNewList

          // 遍历获取的预览图 Base64 格式数据数组，将 Base64 格式的数据改为前端可读取的格式
          for(let i = 0; i < ImgPreviewBase64List.length; i++) {

            // 获取所需的当前遍历项的数据
            const { MimeType, PreviewData } = ImgPreviewBase64List[i]

            // 转换 Base64 数据
            const Base64ImgUrl = `data:${MimeType};base64,${PreviewData}`

            // 更新为转换后的数据
            ImgPreviewBase64List[i].PreviewData = Base64ImgUrl
          }

          this.setData({
            ImgBase64List: ImgPreviewBase64List,
            'ImgPreviewBase64List_Options.Total': Total,
          })
          // 每次更新 Total 时更新 wx 顶部导航栏
          wx.setNavigationBarTitle({
            title: `图片库 (${this.data.ImgBase64List.length}/${Total})`,
          })

          return
        }
        if(result.cancel) {
          return
        }
      },
      fail: (res) => {},
      complete: (res) => {},
    })

  },

  // 获取指定长度的仅无原图数据的图片数据列表
  async RequestImgPreviewBase64List(Options) {
    return new Promise( async (resolve, reject) => {
      try{
        // 获取参数
        const { Length } = Options

        // 校验参数
        if( Length == undefined ) {
          throw new Error('本地函数参数错误')
        }

        // 发起请求
        const Res_RequestNewList = await requestWithLafToken('POST', '/iot2/images/GetImgPreviewBase64List',
        {
          limit: Length,
          skip: 0,
        })

        // 成功
        resolve(Res_RequestNewList)

      } catch(err) {
        console.log("GetImgPreviewBase64List() err:", err)
        switch(err.runCondition) {
          case 'laf_token error':
            on_laf_token_Invalid()
            break
          default:
            on_common_error()
            break
        }
        return reject({
          runCondition: 'local error',
          errMsg: 'GetImgPreviewBase64List() err'
        })
      }
    } )
  },

  // 删除图片
  async DeleteImg(Options) {
    return new Promise( async (resolve, reject) => {
      try{
        // 获取参数
        const {
          Image_Id,
          Mode_Can_Str_Id = false,
        } = Options

        // 校验参数
        if( Image_Id == undefined ) {
          wx.showToast({
            title: 'Image_Id 参数错误',
            duration: 1500,
            icon: 'none',
            mask: false,
          })
          return
        }

        // 发起请求
        let RequestRes_Delete
        try{
          RequestRes_Delete = await requestWithLafToken('POST', '/iot2/images/DeleteImgBase64', 
          {
            Image_Id: Image_Id,
          },
          {
            Mode_Can_Str_Id: Mode_Can_Str_Id,
          })
        } catch(err) {
          console.log("API fail err:", err)
          switch(err.runCondition) {
            case 'laf_token error':
              on_laf_token_Invalid()
              break
            default:
              on_common_error()
              break
          }
          return reject(err)
        }
        console.log("删除图片的 API 响应 RequestRes_Delete:", RequestRes_Delete)

        resolve(RequestRes_Delete)

      } catch(err) {
        console.log("DeleteImg err:", err)
        return reject({
          runCondition: 'local error',
          errMsg: 'DeleteImg err',
        })
      }
    } )
  },

  // 点击图片 id 文本，复制 id 到剪切板
  async On_BindTap_CopyImgId(e) {
    this.Copy(this.data.ImgBase64List[this.data.ImgPopup_Options.ShowingImg_Idx]._id)
  },

  // 复制参数 data 到剪贴板
  async Copy(data) {
    wx.setClipboardData({
      data,
      success: (res) => {
        wx.getClipboardData({
          success: (res) => {
            console.log("getClipboardData res:", res)
          }
        })
      }
    })
  }


  // 可优化：大图可以点击左右换图，已经获取的图会缓存，一个页面声明周期中不重复获取
})