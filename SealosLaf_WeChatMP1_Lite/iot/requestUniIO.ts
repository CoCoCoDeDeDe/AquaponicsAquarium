// https://dhb91nur4r.bja.sealos.run/iot/requestUniIO
// 按 query 中的要查询的属性的名字的列表和设备ID，返回对应设备的对应每个属性的最近的数据及其数据上报时间
import cloud from '@lafjs/cloud';
import { ObjectId } from 'mongodb';
import verifyToken from '../utils/verifyToken'

const db = cloud.mongo.db

export default async function requestLatestDeviceDetail(ctx: FunctionContext) {

  // 验证 token 并从中获取 user_id
  const tokenVerifyResult = await verifyToken(ctx)
  switch (tokenVerifyResult.runCondition) {
    case 'token parse error':
    case 'cant find the account':
      return tokenVerifyResult  // return
    case 'laf_token verify succeed':
      break
  }
  const user_id = tokenVerifyResult.user_id
  console.log('获取到 user_id:', user_id)

  // 参数规范验证
  // console.log('获取到 ctx:', ctx)
  // console.log('获取到 ctx.body:', ctx.body)
  console.log('获取到要查询的uniIOs及其时间范围 ctx.body.uniIOs:', ctx.body.uniIOs)

  if (Object.keys(ctx.body.uniIOs).length <= 0) {
    console.log('未读取到指定的uniIOs ctx.body:', ctx.body)
    return {
      runCondition: 'body error',
      errMsg: 'body error'
    }
  }
  const uniIOs_raw = ctx.body.uniIOs

  // 遍历 uniIOs , 遍历其中每一个 uniIO, 获取对应的device_id, device_name, product_id, product_name, uniIO, uniIO_name, data
  return Promise.all(uniIOs_raw.map(async (item, idx, arr) => {

    let device_id, uniIO, date_period, date_unit

    // 参数 date_period 规范检查
    if (item.date_priod === null || item.date_priod === 0) {
      console.log("未指定日期范围数值 item.date_priod:", item.date_priod)
      // 按默认日期范围数值 1 处理
      date_period = 1
    } else if (typeof item.date_priod === 'number' && (item.date_priod < 0 || !Number.isInteger(item.date_priod))) {
      console.log("指定日期范围数值指定错误 item.date_priod:", item.date_priod)
      // 按默认日期范围数值 1 处理
      date_period = 1
    } else {
      // console.log("有指定日期范围数值 item.date_priod:", item.date_priod)
      date_period = item.date_priod
    }

    // 参数 date_unit 规范检查
    if (item.date_unit != 'day' && item.date_unit != 'month' && item.date_unit != 'year') {
      console.log('日期时间范围无效 item.date_unit:', item.date_unit)
      // 按默认日期单位处理 day
      date_unit = 'day'
    } else {
      date_unit = item.date_unit
    }

    // 参数 device_id 和 uniIO 规范检查
    if (item.device_id === null || item.device_id == '' || item.uniIO === null || item.uniIO == '') {
      console.log('参数 device_id uniIO无效 item:', item)
      return null
    }

    // 获取当前时间
    const now = new Date();
    let startDate;  // 提前声明变量, 防止块级作用域于限制

    // 根据 date_priod 和 date_unit 计算查询的起始日期
    switch (date_unit) {
      case 'day': 
        startDate = new Date(now.getTime() - date_period * 24 * 60 * 60 * 1000)
        break
      case 'month':
        startDate = new Date(now.getFullYear(), now.getMonth() - date_period, now.getDate())
        break
      case 'year':
        startDate = new Date(now.getFullYear() - date_period, now.getMonth(), now.getDate())
        break
      default:
        console.log('date_unit error date_unit:', date_unit)
        return null
    }

    // 将日期转换为 ISO 8601 格式字符串
    const startDateStr = startDate.toISOString().replace(/[-:.]/g, '').replace('Z', '');
    const nowStr = now.toISOString().replace(/[-:.]/g, '').replace('Z', '');
    // console.log('startDateStr:', startDateStr)
    // console.log('nowStr:', nowStr)

    // 获取查询参数 device_id, uniIO
    device_id = item.device_id
    uniIO = item.uniIO

    // 获取含有 uniIO 的数据的 n 条数据
    // .find() 返回的是 sursor 游标, 可以防止一次返回过多数据影响性能，可先遍历 cursor 选择性地返回数据。也可直接 .toArray 将 cursor 转化为数组, 但数据多时影响性能
    const resArr = await db.collection('records_properties')
      .find({
        device_id: device_id,
        event_time: {
          $gte: startDateStr,
          $lte: nowStr
        },
        [`properties.${uniIO}`]: { $exists: true }
      }).toArray()
    // console.log(`查询到设备 ${device_id} 的uniIO ${uniIO} 的从 ${startDateStr} 到 ${nowStr} 的记录 resArr:`, resArr)

    // 将 resArr 处理成 {device_id:'', uniIO:'',data:[{event_time:'',value:''},{event_time:'',value:''}]} 格式
    // 新数组的每一项保留原 resArr 的 event_time , 将 properties 对象中键名为 uniIO 的键值对的值取出作为 vaule 键的值
    const data = await Promise.all(resArr.map((item, idx, arr) => {
      // console.log('item:', item)

      // console.log('item.event_time:', item.event_time)
      // console.log('item.properties[uniIO]', item.properties[uniIO])
      
      return {
        event_time: item.event_time,
        value: item.properties[uniIO]
      }
    }))

    console.log(`设备 ${device_id} 的uniIO ${uniIO} 的从 ${startDateStr} 到 ${nowStr} 的记录 data:`, data)

    // TODO 获取本 uniIO 的 device_name\ product_id\ product_name\ uniIO_name


    const uniIO_return = {
      device_id: device_id,
      uniIO: uniIO,
      data: data
    }
    return uniIO_return
  }))



}






















