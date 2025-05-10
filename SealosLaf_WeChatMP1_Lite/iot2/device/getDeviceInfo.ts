// https://dhb91nur4r.bja.sealos.run/iot2/device/getDeviceInfo
import cloud from '@lafjs/cloud'
import common from '../utils/common'

const db = cloud.mongo.db

export default async function (ctx: FunctionContext) {

  // 检验 laf_token 获取 user_id
  const laf_token_VerifyRes = await common.verifyTokenAndGetUser(ctx)
  switch (laf_token_VerifyRes.runCondition) {
    case 'token error':
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
    console.log('用户与设备匹配，准备执行获取设备信息')
  } catch (err) {
    console.log('findOne err:', err)
    return {
      runCondition: 'db error',
      errMsg: '数据库错误',
    }
  }

  // 获取设备本身信息
  let device_info
  try{
    const response = await db.collection('iot2_devices').findOne({
      huawei_device_id: { $eq: param.huawei_device_id }
    },
    {
      projection: {
        _id: 1,
        product_id: 1,
        huawei_device_id: 1,
        name: 1,
        createdAt: 1,
        updateAt: 1,
      }
    })
    if (response == null || response == undefined) {
      console.log('未找到设备')
      return {
        runCondition: 'no found',
        errMsg: '未找到设备',
      }
    }
    device_info = response
  } catch (err) {
    console.log('findOne err:', err)
    return {
      runCondition: 'db error',
      errMsg: '数据库错误',
    }
  }

  // 获取设备所属产品信息
  let product_info
  try {
    const response = await db.collection('iot2_products').findOne({
      _id: { $eq: device_info.product_id }
    },
      {
        projection: {
          _id: 1,
          name: 1,
          previewImg_url: 1,
          detailPoster_url: 1,
          intro: 1,
          updateAt: 1,
        }
      })
    if (response == null || response == undefined) {
      console.log('未找到产品')
      return {
        runCondition: 'not found',
        errMsg: '未找到产品',
      }
    }
    product_info = response
  } catch (err) {
    console.log('findOne err:', err)
    return {
      runCondition: 'db error',
      errMsg: '数据库错误',
    }
  }


  // 整合设备以及设备所属产品信息
  let common_info = {
    device_info,
    product_info,
  }


  // 返回整合后得到的信息
  return {
    runCondition: 'succeed',
    errMsg: '查询成功',
    common_info,
  }
}
