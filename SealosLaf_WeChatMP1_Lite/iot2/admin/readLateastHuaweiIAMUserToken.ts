// https://dhb91nur4r.bja.sealos.run/iot2/admin/readLateastHuaweiIAMUserToken
import cloud from '@lafjs/cloud'
import common from '../utils/common'

const db = cloud.mongo.db

// 每当执行需要华为云 token 的程序时, 如用华为云下行命令 API 下发下行命令, 调用该函数, 无参数输入, 返回包含 token 键值对的对象
export default async function readLateastHuaweiIAMUserToken() {
  const res = await db.collection('iot2_huaweiIAMTokens')
    .findOne({
      // 只要是最近一条记录即可
    },
    {
      sort: {
        createdAt: -1 // 按创建时间降序( -1 )排序
      },
      projection: {
        _id: 0,
        token: 1,
        updateAt: 1,
      }
    })
    .then(res => {
      console.log('读取最近的华为 token 完成, res.updateAt:', res.updateAt)
      // return res
      return {
        runCondition: 'succeed',
        errMsg: 'succeed',
        token: res.token,
        token_updateAt: res.updateAt,
      }
    })
    .catch(res => {
      console.log('读取最近的华为 token 失败, res:', res)
      return {
        runCondition: 'read error',
        errMsg: '读取最近的华为 token 失败',
      }
    })

  return res
}
