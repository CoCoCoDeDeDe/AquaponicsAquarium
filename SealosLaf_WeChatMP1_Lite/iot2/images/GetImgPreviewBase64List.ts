// https://dhb91nur4r.bja.sealos.run/iot2/images/GetImgPreviewBase64List

import cloud from '@lafjs/cloud'
import common from '../utils/common'

const db = cloud.mongo.db
const ImagesCollection = db.collection('IOT2_Images_Base64')

export default async function (ctx: FunctionContext) {

  try{
    // 检验 laf_token 获取 user_id
    const laf_token_VerifyRes = await common.verifyTokenAndGetUser(ctx)
    switch (laf_token_VerifyRes.runCondition) {
      case 'laf_token error':
        console.log('laf_token 验证失败')
        return laf_token_VerifyRes  // token 错误, 退出
      default:
        // console.log('laf_token 验证成功')
        break
    }
    let user = laf_token_VerifyRes.user  // user._id 即 user_id
    user._id = new ObjectId(user._id)
    // console.log('user._id:', user._id)

    // 获取参数或默认参数
    const limit = Number(ctx.query.limit)
    const skip = Number(ctx.query.skip)

    console.log(`limit: ${limit}`)
    console.log(`skip: ${skip}`)

    const Find_Filter = {
      PreviewData: { $exists: true } // 仅查询有预览图的记录
    }

    // 获取符合要求的数据的总数
    let Res_FindImgPreviewBase64ListTotal
    try{
      Res_FindImgPreviewBase64ListTotal = await ImagesCollection.countDocuments(
        Find_Filter
      )
    } catch (err) {
      console.log('数据库查询总数错误 err:', err)
      return {
        runCondition: 'db error',
        errMsg: '数据库查询总数错误',
      }
    }

    // 从数据库获取预览图列表
    let Res_FindImgPreviewBase64List

    if (Res_FindImgPreviewBase64ListTotal >= 1) {
      try {
        Res_FindImgPreviewBase64List = await db.collection('IOT2_Images_Base64').find(
          Find_Filter,
          {
            projection: {
              Data: 0,  // 返回有预览图的记录时不返回原始图片数据
            },
          }
        )
        .skip(skip)
        .limit(limit)
        .toArray()
      } catch (err) {
        console.log('数据库查询错误 err:', err)
        return {
          runCondition: 'db error',
          errMsg: '数据库查询错误',
        }
      }
    } 

    // 不将查询结果为空看作错误

    console.log('获取成功')
    return {
      runCondition: 'succeed',
      errMsg: '获取成功',
      ImgPreviewBase64List: Res_FindImgPreviewBase64List, // 为赋值则不返回
      Total: Res_FindImgPreviewBase64ListTotal,
    }

  } catch(err) {
    console.log('未知错误 err:', err)
    return {
      runCondition: 'Internal error',
      errMsg: '服务器内部错误',
    }
  }

}
