import cloud from '@lafjs/cloud'
const { MongoClient } = require('mongodb')
import common from '../utils/common'

const db = cloud.mongo.db

export default async function createProduct (ctx: FunctionContext) {

  // 获取当前时间并格式化
  const currentDate = new Date();
  const formattedDate = common.formatDate(currentDate);

  return

  db.collection('iot2_products')
    .insertOne({
      _id: new ObjectId('67e22c942902516e866abb29'),
      name: '鱼菜共生智能鱼缸se1',
      createdAt: formattedDate,
      updateAt: formattedDate,
      previewImg_url: 'https://mp-1b9cd3c8-d666-4006-b791-11d5ce02e1be.cdn.bspapp.com/IoT1/test/previewImg_aqaq.png',
      detailPoster_url: 'https://mp-1b9cd3c8-d666-4006-b791-11d5ce02e1be.cdn.bspapp.com/IoT1/test/devicePoster1x1_22213.png',
    })

}
