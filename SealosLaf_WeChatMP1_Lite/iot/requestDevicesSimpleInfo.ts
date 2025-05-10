// https://dhb91nur4r.bja.sealos.run/iot/requestDevicesSimpleInfo
import cloud from '@lafjs/cloud';
import { ObjectId } from 'mongodb';
import verifyToken from '../utils/verifyToken'

export default async function requestAccountProfile(ctx: FunctionContext) {

  console.log(ctx.query)

  const rawLimit = ctx.query.limit;
  const limit = rawLimit ? parseInt(rawLimit, 10) : 0; // 默认为 0（不限制）

  // 校验数字合法性（确保是正整数）
  if (isNaN(limit) || limit < 0) {
    return {
      runCondition: 'invalid limit',
      errMsg: 'limit must be a positive integer',
    };
  }

  console.log(limit)

  const tokenVerifyResult = await verifyToken(ctx)
  switch (tokenVerifyResult.runCondition) {
    case 'token parse error':
    case 'cant find the account':
      return tokenVerifyResult  // return
    case 'laf_token verify succeed':
      break
  }

  const user_id = tokenVerifyResult.user_id
  const db = cloud.mongo.db

  console.log('获取到 user_id:', user_id)

  let devices
  let region

  try {
    const cursor = await db.collection("devices")
      .find({
        userId: user_id // devices 中的记录的 userId 是 string
      },
      {
        projection: {
          deviceName: 1,
          productName: 1,
          regionId: 1,
          previewImgUrl: 1
        }
      })
      .limit(limit)

    devices = await cursor.toArray()  // await 的作用是 阻塞当前异步函数，等待 Promise 解析为实际结果（数组），否则返回的是 Promise 对象

    console.log('res.length:', devices.length)

    // 没找到设备默认为是没有设备，默认不报错
    // if (devices.length === 0) {
    //   console.log('laf_token验证成功但未找到 devices')
    //   return {
    //     runCondition: 'cant find devices',
    //     errMsg: 'cant find devices'
    //   }
    // } else {
    //   console.log('找 devices 成功')
      
    // }

    console.log('找 devices 成功')
    // 继续找 regionName

  } catch (err) {
    console.log('查找 devices 错误')
    return {
      runCondition: 'find devices error',
      errMsg: 'find devices error'
    }
  }

  let device_count = devices.length

  for ( let i = 0; i < device_count; i++ ) {
    try {
      const cursor = await db.collection('regions')
        .findOne({
          _id: devices[i].regionId
        }, 
         {
           projection: {
            _id: 0,
            regionName: 1
          }
        })
    
      region = await cursor
      console.log(`根据 devices[${i}] 的 regionId 获取到 region:`, region)

      // 没有 region 也不要报错，前端提示没有分配 region

      Object.assign(devices[i], region)

      // to be continue...

    } catch (err) {
      console.log('查找 region 错误')
      return {
        runCondition: 'find region error',
        errMsg: 'find region error'
      }
    }
  }

  return {
    runCondition: 'succeed',
    errMsg: 'succeed',
    data: devices
  }




}