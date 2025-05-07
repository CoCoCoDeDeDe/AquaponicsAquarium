// 用于与 laf 云环境进行交互
import cloud from "@lafjs/cloud";
// 用于创建哈希对象，这里用于对密码进行加密
import { createHash } from "crypto";
import jwt from "jsonwebtoken";
import common from './utils/common'

// 获取 MongoDB 数据库实例，方便后续对数据库进行操作
const db = cloud.mongo.db
// JWT 密钥
const JWT_SECRET = process.env.JWT_SECRET;


// 定义一个默认的异步导出函数
export default async function (ctx: FunctionContext) {

  // console.log('JWT_SECRET:', JWT_SECRET)

  // 从请求体中获取 username 字段的值，如果不存在则赋值为空字符串
  const username = ctx.body?.username || ""
  // 从请求体中获取 password 字段的值，如果不存在则赋值为空字符串
  const password = ctx.body?.password || ""

  // 检查用户登录信息
  const user = await db
    // 选择数据库集合
    .collection("iot2_users")
    // 查文档
    .findOne({
      // 条件：用户名等于从请求体中获取的用户名
      username: username,
      // 条件：密码等于对从请求体中获取的密码进行 sha256 哈希加密后得到的十六进制字符串
      password: createHash("sha256").update(password).digest("hex"),
    })

  // 如果未找到匹配的用户文档
  if (!user) return {
    runCondition: 'param error',
    errMsg: '无效的用户名或密码',
  };

  // 生成 JWT（JSON Web Token）令牌
  // 从找到的用户文档中获取用户的 _id 字段
  const user_id = user._id;
  // 定义 JWT 令牌的负载信息
  const payload = {
    // 用户 ID
    uid: user_id,
    // 令牌的过期时间，这里设置为当前时间加上 7 天（以秒为单位）
    exp: Math.floor(Date.now() / 1000) + 60 * 60 * 24 * 7,
  };

  // 根据负载信息和自定义的 JTW 密钥生成访问令牌
  const access_token = jwt.sign(payload, JWT_SECRET);

  // 返回包含用户 ID 和访问令牌的对象
  return {
    runCondition: 'succeed',
    uid: user_id,
    access_token: access_token,
  };
};