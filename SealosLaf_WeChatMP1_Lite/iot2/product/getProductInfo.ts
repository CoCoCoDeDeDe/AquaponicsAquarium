// https://dhb91nur4r.bja.sealos.run/iot2/product/getProductInfo
import cloud from '@lafjs/cloud'
import common from '../utils/common'

const db = cloud.mongo.db

export default async function getProductInfo (ctx: FunctionContext) {

  // 验证 laf_token
  const laf_token_VerifyRes = await common.verifyTokenAndGetUser(ctx)
  switch (laf_token_VerifyRes.runCondition) {
    case 'token error':
      console.log('laf_token 验证失败')
      return laf_token_VerifyRes  // token 错误, 退出
    default:
      console.log('laf_token 验证成功')
      break
  }
  const user = laf_token_VerifyRes.user  // user._id 即 user_id
  console.log('user._id:', user._id)

  // 获取参数
  let param = {
    product_id: 0,
  }
  param.product_id = ctx.query.product_id // ctx. 解析的内容默认是字符串

  // 校验参数
  if (!common.isValidNonEmptyString(param.product_id)) {
    console.log('参数无效 param:', param)
    return {
      runCondition: 'param error',
      errMsg: '参数无效',
    }
  }
  console.log('参数有效 param:', param)

  // return
  
  const filter = {
    _id: { $eq: new ObjectId(param.product_id) }
  }
  const options = {
    projection: {
      _id: 1,
      name: 1,
      previewImg_url: 1,
      intro: 1,
      createdAt: 1,
      updateAt: 1,
    }
  }
  let productProfile
  try{
    const result = await db.collection('iot2_products').findOne(filter, options)
    console.log('result:', result)
    productProfile = result
  } catch (err) {
    console.log('find 错误 err:', err)
    return {
      runCondition: 'db error',
      errMsg: '数据库查找错误',
    }
  }

  return {
    runCondition: 'succeed',
    errMsg: '成功',
    productProfile,
  }
}
