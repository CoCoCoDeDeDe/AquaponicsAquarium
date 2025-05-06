import cloud from '@lafjs/cloud'
import common from '../utils/common'

const IAM_USER_PASSWORD = process.env.IAM_USER_PASSWORD

const db = cloud.mongo.db

// 当在使用华为云需要 IAMUser Token 的云函数时发现 iot2_huaweiIAMTokens 集合中最近的 token 失效时调用该函数, 无传入参数, 返回包含 token 键值对的对象并将华为云的响应数据存入数据库
export default async function getHuaweiIAMUserTokenByPassword () {

  // 获取当前时间并格式化
  const currentDate = new Date();
  const formattedDate = common.formatDate(currentDate);

  let raw = `{\"auth\":{\"identity\":{\"methods\":[\"password\"],\"password\":{\"user\":{\"domain\":{\"name\":\"yyf1328253308\"},\"name\":\"yyf20250429\",\"password\":\"${IAM_USER_PASSWORD}\"}}},\"scope\":{\"project\":{\"name\":\"cn-north-4\"}}}}`;

  let requestOptions = {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json;charset=utf-8'
    },
    body: raw,
  };

  const token = await fetch("https://iam.cn-north-4.myhuaweicloud.com/v3/auth/tokens", requestOptions)
    .then(async response => {
      if (!response.ok) {
        throw new Error('网络响应不正常');
      }
      console.log('用华为云 IAM 账号密码 fetch token 完成')

      // 获取响应头
      const headers = await response.headers;
      // console.log('headers:', headers)
      // 遍历并打印所有响应头
      // headers.forEach((value, name) => {
      //   console.log(`${name}: ${value}`);
      // });

      // const body = await response.json()
      // console.log('body:', body)

      // 获取特定的响应头
      const IAMUserToken = headers.get('x-subject-token');
      // console.log('x-subject-token:', IAMUserToken);
      return IAMUserToken
    })
    .catch(err => {
      console.error('请求获取华为云 token 失败 err:', err);
      return null
    });
  
  if (token == null) {
    return {
      runCondition: 'request error',
      errMsg: '请求获取华为云 token 失败'
    }
  }

  let insert_res

  try {
    // 将 headers 和 body 存入 iot2_huaweiIAMTokens 集合
    insert_res = await db.collection('iot2_huaweiIAMTokens').insertOne({
      token,
      createdAt: formattedDate,
      updateAt: formattedDate,
    })
  } catch (err) {
    console.log('获取 token 的华为云响应存入数据库出错 err:', err)
  }
  console.log('获取 token 的华为云响应存入数据库的响应 insert_res:', insert_res)

  return { 
    runCondition: 'succeed',
    errMsg: 'succeed',
    token: token,
  }
}
