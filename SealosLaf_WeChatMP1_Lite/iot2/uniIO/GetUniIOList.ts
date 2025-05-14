// https://dhb91nur4r.bja.sealos.run/iot2/uniIO/GetUniIOList
import cloud from '@lafjs/cloud'
import common from '../utils/common'

const db = cloud.mongo.db

// huawei_device_id
// AQAQ25032901
// smartLinkGroup_id
// 681ad710ec955cc190f61ae8

// 若传入 huawei_device_id 按 huawei_device_id 查找
// 若传入 smartLinkGroup_id 按 smartLinkGroup_id 查找
// 若同时传入 huawei_device_id 和 smartLinkGroup_id, 按 huawei_device_id 查找
// 若 huawei_device_id 和 smartLinkGroup_id 都未传入，按 user_id 查找
export default async function GetUniIOList (ctx: FunctionContext) {

  // 检验 laf_token 获取 user_id
  const laf_token_VerifyRes = await common.verifyTokenAndGetUser(ctx)
  switch (laf_token_VerifyRes.runCondition) {
    case 'token error':
      console.log('laf_token 验证失败')
      return laf_token_VerifyRes  // token 错误, 退出
    default:
      // console.log('laf_token 验证成功')
      break
  }
  let user = laf_token_VerifyRes.user  // user._id 即 user_id
  user._id = new ObjectId(user._id)
  // console.log('user._id:', user._id)

  // 获取参数 huawei_device_id, smartLinkGroup_id
  let param = {
    huawei_device_id: 0,
    smartLinkGroup_id: 0,
  }
  param.huawei_device_id = ctx.query.huawei_device_id
  param.smartLinkGroup_id = ctx.query.smartLinkGroup_id
  let paramArr = await Object.values(param)

  // 检验参数 huawei_device_id smartLinkGroup_id
  let uniIOListFilter
  for (let i = 0; i < paramArr.length; i++) {
    if (common.isValidNonEmptyString(paramArr[i])) {
      // 根据循环索引与参数数组元素的顺序判断第一个有效的参数
      // 根据第一个有效的参数决定之后用于在 uniIO 集合中收集 uniIOList 的查询条件
      // 可优化：不用 switch 的方法，体检建立参数对照表，不用参数用不同的 filter 条件，拿到参数后按表匹配
      switch(i) {
        case 0:
          console.log('用 huawei_device_id')
          uniIOListFilter = {
            huawei_device_id: { $eq: paramArr[i]}
          }
          break
        case 1:
          console.log('用 smartLinkGroup_id')
          uniIOListFilter = {
            smartLinkGroup_id: { $eq: new ObjectId(paramArr[i]) }
          }
          break
      }
      // 在 if 内跳出，不再循环本 for
      break
    }
    // 不进 if 则下一个循环
    // 如果到这里时 i >= paramArr.length - 1，则说明没有下一个循环了而且参数都没有匹配到
    if (i >= paramArr.length - 1) {
      console.log('用 user_id')
      
      // 获取 user 的所有 device_id 组成 device_id 数组作为 uniIOs 的查询条件
      let Res_Find_Device_Id
      try{
        Res_Find_Device_Id = await db.collection('iot2_devices')
          .find(
            {
              user_id: { $eq: new ObjectId(user._id) }
            },
            {
              projection: {
                _id: 1,
              }
            }
          ).toArray()
      } catch (err) {
        console.log('查找 device_id 错误 err:', err)
        return {
          runCondition: 'db error',
          errMsg: '查找 device_id 错误',
        }
      }
      // console.log('Res_Find_Device_Id:', Res_Find_Device_Id)
      if (!Res_Find_Device_Id.length) {
        console.log('查找 device_id为空')
        return {
          runCondition: 'records error',
          errMsg: '用户没有任何设备',
        }
      }

      // 将存有 Device_Id 的对象数组转化为 Device_Id ObjectId 数组
      let Device_Id_List = []
      for (let i = 0; i < Res_Find_Device_Id.length; i++) {
        Device_Id_List[i] = new ObjectId(Res_Find_Device_Id[i]._id)
      }
      // console.log('Device_Id_List:', Device_Id_List)
      // Device_Id_List = []
      uniIOListFilter = {
        device_id: { $in: Device_Id_List }
      }
    }
  }
  console.log('uniIOListFilter 构建完成:', uniIOListFilter)

  // 按 uniIOListFilter 查找符合条件的 uniIO
  let uniIOList
  try{
    const response = await db.collection('iot2_uniIOs').find(
      uniIOListFilter,
      {
        projection: {
          _id: 1,
          // templateName: 1,

        }
      }
    ).toArray()

    // 不校验查询结果是否为空，为空是一种情况

    // 获取结果
    uniIOList = response

  } catch (err) {
    console.log('查找 uniIO 错误 err:', err)
    return {
      runCondition: 'db error',
      errMsg: '查找 uniIO 错误',
    }
  }

  // console.log('获取到 uniIOList:', uniIOList)
  console.log('获取 uniIOList 成功, uniIOList.length:', uniIOList.length)
  const returnTmp = {
    runCondition: 'succeed',
    errMsg: '获取 uniIOList 成功',
    uniIOList
  }
  return returnTmp

}
