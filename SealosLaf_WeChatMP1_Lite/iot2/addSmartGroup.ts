import cloud from '@lafjs/cloud'
import common from './utils/common'

const db = cloud.mongo.db

export default async function addSmartGroup (ctx: FunctionContext) {

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

  // 获取参数 user_id, smartGroup_name
  const user = laf_token_VerifyRes.user  // 内含 user._id 即 user_id
  let smartGroup_name
  try {
    smartGroup_name = await ctx.query['name']
    if (smartGroup_name === null || smartGroup_name === '') {
      throw new Error('参数 name 无效')
    }
  } catch (err) {
    console.log('获取参数出错 err:', err)
    return {
      runCondition: 'parameter error',
      errMsg: '获取参数出错',
    }
  }

  // insert smartGroup
  // 获取当前时间并格式化
  const currentDate = new Date();
  const formattedDate = common.formatDate(currentDate);
  try {
    db.collection('iot2_smartLinkGroups')
      .insertOne({
        name: smartGroup_name,
        user_id: new ObjectId(user._id),
        createdAt: formattedDate,
        updatedAt: formattedDate,
      })

  } catch(err) {
    console.log('insert smartGroup err:', err)
    return {
      runCondition: 'db error',
      errMsg: 'insert smartGroup err',
    }
  }

  return {
    runCondition: 'succeed',
    errMsg: 'succeed',
  }

}
