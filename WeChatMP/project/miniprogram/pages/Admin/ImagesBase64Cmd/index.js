// pages/Admin/ImagesCmd/index.js
import { isValidNonEmptyString } from '../../../utils/common'
import { requestWithLafToken, UploadFile, on_laf_token_Invalid, on_common_error } from '../../../apis/laf'

Page({

  /**
   * 页面的初始数据
   */
  data: {
    ImgPreviewBase64List_Options: {
      Limit: 20,
      Total: undefined,
    },

    ImgBase64List: [],

    UploadImgBtn_Options: {
      CanUpload: true,
      IsHiding: false,
    },

    ImgPopup_Options: {
      IsShowing: false,
      ShowingImg_Id: undefined,
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
    // 重置
    this.PageInit()
    
    // 重获
    this.onLoad()
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
        Limit: 20,
        Skip: 0,
        Total: undefined,
      },
      ImgBase64List: []
    })
  },

  // 点击事件：点击获取更多图片列表的按钮
  async On_BindTap_PushNewImgPreviewBase64List(e) {

    // 获取参数
    const {
      Limit = 20,
      Total = 0,
    } = this.data.ImgPreviewBase64List_Options

    // 获取参数
    const Length = this.data.ImgBase64List.length

    if(Length >= Total) {
      // 本地获取的预览图总数已经>=上次获取时获取的Total数量了
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
      // 重置
      this.PageInit()
      // 重获
      this.onLoad()

    }
    

  },

  // 拖拽事件：主滑动页面拖拽开始
  async OnScrollView_DragStart(e) {

    // 隐藏上传图片按钮
    this.setData({
      'UploadImgBtn_Options.IsHiding': true,
    })
  },

  // 拖拽事件：主滑动页面拖拽结束
  async OnScrollView_DragEnd(e) {
    
    // 取消隐藏上传图片按钮
    this.setData({
      'UploadImgBtn_Options.IsHiding': false,
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
        Res_GetImgData = await requestWithLafToken('GET', '/iot2/images/GetImgBase64', {
          Image_Id: Image_Id
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
            errMsg: '本地函数参数无效',
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


  // 可优化：大图可以点击左右换图，已经获取的图会缓存，一个页面声明周期中不重复获取
})