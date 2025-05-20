// https://dhb91nur4r.bja.sealos.run/iot2/smartLinkGroup/DeleteSmartLinkGroup
import cloud from '@lafjs/cloud'
import common from '../utils/common'

const db = cloud.mongo.db

export default async function (ctx: FunctionContext) {

  // 验证 laf_token
  const laf_token_VerifyRes = await common.verifyTokenAndGetUser(ctx)
  switch (laf_token_VerifyRes.runCondition) {
    case 'laf_token error':
      console.log('laf_token 验证失败')
      return laf_token_VerifyRes  // token 错误, 退出
    default:
      // console.log('laf_token 验证成功')
      break
  }

  // 获取参数 user_id
  const user = laf_token_VerifyRes.user  // 内含 user._id 即 user_id

  // 校验参数 SLGroup_Id 的传入情况
  if (ctx.query.SLGroup_Id === undefined || ctx.query.SLGroup_Id === null || typeof ctx.query.SLGroup_Id !== 'string' || ctx.query.SLGroup_Id.trim() === '' ) {
    console.log('获取参数出错 ctx.query:', ctx.query)
    return {
      runCondition: 'para error',
      errMsg: '获取参数出错',
    }
  }

  // 获取参数 SLGroup_Id
  const Target_SLGroup_Id = ctx.query.SLGroup_Id
  // console.log('获取到 SLGroup_Id:', Target_SLGroup_Id)

  // 检查目标是否存在
  let Res_IsExist
  try{
    Res_IsExist = await db.collection('iot2_smartLinkGroups').countDocuments(
      {
        _id: {$eq: new ObjectId(Target_SLGroup_Id)}
      }
    )
  } catch(err) {
    console.log('查找目标智联组错误 err:', err)
    return {
      runCondition: 'db error',
      errMsg: '查找目标智联组错误',
    }
  }
  // console.log('检查目标智联组是否存在 Res_IsExist:', Res_IsExist)
  switch (Res_IsExist) {
    case 1: {
      // console.log('要删除的目标智联组存在且仅存在一个')
      break
    }
    case 0: {
      console.log('要删除的目标智联组记录不存在 Res_IsExist', Res_IsExist)
      return {
        runCondition: 'records error',
        errMsg: `目标智联组不存在`,
      }
    }
    default: {
      console.log('要删除的目标智联组记录错误 Res_IsExist', Res_IsExist)
      return {
        runCondition: 'records error',
        errMsg: `要删除的目标智联组记录错误 ${Res_IsExist}`,
      }
    }
  }

  // 检查目标是否属于 token 对应的用户
  let Res_IsPermit
  const TargetSLGroup_Query = {
    _id: { $eq: new ObjectId(Target_SLGroup_Id) },
    user_id: { $eq: new ObjectId(user._id) },
  }
  try {
    Res_IsPermit = await db.collection('iot2_smartLinkGroups').countDocuments(
      TargetSLGroup_Query
    )
  } catch (err) {
    console.log('查找与用户匹配的目标智联组错误 err:', err)
    return {
      runCondition: 'db error',
      errMsg: '查找与用户匹配的目标智联组错误',
    }
  }
  // console.log('检查目标是否属于 token 对应的用户 Res_IsPermit:', Res_IsPermit)
  switch (Res_IsPermit) {
    case 1: {
      // console.log('要删除的与用户匹配的目标智联组存在且仅存在一个')
      break
    }
    default: {
      console.log('要删除的与用户匹配的目标智联组记录错误 Res_IsPermit', Res_IsPermit)
      return {
        runCondition: 'record error',
        errMsg: `权限错误`,
      }
    }
  }

  // 执行删除目标智联组关联的所有 UniIO
  let Res_Update_BoundUniIO
  try {
    Res_Update_BoundUniIO = await db.collection('iot2_uniIOs').updateMany(
      {
        smartLinkGroup_id: { $eq: new ObjectId(Target_SLGroup_Id) }
      },
      {
        $unset: {
          smartLinkGroup_id: 1,
        }
      }
    )
  } catch (err) {
    console.log('删除目标智联组关联的所有 UniIO 错误 err:', err)
    return {
      runCondition: 'db error',
      errMsg: '删除目标智联组关联的所有 UniIO 错误',
    }
  }
  // console.log('删除目标智联组 Res_Update_BoundUniIO:', Res_Update_BoundUniIO)
  // console.log('删除目标智联组关联的所有 UniIO 操作成功 Res_Update_BoundUniIO:', Res_Update_BoundUniIO)
  const UnbindedUniIOCount = Res_Update_BoundUniIO.modifiedCount
  // console.log('UnbindedUniIOCount:', UnbindedUniIOCount)


  // 执行删除目标智联组
  let Res_Delete_TargetSLGroup
  try {
    Res_Delete_TargetSLGroup = await db.collection('iot2_smartLinkGroups').deleteOne(TargetSLGroup_Query)
  } catch (err) {
    console.log('删除智联组错误 err:', err)
    return {
      runCondition: 'db error',
      errMsg: '删除智联组错误',
    }
  }
  // console.log('删除目标智联组 Res_Delete_TargetSLGroup:', Res_Delete_TargetSLGroup)
  switch (Res_Delete_TargetSLGroup.deletedCount) {
    case 1: {
      console.log('删除操作成功 删除的与用户匹配的目标智联组存在且仅存在一个 Res_Delete_TargetSLGroup', Res_Delete_TargetSLGroup)
      break
    }
    default: {
      console.log('删除操作结果的删除计数错误 Res_Delete_TargetSLGroup', Res_Delete_TargetSLGroup)
      return {
        runCondition: 'record error',
        errMsg: `删除操作错误`,
      }
    }
  }


  // 返回成功删除成功的结果
  return {
    runCondition: 'succeed',
    errMsg: `删除操作成功`,
    UnbindedUniIOCount,
    SLGroup_Id: Target_SLGroup_Id,
  }


}
