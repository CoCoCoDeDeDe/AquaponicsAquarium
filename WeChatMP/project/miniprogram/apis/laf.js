import { TimeStrConvert_ISO8601_To_HHmm } from '../utils/common'

const baseUrl = 'https://dhb91nur4r.bja.sealos.run'

// register
export async function register(username, password) {
  return new Promise((resolve, reject) => {
    wx.request({
      method: "POST",
      url: baseUrl + "/iot2/user/register",
      header: {
        "Content-Type": "application/json"
      },
      data: {
        username: username,
        password: password
      },
      success: res => {
        console.log("res", res)
        switch(res.data.runCondition)
        {
          case 'succeed':
            console.log('注册账号 API 返回成功')
            resolve(res.data)
            return
          default:
            console.log('注册账号 API 返回错误 res.data:', res.data)
            reject(res.data)
            return
        }
      },
      fail: err => {
        console.error("网络请求失败:", err)
        reject({
          runCondition: 'request error',
          errMsg: '网络请求失败',
        })
      }
    })
  })
}

// login
export async function login(username, password) {
  return new Promise((resolve, reject) => {
    wx.request({
      method: "POST",
      url: baseUrl + "/iot2/user/login",
      header: {
        "Content-Type": "application/json"
      },
      data: {
        username: username,
        password: password,
      },
      success: async res => {
        switch(res.data.runCondition){
          case 'param error':
            console.log("登录失败，账号或密码错误，errMsg:", res.data.errMsg)
            reject(res.data)
            return
          case 'succeed':
            console.log("登录成功，token:", res.data.access_token)
            
            // 存储 laf_token 到本地缓存
            try {
              await wx.setStorage({
                key: 'laf_token',
                data: res.data.access_token,
                encrypt: false
              })
            }catch(err) {
              console.log("laf_token 存储到本地失败 err:", err)
              reject({
                runCondition: 'storage error',
                errMsg: 'laf_token 存储到本地失败',
              })
            }
            console.log("获取新 laf_token 并存储到本地成功")
            resolve({
              runCondition: 'succeed',
              errMsg: 'succeed',
            })
            return
        }
      },
      fail: err => {
        console.log("网络请求失败 err:", err)
        reject({
          runCondition: 'request error',
          errMsg: '网络请求失败',
        })
      }
    })

  })
}

// 读取并验证本地缓存的 laf_token
export async function verify_laf_token() {
  return new Promise(async (resolve, reject) => {
    // 读取本地laf_token
    let get_local_laf_token_res
    try {
      get_local_laf_token_res = await wx.getStorage({
        key: 'laf_token'
      })
      if(get_local_laf_token_res.data === null || get_local_laf_token_res.data === '') {
        throw new Error('本地 laf_token 为空')
      }
    } catch(err) {
      console.log("读取本地 laf_token err:", err)
      return {
        runCondition: 'laf_token error',
        errMsg: '读取本地 laf_token 错误',
      }
    }
    const local_laf_token = get_local_laf_token_res.data
    console.log("读取到本地 laf_token", local_laf_token)

    // 网络验证本地 laf_token
    let online_verify_laf_token_res
    try {
      online_verify_laf_token_res = await verify_laf_token_request(local_laf_token)
      switch(online_verify_laf_token_res.runCondition) {
        case 'succeed':
          break
        case 'laf_token token error':
          return {
            runCondition: 'laf_token error',
            errMsg: '网络验证 laf_token 错误',
          }
      }
    } catch(err) {
      console.log("网络验证 laf_token err:", err)
      return {
        runCondition: 'request error',
        errMsg: '网络验证 laf_token 错误',
      }
    }
    return {
      runCondition: 'succeed',
      errMsg: 'succeed',
    }

  })
}

// 验证 laf_token
// 输入：laf_token
// 输出：验证结果（1：成功；2：token失效；3：请求失败）
// 本地缓存的token作为证明登录的唯一凭证，也是用于获取在线信息时用于匹配指定用户的客户端唯一凭证
export async function verify_laf_token_request(laf_token) {
  return new Promise((resolve, reject) => {
    wx.request({
      method: 'GET',
      url: baseUrl + '/iot2/user/verifyLafToken',
      header: {
        'Content-Type': 'application/json',
        'Authorization': 'Bearer ' + laf_token
      },
      success: res => {
        switch(res.data.runCondition) {
          case 'succeed':
            resolve({
              runCondition: 'succeed',
              errMsg: 'laf_token verify succeed'
            })
          return
          case 'laf_token error':
            resolve({
              runCondition: 'error',
              errMsg: 'laf_token verify failed'
            })
            return
        }
      },
      fail: err => {
        console.log("laf_token 验证请求失败 err:", err)
        resolve({
          runCondition: 'error',
          errMsg: 'laf_token 验证请求失败'
        })
        return
      }
    })
  })
}

