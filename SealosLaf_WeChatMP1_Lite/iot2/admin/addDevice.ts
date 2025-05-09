import cloud from '@lafjs/cloud'
import common from '../utils/common'

const db = cloud.mongo.db

export default async function (ctx: FunctionContext) {

  // 获取当前时间并格式化
  const currentDate = new Date();
  const formattedDate = common.formatDate(currentDate);

  return
  
  db.collection('iot2_devices')
    .insertOne({
      // _id: new ObjectId('AQAQ25032901'),
      product_id: new ObjectId('681cbb76e5dd1deebd143ff7'),
      // huawei_device_id: null,
      name: '鱼菜共生智能鱼缸4Promax',
      createdAt: formattedDate,
      updateAt: formattedDate,
      test: 1,
    })
    .then(res => {
      console.log(res)
    })
    .catch(err => {
      console.log(err)
    })

  return

  db.collection('iot2_uniIOs')
    .insertMany([
      {
        name: '水位传感器',
        templateName: 'WSD',
        device_id: new ObjectId('AQAQ25032901'),
        product_id: new ObjectId('67e22c942902516e866abb29'),
        createdAt: formattedDate,
        updateAt: formattedDate,
      },
      {
        name: '水质传感器',
        templateName: 'WQSVR',
        device_id: new ObjectId('AQAQ25032901'),
        product_id: new ObjectId('67e22c942902516e866abb29'),
        createdAt: formattedDate,
        updateAt: formattedDate,
      },
      {
        name: '土壤湿度传感器',
        templateName: 'SMSVR',
        device_id: new ObjectId('AQAQ25032901'),
        product_id: new ObjectId('67e22c942902516e866abb29'),
        createdAt: formattedDate,
        updateAt: formattedDate,
      },
      {
        name: '水温传感器',
        templateName: 'WT',
        device_id: new ObjectId('AQAQ25032901'),
        product_id: new ObjectId('67e22c942902516e866abb29'),
        createdAt: formattedDate,
        updateAt: formattedDate,
      },
      {
        name: '水泵功率百分比',
        templateName: 'WPVR',
        device_id: new ObjectId('AQAQ25032901'),
        product_id: new ObjectId('67e22c942902516e866abb29'),
        createdAt: formattedDate,
        updateAt: formattedDate,
      },
      {
        name: '气泵运行状态',
        templateName: 'APRS',
        device_id: new ObjectId('AQAQ25032901'),
        product_id: new ObjectId('67e22c942902516e866abb29'),
        createdAt: formattedDate,
        updateAt: formattedDate,
      },
      {
        name: '水温加热器运行状态',
        templateName: 'WHRS',
        device_id: new ObjectId('AQAQ25032901'),
        product_id: new ObjectId('67e22c942902516e866abb29'),
        createdAt: formattedDate,
        updateAt: formattedDate,
      },
      {
        name: '光照传感器',
        templateName: 'ISVR',
        device_id: new ObjectId('AQAQ25032901'),
        product_id: new ObjectId('67e22c942902516e866abb29'),
        createdAt: formattedDate,
        updateAt: formattedDate,
      },
      {
        name: '鱼缸灯光功率百分比',
        templateName: 'ALVR',
        device_id: new ObjectId('AQAQ25032901'),
        product_id: new ObjectId('67e22c942902516e866abb29'),
        createdAt: formattedDate,
        updateAt: formattedDate,
      },
      {
        name: '种植灯光功率百分比',
        templateName: 'PGLVR',
        device_id: new ObjectId('AQAQ25032901'),
        product_id: new ObjectId('67e22c942902516e866abb29'),
        createdAt: formattedDate,
        updateAt: formattedDate,
      },
      {
        name: '喂食器运行状态',
        templateName: 'FRS',
        device_id: new ObjectId('AQAQ25032901'),
        product_id: new ObjectId('67e22c942902516e866abb29'),
        createdAt: formattedDate,
        updateAt: formattedDate,
      },
      {
        name: '空气温度传感器',
        templateName: 'AT',
        device_id: new ObjectId('AQAQ25032901'),
        product_id: new ObjectId('67e22c942902516e866abb29'),
        createdAt: formattedDate,
        updateAt: formattedDate,
      },
      {
        name: '空气湿度传感器',
        templateName: 'AH',
        device_id: new ObjectId('AQAQ25032901'),
        product_id: new ObjectId('67e22c942902516e866abb29'),
        createdAt: formattedDate,
        updateAt: formattedDate,
      },
      {
        name: '水泵功率百分比控制',
        templateName: 'WPVRC',
        device_id: new ObjectId('AQAQ25032901'),
        product_id: new ObjectId('67e22c942902516e866abb29'),
        createdAt: formattedDate,
        updateAt: formattedDate,
      },
      {
        name: '气泵控制',
        templateName: 'APRSC',
        device_id: new ObjectId('AQAQ25032901'),
        product_id: new ObjectId('67e22c942902516e866abb29'),
        createdAt: formattedDate,
        updateAt: formattedDate,
      },
      {
        name: '水温加热器控制',
        templateName: 'WHRSC',
        device_id: new ObjectId('AQAQ25032901'),
        product_id: new ObjectId('67e22c942902516e866abb29'),
        createdAt: formattedDate,
        updateAt: formattedDate,
      },
      {
        name: '鱼缸灯光功率百分比控制',
        templateName: 'ALVRC',
        device_id: new ObjectId('AQAQ25032901'),
        product_id: new ObjectId('67e22c942902516e866abb29'),
        createdAt: formattedDate,
        updateAt: formattedDate,
      },
      {
        name: '种植灯光功率百分比控制',
        templateName: 'PGLVRC',
        device_id: new ObjectId('AQAQ25032901'),
        product_id: new ObjectId('67e22c942902516e866abb29'),
        createdAt: formattedDate,
        updateAt: formattedDate,
      },
      {
        name: '喂食器控制',
        templateName: 'FTC',
        device_id: new ObjectId('AQAQ25032901'),
        product_id: new ObjectId('67e22c942902516e866abb29'),
        createdAt: formattedDate,
        updateAt: formattedDate,
      }
    ])

}
