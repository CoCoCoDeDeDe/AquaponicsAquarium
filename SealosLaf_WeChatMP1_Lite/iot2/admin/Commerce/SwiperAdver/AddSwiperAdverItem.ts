// https://dhb91nur4r.bja.sealos.run/iot2/admin/Commerce/SwiperAdver/AddSwiperAdverItem
import cloud from '@lafjs/cloud'
import common from '../../../utils/common'
const db = cloud.mongo.db
const SwiperAdverItemsCol = db.collection('IOT2_SwiperAdverItems')

export default async function (ctx: FunctionContext) {

  // 获取当前时间并格式化
  const currentDate = new Date();
  const formattedDate = common.formatDate(currentDate);

  try {
    
    SwiperAdverItemsCol.insertOne(
      {
        InGroups: [
          {
            _id: new ObjectId("686f8348fc568f9848a9910c")
          }
        ],
        "NavOptions": {
          "NavType": "URL",
          "TargetURL": null,
        },
        "Img": {
          "_id": new ObjectId("6854ef74bc802f29c9f9f773")
        },
        "RecordInfo": {
          "CreateTime": formattedDate,
          "UpdateTime": formattedDate
        }
      }
    )

  } catch(err) {
    console.log(err)
    return {
      code: 500,
      runCondition: 'Internal Error',
      errMsg: 'Internal Error'
    }
  }

  console.log('Hello World')
  return { data: 'hi, laf' }
}
