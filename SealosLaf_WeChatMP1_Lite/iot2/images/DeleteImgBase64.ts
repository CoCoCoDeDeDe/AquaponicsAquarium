// https://dhb91nur4r.bja.sealos.run/iot2/images/DeleteImgBase64


import cloud from '@lafjs/cloud'
import common from '../utils/common'
const fs = require('fs-extra')
const path = require('path')
const sharp = require('sharp')

const db = cloud.mongo.db
const ImagesCol = db.collection('IOT2_Images_Base64')

export default async function (ctx: FunctionContext) {
  try {
    // console.log('ctx:', ctx)
    // console.log('ctx.headers:', ctx.headers)
    console.log('ctx.body:', ctx.body)
    // console.log('ctx.files:', ctx.files)

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

    // 获取参数
    const { Image_Id } = ctx.query
    // 校验参数
    if (Image_Id == undefined || Image_Id == '') {
      console.log(`参数无效 Image_Id: ${Image_Id}`)
      return {
        code: 500,
        runCondition: 'para error',
        errMsg: `参数无效 Image_Id: ${Image_Id}`,
      }
    }

    console.log(`要删除的图片 Image_Id: ${Image_Id}`)

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

    // 查询指定图片记录是否存在
    let Res_IsExist
    try {
      Res_IsExist = await ImagesCol.countDocuments(
          Filter_Delete
        )
    } catch (err) {
      console.log(`数据库查询存在失败 err: `, err)
      return {
        code: 500,
        runCondition: 'db error',
        errMsg: `数据库查询存在失败 err: ${err}`,
      }
    }
    console.log('查询要删除的图片是否存在 Res_IsExist', Res_IsExist)

    // 校验指定图片存在情况
    if (Res_IsExist != 1) {
      console.log(`指定图片数量错误`)
      return {
        code: 500,
        runCondition: 'target error',
        errMsg: `指定图片数量错误`,
      }
    }

    // 从数据库中删除指定图片记录
    let Res_Delete
    try{

      Res_Delete = await ImagesCol.deleteOne(
        Filter_Delete
      )

    } catch(err) {
      const errMsg = `从数据库中删除指定图片记录错误`
      console.log(errMsg, ` err: `, err)
      return {
        code: 500,
        runCondition: 'db error',
        errMsg: errMsg,
      }
    }
    console.log(`从数据库中删除指定图片记录的结果`, ` Res_Delete: `, Res_Delete)
    if (Res_Delete.deletedCount != 1) {
      console.log(`数据库删除图片 失败 Res_Delete:`, Res_Delete)
      return {
        runCondition: 'db failed',
        errMsg: '数据库删除图片 失败',
      }
    }

    console.log(`数据库删除图片 成功 Res_Delete:`, Res_Delete)
    return {
      runCondition: 'succeed',
      errMsg: '数据库删除图片 成功',
      Res_Delete,
    }

  } catch (err) {
    console.log('错误:', err)
    return {
      code: 500,
      runCondition: 'Internal error',
      errMsg: `错误 err: ${err}`,
    }
  }
}
