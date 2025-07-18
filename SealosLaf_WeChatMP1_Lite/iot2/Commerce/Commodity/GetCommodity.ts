// https://dhb91nur4r.bja.sealos.run/iot2/Commerce/Commodity/GetCommodity
import cloud from '@lafjs/cloud'
import common from '../../utils/common'

const db = cloud.mongo.db
const CommodityCol = db.collection('IOT2_Commodities')
const ImageCol = db.collection('IOT2_Images_Base64')
const PromotionCol = db.collection('IOT2_Promotions')

export default async function (ctx: FunctionContext) {
  try {

    // console.log('ctx:', ctx)
    // console.log('ctx.headers:', ctx.headers)
    // console.log('ctx.body:', ctx.body)
    console.log('ctx.query:', ctx.query)

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
    console.log('user._id:', user._id)

    // 获取当前时间并转换为通用格式
    const CurrentTime = common.formatDate(new Date())

    // 获取 API Query 参数
    const Skip = Number(ctx.query.skip)
    const Limit = Number(ctx.query.limit)
    console.log(Skip)
    console.log(Limit)

    // 获取 CommodityCard 记录
    // const FindComodityRes = await CommodityCol.find(
    //   {
    //     // 无筛选
    //   },
    //   {
    //     projection: {

    //     },
    //     sort: {
    //       ["RecordInfo.UpdateTime"]: 1
    //     },
    //     skip: Skip,
    //     limit: Limit
    //   }
    // ).toArray()

    const [result] = await CommodityCol.aggregate([
      {
        $facet: {
          data: [
            // None Filter
            { $sort: { "RecordInfo.UpdateTime": 1 } },
            { $skip: Skip },
            { $limit: Limit }
          ],
          totalCount: [
            { $count: "count" }
          ]
        }
      }
    ]).toArray()


    const FindComodityRes = result.data
    const CommodityCardList_Total = result.totalCount[0]?.count || 0

    // 根据 Commodity 列表获取图片、促销数据
    console.log("FindComodityRes:", FindComodityRes)
    let CommodityCardList = await Promise.all(
      FindComodityRes.map(
        async (value, index, array) => {

          console.log('value:', value)

          // 将数据库中商品记录的 CrosswiseSwiperImgs 的第 1 个元素图片作为要返回的商品卡片数据的图片
          let Image_Main = value.CrosswiseSwiperImgs[0]

          // console.log('Image_Main:', Image_Main)

          const FindImageRes = await ImageCol.findOne(
            {
              _id: new ObjectId(Image_Main._id)
            },
            {
              projection: {
                _id: 0,
                FileName: 1,
                MimeType: 1,
                Size: 1,
                Data: 1,
              },
              sort: {
                UploadTime: 1
              },
              skip: 0,
              limit: 1
            }
          )

          // console.log('FindImageRes:', FindImageRes)

          Image_Main['FileName'] = FindImageRes.FileName
          Image_Main['MimeType'] = FindImageRes.MimeType
          Image_Main['Size'] = FindImageRes.Size
          Image_Main['Data'] = common.GetBase64ImgSrc(FindImageRes)

          // console.log('Image_Main:', Image_Main)

          // 照片数据获取完成，接下来遍历 value 的 Promotions ，根据 _id 获取每个 promotion 的 Intro_Short
          let Promotions_Main = await Promise.all(
            value.Promotions.map(
              async (value, index, array) => {

                // console.log('Promotions value:', value)

                // 根据 _id 查询 Intro_Short
                const FindPromotionRes = await PromotionCol.findOne(
                  {
                    _id: new ObjectId(value._id)
                  },
                  {
                    projection: {
                      _id: 1,
                      Title_Short: 1,
                      RecordInfo: 1,
                    },
                    sort: {
                      ["RecordInfo.UpdateTime"]: 1
                    },
                    skip: 0,
                    limit: null
                  }
                )

                // console.log('FindPromotionRes:', FindPromotionRes)
                
                return FindPromotionRes

              }
            )
          )

          // console.log('Promotions_Main:', Promotions_Main)

          // 获取完 Promotions 详细数据

          const ToReturn = {
            _id: value._id,
            Image: Image_Main,
            TextInfo: value.TextInfo,
            Promotions: Promotions_Main,
            Price: {
              Raw: value.Price.Raw,
              Current: value.Price.Raw
            },
            RecordInfo: value.RecordInfo
          }

          console.log('ToReturn:', ToReturn)

          return ToReturn
        }
      )
    )

    console.log('ToReturn CommodityCardList:', CommodityCardList)

    // 组织要返回的数据
    const ForRetrun = {
      code: 200,
      runCondition: 'succeed',
      errMsg: '获取成功',
      Results: {
        CurrentTime,
        CommodityCardList,
        CommodityCardList_Total
      }
    }

    return ForRetrun

  } catch (err) {
    console.log(err)
    return {
      code: 500,
      runCondition: 'Internal Error',
      errMsg: 'Internal Error'
    }
  }
}
