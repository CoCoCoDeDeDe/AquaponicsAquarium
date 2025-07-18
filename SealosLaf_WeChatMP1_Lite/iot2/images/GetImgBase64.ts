import cloud from '@lafjs/cloud'
import common from '../utils/common'

const db = cloud.mongo.db
const ImagesCollection = db.collection('IOT2_Images_Base64')

// 核心业务逻辑抽离为独立函数
export async function GetImageBase64Core(Image_Id, allowStrId = false) {
  try {
    if (!Image_Id) {
      throw new Error('未传入图片Id')
    }

    let filter = {
      $or: [
        { _id: new ObjectId(Image_Id) },
      ]
    }

    if (allowStrId) {
      filter.$or.push({ _id: Image_Id })
    }

    const image = await ImagesCollection.findOne(filter)

    if (!image) {
      throw new Error('未找到图片')
    }

    return {
      code: 200,
      runCondition: 'succeed',
      errMsg: '获取成功',
      FileName: image.FileName,
      MimeType: image.MimeType,
      Data: image.Data,
    }
  } catch (err) {
    console.log('获取失败:', err)
    return {
      code: 500,
      runCondition: 'Internal error',
      errMsg: err.message || '获取失败',
    }
  }
}

// 原始云函数入口保持兼容
export default async function GetImgBase64(ctx: FunctionContext) {
  // 校验token
  const tokenResult = await common.verifyTokenAndGetUser(ctx)
  if (tokenResult.runCondition === 'laf_token error') {
    console.log('laf_token 验证失败')
    return tokenResult
  }

  const user = tokenResult.user
  user._id = new ObjectId(user._id)

  // 调用核心逻辑
  return GetImageBase64Core(
    ctx.query.Image_Id,
    ctx.body?.Mode_Can_Str_Id
  )
}  