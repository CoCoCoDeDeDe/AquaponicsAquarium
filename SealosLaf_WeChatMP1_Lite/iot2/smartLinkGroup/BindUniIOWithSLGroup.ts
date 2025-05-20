// https://dhb91nur4r.bja.sealos.run/iot2/smartLinkGroup/BindUniIOWithSLGroup
import cloud from '@lafjs/cloud'
import common from '../utils/common'

const db = cloud.mongo.db

// 接收 {_id: 'uniIO_id'} 数组
// 缺点1：Device权限验证不严谨
// 缺点2：重复绑定未校验和阻止
export default async function addUnIOsToSmartLinkGroup (ctx: FunctionContext) {

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
  // console.log('ctx.body.UniIOList:', ctx.body.UniIOList)
  // console.log('ctx.body.UniIOList.length:', ctx.body.UniIOList.length)
  try{
    if (ctx.body === undefined || ctx.body.UniIOList === undefined || ctx.body.UniIOList === null || ctx.body.UniIOList.length <= 0) {
      throw new Error('传入的 UniIOList 无效')
    }
  } catch (err) {
    console.log('无效的 UniIOList 参数 ctx.body.UniIOList:', ctx.body.UniIOList)
    return {
      runCondition: 'para error',
      errMsg: '无效的 UniIOList 参数',
    }
  }

  // 组织 UniIOList 结构用于数据库搜索
  const Target_UniIOList = ctx.body.UniIOList
  let Target_UniIOIdList = [] //  要作为数组操作，先赋值空数组
  // console.log('Target_UniIOList:', Target_UniIOList)
  for (let i = 0; i < Target_UniIOList.length; i++) {
    // console.log('Target_UniIOList[i]:', Target_UniIOList[i])
    // console.log('Target_UniIOList[i]._id:', Target_UniIOList[i]._id)
    try{
      Target_UniIOIdList[i] = new ObjectId(Target_UniIOList[i]._id)
    } catch (err) {
      console.log('传入的 UniIOList 参数错误 ctx.body.UniIOList:', ctx.body.UniIOList)
      console.log('err:', err)
      return {
        runCondition: 'para error',
        errMsg: '无效的 UniIOList 参数',
      }
    }
    // console.log('Target_UniIOIdList[i]:', Target_UniIOIdList[i])
  }

  // 校验 uniIO_id 对应 uniIOs 是否存在
  // 检查目标是否存在
  let Res_IsUniIOExist
  try {
    Res_IsUniIOExist = await db.collection('iot2_uniIOs').countDocuments(
      {
        _id: { $in: Target_UniIOIdList }
      }
    )
  } catch (err) {
    console.log('校验 uniIO_id 对应 uniIOs 是否存在时错误 err:', err)
    return {
      runCondition: 'db error',
      errMsg: '校验 uniIO_id 对应 uniIOs 是否存在时错误',
    }
  }
  console.log('校验 uniIO_id 对应 uniIO 是否存在 Res_IsUniIOExist:', Res_IsUniIOExist)
  if (Res_IsUniIOExist != Target_UniIOList.length) {
    console.log('UniIO 记录的数量错误')
    return {
      runCondition: 'records error',
      errMsg: 'UniIO 记录的数量错误',
    }
  }

  // 校验 uniIO_id 对应 uniIO 是否属于该 user
  // 先获取每个 UniIO_Id 对应的 Device_Id ，再校验这些 Device_Id 是否都属于 token 对应的用户
  let Res_Get_Device_Id_List
  try{
    Res_Get_Device_Id_List = await db.collection('iot2_uniIOs')
      .find(
        {
          _id: { $in: Target_UniIOIdList }
        },
        {
          projection: {
            _id: 0,
            device_id: 1,
          }
        }
      ).toArray()
  } catch (err) {
    console.log('寻找每个 UniIO 对应的 Device 错误', err)
    return {
      runCondition: 'db error',
      errMsg: '寻找每个 UniIO 对应的 Device 错误',
    }
  }
  // console.log('Res_Get_Device_Id_List:', Res_Get_Device_Id_List)

  // 转对象数组为字符串集合 并且 去除重复的 Device_Id
  let Device_Id_Set = new Set()
  let Device_Id_List = []
  for (let i = 0; i < Res_Get_Device_Id_List.length; i++) {
    // 没有获取过的 Device_Id 才保存
    if (!Device_Id_Set.has(Res_Get_Device_Id_List[i].device_id.toString())) {
      
      // Set 用于管理是否重复获取 Device_Id ，其判断功能需要依赖转为 string 的 ObjectId
      Device_Id_Set.add(Res_Get_Device_Id_List[i].device_id.toString())
      // List 仍然保持 Object_Id
      Device_Id_List[i] = Res_Get_Device_Id_List[i].device_id
    }
  }
  // console.log('Device_Id_List:', Device_Id_List)
  // console.log('Device_Id_Set:', Device_Id_Set)
  // console.log('D[...Device_Id_Set]:', [...Device_Id_Set])
  // console.log('Device_Id_List:', Device_Id_List)

  // 验证 UniIOs 所属的 Devices 是否属于 token user
  // 判断不严重，每个记录的情况不清楚，可能有的少了有的多了导致总体数量正常
  let Res_Is_Device_Permit
  try {
    Res_Is_Device_Permit = await db.collection('iot2_devices').countDocuments(
      {
        _id: { $in: Device_Id_List },
        user_id: { $eq: user._id }
      }
    )
  } catch (err) {
    console.log('校验 uniIO_id 对应 uniIO 是否属于该 user 时错误 err:', err)
    return {
      runCondition: 'permit error',
      errMsg: '权限验证时错误',
    }
  }
  // console.log('校验 uniIO_id 对应 uniIO 是否属于该 user Res_Is_Device_Permit:', Res_Is_Device_Permit)
  if (Res_Is_Device_Permit != Device_Id_List.length) {
    console.log('校验 Device 是否属于该 user 记录数量错误 或 权限错误')
    return {
      runCondition: 'records error',
      errMsg: 'Device 记录或权限错误',
    }
  }
  // console.log('设备权限验证通过')

  // 绑定 UniIOs 与 SLGroup
  let Res_Bind_UniIOs_With_SLGroup
  try{
    Res_Bind_UniIOs_With_SLGroup = await db.collection('iot2_uniIOs')
      .updateMany(
        {
          _id: { $in: Target_UniIOIdList }
        },
        {
          $set: {
            smartLinkGroup_id: new ObjectId(Target_SLGroup_Id)
          }
        }
      )
  } catch (err) {
    console.log('绑定 UniIOs 与 SLGroup 错误 err', err)
    return {
      runCondition: 'db error',
      errMsg: '绑定 UniIOs 与 SLGroup',
    }
  }

  console.log('绑定 UniIOs 与 SLGroup 成功 Res_Bind_UniIOs_With_SLGroup', Res_Bind_UniIOs_With_SLGroup)
  return {
    runCondition: 'succeed',
    errMsg: '绑定 UniIOs 与 SLGroup 成功',
    Target_UniIOIdList,
  }

}
