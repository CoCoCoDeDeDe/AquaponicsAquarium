// https://dhb91nur4r.bja.sealos.run/iot2/admin/Commerce/Commodity/AddCommodity
import cloud from '@lafjs/cloud'
import common from '../../../utils/common'
const db = cloud.mongo.db
const CommodityCol = db.collection('IOT2_Commodities')

export default async function (ctx: FunctionContext) {

  try {

    // 获取当前时间并格式化
    const CurrentTime = common.formatDate(new Date())

    CommodityCol.insertOne(
      {
        "TextInfo": {
          "Title": "鱼菜共生智能鱼缸 Max 2",
          "Intro": "用于测试的模拟商品的简介 简介 简介 鱼菜共生智能鱼缸 Max 2",
        },
        "CrosswiseSwiperImgs": [
          {
            "_id": new ObjectId("6854ef7fbc802f29c9f9f776"),
            "Intro_Short": "商品图"
          },
          {
            "_id": new ObjectId("682d35989d44566b7c4dc5ed"),
            "Intro_Short": "商品图"
          },
          {
            "_id": new ObjectId("6854ef7fbc802f29c9f9f776"),
            "Intro_Short": "商品图"
          }
        ],
        "VerticalSwiperImgs": [
          {
            "_id": new ObjectId("682d35989d44566b7c4dc5ed"),
            "Intro_Short": "商品图"
          },
          {
            "_id": new ObjectId("6854ef7fbc802f29c9f9f776"),
            "Intro_Short": "商品图"
          }
        ],
        "Promotions": [
          {
            "_id": new ObjectId("6870724ffc568f9848a99113")
          },
          {
            "_id": new ObjectId("6870795bfc568f9848a99119")
          },
          {
            "_id": new ObjectId("6870795dfc568f9848a9911a")
          }
        ],
        "Price": {    // 价格信息，当 SelectionTypes 数组元素数量为 0 时此属性有效
          "Raw": 400    // 原价
        },
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
