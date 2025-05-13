
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