// 解析 query 对象格式为字符串格式
// 将 query 转化为 ?key1=value1&key2=value2的格式的字符串 query_str
export async function QueryObjectToString(Options) {
  return new Promise( (resolve, reject) => {
    try{   
      const {
        Query = undefined
      } = Options

      if(Query == undefined) {
        return reject({
          runCondition: 'para error',
          errMsg: 'QueryObjectToString 本地函数参数无效',
          Query,
        })
      }

      let query_str = '';
      if (typeof Query === 'object' && Query!== null) {
        // Object.entries() 将 query: { key1: 'value1', key2: 'value2' } 此类对象转化为数组，期内的键值对也用含两个元素的数组表示，两个元素分别对应键名和值，形式如 query: [ ['key1', 'value1'], ['key2', 'value2'] ] 此方法通常用于用遍历数组的方式遍历对象
        // [key, value] 是将 map 向回调函数的参数 item 进行结构, item 的形式是 Object.entire() 将键值对对象转化得到的 ['key1', 'value1'] 数组, 对应转化前的一个键值对, 用 [key, value] 结构 ['key1', 'value1'] 可以将 'key1' 和 'value1' 分别赋值给 key 和 value 作为回调函数的参数
        // encodeURIComponent解析: 因为转换后的 query 要作为 URL 的一部分使用, URL 有特殊的格式要求, 要在将 query 的 key 和 value 字符串拼入 URL 前用 encodeURIComponent() 对字符串进行编码, 将字符串中除了字母和数字以及部分特定字符（-、_、.、!、~、*、'、(、)）之外的所有字符都转为 URL 规范的格式, 为了避免出现 URL 的保留字符导致 URL 解析问题, URL 的保留字符有如 :, /, ?, #, ;, =, &, @, +, $, , 等. 被编码的字符会被转换为 UTF - 8 编码的转义序列
        // 后端 解析 query 时要注意 query 中被 encodeURIComponent 转换后的 UTF - 9 编码的转义序列
        // encodeURI 不同于 encodeURIComponent, 前者会保留 URL 保留字符, 因为前者用于对已经拼接完成的整个 URL 进行转换, 默认其中的 URL 保留字符是符合使用者要求的, 而后者是用于处理使用者明确了 其中的可能出现的 URL 保留字符不是用于 URL 格式解析的
        // URL, URN, URI Fragment 是 URI 的子集, URI 统一资源标识符, URL 统一资源定位符, URI Fragment 片段标识符
        const queryPairs = Object.entries(Query).map(([key, value]) => `${encodeURIComponent(key)}=${encodeURIComponent(value)}`);
        if (queryPairs.length > 0) {
          query_str = '?' + queryPairs.join('&');
        }
      }

      return resolve(query_str)

    } catch(err) {
      console.log(`本地函数运行错误 err: ${err}`)
      return reject({
        runCondition: 'local error',
        errMsg: `本地函数运行错误 err: ${err}`,
      })
    }
  } )
}

