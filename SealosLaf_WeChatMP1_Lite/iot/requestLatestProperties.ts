// https://dhb91nur4r.bja.sealos.run/iot/requestLatestProperties
// 按 query 中的要查询的属性的名字的列表和设备ID，返回对应设备的对应每个属性的最近的数据及其数据上报时间
import cloud from '@lafjs/cloud';
import { ObjectId } from 'mongodb';
import verifyToken from '../utils/verifyToken'

const db = cloud.mongo.db

export default async function requestProperties(ctx: FunctionContext) {
  // console.log('ctx:', ctx)

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

  // 获取 Query
  // 要判断键值对对象是否为空, 用 Object.keys() , 它会返回对象内所有的键名转化为字符后组成的数组, 转化后的数组若长度为 0 则代表对象为空
  if (ctx.query == null || Object.keys(ctx.query).length === 0) {
    console.log('query 无效, ctx.query:', ctx.query)
    return {
      runCondition: 'query invalid',
      errMsg: 'query invalid',
    }
  }
  const query = ctx.query;
  console.log('获取到 query:', query)


  // 从 Query 中获取要读取的属性的名字
  const propertyNameArray = query.properties.split(',');
  console.log('获取到propertyNameArray:', propertyNameArray);

  // 从 Query 中获取要读取的设备的名字
  const device_id = query.device_id;  // 可能的风险：暂时没有用户对应设备校验，用户盗窃其他用户的设备的数据
  console.log('获取到device_id:', device_id);

  // 按 device_id 和 属性名列表 读取 所需要的设备的属性值列表
  // 输入: device_id, propertyNameArray,
  // propertyNameArray 示例：propertyNameArray: [ 'wsd', 'wt', 'at' ]
  // 返回: properties: [ { property_name: '', value: '', event_time: '' }, ... ] // 前端记得将字符串转换为对应格式数据
  // map 方法会同步地遍历数组并返回一个新数组, map 遍历中如果有异步操作要用 Promise.all() 在 map 返回未解决完的 Promise 数组后等待所有 Promise 都解决
  return await Promise.all(propertyNameArray.map(async (item, idx, src) => {
    console.log('正在遍历属性名数组 item:', item)

    // console.log('`properties.${item}`:', `properties.${item}`)
    // console.log('[`properties.${item}`]:', [`properties.${item}`])

    // 按 property_name 和 device_id 按 最近时间 排序搜索对应的 1 条记录，从中取出 value
    // 搜索的记录要同时满足两个条件: 1. device_id 与 Query 的 device_id 相同. 2. 记录中有 'properties' 对象且对象中有键名与 item 相同的键值对
    // 返回记录中的 'properties' 对象的与 item 相同的键值对
    return await db.collection('records_properties')
      .findOne({
        device_id: device_id,
        // 用点号表示法检查 properties 对象中的特定键 item 是否存在
        // 借助 '[', ']' 访问动态属性名 item 
        [`properties.${item}`]: { $exists: true }
      },
      {
        sort: { event_time: -1 },
        projection: {
          _id: 0,
          properties: 1,
          event_time: 1
        }
      })
      .then(res => {
        // console.log('查询属性名对应记录 res:', res)

        // 当未找到符合的记录返回 null, 对应的 map 返回的数组的某个一级对象是 null
        if (!res) {
          console.log('查询属性名对应记录未找到 property_name:', item)
          return null
        }

        // 找了到符合条件的记录
        console.log('查询属性名对应记录成功 property_name:', item)
        console.log('查询属性名对应记录成功 value:', res.properties[item])
        console.log('查询属性名对应记录成功 event_time:', res.event_time)
        return {
          // test: 'test 找了到符合条件的记录',
          property_name: item,
          // 查询对象properties中键名为 item 的键值对的值
          // 若键名符合对象名规范且非动态可用'.'
          value: res.properties[item], // '[', ']' 按键名查找对应值
          event_time: res.event_time
        }
      })
      .catch(err => {
        // catch 可能的触发条件
        // 1. 数据库连接问题
        // 2. 集合或索引问题
        // 3. 查询条件错误
        // 4. 权限问题
        console.log('查询属性名对应记录出错 err:', err)
        return null
      })

    // map 的回调函数返回的值对应遍历的 item 处理后的值, 若回调函数不返回有效值则默认该 tiem 处理成 null
  }))
  


}