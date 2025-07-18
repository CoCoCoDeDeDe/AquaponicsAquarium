// https://dhb91nur4r.bja.sealos.run/iot2/Commerce/Adver/GetSwiperAdverGroupData
import cloud from '@lafjs/cloud'
import common from '../../utils/common'

const db = cloud.mongo.db
const SwiperAdverGroupsCol = db.collection('IOT2_SwiperAdverGroups')
const SwiperAdverItemsCol = db.collection('IOT2_SwiperAdverItems')
const ImageCol = db.collection('IOT2_Images_Base64')

export default async function GetSwiperAdverGroupData(ctx: FunctionContext) {
  
  // console.log('ctx:', ctx)
  // console.log('ctx.headers:', ctx.headers)
  console.log('ctx.body:', ctx.body)
  // console.log('ctx.files:', ctx.files)

  // 检验 laf_token 获取 user_id
  const laf_token_VerifyRes = await common.verifyTokenAndGetUser(ctx)
  switch (laf_token_VerifyRes.runCondition) {
    case 'laf_token error':
      console.log('laf_token 验证失败')
      return laf_token_VerifyRes  // token 错误, 退出
    default:
      // console.log('laf_token 验证成功')
      break
  }

  let user = laf_token_VerifyRes.user
  user._id = new ObjectId(user._id)
  // console.log('user._id:', user._id)
  
  try {

    // 获取当前时间并转换为通用格式
    const CurrentTime = common.formatDate(new Date())

    // 获取最新的 Group
    const FindGroupRes = await SwiperAdverGroupsCol.findOne(
      {
        // filter
        
      },
      {
        // options
        projection: {

        },
        sort: {
          "RecordInfo.UpdateTime": -1
        },
        limit: 1
      }
    )

    // 根据已经确定的 Group 的 _id 查询属于其的 Items
    const FindItemsRes = await SwiperAdverItemsCol.find(
      {
        "InGroups._id": { $eq: new ObjectId(FindGroupRes._id) }
      },
      {
        projection: {
          InGroups: 0
        },
        sort: {
          "RecordInfo.UpdateTime": 1
        },
        limit: null
      }
    )
    .toArray()

    // 根据 Items 的 Img 的 _id 获取对应图片 Base64 数据并在处理后嵌入各个 Item 中
    let AdverSwiperItemList = await Promise.all(
      FindItemsRes.map(
        async (value, index, array) => {
          
          const Image_Oid = value.Img._id

          const FindImgRes = await ImageCol.findOne(
            {
              _id: { $eq: new ObjectId(Image_Oid) }
            },
            {
              projection: {
                _id: 0,
                FileName: 1,
                MimeType: 1,
                Size: 1,
                Data: 1,
                UploadTime: 1,
              },
              sort: {
                UploadTime: 1
              },
              limit: 1
            }
          )

          let NewImg = FindImgRes
          NewImg['Data'] = common.GetBase64ImgSrc(NewImg)
          
          let NewValue = value
          NewValue['Img'] = NewImg

          return NewValue
        }
      )
    )

    // 组合 Group 与 Item 成为前端直接可用的数据
    const AdverSwiperGroup = {
      ...FindGroupRes,
      AdverSwiperItemList
    }

    // 组织要返回的数据
    const ForRetrun = {
      code: 200,
      runCondition: 'succeed',
      errMsg: '获取成功',
      Results: {
        AdverSwiperGroup,
        CurrentTime
      }
    }

    return ForRetrun

  } catch (err) {
    console.log('获取失败:', err)
    return {
      code: 500,
      runCondition: 'Internal error',
      errMsg: '获取失败',
    }
  }


}
