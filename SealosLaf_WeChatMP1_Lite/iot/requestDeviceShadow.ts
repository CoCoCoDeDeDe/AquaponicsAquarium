// https://dhb91nur4r.bja.sealos.run/iot/requestDeviceShadow
import cloud from '@lafjs/cloud'
import readLateastHuaweiIAMUserToken from '../iot2/admin/readLateastHuaweiIAMUserToken'
import getHuaweiIAMUserTokenByPassword from '../iot2/admin/getHuaweiIAMUserTokenByPassword'

// 假设这是获取Token的函数
async function getToken() {
  // 实现获取Token的逻辑

  const env = {
    IAMUserName: 'yyf20250429',
    IAMPassword: 'Yan2839509393',
    IAMDoaminId: 'yyf1328253308',
    region: 'cn-north-4'
  }

  const url = 'https://iam.cn-north-4.myhuaweicloud.com/v3/auth/tokens'
  const body = {
    "auth": {
      "identity": {
        "methods": ["password"],
        "password": {
          "user": {
            "name": env.IAMUserName,
            "password": env.IAMPassword,
            "domain": {
              "name": env.IAMDoaminId
            }
          }
        }
      },
      "scope": {
        "project": {
          "name": env.region
        }
      }
    }
  }

  const response = await fetch(url, {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json;charset=utf-8'
    },
    body: JSON.stringify(body)
  })

  const headers = {
    tokenHeader: response.headers.get('X-Subject-Token'),
  }

  // 返回新的Token
  return headers.tokenHeader
}

export default async function (ctx: FunctionContext) {

  // const token = await getToken()
  // console.log('token: ', token)

  const getNewTokenRes = await getHuaweiIAMUserTokenByPassword()
  console.log('getNewTokenRes:', getNewTokenRes)
  const token = getNewTokenRes.token

  const url = 'https://ad0ce5c71f.st1.iotda-app.cn-north-4.myhuaweicloud.com:443/v5/iot/509f40fd6e084d55897ef136b49777ed/devices/AQAQ25032901/shadow';

  const response = await fetch(url, {
    method: 'GET',
    headers: {
      "X-Auth-Token": token,
      'Content-Type': 'application/json;charset=utf-8'
    },
    redirect: 'follow'
  })

  const bodyResponse = await response.json()

  return bodyResponse
}