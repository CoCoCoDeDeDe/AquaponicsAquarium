import { TimeStrConvert_ISO8601_To_HHmm, formatUnixTime_Type1, formatUnixTime_Type2, convertObjToArray, remainInArray } from '../utils/common'

const global_config = require('../config/index')

// register
export async function register(username, password) {
  return new Promise((resolve, reject) => {
    wx.request({
      method: "POST",
      url: global_config.laf.base_url + "/iot2/user/register",
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
      url: global_config.laf.base_url + "/iot2/user/login",
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
      url: global_config.laf.base_url + '/iot2/user/verifyLafToken',
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
    console.log()
    console.log(
      "开始请求API:", 
      {
      url: global_config.laf.base_url + last_url + query_str,
      method: method,
      query: query,
      body: data
    })

    await wx.request({
      method: method,
      url: global_config.laf.base_url + last_url + query_str,
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
          url: global_config.laf.base_url + Last_Url + Query_Str,
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

// 处理 laf_token 无效错误
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

// 处理 common 错误
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

// 在线获取 BotInfo
export async function requestBotInfo() {
  try {
    const response = await requestWithLafToken(
      'POST', // method
      '/iot2/Coze/Relay', // last url
      {}, // query
      {
        "url": "/v1/bots/7525815471376465929",
        "method": "GET",
        "query": {},
        "headers": {},
        "body": {}
      } // data
    )

    // 组装 model info card data
    // 将 this.data.Bot_Info.model_Info 下目标的若干个键值对按指定 key 找出，转换为数组，每个元素为格式如下的对象 {name: 原Key, name_CN: 附加新中文名, value: 原value} 存储到 this.data.Model_Info
    const map = [
      {
        key: 'context_round',
        name_CN: '上下文轮数'
      },
      {
        key: 'max_tokens',
        name_CN: '最大 token'
      },
      {
        key: 'temperature',
        name_CN: '生成随机性'
      }
    ]
    let Main_Model_Info = {
      ModelInfoCard_DataList: []
    }

    Main_Model_Info.ModelInfoCard_DataList = convertObjToArray(response.data.data.model_info)

    let to_remain = []
        
    for(let i = 0; i < Main_Model_Info.ModelInfoCard_DataList.length; i++) {
      for(let j = 0; j < map.length; j++) {
        if (Main_Model_Info.ModelInfoCard_DataList[i].name == map[j].key) {
          Main_Model_Info.ModelInfoCard_DataList[i]['name_CN'] = map[j].name_CN
          to_remain.push(i)
          j = map.length  // 跳过当前 ModelInfoCard_DataList item 的剩下的与 map 的 item 的匹配，遍历下一个 ModelInfoCard_DataList item
        }
      }
    }
    Main_Model_Info.ModelInfoCard_DataList = remainInArray(Main_Model_Info.ModelInfoCard_DataList, to_remain)

    response.data.data.model_info_toshow_list = Main_Model_Info.ModelInfoCard_DataList

    // 格式化 BotInfo 中时间格式
    let Main_BotInfo = response.data.data
    Main_BotInfo.update_time_formated = formatUnixTime_Type2(Main_BotInfo.update_time)

    return response.data.data
  } catch(err) {
    switch(err.runCondition) {
      case 'laf_token error': 
        on_laf_token_Invalid()
        break
      default:
        on_common_error()
        break
    }
    return
  }
}

// 在线获取 ConversationList
export async function requestConversationList() {
  try {
    const response = await requestWithLafToken(
      'POST', // method
      '/iot2/Coze/Relay', // last url
      {}, // query
      {
        "url": "/v1/conversations",
        "method": "GET",
        "query": {
          "bot_id": "7525815471376465929",
          "sort_order": "desc"
        },
        "headers": {},
        "body": {}
      } // data
    )

    // 格式化会话信息数组中时间格式
    let main_conversation_list = response.data.data.conversations
    for(let i = 0; i < main_conversation_list.length; i++) {
      main_conversation_list[i].created_at_formated = formatUnixTime_Type1(main_conversation_list[i].created_at)
    }
    response.data.data.conversations = main_conversation_list

    return response.data.data
  } catch(err) {
    console.log("requestConversationList() err:", err)
    switch(err.runCondition) {
      case 'laf_token error': 
        on_laf_token_Invalid()
        break
      default:
        on_common_error()
        break
    }
    return
  }
}

// 获取在线的 会话消息列表
export async function requestConversationMessageList(
  options = {
    conversation_id: '',
    order: '',
    before_id: '',
    after_id: '',
    limit: ''
  }
) {
  try {
    const response = await requestWithLafToken(
      'POST', // method
      '/iot2/Coze/Relay', // last url
      {}, // query
      {
        "url": "/v1/conversation/message/list",
        "method": "GET",
        "query": {
          "conversation_id": options.conversation_id,
        },
        "headers": {},
        "body": {
          "order": options.order,
          "before_id": options.before_id,
          "after_id": options.after_id,
          "limit": options.limit
        }
      } // data
    )
    
    // 格式化消息数组中时间格式
    let message_list = response.data.data

    for(let i = 0; i < message_list.length; i++) {
      message_list[i].created_at_formated = formatUnixTime_Type1(message_list[i].created_at)
    }

    return {
      message_list: response.data.data,
      first_id: response.data.first_id,
      last_id: response.data.last_id,
      has_more: response.data.has_more
    }
  } catch(err) {
    switch(err.runCondition) {
      case 'laf_token error': 
        on_laf_token_Invalid()
        break
      default:
        on_common_error()
        break
    }
    return
  }
}

// 获取在线的 会话消息
export async function requestConversationRetrive(
  options = {
    conversation_id: ''
  }
) {
  try {
    const response = await requestWithLafToken(
      'POST', // method
      '/iot2/Coze/Relay', // last url
      {}, // query
      {
        "url": "/v1/conversation/retrieve",
        "method": "GET",
        "query": {
          "conversation_id": options.conversation_id,
        },
        "headers": {},
        "body": {}
      } // data
    )

    return response.data.data
  } catch(err) {
    switch(err.runCondition) {
      case 'laf_token error': 
        on_laf_token_Invalid()
        break
      default:
        on_common_error()
        break
    }
    return
  }
}

// 获取删除指定消息
export async function requestDeleteMessage(
  options = {
    conversation_id,
    message_id,
  }
) {
  try {
    const response = await requestWithLafToken(
      'POST', // method
      '/iot2/Coze/Relay', // last url
      {}, // query
      {
        "url": "/v1/conversation/message/delete",
        "method": "GET",
        "query": {
          "conversation_id": options.conversation_id,
          "message_id": options.message_id,
        },
        "headers": {},
        "body": {}
      } // data
    )

    return response.data.data
  } catch(err) {
    console.log("requestDeleteMessage() err:", err)
    switch(err.runCondition) {
      case 'laf_token error': 
        on_laf_token_Invalid()
        break
      default:
        on_common_error()
        break
    }
    return
  }
}

// 新建会话
export async function requestCreateConversation(
  options = {
    bot_id: '',
    messages: [
      {
        content: ''
      }
    ]
  }
) {
  try {
    // 将 options.messages 中每个 item 的 content 取出到机器人 message item 模板中组成新的 messages 数组
    let main_messages = []
    const messages_item_template = {
      "content_type": "text",
      "role": "assistant",
      "type": "answer"
    }
    for(let i = 0; i < options.messages.length; i++) {
      main_messages[i] = {...messages_item_template}  // 将对象引用解构为普通对象再重构
      main_messages[i]['content'] = options.messages[i].content
    }

    const response = await requestWithLafToken(
      'POST', // method
      '/iot2/Coze/Relay', // last url
      {}, // query
      {
        "url": "/v1/conversation/create",
        "method": "GET",
        "query": {},
        "headers": {},
        "body": {
          "bot_id": options.bot_id,
          "messages": main_messages
        }
      } // data
    )

    return response.data.data
  } catch(err) {
    console.log("requestCreateConversation() err:", err)
    switch(err.runCondition) {
      case 'laf_token error': 
        on_laf_token_Invalid()
        break
      default:
        on_common_error()
        break
    }
    return
  }
}

// 创建新消息
export async function requestCreateMessage(
  options = {
    conversation_id: '',
    role: 'user',
    content: '',
    content_type: 'text'
  }
) {
  try {
    console.log("requestCreateMessage(options) options:", options)

    const response = await requestWithLafToken(
      'POST', // method
      '/iot2/Coze/Relay', // last url
      {}, // query
      {
        "url": `/v1/conversation/message/create?conversation_id=${options.conversation_id}`,
        "method": "POST",
        "query": {},
        "headers": {},
        "body": {
          "role": options.role,
          "content": options.content,
          "content_type": options.content_type
        }
      } // data
    )

    return response.data.data
  } catch(err) {
    console.log("requestCreateMessage() err:", err)
    switch(err.runCondition) {
      case 'laf_token error': 
        on_laf_token_Invalid()
        break
      default:
        on_common_error()
        break
    }
    return
  }
}

// 创建新对话
/**
 * 创建新对话请求函数
 * 
 * 用于向服务器发送创建新对话的请求，支持流式响应处理
 * 
 * @param {Object} options - 请求配置选项
 * @param {String} options.conversation_id - 对话ID，用于指定要继续的对话（新建对话时可省略）
 * @param {String} options.bot_id - 机器人ID，指定使用哪个机器人进行对话
 * @param {String} options.user_id - 用户ID，标识发起对话的用户
 * @param {Boolean} [options.stream=false] - 是否使用流式响应，默认为false
 * @param {String} options.additional_message_content - 附加的消息内容，作为对话的初始消息
 * @param {Boolean} [options.auto_save_history=true] - 是否自动保存对话历史，默认为true
 * @param {Object} options.custom_variables - 自定义请求变量
 * @param {String} options.custom_variables.Authorization - 授权令牌，用于身份验证
 * 
 * @param {Object} config - 客户端处理配置
 * @param {Page} config.page - 调用当前函数的页面实例，用于处理页面级别的状态更新
 * @param {Function} config.onChunkParsed - 流式响应处理函数
 * @param {Function} config.onMessageCreated
 * @param {Function} config.onChatOver
 * 
 * @returns {Promise<Object>} 返回一个Promise对象，解析为服务器响应结果
 *   当stream为true时，会通过onChunkParsed逐步返回数据
 */
export async function requestCreateChat(
  options = {
    conversation_id: String,
    bot_id: String,
    user_id: String,
    stream: Boolean,
    additional_message_content: String,
    auto_save_history: Boolean,
    custom_variables: {
      Authorization: String
    }
  },
  config = {
    page,
    onChunkParsed,
    onMessageCreated,
    onChatOver,
  }
) {
  try {
    // 函数: 设置目标页面的 chat_stream.is_processing 标志位
    const setIsProcessing = ( whether, page ) => {
      page.setData({
        ['chat_stream.is_processing']: whether
      })
    }

    // 函数：刷新 chat
    const refreshChatConfig = (
      object = {
        page,
        onChatOver
      }
    ) => {
      // 清零 delta 次数
      object.page.setData({
        ['chat_stream.message_delta_times']: 0
      })

      // 清除处理中标志位
      setIsProcessing(false, object.page)

      // 重置新 chat 的 message 的 is_bg_shing
      let main_message_list = [...object.page.data.message_info.message_list]
      main_message_list[main_message_list.length - 1]['is_bg_shing'] = false
      object.page.setData({
        ['message_info.message_list']: main_message_list
      })

      // 执行生命周期函数-chat over
      if ( object.onChatOver ) {
        object.onChatOver()
      }
    }

    console.log("requestCreateChat(options) options:", options)
    // 验证用户 token
    const res_verifyLafToken = await requestWithLafToken(
      'GET',
      '/iot2/user/verifyAccessToken',
      {},
      {}
    )
    const user = {
      id: res_verifyLafToken.user._id
    }

    // 直接请求 coze
    // 提前准备 request data
    let request_createChat_data = {
      bot_id: options.bot_id,
      user_id: options.user_id,
      stream: true,
      auto_save_history: options.auto_save_history,
      custom_variables: options.custom_variables
    }

    // 如果有消息内容就加入消息列表到请求 data 中
    if ( options.additional_message_content ) {
      request_createChat_data['additional_messages'] = [
        {
          role: "user",
          type: "question",
          content_type: "text",
          content: options.additional_message_content
        }
      ]
    }

    // 开始请求
    console.log("request_createChat_data", request_createChat_data)
    const requestTask = wx.request({
      url: global_config.coze.base_url + `/v3/chat?conversation_id=${options.conversation_id}`,
      method: 'POST',
      header: {
        Authorization: `Bearer ${global_config.coze.token}`,
        "Content-Type": "application/json"
      },
      data: request_createChat_data,
      timeout: 20000,
      dataType: 'json',
      responseType: 'arraybuffer',
      useHighPerformanceMode: true,
      enableHttp2: true,
      enableCache: false,
      enableChunked: true,
      success: (result) => {
        console.log("chat success() result:", result)
      },
      fail: (err) => {
        console.log("chat fail() err:", err)
      },
      complete: (res) => {
        console.log("chat complete() res:", res)
      },
    })
    
    // 收到新分块数据的响应和处理
    requestTask.onChunkReceived( async (event) => {
      // 1. 将 ArrayBuffer 转换为 UTD-8 字符串
      // const decoder = new TextDecoder('utf-8')
      // const chunkStr = await decoder.decode(event.data)

      const uint8Array = new Uint8Array(event.data)
      let chunkStr = String.fromCharCode.apply(null, uint8Array)
      chunkStr = decodeURIComponent(escape(chunkStr))

      // console.log("解码后的字符串 chunkStr:", chunkStr)

      // 2. 处理 SSE 格式（过滤 data: 前缀和空行）
      // SSE（Server-Sent Events，服务器发送事件）是一种基于 HTTP 协议的服务器向客户端单向推送实时数据的技术格式规范。它允许服务器主动、持续地向客户端发送数据，而无需客户端频繁发起请求，非常适合实时更新场景（如实时通知、股票行情、日志流等）。
      // 每条 SSE 消息由一个或多个 “字段行” 组成，字段行的格式为 字段名: 值\n（注意是换行符 \n 结尾），且整条消息必须以两个连续的换行符 \n\n 结束。
      const lines = await chunkStr.split('\n')
      const valid_lines_data = await lines
        .filter( line => line.startsWith('data:') ) // 保留 data: 开头的行
        .map(line => line.replace( 'data:', '' ).trim())  // 去除 data: 前缀
      const valid_lines_event = await lines
        .filter( line => line.startsWith('event:') ) // 保留 event: 开头的行
        .map(line => line.replace( 'event:', '' ).trim())  // 去除 event: 前缀

      // 解析每一行的 JSON 并提取内容
      const parsed_data_list = await valid_lines_data.map( line => {
        try {
          return JSON.parse(line)
          // console.log("解析出的一行 JSON 数据 data:", data)
        } catch(err) {
          console.log("解析 data 分块 JSON 失败 err:", err, '原始内容 line:', line)
          return null
        }
      })

      // 生命周期函数-流式数据块解析完成
      if (config.onChunkParsed) {
        config.onChunkParsed()
      }

      // event data 组合成每个元素为 event 和 data 组合的对象的数组。同时存储到 page.data.chat_stream.current_event\current_data
      // 确保 event 和 data 的长度匹配（避免索引越界）
      const minLength = Math.min(valid_lines_event.length, parsed_data_list.length);
      let new_chunks = []
      for(let i = 0; i < minLength; i++) { // 用最小长度循环，避免越界
        const tmp_chunk = {
          event: valid_lines_event[i],
          data: parsed_data_list[i] // 此时是解析后的对象（或 null）
        }
        new_chunks.push(tmp_chunk);
        config.page.setData({
          ['chat_stream.current_chunk']: tmp_chunk
        })
      }
      let main_chunks = Array.isArray(config.page.data.chat_stream.chunks) ? config.page.data.chat_stream.chunks : []
      main_chunks.push(...new_chunks)
      config.page.setData({
        'chat_stream.chunks': main_chunks
      });

      // 处理响应-遍历本 chunk 的 消息
      // 获取当前目标 page 的 message_list
      // console.log("config.page:", config.page)
      let main_message_list = [...config.page.data.message_info.message_list]
      for(let i = 0; i < new_chunks.length; i++) {
        switch(new_chunks[i].event) {
          case 'conversation.chat.created':
            // 置标志位
            setIsProcessing(true, config.page)

            // 拼装和 push 新的在处理的 message 并同步 UI
            const main_message = {
              bot_id: new_chunks[i].data.bot_id,
              chat_id: new_chunks[i].data.chat_id,
              content: '',
              // content_type: 'text',
              conversation_id: new_chunks[i].data.conversation_id,
              created_at: new_chunks[i].data.created_at,
              // id: "7527601796865343551",
              meta_data: {},  // 主动设置默认值
              // reasoning_content: "",
              role: "assistant",  // 主动设置默认值
              section_id: new_chunks[i].data.section_id,
              // type: "answer",
              // updated_at: 1752656374,
              is_bg_shing: true
            }
            main_message_list.push(main_message)
            config.page.setData({
              ['message_info.message_list']: main_message_list
            })

            break
          case 'conversation.chat.in_progress':
            
            break
          case 'conversation.message.delta':
            // 计数 delta 次数
            let main_message_delta_times = config.page.data.chat_stream.message_delta_times
            main_message_delta_times++
            config.page.setData({
              ['chat_stream.message_delta_times']: main_message_delta_times
            })

            // 拼接非内容数据到 page 的指定的 message。仅仅第一次 delta 时
            if (main_message_delta_times == 1) {
              main_message_list[main_message_list.length - 1]['content_type'] = new_chunks[i].data.content_type
              main_message_list[main_message_list.length - 1]['id'] = new_chunks[i].data.id
              main_message_list[main_message_list.length - 1]['role'] = new_chunks[i].data.role
              main_message_list[main_message_list.length - 1]['type'] = new_chunks[i].data.type
              // 保存 message_info.message_list 改动
              config.page.setData({
                ['message_info.message_list']: main_message_list
              })
            }

            // 运行生命周期函数-消息创建：仅当传入onMessageCreated时执行
            if (config.onMessageCreated) {
              config.onMessageCreated()
            }

            // 拼接 content 到 page 的指定的 message。
            main_message_list[main_message_list.length - 1].content = main_message_list[main_message_list.length - 1].content + new_chunks[i].data.content
            // 保存 message_info.message_list 改动
            config.page.setData({
              ['message_info.message_list']: main_message_list
            })

            break
          case 'conversation.audio.delta':
            
            break
          case 'conversation.message.completed':
            
            break
          case 'conversation.chat.completed':
            
            break
          case 'conversation.chat.failed':
            // 刷新 chat
            refreshChatConfig(
              {
                page: config.page,
                onChatOver: config.onChatOver
              }
            )
            break
          case 'conversation.chat.requires_action':
            // 刷新 chat
            refreshChatConfig({page: config.page, onChatOver: config.onChatOver})
            break
          case 'error':
            // 刷新 chat
            refreshChatConfig({page: config.page, onChatOver: config.onChatOver})
            break
          case 'done':
            // 刷新 chat
            refreshChatConfig({page: config.page, onChatOver: config.onChatOver})
            break
          default:
            throw new Error('未知的 event')
            break
        }
      }


    })

    return requestTask

  } catch(err) {
    console.log("requestCreateChat() err:", err)
    switch(err.runCondition) {
      case 'laf_token error': 
        on_laf_token_Invalid()
        break
      default:
        on_common_error()
        break
    }
    return
  }
}

// 获取用户的 laf_cloud 简介
export async function requestUserProfile() {
  try{

    const response = await requestWithLafToken(
      'GET', // method
      '/iot2/user/getUserProfile', // last url
      {}, // query
      {} // data
    )

    return {
      id: response.userProfile._id,
      ...response.userProfile
    }
  } catch(err) {
    console.log("requestLafCloudId() err:", err)
    switch(err.runCondition) {
      case 'laf_token error': 
        on_laf_token_Invalid()
        break
      default:
        on_common_error()
        break
    }
    return
  }
}

// 读取本地 laf_cloud token
export async function readLocalLafCloudToken() {
  try {
    // 读取本地laf_token
    let res_get_local_laf_token

    res_get_local_laf_token = await wx.getStorage({
      key: 'laf_token'
    })
    if(res_get_local_laf_token.data === null || res_get_local_laf_token.data === '') {
      throw new Error('本地 laf_token 为空')
    }
    console.log("读取到本地 laf_token", res_get_local_laf_token)

    return {
      Authorization: `Bearer ${res_get_local_laf_token.data}`
    }

  } catch(err) {
    console.log("readLocalLafCloudToken() err:", err)
    switch(err.runCondition) {
      case 'laf_token error': 
        on_laf_token_Invalid()
        break
      default:
        on_common_error()
        break
    }
    return
  }
}





