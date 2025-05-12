
// 类型是 string 且非空则返回 true, 反之返回 false
export function isValidNonEmptyString(value) {
  return typeof value === 'string' && value.trim().length > 0;
}