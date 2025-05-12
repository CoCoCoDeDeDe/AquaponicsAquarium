// https://dhb91nur4r.bja.sealos.run/iot2/uniIO/GetUniIOCardProfileList
import cloud from '@lafjs/cloud'
import common from '../utils/common'
import GetUniIOList from './GetUniIOList'

// 定义 GetUniIOList(ctx) 结果的类型
type Type_GetUniIOList_Result = {
  runCondition: string;
  errMsg: string;
  uniIOList: any;
}

// 定义 UniIODataList 的项的类型
type Type_UniIODataList_Item = {
  UniIO_Id: string,
  UniIO_Name: string,
  UniIO_TemplateName: string,
  UniIO_ExternalName: string,
  UniIO_Type: string,
  Cmd_Config: string,
  UniIO_Data_Type: string,
  UniIO_Value_Mean_Pair: string,
  UniIO_Value_Min: string,
  UniIO_Value_Max: string,
  UniIO_Value_MaxLength: number,
  UniIO_Value_Step: number,
  UniIO_Value_Unit: string,
  Device_Id: string,
  Device_Name: string,
  Product_Id: string,
  SmartLinkGroup_Id: string,
  SmartLinkGroup_Name: string,
  UniIO_MainColor: string,
}

const db = cloud.mongo.db

// huawei_device_id
// AQAQ25032901
// smartLinkGroup_id
// 681ad710ec955cc190f61ae8

export default async function GetUniIOCardProfileList(ctx: FunctionContext) {

  // 调用 GetUniIOList 获取 UniIOList
  let UniIOList
  try {
    const Result = await GetUniIOList(ctx) as Type_GetUniIOList_Result
    // console.log('GetUniIOList(ctx) Result:', Result)
    switch (Result.runCondition) {
      case 'succeed':
        UniIOList = await Result.uniIOList
        break
      default:
        return Result
    }
  } catch (err) {
    console.log('调用 GetUniIOList(ctx) 出错 err:', err)
    return {
      runCondition: 'internal error',
      errMsg: '调用 GetUniIOList(ctx) 出错',
    }
  }
  // console.log('UniIOList:', UniIOList)

  // map 内捕捉错误麻烦, 在外层捕捉
  let UniIODataList
  try {
    // 遍历 UniIOList 中对应的每一个 UniIO, 获取其信息和记录。记录现定默认获取24小时以内的。
    UniIODataList = await Promise.all(UniIOList.map(async (item, idx, arr) => {

      // 组织结构 NewItem 并获取 item 的 UniIO_Id
      let NewItem = {
        UniIO_Id: item._id
      } as Type_UniIODataList_Item
      // console.log('NewItem:', NewItem)

      // 获取 UniIO 实例信息
      const FindUniIOInfo_Result = await db.collection('iot2_uniIOs').findOne(
        {
          _id: { $eq: new ObjectId(NewItem.UniIO_Id) }
        },
        {
          projection: {
            _id: 0,
            name: 1,
            templateName: 1,
            device_id: 1,
            product_id: 1,
            smartLinkGroup_id: 1,
          }
        }
      )
      // console.log('FindUniIOInfo_Result:', FindUniIOInfo_Result)
      NewItem.UniIO_Name = FindUniIOInfo_Result.name
      NewItem.UniIO_TemplateName = FindUniIOInfo_Result.templateName
      NewItem.Device_Id = FindUniIOInfo_Result.device_id
      NewItem.Product_Id = FindUniIOInfo_Result.product_id
      NewItem.SmartLinkGroup_Id = FindUniIOInfo_Result.smartLinkGroup_id
      // console.log('NewItem:', NewItem)

      // return

      // 获取 UniIO 模板信息
      const FindTemplateUniIOInfo_Result = await db.collection('iot2_templateUniIOs').findOne(
        {
          templateName: { $eq: NewItem.UniIO_TemplateName },
          product_id: { $eq: new ObjectId(NewItem.Product_Id) },
        },
        {
          projection: {
            _id: 0,
            external_name: 1,
            type: 1,
            Cmd_Config: 1,
            data_type: 1,
            value_mean_pair: 1,
            min: 1,
            max: 1,
            max_length: 1,
            step: 1,
            unit: 1,
            MainColor: 1,
          }
        }
      )
      // console.log('FindTemplateUniIOInfo_Result:', FindTemplateUniIOInfo_Result)
      NewItem.UniIO_ExternalName = FindTemplateUniIOInfo_Result.external_name
      NewItem.UniIO_Type = FindTemplateUniIOInfo_Result.type
      NewItem.Cmd_Config = FindTemplateUniIOInfo_Result.Cmd_Config
      NewItem.UniIO_Data_Type = FindTemplateUniIOInfo_Result.data_type
      if (!(FindTemplateUniIOInfo_Result.value_mean_pair === null || FindTemplateUniIOInfo_Result.value_mean_pair === undefined)) {
        NewItem.UniIO_Value_Mean_Pair = FindTemplateUniIOInfo_Result.value_mean_pair
      }
      NewItem.UniIO_Value_Min = FindTemplateUniIOInfo_Result.min
      NewItem.UniIO_Value_Max = FindTemplateUniIOInfo_Result.max
      NewItem.UniIO_Value_MaxLength = FindTemplateUniIOInfo_Result.max_length
      NewItem.UniIO_Value_Step = FindTemplateUniIOInfo_Result.step
      NewItem.UniIO_Value_Unit = FindTemplateUniIOInfo_Result.unit
      NewItem.UniIO_MainColor = FindTemplateUniIOInfo_Result.MainColor
      // console.log('NewItem:', NewItem)

      // return

      // 获取 UniIO 所属智联组名称
      const FindSmartLinkGroupInfo_Result = await db.collection('iot2_smartLinkGroups').findOne(
        {
          _id: { $eq: new ObjectId(NewItem.SmartLinkGroup_Id) }
        },
        {
          projection: {
            _id: 0,
            name: 1,
          }
        }
      )
      // console.log('FindSmartLinkGroupInfo_Result:', FindSmartLinkGroupInfo_Result)
      if (FindSmartLinkGroupInfo_Result != null || FindSmartLinkGroupInfo_Result != undefined) {
        NewItem.SmartLinkGroup_Name = FindSmartLinkGroupInfo_Result.name
      }
      // console.log('NewItem:', NewItem)

      // return

      // 获取 UniIO 所属设备名称
      const FindDeviceInfo_Result = await db.collection('iot2_devices').findOne(
        {
          _id: { $eq: new ObjectId(NewItem.Device_Id) }
        },
        {
          projection: {
            _id: 0,
            name: 1,
          }
        }
      )
      // console.log('FindDeviceInfo_Result:', FindDeviceInfo_Result)
      NewItem.Device_Name = FindDeviceInfo_Result.name
      // console.log('NewItem:', NewItem)

      // return

      // 将操作后的项返还给原数组
      return NewItem
    }))
  } catch (err) {
    const errMsg = 'map() 遍历 UniIOList 错误'
    console.log(`${errMsg} err:`, err)
    return {
      runCondition: 'internal error',
      errMsg,
    }
  }

  // console.log('UniIODataList:', UniIODataList)

  const errMsg = '查询 UniIODataList 成功'
  console.log(`${errMsg} UniIOList.length:`, UniIOList.length)
  return {
    runCondition: 'succeed',
    errMsg,
    UniIODataList
  }

}
