// https://dhb91nur4r.bja.sealos.run/iot/requestAccountProfile
import cloud from '@lafjs/cloud';
import { ObjectId } from 'mongodb';
import verifyToken from '../utils/verifyToken'

export default async function requestAccountProfile(ctx: FunctionContext) {

  const tokenVerifyResult = await verifyToken(ctx)
  switch (tokenVerifyResult.runCondition) {
    case 'token parse error':
    case 'cant find the account':
      return tokenVerifyResult  // return
    case 'laf_token verify succeed':
      break
  }

  const user_id = tokenVerifyResult.user_id
  const db = cloud.mongo.db

  console.log('获取到 user_id:', user_id)

  try{
    const res = await db.collection("users")
      .findOne({
        _id: new ObjectId(user_id)
      })

    if(res == null || res == '') {
      console.log('laf_token验证成功但未找到account profule')
      return {
        runCondition: 'cant find the account',
        errMsg: 'cant find the account'
      }
    } else {
      console.log('找 account profule 成功')
      return {
        runCondition: 'find account profile succeed',
        errMsg: 'find account profile succeed',
        data: res
      }
    }

  } catch (err) {
    console.log('查找 account profile 错误')
    return {
      runCondition: 'find account profile error',
      errMsg: 'find account profile error'
    }
  }


}