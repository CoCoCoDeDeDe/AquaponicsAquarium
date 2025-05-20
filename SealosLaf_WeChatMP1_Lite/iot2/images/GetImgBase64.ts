// https://dhb91nur4r.bja.sealos.run/iot2/images/GetImgBase64

import cloud from '@lafjs/cloud'
import common from '../utils/common'

const db = cloud.mongo.db
const ImagesCollection = db.collection('IOT2_Images_Base64')

export default async function (ctx: FunctionContext) {

  // 检验 laf_token 获取 user_id
  const laf_token_VerifyRes = await common.verifyTokenAndGetUser(ctx)
  switch (laf_token_VerifyRes.runCondition) {
    case 'laf_token error':
      console.log('laf_token 验证失败')
      return laf_token_VerifyRes  // token 错误, 退出
    default:
      break
  }
  let user = laf_token_VerifyRes.user  // user._id 即 user_id
  user._id = new ObjectId(user._id)

  try{
    // 获取图片Id
    const { Image_Id } = ctx.query

    // 校验是否传入Id
    if(!Image_Id) {
      console.log('未传入图片Id')
      return {
        runCondition: 'para error',
        errMsg: '未传入图片Id',
      }
    }

    // Mode_Can_Str_Id 调试模式支持删除 _id 为字符串的图片记录
    let Filter_Delete = {
      $or: [
        { _id: { $eq: new ObjectId(Image_Id) } },
      ]
    }
    if (ctx.body.Mode_Can_Str_Id) {
      Filter_Delete = {
        $or: [
          { _id: { $eq: new ObjectId(Image_Id) } },
          { _id: { $eq: Image_Id } },
        ]
      }
    }

    // 从数据库查询图片
    const GetImageResult = await ImagesCollection.findOne(
      Filter_Delete
      )

    // 检查是否找到图片
    if(GetImageResult == undefined) {
      console.log('未找到图片')
      return {
        runCondition: 'find error',
        errMsg: '未找到图片',
      }
    }

    // 返回图片数据（Base64格式）
    return {
      code: 200,
      runCondition: 'succeed',
      errMsg: '获取成功',
      FileName: GetImageResult.FileName,
      MimeType: GetImageResult.MimeType,
      Data: GetImageResult.Data,
    }

  } catch (err) {
    console.log('获取失败:', err)
    return {
      code: 500,
      runCondition: 'Internal error',
      errMsg: '获取失败',
    }
  }

}
