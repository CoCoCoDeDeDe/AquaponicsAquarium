// https://dhb91nur4r.bja.sealos.run/iot2/admin/db/GetColList
import cloud from '@lafjs/cloud'

export async function main(ctx: FunctionContext) {
  // 获取 MongoDB 客户端
  const db = cloud.mongo.db

  try {
    // 定义集合名称的正则表达式（示例：以 "user_" 开头）
    const FIlter_Prefix = { 
      $or: [
        { name: { $regex: /^IOT2/ } },
        { name: { $regex: /^iot2/ } },
      ] 
    }

    // 列出所有符合条件的集合
    const collections = await db.listCollections(
      FIlter_Prefix
    ).toArray()

    // 提取集合名称
    const collectionNames = collections.map(c => c.name)

    return {
      success: true,
      runCondition: 'succeed',
      errMsg: '获取成功',
      collections: collectionNames
    }
  } catch (error) {
    return {
      success: false,
      runCondition: 'internal error',
      errMsg: '获取失败',
      error: error.message
    }
  }
}