// 前端调用laf 云函数 需要laf_token的 API 的通用函数
// 传入的 query 的格式：一个query对象内有若干个键值对。query: { key1: 'value1', key2: 'value2' }
// 成功的返回：runCondition、errMsg、API返回的结果  // 1种成功：request succeed // 在此基础上还要解析 runCondition 云函数是不是还有错误
// 失败的返回：runCondition、errMsg // 2种失败：laf_token error、request error
export async function requestWithLafToken( method, last_url, query={}, data ) {
  if( method == null || last_url == null ) return

  // 将 query 转化为 ?key1=value1&key2=value2的格式的字符串 query_str
  let query_str = '';
  if (typeof query === 'object' && query!== null) {
    // Object.entries() 将 query: { key1: 'value1', key2: 'value2' } 此类对象转化为数组，期内的键值对也用含两个元素的数组表示，两个元素分别对应键名和值，形式如 query: [ ['key1', 'value1'], ['key2', 'value2'] ] 此方法通常用于用遍历数组的方式遍历对象
    // [key, value] 是将 map 向回调函数的参数 item 进行结构, item 的形式是 Object.entire() 将键值对对象转化得到的 ['key1', 'value1'] 数组, 对应转化前的一个键值对, 用 [key, value] 结构 ['key1', 'value1'] 可以将 'key1' 和 'value1' 分别赋值给 key 和 value 作为回调函数的参数
    // encodeURIComponent解析: 因为转换后的 query 要作为 URL 的一部分使用, URL 有特殊的格式要求, 要在将 query 的 key 和 value 字符串拼入 URL 前用 encodeURIComponent() 对字符串进行编码, 将字符串中除了字母和数字以及部分特定字符（-、_、.、!、~、*、'、(、)）之外的所有字符都转为 URL 规范的格式, 为了避免出现 URL 的保留字符导致 URL 解析问题, URL 的保留字符有如 :, /, ?, #, ;, =, &, @, +, $, , 等. 被编码的字符会被转换为 UTF - 8 编码的转义序列
    // 后端 解析 query 时要注意 query 中被 encodeURIComponent 转换后的 UTF - 9 编码的转义序列
    // encodeURI 不同于 encodeURIComponent, 前者会保留 URL 保留字符, 因为前者用于对已经拼接完成的整个 URL 进行转换, 默认其中的 URL 保留字符是符合使用者要求的, 而后者是用于处理使用者明确了 其中的可能出现的 URL 保留字符不是用于 URL 格式解析的
    // URL, URN, URI Fragment 是 URI 的子集, URI 统一资源标识符, URL 统一资源定位符, URI Fragment 片段标识符
    const queryPairs = Object.entries(query).map(([key, value]) => `${encodeURIComponent(key)}=${encodeURIComponent(value)}`);
    if (queryPairs.length > 0) {
      query_str = '?' + queryPairs.join('&');
    }
  }
  // console.log("requestWithLafToken query_str:", query_str)

  let laf_token
  return await new Promise(async (resolve, reject) => {

    // 本地token有无：是否提醒登录并return
    try {
      const res = await wx.getStorage({key: 'laf_token'})
      if(res.data === '' || res.data === null ) {
        throw new Error('"requestWithLafToken 读取本地缓存 laf_token 为空')
      } else {
        // console.log("requestWithLafToken 读取本地缓存laf_token成功 res.data:", res.data)
        // 继续
        laf_token = res.data
      }
    } catch(err) {
      console.log("requestWithLafToken 读取本地缓存laf_token错误 err:", err)
      reject({
        runCondition: 'laf_token error',
        errMsg: 'token 错误',
      })
      return
    }

    // 根据参数（ method, last_url, data ）请求云函数
    console.log("开始请求API:", baseUrl + last_url + query_str)
    await wx.request({
      method: method,
      url: baseUrl + last_url + query_str,
      header: {
        'Content-Type': 'application/json',
        'Authorization': 'Bearer ' + laf_token,
      },
      data: data,
      success: (res) => {
        console.log("网络请求成功 res:", res)
        switch(res.data.runCondition) {
          case 'succeed':
            break //继续
          default:
            reject(res.data)  // succeed  之外都在 catch 中
            return
        }

        resolve(res.data) // 请求成功
        return
      },
      fail: (err) => {
        console.log("网络请求失败 err:", err)
        reject({
          runCondition: 'request error',
          errMsg: '网络请求失败',
        })
        return
      }
    })

  })
}

