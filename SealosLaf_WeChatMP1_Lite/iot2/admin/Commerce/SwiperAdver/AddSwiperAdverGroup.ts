// https://dhb91nur4r.bja.sealos.run/iot2/admin/Commerce/SwiperAdver/AddSwiperAdverGroup
import cloud from '@lafjs/cloud'
import common from '../../../utils/common'
const db = cloud.mongo.db
const SwiperAdverGroupsCol = db.collection('IOT2_SwiperAdverGroups')

export default async function (ctx: FunctionContext) {

  // 获取当前时间并格式化
  const currentDate = new Date();
  const formattedDate = common.formatDate(currentDate);

  try{

    SwiperAdverGroupsCol.insertOne(
      {
        RecordInfo: {
          CreateTime: formattedDate,
          UpdateTime: formattedDate,
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
