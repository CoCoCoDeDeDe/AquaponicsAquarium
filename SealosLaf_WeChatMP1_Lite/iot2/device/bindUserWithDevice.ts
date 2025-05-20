// https://dhb91nur4r.bja.sealos.run/iot2/device/bindUserWithDevice
import cloud from '@lafjs/cloud'
import common from '../utils/common'

const db = cloud.mongo.db

// 验证和获取用户 user._id，获取参数 huawei_device_id ，更改指定 huawei_device_id 的设备记录，添加 user_id 为 user._id
export default async function (ctx: FunctionContext) {

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
  const user = laf_token_VerifyRes.user  // user._id 即 user_id
  console.log('user._id:', user._id)

  // 获取参数
  let param = {
    huawei_device_id: '',
  }
  param.huawei_device_id = ctx.query.huawei_device_id

  // 校验参数
  if (!common.isValidNonEmptyString(param.huawei_device_id)) {
    console.log('参数无效 param:', param)
    return {
      runCondition: 'param error',
      errMsg: '参数无效',
    }
  }
  console.log('参数有效 param:', param)

  // db 配置准备
  const document_common = 'iot2_devices'
  const filter_com = {
    huawei_device_id: { $eq: param.huawei_device_id }
  }
  const options_check = {
    projection: {
      _id: 1,
      user_id: 1,
    }
  }
  // 检查设备是否存在
  try{
    const result = await db.collection(document_common).countDocuments({
      huawei_device_id: { $eq: param.huawei_device_id },
    })
    console.log('result1:', result)
    if(result === 1) {
      console.log('要绑定的设备匹配到 1 个 result:', result)
    } else {
      console.log('数据库设备匹配错误')
      return {
        runCondition: 'id error',
        errMsg: '设备 ID 无效',
      }
    }
    // 继续
  } catch (err) {
    console.log('findOneAndUpdate err:', err)
    return {
      runCondition: 'db error',
      errMsg: '数据库错误',
    }
  }
  // 检查目标设备是否有匹配的用户
  try{
    const result = await db.collection(document_common).countDocuments({
      huawei_device_id: { $eq: param.huawei_device_id },
      user_id: { $exists: 1}
    })
    console.log('reslut2:', result)
    if (result === 1) {
      console.log('查找的设备已有匹配的用户 result:', result)
      return {
        runCondition: 'device bound',
        errMsg: '设备已被绑定',
      }
    }
    if (result === 0) {
      console.log('查找的设备没有匹配的用户 result:', result)
    }
  } catch (err) {
    console.log('findOneAndUpdate err:', err)
    return {
      runCondition: 'db error',
      errMsg: '数据库错误',
    }
  }

  // return

  // db 配置准备
  const update = {
    $set: {
      user_id: user._id
    }
  }
  const options = {
    upsert: false as false,  //文档不存在时是否插入新文档
    returnDocument: 'after' as 'after',  //更新后返回的文档
  }

  try{
    // 执行更新
    const result = await db.collection('iot2_devices').findOneAndUpdate(filter_com, update, options);
    console.log('result:', result)
    if (result.value) {
      console.log('更新成功:', result.value);
    } else {
      console.log('未找到匹配的文档');
    }
  } catch(err) {
    console.log('findOneAndUpdate err:', err)
    return {
      runCondition: 'db error',
      errMsg: '数据库错误',
    }
  }

  return {
    runCondition: 'succeed',
    errMsg: '成功绑定',
  }

}
