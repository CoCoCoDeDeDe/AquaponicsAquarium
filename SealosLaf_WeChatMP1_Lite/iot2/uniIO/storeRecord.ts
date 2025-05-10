// https://dhb91nur4r.bja.sealos.run/iot2/uniIO/storeRecord
import cloud from '@lafjs/cloud'
import common from '../utils/common'


export default async function (ctx: FunctionContext) {

  const db = await cloud.mongo.db

  // console.log('ctx.query:', ctx.query)
  // console.log('ctx.headers:', ctx.headers)
  // console.log('ctx.body:', ctx.body)
  // console.log('ctx.body.notify_data:', ctx.body.notify_data)
  // console.log('ctx.body.notify_data.body:', ctx.body.notify_data.body)
  // console.log('ctx.body.notify_data.body.services:', ctx.body.notify_data.body.services)
  // console.log('ctx.body.notify_data.body.services[0].properties:', ctx.body.notify_data.body.services[0].properties)
  // 虽然可以接收到规则转发的消息，但是消息只是自动同步到了laf，laf数据的更新要触发local数据更新需要websocket
  let device_id, product_id, event_time, services, properties

  // 获取设备ID和产品ID
  device_id = await ctx.body.notify_data.header.device_id
  product_id = await ctx.body.notify_data.header.product_id
  // console.log('获取的 device_id:', device_id)
  // console.log('获取的 product_id:', product_id)

  // 获取事件时间 event_time 由前端解析
  event_time = await ctx.body.event_time
  // console.log('获取的 event_time:', event_time)

  // 获取属性键值对
  services = await ctx.body.notify_data.body.services // 一个对象中包含 n 个属性键值对
  properties = await ctx.body.notify_data.body.services[0].properties // 先取出 services 的第一个元素
  for (let i = 1; i < services.length; i++) { // 把 services 的所有元素拼接
    await Object.assign(properties, services[i].properties)
  }
  // console.log('获取的 properties:', properties)

  // 将键值对对象 properties 的每个键值对都与 uniIO_id\ event_time\ value 组合成一个键值对对象, 下一步存入 iot2_records 集合
  // 获取 uniIO_id 数组
  const uniIO_templateNameArr = await Object.keys(properties)
  // console.log('获取到要存储记录的 uniIO 的 templateName 组成的数组 uniIO_templateNameArr:', uniIO_templateNameArr)
  console.log('uniIO_templateNameArr.length:', uniIO_templateNameArr.length)
  // 获取 value 数组
  const uniIO_valueArr = await Object.values(properties)
  // console.log('获取到要存储记录的 uniIO 的 value 组成的数组 uniIO_valueArr:', uniIO_valueArr)
  console.log('uniIO_valueArr.length:', uniIO_valueArr.length)

  // 声明 document 用于存储格式转换完成后的 records
  let document: Array<Object> = []

  for (let i = 0; i < uniIO_templateNameArr.length; i++) {
    // console.log(`正在组装 ${uniIO_templateNameArr[i]}`)
    // console.log(`值 ${uniIO_valueArr[i]}`)

    // 获取 uniIO_id
    const res = await db.collection('iot2_uniIOs')
      .findOne({
        device_id: {$eq: new ObjectId(device_id)},
        templateName: {$eq: uniIO_templateNameArr[i]}
      },{
        projection: {
          _id: 1
        }
      }).then(res => {
        // console.log('依据 device_id 和 uniIO_templateNameArrd 对 uniIO_id查询完成 res:', res)
        return res
      }).catch(err => {
        // console.log('依据 device_id 和 uniIO_templateNameArrd 对 uniIO_id查询失败 err:', err)
        return null
      })

    
    if (res != null) {
      // console.log('获取到 uniIO res:', res)
      const uniIO_id = res._id
      // console.log(`获取到 uniIO_id: ${uniIO_id}`)
      document.push({
        uniIO_id: uniIO_id,
        event_time: event_time,
        value: uniIO_valueArr[i],
        type: 'up',
      })
    }
  }
  // 规则转发的数据格式处理完成
  // console.log('规则转发的数据格式处理完成 document:', document)
  // console.log('规则转发的数据格式处理完成 document.length:', document.length)

// 插入记录
  let res
  try {
    res = await db.collection('iot2_records')
      .insertMany(document)
  } catch (err) {
    console.log('插入 MQTT 规则转发记录出错 err:', err)
    return {
      runCondition: 'insert error',
      errMsg: 'insert error'
    }
  }

  // console.log('规则转发数据存储成功 res:', res)
  return {
    runCondition: 'success',
    errMsg: 'success'
  }


}