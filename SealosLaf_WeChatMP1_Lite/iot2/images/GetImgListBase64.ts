// https://dhb91nur4r.bja.sealos.run/iot2/images/GetImgListBase64
import cloud from '@lafjs/cloud'

export default async function (ctx: FunctionContext) {
  console.log('Hello World')
  return { data: 'hi, laf' }
}
