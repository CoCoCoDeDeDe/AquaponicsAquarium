// https://dhb91nur4r.bja.sealos.run/iot/requestToken
import cloud from '@lafjs/cloud'

// 小程序当发现Token时效时，请求获取新的华为云Token
export default async function (ctx: FunctionContext) {

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
    allHeaders: Object.fromEntries(response.headers.entries())
  }

  const bodyResponse = await response.json()

  return {
    headers
  }

}
