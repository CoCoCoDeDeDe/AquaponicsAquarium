// https://dhb91nur4r.bja.sealos.run/iot2/product/getProductList
import cloud from '@lafjs/cloud'
import common from '../utils/common'

const db = cloud.mongo.db

export default async function getProductList (ctx: FunctionContext) {

  // 验证 laf_token
  const laf_token_VerifyRes = await common.verifyTokenAndGetUser(ctx)
  switch (laf_token_VerifyRes.runCondition) {
    case 'laf_token error':
      console.log('laf_token 验证失败')
      return laf_token_VerifyRes  // token 错误, 退出
    default:
      console.log('laf_token 验证成功')
      break
  }

  // 获取参数
  let param = {
    skip: 0,
    limit: 0,
  }
  param.skip = Number(ctx.query.skip) // ctx. 解析的内容默认是字符串
  param.limit = Number(ctx.query.limit)
  // 校验参数
  if (!((typeof param.skip === 'number' && !isNaN(param.skip) && isFinite(param.skip)) &&
    (typeof param.limit === 'number' && !isNaN(param.limit) && isFinite(param.limit)))) {
    console.log('参数无效 param:', param)
    return {
      runCondition: 'param error',
      errMsg: '参数无效',
    }
  }

  // 获取产品信息记录
  const user = laf_token_VerifyRes.user  // user._id 即 user_id
  let productList
  try{
    const filter = {}
    const findRes = await db.collection('iot2_products')
      .find(
      filter,
      {
        projection: {
          _id: 1,
          name: 1,
          previewImg_url: 1,
          intro: 1,
          createdAt: 1,
          updateAt: 1,
        },
        sort: {
          updateAt: -1,
          _id: -1,
        },
        skip: param.skip,
        limit: param.limit,
      }).toArray()
      // 结果为空不算错, 是一种正常情况

    productList = findRes
    // console.log('productList:', productList)
  
  } catch(err) {
    console.log('find 错误 err:', err)
    return {
      runCondition: 'db error',
      errMsg: '数据库查找错误',
    }
  }

  // 获取产品信息记录总数
  let total
  try {
    const filter = {}
    const findRes = await db.collection('iot2_products').find(filter).count()
    total = findRes
  } catch (err) {
    console.log('find 错误 err:', err)
    return {
      runCondition: 'db error',
      errMsg: '数据库查找错误',
    }
  }
  console.log('total:', total)

  return {
      runCondition: 'succeed',
      errMsg: '成功',
      productList,
      total: total,
  }
}
