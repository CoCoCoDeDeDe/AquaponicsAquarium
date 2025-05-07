import cloud from '@lafjs/cloud'
import common from './utils/common'
import getHuaweiIAMUserTokenByPassword from './admin/getHuaweiIAMUserTokenByPassword'
import readLateastHuaweiIAMUserToken from './admin/getHuaweiIAMUserTokenByPassword'

const db = cloud.mongo.db

export default async function sendCommandV2(ctx: FunctionContext) {

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

  // 获取用户信息记录
  const user = laf_token_VerifyRes  // user._id 即 user_id

  // 获取 sendCommandV2 除了 laf_token 以外所有的参数
  let cmd_uniIO_id, cmd_value
  try {
    cmd_uniIO_id = await ctx.query['uniIO_id']
    cmd_value = await ctx.query['value']
    if (cmd_uniIO_id === null || cmd_uniIO_id === '') {
      throw new Error('参数 uniIO 无效')
    }
    if (cmd_value === null || cmd_value === '') {
      throw new Error('参数 value 无效')
    }
  } catch(err) {
    console.log('获取 sendCommandV2 参数出错 err:', err)
    return {
      runCondition: 'parameter error',
      errMsg: '获取 sendCommandV2 参数出错',
    }
  }

  // find uniIO 信息记录（device_id, templateName）
  let findUniIORes
  try {
    findUniIORes = await db.collection('iot2_uniIOs')
      .findOne({
        _id: { $eq: new ObjectId(cmd_uniIO_id) }
      },
      {
        projection: {
          _id: 0,
          name: 1,
          templateName: 1,
          device_id: 1,
        }
      })

      if( findUniIORes === null || findUniIORes === '' ){
        console.log('find res:', findUniIORes)
        throw new Error('find uniIO res 空')
      }

  } catch (err) {
    console.log('find uniIO 记录出错 err:', err)
    return {
      runCondition: 'find error',
      errMsg: 'find uniIO 记录出错',
    }
  }

  // find device 信息记录（product_id, huawei_device_id）
  let findDeviceRes
  try {
    findDeviceRes = await db.collection('iot2_devices')
      .findOne({
        _id: { $eq: findUniIORes.device_id }  // 通过 find 从 iot2_uniIOs 集合获取到的 device_id 本身就是 ObjectId
      },
        {
          projection: {
            _id: 0,
            product_id: 1,
            huawei_device_id: 1,
          }
        })

    if (findDeviceRes === null || findDeviceRes === '') {
      console.log('find res:', findUniIORes)
      throw new Error('find device res 空')
    }

  } catch (err) {
    console.log('find device 记录出错 err:', err)
    return {
      runCondition: 'find error',
      errMsg: 'find device 记录出错',
    }
  }

  // find template uniIO 信息记录（）
  let findTemplateUniIORes
  try {
    findTemplateUniIORes = await db.collection('iot2_templateUniIOs')
      .findOne({
        templateName: { $eq: findUniIORes.templateName },
        product_id: { $eq: findDeviceRes.product_id },
      },
      {
        projection: {
          _id: 0,
          type: 1,
          data_type: 1,
          enum_list: 1,
          min: 1,
          max: 1,
          max_length: 1,
          step: 1,
          unit: 1,
        }
      }
    )

    if (findTemplateUniIORes === null || findTemplateUniIORes === '') {
      console.log('find res:', findUniIORes)
      throw new Error('find template uniIO res 空')
    }
  } catch (err) {
    console.log('find template uniIO 记录出错 err:', err)
    return {
      runCondition: 'find error',
      errMsg: 'find template uniIO 记录出错',
    }
  }

  // 整理华为命令 API 要用到的参数
  let huaweiAPIParams = {
    'X-Auth-Token': null,
    device_id: findUniIORes.device_id,
    huawei_device_id: findDeviceRes.huawei_device_id,
    templateName: findUniIORes.templateName,
    value: cmd_value
  }

  // 循环调用华为命令 API
  for(let i = 0; i < 1; i++) {

    // 获取 huawei_token
    // let huawei_token
    try {
      if (i < 1) {
        // 第 1 次调用华为命令 API, 读取最近的 huawei_token
        const readLateastHWTokenRes = await readLateastHuaweiIAMUserToken()
        huaweiAPIParams['X-Auth-Token'] = readLateastHWTokenRes.token
      } else if (0 < i && i < 2) {
        // 第 2 次调用华为命令 API, 获取新的 huawei_token
        const getHWTokenRes = await getHuaweiIAMUserTokenByPassword()
        huaweiAPIParams['X-Auth-Token'] = getHWTokenRes.token
      }
    } catch (err) {
      console.log('获取 huawei_token err:', err)
      return {
        runCondition: 'internal error',
        errMsg: '获取 huawei_token err',
      }
    }
    // console.log('整理华为命令 API 要用到的参数, huaweiAPIParams:', huaweiAPIParams)

    try {
      // 准备调用华为命令 API 的参数
      const myHeaders = {
        "X-Auth-Token": huaweiAPIParams['X-Auth-Token'],
        'Content-Type': 'application/json;charset=utf-8'
      }
      // var raw = "{\"service_id\":\"All\",\"command_name\":\"FTC\",\"paras\":{\"FT\":8}}"
      const raw = `{\"service_id\":\"All\",\"paras\":{\"${huaweiAPIParams.templateName}\":${huaweiAPIParams.value}}}` // command_name 省略
      const url = `https://ad0ce5c71f.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/509f40fd6e084d55897ef136b49777ed/devices/${huaweiAPIParams.huawei_device_id}/commands`
      const requestOptions = {
        method: 'POST',
        headers: myHeaders,
        body: raw,
      };
      const fetchRes = await fetch(url, requestOptions)
        .then(response => response.text())
        .then(result => {
          return JSON.parse(result)
        })
      console.log('华为 API 返回结果处理后的 body fetchRes:', fetchRes)

      if ('command_id' in fetchRes) {  // 命令下发成功
        i = 10 // 结束 for 循环调用华为 API
        break
      } else if ('error_code' in fetchRes) {
        // console.log('华为命令 API 调用错误 fetchRes:', fetchRes)
        switch (fetchRes.error_code) {
          case 'IOTDA.014016':  // 设备不在线，不再重新调用，返回标志让前端提醒用户设备未在线（原则上未在线不可调用本 API）
            return {
              runCondition: 'device offline',
              errMsg: '设备不在线',
            }
          case 'IOTDA.014111':  // 设备命令响应超时
            return {
              runCondition: 'device error',
              errMsg: '设备命令响应超时',
            }
          default:  // 没有 case 的默认是 huawei_token 时效, 循环调用华为命令 API 并在第二次调用时使用新获取的 huawei_token
          if (i > 0) {  // 若已经调用过1次华为命令 API 则不在认为 huawei_token 有错误, 而是认为是华为 API 返回未知的 error_code
            return {
              runCondition: 'internal error',
              errMsg: '华为 API 返回未知 error_code',
            }
          }
            console.log('重新调用华为命令 API, 并获取新 huawei_token')
            // 继续循环调用华为命令 API
        }
      } else {
        console.log('华为 API 返回未知, 无 command_id 和 error_code fetchRes:', fetchRes)
        return {
          runCondition: 'internal error',
          errMsg: '华为 API 返回未知, 无 command_id 和 error_code',
        }
      }

    } catch(err) {
      console.log('fetch 华为 API err:', err)
      return {
        runCondition: 'internal error',
        errMsg: 'fetch 华为 API error',
      }
    }
  }
  
  // 华为命令成功后存入命令记录到 iot2_records
  try {
    // 获取当前时间并格式化
    const currentDate = new Date();
    const formattedDate = common.formatDate(currentDate);

    db.collection('iot2_records')
      .insertOne({
        uniIO_id: new ObjectId(cmd_uniIO_id),
        event_time: formattedDate,
        value: cmd_value,
      })

  } catch (err) {
    console.log('insert 命令记录 err:', err)
    return {
      runCondition: 'internal error',
      errMsg: 'insert 命令记录 err',
    }
  }

  // 本 API 全部成功
  return {
    runCondition: 'succeed',
    errMsg: 'succeed',
  }
}
