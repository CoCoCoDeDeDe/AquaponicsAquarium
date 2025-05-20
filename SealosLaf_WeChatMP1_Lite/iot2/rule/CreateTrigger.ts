// https://dhb91nur4r.bja.sealos.run/iot2/rule/CreateTrigger

import cloud from '@lafjs/cloud'
import common from '../utils/common'

export default async function (ctx: FunctionContext) {

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



}
