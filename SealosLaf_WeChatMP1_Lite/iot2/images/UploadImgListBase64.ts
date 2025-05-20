// https://dhb91nur4r.bja.sealos.run/iot2/images/UploadImgListBase64
// Col_Name: Iot2_Images

import cloud from '@lafjs/cloud'
import common from '../utils/common'
const fs = require('fs-extra')
const path = require('path')
const sharp = require('sharp')

const db = cloud.mongo.db
const ImagesCol = db.collection('IOT2_Images_Base64')
const PREVIEW_WIDTH = parseInt(process.env.PREVIEW_WIDTH, 10) || 200

export default async function (ctx: FunctionContext) {
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
  

  try{

    // 检查是否有文件上传
    if(!ctx.files || ctx.files.length <= 0) {
      return {
        code: 400,
        runCondition: 'not found file',
        errMsg: '没有找到文件',
      }
    }

    // 处理上传文件的数组
    const UploadResults = []

    // 遍历上传的 files, 将文件存入数据库
    for(const file of ctx.files) {

      // 检查是否为图片文件
      const IsImg = file.mimetype.startsWith('image/')
      if (!IsImg) {
        console.log('文件非图片')
        return {
          code: 400,
          runCondition: 'invalid file type',
          errMsg: '仅支持图片文件',
        }
      }

      // 读取文件内容
      const FileContent = fs.readFileSync(file.path)

      // 生成预览图
      let PreviewBuffer
      try {
        PreviewBuffer = await sharp(FileContent)
          .resize({ width: PREVIEW_WIDTH }) // 设置预览图宽度（高度自动适应）
          .toBuffer() // 生成预览图 Buffer
      } catch (err) {
        console.log('生成预览图错误 err:', err)
        return {
          code: 400,
          runCondition: 'invalid image',
          errMsg: '图片无效',
        }
      }

      // 转预览图为 Base64 格式
      const PreviewBase64 = PreviewBuffer.toString('base64')

      // 准备文件基本信息
      // 获取当前时间并格式化
      const currentDate = new Date();
      const formattedDate = common.formatDate(currentDate);
      const ImageData = {
        // 文件基本信息
        FileName: file.originalname,
        MimeType: file.mimetype,
        Size: file.size,
        UploadTime: formattedDate,

        // 用户信息
        User_Id: new ObjectId(user._id),

        // 文件内容（Base64）
        Data: FileContent.toString('base64'),
        PreviewData: PreviewBase64,

        // 其他元数据
        FieldName: file.fieldname,
        Encoding: file.encoding,
        StoragePath: file.path,
        MetaData: {
          Refer: ctx.headers.referer,
          UserAgent:ctx.headers['user-agent'], 
        }
      }

      // 将images集合存入数据库
      const Result = await ImagesCol.insertOne(ImageData)

      // 收集结果
      UploadResults.push({
        Id: Result.insertedId,
        FileName: file.originalname,
        Size: file.size,
        MimeType: file.mimetype,
        UploadTime: formattedDate,
      })

      // 删除临时文件
      fs.unlinkSync(file.path)
    }

    return {
      code: 200,
      runCondition: 'succeed',
      errMsg: '上传成功',
      UploadResults,
    }

  } catch(err) {
    console.log('上传失败:', err)
    return{
      code: 500,
      runCondition: 'Internal error',
      errMsg: '上传失败',
    }
  }


  
}
