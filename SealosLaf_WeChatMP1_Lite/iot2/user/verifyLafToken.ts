// https://dhb91nur4r.bja.sealos.run/iot2/user/verifyLafToken
import cloud from '@lafjs/cloud'
import common from '../utils/common'

export default async function verifyLafToken (ctx: FunctionContext) {

  // 验证 laf_token
  const laf_token_VerifyRes = await common.verifyTokenAndGetUser(ctx)
  switch (laf_token_VerifyRes.runCondition) {
    case 'token error':
      console.log('laf_token 验证失败')
      return laf_token_VerifyRes  // token 错误, 退出
    default:
      console.log('laf_token 验证成功')
  }

  // 获取用户信息记录
  const user = laf_token_VerifyRes.user  // user._id 即 user_id

  return {
    runCondition: 'succeed',
    user: {
      _id: user._id,
      username: user.username,
      createdAt: user.createdAt,
    }
  }
}
