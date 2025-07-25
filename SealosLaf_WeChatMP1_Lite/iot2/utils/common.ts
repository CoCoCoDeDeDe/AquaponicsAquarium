import cloud from '@lafjs/cloud'
import { createHash } from "crypto";
import jwt from "jsonwebtoken";

// JWT 密钥
const JWT_SECRET = process.env.JWT_SECRET;

const db = cloud.mongo.db;

// 将日期转换为指定格式
function formatDate(date) {
  const year = date.getUTCFullYear();
  const month = String(date.getUTCMonth() + 1).padStart(2, '0');
  const day = String(date.getUTCDate()).padStart(2, '0');
  const hours = String(date.getUTCHours()).padStart(2, '0');
  const minutes = String(date.getUTCMinutes()).padStart(2, '0');
  const seconds = String(date.getUTCSeconds()).padStart(2, '0');

  return `${year}${month}${day}T${hours}${minutes}${seconds}Z`;
}

// 封装 token 验证函数
async function verifyTokenAndGetUser(ctx: any) {

  // console.log('JWT_SECRET:', JWT_SECRET)

  const token = ctx.headers?.authorization?.split(' ')[1];  // 去掉'Bearer '
  if (token) {
    try {
      // 验证 JWT 令牌
      const decoded = jwt.verify(token, JWT_SECRET) as { uid: string; exp: number };  // 类型断言
      const user_id = decoded.uid;
      console.log('user_id:', user_id)
      const user = await db.collection("iot2_users").findOne(
        {
          _id: new ObjectId(user_id)
        },
        {
          projection: {
            password: 0,
          }
        });
      if (user) {
        return {
          runCondition: 'succeed',
          error: '验证成功',
          user,
        };
      } else {
        return {
          runCondition: 'laf_token error',
          error: 'Invalid or expired token'
        };
      }
    } catch (error) {
      return {
        runCondition: 'laf_token error',
        error: 'Invalid or expired token' 
      };
    }
  }
  return {
    runCondition: 'laf_token error',
    error: 'Invalid or expired token',
    code: 400,
    errMsg: '用户验证失败'
  };
}

function isValidNonEmptyString(value: unknown): value is string {
  return typeof value === 'string' && value.trim().length > 0;
}

function GetBase64ImgSrc(img: { MimeType: String, Data: String }) {
  const { MimeType, Data } = img
  return `data:${MimeType};base64,${Data}`
}


export default { formatDate, verifyTokenAndGetUser, isValidNonEmptyString, GetBase64ImgSrc }