// 上传文件到 API
export async function UploadFile(Options) {
  // 获取参数
  const { 
    Last_Url,
    FilePath,
    FormData = undefined,
    Timeout = 10000,
    Query = undefined,
  } = Options

  // Promise 风格
  return new Promise(async (resolve, reject) => {
    try{
      // 校验本函数的参数
      if(Last_Url == undefined || FilePath == undefined ) {
        console.log("Last_Url:", Last_Url)
        console.log("FilePath:", FilePath)
        return reject({
          runCondition: 'invalid para',
          errMsg: 'UploadFile 本地调用函数参数无效',
          Last_Url,
          FilePath
        })
      }

      try{
        // 获取和校验本地 laf_token
        let Res_Get_Local_Laf_Token
        try{
          Res_Get_Local_Laf_Token = await wx.getStorage({
            key: 'laf_token'
          })
        } catch(err) {
          console.log("获取本地 Laf_Token 错误 err:", err)
          reject({
            runCondition: 'laf_token error',
            errMsg: '本地无 Laf_Token',
          })
        }
        const Local_Laf_Token = Res_Get_Local_Laf_Token.data
        // console.log("Local_Laf_Token:", Local_Laf_Token)

        // 解析 query
        let Query_Str = ''
        if(Query) {
          Query_Str = await QueryObjectToString({
            Query: Query,
          })
        }

        // 请求更新文件
        const UploadTask = await wx.uploadFile({
          url: baseUrl + Last_Url + Query_Str,
          filePath: FilePath,
          name: 'file',
          header: {
            'Authorization': 'Bearer ' + Local_Laf_Token,
          },
          formData: FormData,
          timeout: Timeout,

          success: async (res) => {
            // console.log("文件上传网络成功 res:", res)
            // uploadFile 接收的 data 默认为字符串，将字符串解析为 JSON 对象
            const Res_Data = JSON.parse(res.data);
            res.data = Res_Data
            console.log("文件上传网络成功 res:", res) 	// 方便 console dubug
            // console.log("文件上传网络成功 Res_Data:", Res_Data)
            switch(Res_Data.runCondition) {
              case 'succeed':
                break //继续
              default:
                reject(Res_Data)  // succeed  之外都在 catch 中
                return
            }

            // succeed
            resolve(res.data)
          },
          fail: async (err) => {
            console.log("文件上传网络失败 err:", err)
            return reject({
              runCondition: 'request error',
              errMsg: '网络请求失败',
            })
          },
        })

      } catch(err) {
        console.log("文件上传错误 err:", err)
        return reject({
          runCondition: 'local error',
          errMsg: '文件上传 本地错误',
        })
      }

    } catch(err) {
      console.log(`本地函数运行错误 err: ${err}`)
      return reject({
        runCondition: 'local error',
        errMsg: `本地函数运行错误 err: ${err}`,
      })
    }
  })
}

export async function on_laf_token_Invalid( title = '请在登录后使用' ) {
  // 提示登录
  wx.showToast({
    title: title,
    duration: 1500,
    icon: 'error',
    mask: true,
  })

  // 跳转到登录和注册的页面
  wx.navigateTo({
    url: '/pages/login/index',
  })
}

export async function on_request_error(title = '网络请求失败') {
  // 提示
  wx.showToast({
    title: title,
    duration: 1500,
    icon: 'error',
    mask: true,
  })
}

export async function on_db_error(title = '数据库错误') {
  // 提示
  wx.showToast({
    title: title,
    duration: 1500,
    icon: 'error',
    mask: true,
  })
}

export async function on_param_error(title = '参数无效') {
  // 提示
  wx.showToast({
    title: title,
    duration: 1500,
    icon: 'error',
    mask: true,
  })
}

export function on_common_error(err) {
  console.log("err:", err)

  // 提示
  // const iconStr = err.errMsg.length < 8 ? 'error' : 'none'

  let TitleSuffix = '未知错误消息'
  if(typeof err.errMsg == 'undefined') {
    TitleSuffix = err
  } else {
    TitleSuffix = err.errMsg
  }

  wx.showToast({
    title: 'err:' + TitleSuffix,
    duration: 1500,
    icon: 'none',
    mask: true,
  })
}


