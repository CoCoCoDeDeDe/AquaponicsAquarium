import cloud from '@lafjs/cloud'
import common from './utils/common'
import getHuaweiIAMUserTokenByPassword from './admin/getHuaweiIAMUserTokenByPassword'
import readLateastHuaweiIAMUserToken from './admin/getHuaweiIAMUserTokenByPassword'

const PROJECT_ID = process.env.PROJECT_ID

const db = cloud.mongo.db

export default async function sendCommand(ctx: FunctionContext) {

    // 请求情况分析: 用户 a 请求控制其所拥有的 b 设备的 c actor uniIO
    // 输入参数1: from Headers: 用户的 laf_token, 包含了用户的 user_id, 要验证是否有效并提取出用户 user_id
    // 输入参数2: from Query: 要控制的 actor uniIO 的 id, 需要验证是否是该用户的 uniIO, uniIO 的 type 是否是 actor
    // 输入参数3: from Query:  要输入 acrot uniIO 的用户控制的 参数数值

    // 获取并验证 laf_token
    const tokenVerifyReport = await common.verifyTokenAndGetUser(ctx)
    switch (tokenVerifyReport.runCondition) {
    case 'token error':
        console.log('laf_token 验证失败')
        return tokenVerifyReport  // token 错误, 退出
    default:
        console.log('laf_token 验证成功')
        break
    }

    // 提取和校验 query
    const parseQueryResult = await ctx.query
    console.log('parseQueryResult: ', parseQueryResult)
    if (parseQueryResult === null || Object.entries(parseQueryResult).length == 0) {
        console.log('解析 query 错误, parseQueryResult:', parseQueryResult)
        return {
            runCondition: 'query error',
            errMsg: `解析 query 错误 parseQueryResult: ${parseQueryResult}`
        }
    }

    // 获取并检验 uniIO_id 字符串
    let uniIO_id
    try {
        uniIO_id = parseQueryResult['uniIO_id']
    } catch (err) {
        console.log('uniIO_id:', uniIO_id)
        return {
            runCondition: 'query error',
            errMsg: `解析 query 错误 parseQueryResult: ${parseQueryResult}`
        }
    }
    console.log('uniIO_id:', uniIO_id)

    // 根据 uniIO_id 字符串获取 uniIO 的 product_id
    let findUniIOResult
    try {
        findUniIOResult = await db.collection('iot2_uniIOs')
            .findOne({
                _id: new ObjectId(uniIO_id)
            },
            {
                projection: {
                    _id: 0,
                    name: 1,
                    templateName: 1,
                    device_id: 1,
                    product_id: 1,
                }
            })
    } catch (err) {
        console.log('uniIO_id:', uniIO_id)
        return {
            runCondition: 'find uniIO error',
            errMsg: `find uniIO error uniIO_id: ${uniIO_id}`
        }
    }
    console.log('获取 uniIO 信息完成 findUniIOResult:', findUniIOResult)
    const { product_id, templateName, device_id } = findUniIOResult
    console.log('获取 product_id 完成 product_id:', product_id)
    console.log('获取 templateName 完成 templateName:', templateName)

    // 根据 uniIO_id 和 product_id 获取 uniIO 的 type
    let findTemplateUniIOResult
    try {
    
        findTemplateUniIOResult = await db.collection('iot2_templateUniIOs')
            .findOne({
                product_id: { $eq: product_id },
                templateName: { $eq: templateName }
            },
            {
                projection: {
                    _id: 0,
                }
            })
    } catch(err) {
        
    }
    console.log('获取到 findTemplateUniIOResult:', findTemplateUniIOResult)

    // 获取控制 actor uniIO 的参数数值字
    let value
    try {
        value = Number(parseQueryResult['value'])
    } catch (err) {
        console.log('value:', value)
        return {
            runCondition: 'query error',
            errMsg: `解析 query 错误 parseQueryResult: ${parseQueryResult}`
        }
    }
    console.log('value:', value)

    // 从数据库读取最近的华为 token
    const readTokenResult = await readLateastHuaweiIAMUserToken()
    // console.log(readTokenResult)

    switch (readTokenResult.runCondition) {
        case 'request error':
            return readTokenResult
        case 'succeed':
            console.log('读取最近华为 token 成功')
            break
        default:
            console.log('readLateastHuaweiIAMUserToken 返回无法识别')
            return readTokenResult
    }
    const token = readTokenResult.token
    // console.log('读取到的 token 字符串 token:', token)

    // 用读取到的华为 token 调用华为 command API, 分辨返回结果
    // 成功情况:
    // body:
    // {
    //     "command_id": "c241008e-fc48-4df7-a138-03bad75e68df",
    //     "response": null
    // }
    // 失败情况1: token 无效, 调用 getHuaweiIAMUserTokenByPassword 获取并存储新华为 token

    // 失败情况2: 设备不在线
    // body:
    // {
    //     "error_code": "IOTDA.014016",
    //     "error_msg": "Operation not allowed. The device is not online."
    // }
    // 失败情况3: 设备超时未响应
    // body:
    // {
    //     "error_code": "IOTDA.014111",
    //     "error_msg": "Command request timed out. Check whether the device returns a response within the specified time after receiving the request."
    // }

    const myHeaders = {
        // "Authorization": "3pqDewsv2EzKAAVBMquxpE7PSwFMZt5Eq8EAf2t0",
        // "Authorization": `Bearer  ${token}`,

        "X-Auth-Token": token,
        'Content-Type': 'application/json;charset=utf-8'
    }

    // console.log('myHeaders:', myHeaders)

    var raw = "{\"service_id\":\"All\",\"command_name\":\"FTC\",\"paras\":{\"FT\":8}}"

    var requestOptions = {
        method: 'POST',
        headers: myHeaders,
        body: raw,
    };

    return await fetch("https://ad0ce5c71f.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/509f40fd6e084d55897ef136b49777ed/devices/AQAQ25032901/commands", requestOptions)
        .then(response => response.text())
        .then(result => console.log(result))
        .catch(error => console.log('error', error));


    // 获取当前时间并格式化
    // const currentDate = new Date();
    // const formattedDate = common.formatDate(currentDate);

    // console.log('PROJECT_ID:', PROJECT_ID)

    // const url = `https://iotda.cn-north-4.myhuaweicloud.com/v5/iot/${PROJECT_ID}/devices/${device_id}/commands`
    // const raw = `{\"service_id\":\"All\",\"command_name\":\"${templateName}\",\"paras\":${value}}`

    // const huaweiRes = await fetch(url, {
    //     method: 'POST',
    //     headers: {
    //         "Authorization": `Bearer  ${token}`,
    //         "User-Agent": "API Explorer",
    //         "Host": "ad0ce5c71f.st1.iotda-app.cn-north-4.myhuaweicloud.com:443",
    //         "X-Language": "zh-cn",
    //         "X-Project-Id": PROJECT_ID,
    //         "X-Sdk-Date": formattedDate,
    //         "Content-Type": "application/json"
    //     },
    //     body: raw
    // })
    // .then(response => response.text())
    // .then(result => console.log(result))
    // .catch(error => console.log('error', error));

    // return huaweiRes

}

    // TODO 小程序读取记录的 API
    // GIVEUP 将云函数中读取最近的保存的华为 token 的程序和获取新华为 token 的程序封装