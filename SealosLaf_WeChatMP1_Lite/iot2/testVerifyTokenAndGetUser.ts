// ./代表当前目录，../代表双亲目录，/代表根目录
import cloud from '@lafjs/cloud'
import common from './utils/common'

export default async function (ctx: FunctionContext) {
  
  // 验证 token
  const tokenVerifyReport = await common.verifyTokenAndGetUser(ctx)
  switch (tokenVerifyReport.runCondition) {
    case 'token error':
      console.log('laf_token 验证失败')
      return tokenVerifyReport  // token 错误, 退出
    default:
      console.log('laf_token 验证成功')
      break
  }



}
