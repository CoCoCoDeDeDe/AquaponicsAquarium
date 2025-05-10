// https://dhb91nur4r.bja.sealos.run/iot/storeProperties
import cloud from '@lafjs/cloud'

export default async function (ctx: FunctionContext) {

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
    device_id = ctx.body.notify_data.header.device_id
    product_id = ctx.body.notify_data.header.product_id
    console.log('获取的 device_id:', device_id)
    console.log('获取的 product_id:', product_id)

    // 获取事件时间 event_time 由前端解析
    event_time = ctx.body.event_time
    console.log('获取的 event_time:', event_time)

    // 获取属性值
    services = ctx.body.notify_data.body.services // 一个对象中包含 n 个属性键值对
    properties = ctx.body.notify_data.body.services[0].properties

    for (let i = 1; i < services.length; i++) {
      Object.assign(properties, services[i].properties)
    }
    console.log('获取的 properties:', properties)

  // 存储一次 MQTT 规则转发数据到数据库
  const db = cloud.mongo.db

  try {
    await db.collection('records_properties')
      .insertOne({
        device_id,
        product_id,
        event_time,
        properties
      })
  } catch(err) {
    console.log('插入 MQTT 规则转发记录出错 err:', err)
    return {
      runCondition: 'insert error',
      errMsg: 'insert error'
    }
  }

  return {
    runCondition: 'success',
    errMsg: 'success'
  }


  // 一个设备一个表？所有 MQTT 规则转发数据一个表？
  // 一个记录——一次 MQTT 规则转发数据

}

// 模拟输入模板：
// {
//   "resource": "device.property",
//     "event": "report",
//       "event_time": "string", 7
//   "notify_data": {
//     "header": {
//       "app_id": "d4922d8a-6c8e-4396-852c-164aefa6638f",
//         "device_id": "d4922d8a-6c8e-4396-852c-164aefa6638f",
//           "node_id": "ABC123456789",
//             "product_id": "ABC123456789",
//               "gateway_id": "d4922d8a-6c8e-4396-852c-164aefa6638f",
//                 "tags": [
//                   {
//                     "tag_key": "testTagName",
//                     "tag_value": "testTagValue"
//                   }
//                 ]
//     },
//     "body": {
//       "services": [
//         {
//           "service_id": "string",
//           "properties": {},
//           "event_time": "string"
//         }
//       ]
//     }
//   }
// }