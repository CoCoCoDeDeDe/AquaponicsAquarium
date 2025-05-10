// https://dhb91nur4r.bja.sealos.run/iot2/smartLinkGroup/RemoveUnIOsFromSmartLinkGroup
import cloud from '@lafjs/cloud'
import common from '../utils/common'

const db = cloud.mongo.db

// 接收 {_id: 'uniIO_id'} 数组

export default async function (ctx: FunctionContext) {

  // 验证 laf_token
  const laf_token_VerifyRes = await common.verifyTokenAndGetUser(ctx)
  switch (laf_token_VerifyRes.runCondition) {
    case 'token error':
      console.log('laf_token 验证失败')
      return laf_token_VerifyRes  // token 错误, 退出
    default:
      console.log('laf_token 验证成功')
      break
  }

  // 获取 user 信息
  const user = laf_token_VerifyRes.user  // 内含 user._id 即 user_id
  // console.log('user:', user)

  // 校验 uniIOList 是否有传入且有效
  if (ctx.body.uniIOList.length < 1) {
    console.log('无效的 uniIOList 参数 ctx.body.uniIOList:', ctx.body.uniIOList)
    return {
      runCondition: 'para error',
      errMsg: '无效的 uniIOList 参数',
    }
  }

  // 校验 uniIO_id 对应 uniIO 是否存在
  // 略

  // 校验 uniIO_id 对应 uniIO 是否属于该 user
  // 略

  // 获取 smartLinkGroup_id
  const query = await ctx.query
  // console.log('query:', query)

  // 获取 uniIOList
  let uniIOList = await ctx.body.uniIOList
  // console.log('uniIOList:', uniIOList)

  // 将 uniIOList 中 uniIO 的 _id 改为 ObjectId 类型, 并去除嵌套的对象转换为 UniIO_Id_List
  const UniIO_Id_List = await Promise.all(uniIOList.map(async (item, idx, arr) => {
    // console.log('map 正在遍历 uniIOList 的 item 为 item:', item)

    // 类型转换
    const NewItem = new ObjectId(item['_id'])

    // console.log('map 正在变量 uniIOList 的 item 的结果为 NewItem:', NewItem)
    return NewItem
  }))
  // console.log('UniIO_Id_List:', UniIO_Id_List)

  // 为 UniIO_Id_List 指定的 uniIO 记录更新键名为 smartLinkGroup_id 的属性
  try {
    const result = await db.collection('iot2_uniIOs').updateMany(
      {
        _id: { $in: UniIO_Id_List }
      },
      {
        $unset: { smartLinkGroup_id: '' } // $unset 的字段值可以是任意值, 操作结果都是移除
      }
    )
    console.log('更新结果 result:', result)
  } catch (err) {
    console.log('为 UniIO_Id_List 指定的 uniIO 记录更新键名为 smartLinkGroup_id 的属性错误 err:', err)
    return {
      runCondition: 'db error',
      errMsg: '为 UniIO_Id_List 指定的 uniIO 记录更新键名为 smartLinkGroup_id 的属性错误',
    }
  }


  console.log('移除成功')
  return {
    runCondition: 'succeed',
    errMsg: '移除成功',
    targetSmartLinkGroup_id: query.smartLinkGroup_id,
  }

  

}
