// https://dhb91nur4r.bja.sealos.run/iot2/device/unbindUserWithDevice
import cloud from '@lafjs/cloud'
import common from '../utils/common'

const db = cloud.mongo.db

export default async function (ctx: FunctionContext) {
  
  // 检验 laf_token 获取 user_id
  const laf_token_VerifyRes = await common.verifyTokenAndGetUser(ctx)
  switch (laf_token_VerifyRes.runCondition) {
    case 'laf_token error':
      console.log('laf_token 验证失败')
      return laf_token_VerifyRes  // token 错误, 退出
    default:
      console.log('laf_token 验证成功')
      break
  }
  let user = laf_token_VerifyRes.user  // user._id 即 user_id
  user._id = new ObjectId(user._id)
  console.log('user._id:', user._id)

  // 获取参数 huawei_device_id
  let param = {
    huawei_device_id: 0,
  }
  param.huawei_device_id = ctx.query.huawei_device_id // ctx. 解析的内容默认是字符串
  // 检验参数 huawei_device_id
  if (!common.isValidNonEmptyString(param.huawei_device_id)) {
    console.log('参数无效 param:', param)
    return {
      runCondition: 'param error',
      errMsg: '参数无效',
    }
  }
  console.log('参数有效 param:', param)

  // 查找 huawei_device_id 对应设备记录的 已经匹配的用户 user_id
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
  try {
    const result = await db.collection(document_common).findOne(filter_com, options_check)

  // 检验已匹配的 user_id 是否存在是否与发起请求的 user_id 相同
    // console.log('reslut:', result)
    if (result.user_id === undefined) { // 这是不符合调用规范的情况，前端只能用正在绑定的设备的华为 id 申请本 API
      // 不太可能发生并且没什么影响，本就是没有绑定的，暂不处理
    }
    // console.log('result.user_id:', result.user_id.toString())
    // console.log('user._id:', user._id.toString())
    if (result.user_id.toString() != user._id.toString()) {
      console.log('用户与设备不匹配 result:', result)
      return {
        runCondition: 'unmatch',
        errMsg: '用户与设备不匹配',
      }
    }
    console.log('用户与设备匹配，准备执行删除')
  } catch (err) {
    console.log('findOne err:', err)
    return {
      runCondition: 'db error',
      errMsg: '数据库错误',
    }
  }


  // 更新 huawei_device_id 对应设备记录的 user_id 为空
  try{

    const updateResult = await db.collection(document_common).updateOne(
      {
        huawei_device_id: { $eq: param.huawei_device_id }
      },
      {
        $unset: { user_id: '' }
      }
    )

    // 处理成功操控数据库时的返回结果
    if (updateResult.modifiedCount === 1) {
      console.log(`成功删除属性: user_id`);
    } else {
      throw Error('未找到匹配的文档或属性未修改')
    }

  } catch (err) {
    console.log('updateOne err:', err)
    return {
      runCondition: 'db error',
      errMsg: '数据库错误',
    }
  }

  return {
    runCondition: 'succeed',
    errMsg: '成功解绑',
  }
}
