// https://dhb91nur4r.bja.sealos.run/iot2/smartLinkGroup/GetSLGroupProfileList
import cloud from '@lafjs/cloud'
import common from '../utils/common'

const db = cloud.mongo.db

export default async function GetSLGroupProfileList(ctx: FunctionContext) {

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

  // 获取 user 信息
  const user = laf_token_VerifyRes.user  // 内含 user._id 即 user_id
  // console.log('user:', user)
  
  // 查询 智联组 集合中 user_id 属性 符合 token 中的 user_id 的 智联组 记录
  let Res_FindSLGroup
  try{
    Res_FindSLGroup = await db.collection('iot2_smartLinkGroups').find(
      {
        user_id: {$eq: new ObjectId(user._id)},
      }
    ).toArray()
  } catch (err) {
    console.log('查询智联组错误 err:', err)
    return {
      runCondition: 'db error',
      errMsg: '查询智联组错误',
    }
  }
  console.log('Res_FindSLGroup:', Res_FindSLGroup)

  // 整理智联组表的数据格式
  let SLGroupProfileList = []
  try{
    for (let i = 0; i < Res_FindSLGroup.length; i++) {
      SLGroupProfileList[i] = {
        SLGroup_Id: Res_FindSLGroup[i]._id,
        SLGroup_Name: Res_FindSLGroup[i].name,
        SLGroup_CreatedTime: Res_FindSLGroup[i].createdAt,
        SLGroup_UpdatedTime: Res_FindSLGroup[i].updatedAt,
      }
    }
  } catch(err) {
    const errMsg = '数据处理出错'
    console.log(errMsg, ' err:', err)
    return {
      runCondition: 'data error',
      errMsg,
    }
  }

  const errMsg = '获取token用户的智联组简介表成功'
  console.log(errMsg)
  return {
    runCondition: 'succeed',
    errMsg,
    SLGroupProfileList,
  }

}
