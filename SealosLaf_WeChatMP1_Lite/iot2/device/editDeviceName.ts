// https://dhb91nur4r.bja.sealos.run/iot2/device/editDeviceName
import cloud from '@lafjs/cloud'
import common from '../utils/common'

const db = cloud.mongo.db

export default async function (ctx: FunctionContext) {

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
  const user = laf_token_VerifyRes.user  // user._id 即 user_id
  console.log('user._id:', user._id)

  // 获取参数
  let param = {
    name: '',
    huawei_device_id: '',
  }
  param.name = ctx.query.name
  param.huawei_device_id = ctx.query.huawei_device_id

  // 校验参数
  if ((!common.isValidNonEmptyString(param.name)) || (!common.isValidNonEmptyString(param.huawei_device_id))) {
    console.log('参数无效 param:', param)
    return {
      runCondition: 'param error',
      errMsg: '参数无效',
    }
  }
  console.log('参数有效 param:', param)

  // // 校验设备是否存在
  // try{
  //   const result = await db.collection('iot2_devices').countDocuments({
  //     huawei_device_id: { $eq: param.huawei_device_id }
  //   })

  //   console.log('匹配设备 result:', result)
  //   if (result === 0) {
  //     console.log('未匹配到设备')
  //     return {
  //       runCondition: 'unmatch',
  //       errMsg: '未匹配到设备',
  //     }
  //   }
  //   if (result !== 1) {
  //     console.log('匹配的设备多于 1 个')
  //     return {
  //       runCondition: 'unmatch',
  //       errMsg: '匹配的设备多于 1 个',
  //     }
  //   }
  //   // 匹配到 1 个设备，情况正常，继续
  // } catch (err) {
  //   console.log('db err:', err)
  //   return {
  //     runCondition: 'db error',
  //     errMsg: '数据库错误',
  //   }
  // }

  // 校验用户与设备绑定情况
  try {
    const result = await db.collection('iot2_devices').countDocuments({
      huawei_device_id: { $eq: param.huawei_device_id },
      user_id: {$eq: new ObjectId(user._id)}
    })

    console.log('用户与设备匹配情况 result:', result)
    if (result === 0) {
      console.log('设备不匹配')
      return {
        runCondition: 'unmatch',
        errMsg: '设备不匹配',
      }
    }
    if (result !== 1) {
      console.log('匹配的设备多于 1 个')
      return {
        runCondition: 'unmatch',
        errMsg: '匹配的设备多于 1 个',
      }
    }
    // 匹配到 1 个同时匹配华为 id 和 user_id的设备，情况正常，继续
    console.log('同时匹配华为 id 和 user_id的设备匹配成功')
  } catch (err) {
    console.log('db err:', err)
    return {
      runCondition: 'db error',
      errMsg: '数据库错误',
    }
  }

  // 执行编辑
  try{
    const result = await db.collection('iot2_devices').updateOne(
      {
        huawei_device_id: { $eq: param.huawei_device_id },
        user_id: { $eq: new ObjectId(user._id) }
      },
      {
        $set: {
          name: param.name
        }
      }
    )
    console.log('updateOne result:', result)
  } catch (err) {
    console.log('db err:', err)
    return {
      runCondition: 'db error',
      errMsg: '数据库错误',
    }
  }

  // 返回结果（更改的设备信息，更改后的名字，更改前的名字）
  const returnData = {
    runCondition: 'succeed',
    errMsg: '改名成功',
    huawei_device_id: param.huawei_device_id,
    newName: param.name
  }
  console.log('成功 returnData:', returnData)
  return returnData

}
