// https://dhb91nur4r.bja.sealos.run/iot2/smartLinkGroup/GetSmartLinkGroupInfo
import cloud from '@lafjs/cloud'
import common from '../utils/common'

type Type_SmartLinkGroupInfo = {
  SmartLinkGroup_Name: string,
  SmartLinkGroup_CreateTime: string,
  SmartLinkGroup_UpdateTime: string,
}

const db = cloud.mongo.db

// smartLinkGroup_id
// 681ad710ec955cc190f61ae8
// Authorization
// Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1aWQiOiI2ODE0NzBhYzY1OGI1MDQ1NjZjYTJjMjUiLCJleHAiOjE3NDczMTU2MjgsImlhdCI6MTc0NjcxMDgyOH0.006x74ukzwtp1fW3s-vzZp7KqsFIKiHuQwyKjO-QXhY

export default async function GetSmartLinkGroupInfo(ctx: FunctionContext) {

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

  // 获取 user 信息
  const user = laf_token_VerifyRes.user  // 内含 user._id 即 user_id
  // console.log('user:', user)

  // 校验 smartLinkGroup_id 是否有传入且有效
  if (ctx.query.smartLinkGroup_id == undefined ||
    ctx.query.smartLinkGroup_id.trim() == '') {
    console.log('无效的 Query 参数 ctx.Query.smartLinkGroup_id:', ctx.query.smartLinkGroup_id)
    return {
      runCondition: 'para error',
      errMsg: '无效的 Query 参数',
    }
  }

  // 校验 smartLinkGroup_id 对应 group 是否存在
  // 略

  // 校验 smartLinkGroup_id 对应 group 是否属于该 user
  // 略

  // 获取 smartLinkGroup_id
  const Query = await ctx.query
  // console.log('Query:', Query)

  // 查询 smart link group info
  let SmartLinkGroupInfo = {} as Type_SmartLinkGroupInfo
  try{
    const Result = await db.collection('iot2_smartLinkGroups').findOne(
      {
        _id: { $eq: new ObjectId(Query.smartLinkGroup_id)}
      },
      {
        projection: {
          _id: 0,
          name: 1,
          createdAt: 1,
          updatedAt: 1,
        }
      }
    )
    if (Result == null || Result == undefined) {
      console.log('查询 SmartLinkGroupInfo 为空')
      return {
        runCondition: 'not found',
        errMsg: '查询 SmartLinkGroupInfo 为空',
      }
    }
    SmartLinkGroupInfo.SmartLinkGroup_Name = Result.name
    SmartLinkGroupInfo.SmartLinkGroup_CreateTime = Result.createdAt
    SmartLinkGroupInfo.SmartLinkGroup_UpdateTime = Result.updatedAt
  } catch (err) {
    console.log('查询 SmartLinkGroupInfo 错误 err:', err)
    return {
      runCondition: 'db error',
      errMsg: '查询 SmartLinkGroupInfo 错误',
    }
  }

  const errMsg = '获取智联组信息成功'
  console.log(errMsg)
  return {
    runCondition: 'succeed',
    errMsg,
    SmartLinkGroupInfo,
  }
  
}
