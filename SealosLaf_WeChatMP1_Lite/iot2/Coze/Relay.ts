// https://dhb91nur4r.bja.sealos.run/iot2/Coze/Relay

import cloud from '@lafjs/cloud'
import common from '../utils/common'
import axios from 'axios'
import https from 'https'

const db = cloud.mongo.db
const env = process.env

export default async function Relay (ctx: FunctionContext) {

  try {
    // console.log('ctx:', ctx)
    // console.log('ctx.headers:', ctx.headers)
    console.log('ctx.query:', ctx.query)
    console.log('ctx.body:', ctx.body)
    // console.log('ctx.files:', ctx.files)

    // 检验 laf_token 获取 user_id
    const laf_token_VerifyRes = await common.verifyTokenAndGetUser(ctx)
    switch (laf_token_VerifyRes.runCondition) {
      case 'laf_token error':
        console.log('laf_token 验证失败')
        return laf_token_VerifyRes  // token 错误, 退出
      default:
        // console.log('laf_token 验证成功')
        break
    }

    let user = laf_token_VerifyRes.user
    user._id = new ObjectId(user._id)
    console.log('user._id:', user._id)

    // 解析参数 url, Query, Body, Headers
    

    // 配置 axios 默认配置
    // axios.defaults.baseURL = env.COZE_DOMAINNAME
    axios.defaults.timeout = 10000

    // 添加 axios 配置
    axios.defaults.httpsAgent = new https.Agent({
      rejectUnauthorized: false
    });

    const response = await axios.request(
      {
        baseURL: env.COZE_DOMAINNAME,
        url: ctx.body.url,
        method: ctx.body.method,
        params: ctx.body.query,
        headers: {
          "Authorization": "Bearer " + env.COZE_TOKEN,
          "Content-Type": "application/json",
          ...ctx.body.headers
        },
        data: ctx.body.body,
        // 如果请求时间超过 `timeout` 的值，则请求会被中断
        timeout: 1000, // 默认值是 `0` (永不超时)
        // `withCredentials` 表示跨域请求时是否需要使用凭证
        withCredentials: false // default
      }
    )

    // console.log('response:', response)
    // console.log('response.data:', response.data)

    const for_return = {
      code: 200,
      runCondition: 'succeed',
      errMsg: '成功',
      msg: '成功',
      data: response.data
    }

    return for_return


  } catch(err) {
    // console.log('iot2/Coze/Relay', '失败:', err)
    // console.log('err.response:', err.response)
    console.log('err.response.data:', err.response.data)
    return {
      code: 500,
      runCondition: 'Internal error',
      errMsg: '获取失败',
      msg: '获取失败'
    }
  }
}
