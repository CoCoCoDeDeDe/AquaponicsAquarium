// https://dhb91nur4r.bja.sealos.run/iot2/smartLinkGroup/EditSLGroup
import cloud from '@lafjs/cloud'
import common from '../utils/common'

const db = cloud.mongo.db

// 目前只适合编辑名字
export default async function EditSLGroup(ctx: FunctionContext) {

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
  let user = laf_token_VerifyRes.user  // 内含 user._id 即 user_id
  user._id = new ObjectId(user._id) // 可能有bug，无错误处理
  // console.log('user:', user)

  // 校验参数 SLGroup_Id 的传入情况
  if (ctx.query.SLGroup_Id === undefined || ctx.query.SLGroup_Id === null || typeof ctx.query.SLGroup_Id !== 'string' || ctx.query.SLGroup_Id.trim() === '') {
    console.log('获取参数出错 ctx.query:', ctx.query)
    return {
      runCondition: 'para error',
      errMsg: '获取参数出错',
    }
  }

  // 获取参数 SLGroup_Id
  const Target_SLGroup_Id = ctx.query.SLGroup_Id
  // console.log('获取到 SLGroup_Id:', Target_SLGroup_Id)

  // 校验 smartLinkGroup_id 对应 group 是否存在
  // 检查目标是否存在
  let Res_IsSLGroupExist
  try {
    Res_IsSLGroupExist = await db.collection('iot2_smartLinkGroups').countDocuments(
      {
        _id: { $eq: new ObjectId(Target_SLGroup_Id) }
      }
    )
  } catch (err) {
    console.log('查找目标智联组错误 err:', err)
    return {
      runCondition: 'db error',
      errMsg: '查找目标智联组错误',
    }
  }
  // console.log('检查目标智联组是否存在 Res_IsSLGroupExist:', Res_IsSLGroupExist)
  switch (Res_IsSLGroupExist) {
    case 1: {
      // console.log('要删除的目标智联组存在且仅存在一个')
      break
    }
    case 0: {
      console.log('要删除的目标智联组记录不存在 Res_IsSLGroupExist', Res_IsSLGroupExist)
      return {
        runCondition: 'records error',
        errMsg: `目标智联组不存在`,
      }
    }
    default: {
      console.log('要删除的目标智联组记录错误 Res_IsSLGroupExist', Res_IsSLGroupExist)
      return {
        runCondition: 'records error',
        errMsg: `要删除的目标智联组记录错误 ${Res_IsSLGroupExist}`,
      }
    }
  }

  // 检查目标是否属于 token 对应的用户
  let Res_IsSLGroupPermit
  const TargetSLGroup_Query = {
    _id: { $eq: new ObjectId(Target_SLGroup_Id) },
    user_id: { $eq: new ObjectId(user._id) },
  }
  try {
    Res_IsSLGroupPermit = await db.collection('iot2_smartLinkGroups').countDocuments(
      TargetSLGroup_Query
    )
  } catch (err) {
    console.log('查找与用户匹配的目标智联组错误 err:', err)
    return {
      runCondition: 'permit error',
      errMsg: '查找与用户匹配的目标智联组错误',
    }
  }
  // console.log('检查目标智联组是否属于 token 对应的用户 Res_IsSLGroupPermit:', Res_IsSLGroupPermit)
  switch (Res_IsSLGroupPermit) {
    case 1: {
      // console.log('要绑定的与用户匹配的目标智联组存在且仅存在一个')
      break
    }
    default: {
      console.log('要绑定的与用户匹配的目标智联组记录错误 Res_IsSLGroupPermit', Res_IsSLGroupPermit)
      return {
        runCondition: 'records error',
        errMsg: `权限验证时错误`,
      }
    }
  }

  // 校验 UniIOList 是否有传入且有效
  // console.log('ctx.body:', ctx.body)
  // console.log('ctx.body.SLGroup_Name:', ctx.body.SLGroup_Name)
  // console.log('ctx.body.SLGroup_Name.length:', ctx.body.SLGroup_Name.length)
  try {
    if (ctx.body === undefined || ctx.body.SLGroup_Name === undefined 
    || ctx.body.SLGroup_Name === null || typeof ctx.body.SLGroup_Name !== 'string' 
    || ctx.body.SLGroup_Name.trim() === '') {
      throw new Error('传入的 SLGroup_Name 无效')
    }
  } catch (err) {
    console.log('无效的 SLGroup_Name 参数 ctx.body.SLGroup_Name:', ctx.body.SLGroup_Name)
    return {
      runCondition: 'para error',
      errMsg: '无效的 SLGroup_Name 参数',
    }
  }
  const New_SLGroup_Name = ctx.body.SLGroup_Name
  // console.log('New_SLGroup_Name:', New_SLGroup_Name)
  
  // 更新
  let Res_Update_SLGroup
  try{
    Res_Update_SLGroup = await db.collection('iot2_smartLinkGroups')
      .updateOne(
        {
          _id: { $eq: new ObjectId(Target_SLGroup_Id) }
        },
        {
          $set: {
            name: New_SLGroup_Name,
          }
        }
      )
  } catch (err) {
    console.log('更新智联组记录错误 err:', err)
    return {
      runCondition: 'db error',
      errMsg: '更新智联组记录错误',
    }
  }
  // console.log('Res_Update_SLGroup:', Res_Update_SLGroup)

  if (Res_Update_SLGroup.modifiedCount === 0 && Res_Update_SLGroup.matchedCount === 1) {
    // 更改无效，新旧名字相同
    console.log('更改无效，新旧名字相同, Res_Update_SLGroup:', Res_Update_SLGroup)
    return {
      runCondition: 'update error',
      errMsg: '更改无效，新旧名字相同',
    }
  } else if (!(Res_Update_SLGroup.modifiedCount === 1 && Res_Update_SLGroup.matchedCount === 1)) {
    // 更改无效
    console.log('未知更新结果, Res_Update_SLGroup:', Res_Update_SLGroup)
    return {
      runCondition: 'succeed',
      errMsg: '未知更新结果',
    }
  }

  // 更改有效
  console.log('更改成功')
  return {
    runCondition: 'succeed',
    errMsg: '更改无效，新旧名字相同',
  }


}
