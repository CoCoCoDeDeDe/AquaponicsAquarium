import cloud from '@lafjs/cloud'
import common from './utils/common'

const db = cloud.mongo.db

export default async function addUnIOsToSmartLinkGroup (ctx: FunctionContext) {

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

  // 获取参数 user_id, smartLinkGroup_id, uniIO_id
  const user = laf_token_VerifyRes.user  // 内含 user._id 即 user_id
  



}
