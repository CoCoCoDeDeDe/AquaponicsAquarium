
// 类型是 string 且非空则返回 true, 反之返回 false
export function isValidNonEmptyString(value) {
  return typeof value === 'string' && value.trim().length > 0;
}

export function TimeStrConvert_ISO8601_To_HHmm(isoString) {

  const [datePart, timePart] = isoString.split("T");
  // console.log("timePart:", timePart)
  let result = 'TT:SS'
  result = timePart[0] + timePart[1] + ':' + timePart[2] + timePart[3]
  // console.log("result:", result)
  return result
}

export function GetBase64ImgSrc(img) {
  const { MimeType, Data } = img
  return `data:${MimeType};base64,${Data}`
}

export function formatUnixTime_Type1(timestamp) {
  // 创建Date对象（Unix时间戳为秒，需转为毫秒）
  const date = new Date(timestamp * 1000);
  
  // 获取各时间部分
  const year = date.getFullYear();
  // 月份从0开始，需+1并补零
  const month = String(date.getMonth() + 1).padStart(2, '0');
  // 日期补零
  const day = String(date.getDate()).padStart(2, '0');
  // 小时补零（24小时制）
  const hours = String(date.getHours()).padStart(2, '0');
  // 分钟补零
  const minutes = String(date.getMinutes()).padStart(2, '0');
  
  // 拼接成目标格式
  return `${year}年${month}月${day}日 ${hours}时${minutes}分`;
}

export function formatUnixTime_Type2(timestamp) {
  // 创建Date对象（Unix时间戳为秒，需转为毫秒）
  const date = new Date(timestamp * 1000);
  
  // 获取各时间部分
  const year = date.getFullYear();
  // 月份从0开始，需+1并补零
  const month = String(date.getMonth() + 1).padStart(2, '0');
  // 日期补零
  const day = String(date.getDate()).padStart(2, '0');
  // 小时补零（24小时制）
  const hours = String(date.getHours()).padStart(2, '0');
  // 分钟补零
  const minutes = String(date.getMinutes()).padStart(2, '0');
  
  // 拼接成目标格式
  return `${year}年${month}月${day}日`;
}

export function convertObjToArray(obj) {

  // 获取对象所有键名，再映射为目标结构
  return Object.keys(obj).map(key => ({
    name: key,       // name属性为原对象的键名
    value: obj[key]  // value属性为原对象的键值
  }));
}

export function remainInArray(rawArray = [], remainIdxs = []) {

  let newArray = []
  for(let i = 0; i < remainIdxs.length; i ++) {
    newArray.push(rawArray[remainIdxs[i]])
  }
  return newArray
}