/* 通过 huawei_device_id 或 SmartLinkGroup_Id 获取 uniIOCardDataList。数据由 html 的 wx:for 用于渲染卡片 */
export const GetUniIODataList = async (Pgae, Query) => {
  // 需传入 Page 示例一方分
  let resData
  try{
    resData = await requestWithLafToken('GET', '/iot2/uniIO/GetUniIOCardDataList', Query)
    // console.log("resData:", resData)
  } catch(err) {
    switch(err.runCondition) {
      case 'laf_token error':
        on_laf_token_Invalid()
        return
      default:
        on_common_error(err)
        return
    }
  }
    
  // 处理每个 UniIOData
  const UniIODataList = await Promise.all( resData.UniIODataList.map( async (item, idx, arr) => {
    // console.log("item 1:", item)

    let NewItem = {}

    // 遍历每个 UniIOData 项，找到其 Records 中最近的一项 Record 并单独存放
    // 校验 Records
    let LateastRecord = null
    // console.log("item.Records.length:", item.Records.length)
    if(item.Records.length > 0) { // 获取的记录为空时不 reduce()
      LateastRecord = item.Records.reduce( (prev, current) => {
        const prevTime = new Date(prev.event_time).getTime()
        const currentTime = new Date(current.event_time).getTime()
        return currentTime > prevTime ? current : prev
      } )
      Object.assign(NewItem, {LateastRecord})
      // console.log("NewItem 1:", NewItem)
    } else{
      Object.assign(NewItem, {LateastRecord})
      // console.log("NewItem 2:", NewItem)
    }

    // 存储已经可用的数据到新介质
    Object.assign(NewItem,
      {
        UniIO_Id: item.UniIO_Id,
        UniIO_Type: item.UniIO_Type,
        UniIO_TemplateName: item.UniIO_TemplateName,
        UniIO_ExternalName: item.UniIO_ExternalName,
        SmartLinkGroup_Name: item.SmartLinkGroup_Name,
        Device_Name: item.Device_Name,
        UniIO_Value_Mean_Pair: item.UniIO_Value_Mean_Pair,  // 没有则为 undefined
        UniIO_Value_Unit: item.UniIO_Value_Unit,
        Cmd_Config: item.Cmd_Config,
      })
      // console.log("NewItem 3:", NewItem)
      // console.log("NewItem.UniIO_Type:", NewItem.UniIO_Type)
      // console.log("NewItem.UniIO_Type:", NewItem.UniIO_Type)
      // console.log("NewItem.Cmd_Config:", NewItem.Cmd_Config)

    // 将部分数据改为适合 echart 的格式
    let DataX = [], DataY = []
    if(item.Records.length > 0) {
      for (let i = 0; i < item.Records.length; i++) {

        // 把20250510T120031Z字符串格式时间转化为 06:00 格式的字符串
        DataX[i] = TimeStrConvert_ISO8601_To_HHmm(item.Records[item.Records.length - 1 - i].event_time)
        
        // 转化单独 Y 轴数据到数组
        DataY[i] = item.Records[item.Records.length - 1 - i].value
        // 如果 UniIO_Value_Mean_Pair 有值则匹配为对应意义 string
        // 本方案已放弃，echart 折线图的 y 周数据只可为数值
        // if(NewItem.UniIO_Value_Mean_Pair !== undefined) {
        //   DataY[i] = NewItem.UniIO_Value_Mean_Pair[DataY[i].toString()]
        // }   
      }
    } else{
      // 无 Records 不处理
    }
    // console.log("DataX:", DataX)
    // console.log("DataY:", DataY)
    Object.assign(NewItem, {
      EChartData: {
        xAxis: {
          data: DataX,
        },
        series: [{  // 可一个表显示多条数据
          data: DataY,
        }]
      }
    })
    // console.log("NewItem 4:", NewItem)

    // echart 配置项
    NewItem.EChartData.series[0].name = NewItem.UniIO_ExternalName
    NewItem.EChartData.series[0].itemStyle = { color: item.MainColor }

    return NewItem
  } ) )

  // return

  Pgae.setData({
    UniIODataList: UniIODataList,
  })

}

// 已弃用
export async function on_unknown_error(title = '未知错误') {
  // 提示
  wx.showToast({
    title: title,
    duration: 1500,
    icon: 'error',
    mask: true,
  })
}

// requestWithLafToken() 与其错误处理函数 on_laf_token_Invalid() on_request_error() 打包, 自动处理 laf_token error 和 request error 的错误。
// 网络 API 返回的其他错误需要在改函数后另外识别和处理
// 调用该函数记得 await
// 该函数目前弃用
export async function on_request_kit( method, last_url, query, data ) {
  try{
    const requestRes = await requestWithLafToken(method, last_url, query, data)
    return requestRes
  } catch(err) {
    console.log("请求失败 err:", err)
    switch(err.runCondition) {
      case 'laf_token error':
        on_laf_token_Invalid()
        return
      case 'request error':
        on_request_error()
        return
    }
  }
}