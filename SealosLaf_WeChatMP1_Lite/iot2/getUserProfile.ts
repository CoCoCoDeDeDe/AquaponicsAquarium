// https://dhb91nur4r.bja.sealos.run/iot2/getUserProfile
import cloud from '@lafjs/cloud'
import common from './utils/common'

const db = cloud.mongo.db

export default async function getUserProfile (ctx: FunctionContext) {

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

  // 获取用户信息记录
  const user = laf_token_VerifyRes.user  // user._id 即 user_id
  let userProfile
  try{
    const findUserProfileRes = await db.collection('iot2_users')
      .findOne({
        _id: { $eq: new ObjectId(user._id)}
      }, 
      {
        projection: {
          _id: 1,
          username: 1,
          nickname: 1,
          avatar_url: 1,
          createdAt: 1,
          updateAt: 1,
        }
      })

    if (findUserProfileRes === null || findUserProfileRes === '') {
      throw Error('find 结果为空')
    }

    // 成功的继续
    userProfile = findUserProfileRes
  } catch(err) {
    console.log('find 用户信息记录错误 err:', err)
    return {
      runCondition: 'db error',
      errMsg: '查找用户信息错误',
    }
  }

  return {
      runCondition: 'succeed',
      errMsg: '查找用户信息成功',
      userProfile,
  }
}
