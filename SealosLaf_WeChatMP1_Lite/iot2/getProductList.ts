import cloud from '@lafjs/cloud'
import common from './utils/common'

const db = cloud.mongo.db

export default async function getProductList (ctx: FunctionContext) {

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

  // 获取产品信息记录
  const user = laf_token_VerifyRes.user  // user._id 即 user_id
  let productList
  try{
    const filter = {}
    const findRes = await db.collection('iot2_products')
      .find(
      filter,
      {

      })
  } catch(err) {
    console.log('find 错误 err:', err)
    return {
      runCondition: 'db error',
      errMsg: '数据库查找错误',
    }
  }
}
