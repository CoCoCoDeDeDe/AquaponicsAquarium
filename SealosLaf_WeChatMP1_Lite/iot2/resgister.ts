// 从 @lafjs/cloud 模块导入 cloud 对象
import cloud from "@lafjs/cloud";
// 从 crypto 模块导入 createHash 函数，用于创建哈希对象
import { createHash } from "crypto";
import common from './utils/common'

// 获取云数据库的实例
const db = cloud.mongo.db

// 导出一个异步函数作为默认导出，该函数接收一个上下文对象 ctx
export default async function (ctx: FunctionContext) {
  // 从上下文对象的 body 中获取用户名，如果不存在则赋值为空字符串
  const username = ctx.body?.username || "";
  // 从上下文对象的 body 中获取密码，如果不存在则赋值为空字符串
  const password = ctx.body?.password || "";

  // 检查用户名是否符合规则（3 到 16 位字母或数字）
  // 如果不符合规则，返回包含错误信息的对象
  if (!/^[a-zA-Z0-9]{3,16}$/.test(username))
    return { error: "invalid username" };
  // 检查密码是否符合规则（3 到 16 位字母或数字）
  // 如果不符合规则，返回包含错误信息的对象
  if (!/^[a-zA-Z0-9]{3,16}$/.test(password))
    return { error: "invalid password" };

  // 检查用户名是否已经存在于数据库中
  // 使用 countDocuments 方法统计 users 集合中具有该用户名的文档数量
  const exists = await db
    .collection("iot2_users")
    .countDocuments({ username: username })

  // 如果用户名已存在，返回包含错误信息的对象
  if (exists > 0) return { error: "username already existed" };

  // 获取当前时间并格式化
  const currentDate = new Date();
  const formattedDate = common.formatDate(currentDate);

  // 创建要插入的文档
  const document: any = {
    // 存储用户名
    username: username,
    // 对密码进行 sha256 哈希处理，并将结果转换为十六进制字符串后存储
    password: createHash("sha256").update(password).digest("hex"),
    // 记录用户创建的时间
    createdAt: formattedDate,
    updateAt: formattedDate,
  };

  const { avatar_url } = ctx.body;

  // 检查 avatar_url 是否有效
  if (avatar_url && typeof avatar_url === 'string' && avatar_url.trim() !== '') {
    document.avatar_url = avatar_url;
  }

  // 若用户名可用，则向 users 集合中插入新用户文档
  const res = await db.collection("iot2_users").insertOne(document);

  // 打印新注册用户的插入 ID
  console.log("user registered: ", res.insertedId);
  // 返回包含插入 ID 的对象
  return { data: res.insertedId };
};