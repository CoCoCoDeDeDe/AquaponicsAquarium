// https://dhb91nur4r.bja.sealos.run/iot2/device/getDeviceListOfUser
import cloud from '@lafjs/cloud'
import common from '../utils/common'

const db = cloud.mongo.db

// 通过 laf_token 指定用户，按 limit 和 skip 返回该用户的设备
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

  // 获取参数
  let param = {
    skip: 0,
    limit: 0,
  }
  param.skip = Number(ctx.query.skip) // ctx. 解析的内容默认是字符串
  param.limit = Number(ctx.query.limit)

  // 校验参数
  if (!((typeof param.skip === 'number' && !isNaN(param.skip) && isFinite(param.skip)) &&
    (typeof param.limit === 'number' && !isNaN(param.limit) && isFinite(param.limit)))) {
    console.log('参数无效 param:', param)
    return {
      runCondition: 'param error',
      errMsg: '参数无效',
    }
  }

  // 获取集合 iot2_devices 中设备信息记录
  const user = laf_token_VerifyRes.user  // user._id 即 user_id
  let deviceList
  const filter = {
    user_id: { $eq: user._id }
  }
  const options = {
    projection: {
      _id: 1,
      product_id: 1,
      huawei_device_id: 1,
      name: 1,
      createdAt: 1,
      updateAt: 1,
    },
    sort: {
      updateAt: -1 as 1 | -1, // 明确指定类型
      _id: -1 as 1 | -1, // 明确指定类型
    },
    skip: param.skip,
    limit: param.limit,
  }
  try {
    const findRes = await db.collection('iot2_devices')
      .find(
        filter,
        options
        ).toArray()
    // 结果为空不算错, 是一种正常情况

    deviceList = findRes
    // console.log('deviceList:', deviceList)

  } catch (err) {
    console.log('find 错误 err:', err)
    return {
      runCondition: 'db error',
      errMsg: '数据库查找错误',
    }
  }

  // 遍历 deviceList 查找对应的产品信息并附加到每个设备信息元素后
  // 当用 map() 处理异步操作时即 map() 的回调函数中有异步函数时, map() 返回的数据是 Promise 数组, 用 Promise.all() 处理这个数组可等待其中 promise 都 resolve() 后在将数据返回
  deviceList = await Promise.all(deviceList.map(async (item, idx, arr) => {
    console.log(`map 遍历到 item:`, item)
    let productProfile
    try{
      const result = await db.collection('iot2_products').findOne({
          _id: { $eq: new ObjectId(item.product_id) }
        },
        {
          _id: 1,
          name: 1,
          previewImg_url: 1,
          detailPoster_url: 1,
          intro: 1,
          updateAt: 0,
        })
      if (result == null) {
        console.log('找不到对应产品')
        return {
          runCondition: 'cant find product',
          errMsg: '找不到对应产品',
        }
      }
      productProfile = result
    } catch (err) {
      console.log('findOne err:', err)
      return {
        runCondition: 'db error',
        errMsg: '数据库错误',
      }
    }
    // 安全删除获取的 productProfile 中的 product 的键名为 _id 的 product_id
    if ('_id' in productProfile) {
      delete productProfile._id
    }
    // 新增 product_name 到 productProfile
    productProfile.product_name = productProfile.name
    // 安全删除获取的 productProfile 中的 product 的键名为 name 的 product_name
    if ('name' in productProfile) {
      delete productProfile.name
    }
    console.log('productProfile:', productProfile)

    return Object.assign(
      item,
      productProfile,
    )
  }))

  // 获取设备信息记录总数
  let total
  try {
    const findRes = await db.collection('iot2_devices').find(filter).count()
    total = findRes
  } catch (err) {
    console.log('find 错误 err:', err)
    return {
      runCondition: 'db error',
      errMsg: '数据库查找错误',
    }
  }
  console.log('total:', total)

  return {
    runCondition: 'succeed',
    errMsg: '成功',
    deviceList,
    total: total,
  }

}
