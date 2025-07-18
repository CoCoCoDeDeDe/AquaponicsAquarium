// https://dhb91nur4r.bja.sealos.run/iot2/admin/Commerce/Commodity/AddCommodity
import cloud from '@lafjs/cloud'
import common from '../../../utils/common'
const db = cloud.mongo.db
const PromotionCol = db.collection('IOT2_Promotions')

export default async function (ctx: FunctionContext) {

  try {

    // 获取当前时间并格式化
    const CurrentTime = common.formatDate(new Date())

    PromotionCol.insertOne(
      {
        "Title_Short": "满10减6",
        "Intro": "优惠介绍 很长很长的多段文本",
        "RecordInfo": {
          "CreateTime": CurrentTime,
          "UpdateTime": CurrentTime
        }
      }
    )

  } catch (err) {
    console.log(err)
    return {
      code: 500,
      runCondition: 'Internal Error',
      errMsg: 'Internal Error'
    }
  }
}
