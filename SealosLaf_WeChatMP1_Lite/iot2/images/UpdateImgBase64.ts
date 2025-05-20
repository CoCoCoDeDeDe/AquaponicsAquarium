// https://dhb91nur4r.bja.sealos.run/iot2/images/UpdateImgBase64

import cloud from '@lafjs/cloud'
import common from '../utils/common'
const fs = require('fs-extra')
const path = require('path')
const sharp = require('sharp')

const db = cloud.mongo.db
const ImagesCol = db.collection('IOT2_Images_Base64')
const PREVIEW_WIDTH = parseInt(process.env.PREVIEW_WIDTH, 10) || 200

export default async function (ctx: FunctionContext) {
  try {

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

    // 检查是否有文件上传
    if (!ctx.files || ctx.files.length <= 0) {
      return {
        code: 400,
        runCondition: 'not found file',
        errMsg: '没有找到文件',
      }
    }

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

    // 检查上传的文件格式是否为图片
    const IsImg = ctx.files[0].mimetype.startsWith('image/')
    if(!IsImg) {
      console.log('错误: 上传的文件不是图片')
      return {
        code: 500,
        runCondition: 'file error',
        errMsg: `错误: 上传的文件不是图片`,
      }
    }

    // 读取文件内容
    const FileContent = fs.readFileSync(ctx.files[0].path)

    // 生成预览图
    let PreviewBuffer
    try{
      PreviewBuffer = await sharp(FileContent)
        .resize({
          width: PREVIEW_WIDTH,
        })
        .toBuffer()
    } catch(err) {
      console.log(`生成预览图失败 err: ${err}`)
      return {
        code: 500,
        runCondition: 'internal error',
        errMsg: `生成预览图失败 err: ${err}`,
      }
    }

    // 准备要上传至数据库的记录
    const CurrentDate = new Date()
    const FormattedDate = common.formatDate(CurrentDate)
    const UpdateField = {
      // 文件基本信息
      FileName: ctx.files[0].originalname,
      MimeType: ctx.files[0].mimetype,
      Size: ctx.files[0].size,
      UploadTime: FormattedDate,

      // 用户信息
      User_Id: new ObjectId(user._id),

      // 文件内容（Base64）
      Data: FileContent.toString('base64'),
      PreviewData: PreviewBuffer.toString('base64'),

      // 其他元数据
      FieldName: ctx.files[0].fieldname,
      Encoding: ctx.files[0].encoding,
      StoragePath: ctx.files[0].path,
      MateData: {
        Refer: ctx.headers.referer,
        UserAgent: ctx.headers['user-agent'],
      },
    }

    console.log(`要更改的图片 Image_Id: ${Image_Id}`)

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
    try{
      Res_IsExist = await ImagesCol.countDocuments(
        Filter_Delete
      )
    } catch (err) {
      console.log(`数据库查询存在失败 err: ` + err)
      return {
        code: 500,
        runCondition: 'db error',
        errMsg: `数据库查询存在失败 err: ${err}`,
      }
    }
    console.log('查询要更新的图片是否存在 Res_IsExist', Res_IsExist)

    if (Res_IsExist != 1) {
      console.log(`指定图片数量错误`)
      return {
        code: 500,
        runCondition: 'target error',
        errMsg: `指定图片数量错误`,
      }
    }

    // 存储数据到数据库中指定 _id 的图片的记录
    let UpdateResult
    try{
      UpdateResult = await ImagesCol.updateOne(
        Filter_Delete,
        {
          $set: {
            FileName: UpdateField.FileName,
            MimeType: UpdateField.MimeType,
            Size: UpdateField.Size,
            UploadTime: UpdateField.UploadTime,
            User_Id: UpdateField.User_Id,
            Data: UpdateField.Data,
            PreviewData: UpdateField.PreviewData,
            FieldName: UpdateField.FieldName,
            Encoding: UpdateField.Encoding,
            StoragePath: UpdateField.StoragePath,
            MateData: UpdateField.MateData,
          },
        },
        {
          upsert: false,  // 指定图片若不存在则不更新
        }
      )
      // 更新效果校验
      console.log('数据库更新结果 UpdateResult:', UpdateResult)
      const { modifiedCount } = UpdateResult
      if (modifiedCount != 1) {
        throw new Error(`更新错误 modifiedCount: ${modifiedCount}`)
      }
    } catch (err) {
      console.log(`数据库更新失败 err: ${err}`)
      return {
        code: 500,
        runCondition: 'db error',
        errMsg: `数据库更新失败 err: ${err}`,
      }
    }

    // 删除临时文件
    fs.unlinkSync(ctx.files[0].path)

    // 成功
    console.log(`图片更新成功 Image_Id: ${Image_Id}`)
    return {
      runCondition: 'succeed',
      errMsg: `图片更新成功 Image_Id: ${Image_Id}`,
      UpdateResult,
      UpdateField,
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