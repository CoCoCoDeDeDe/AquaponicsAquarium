// components/uniIOCard/UniIOCard_Common_1/index.js
import { isValidNonEmptyString } from '../../../utils/common'
import { requestWithLafToken, on_laf_token_Invalid, on_request_error, on_db_error, on_param_error, on_unknown_error, on_common_error } from '../../../apis/laf'
import * as echarts from '../../../components/ec-canvas/echarts';

Component({

  // 组件的属性列表
  properties: {
    UniIOData: {
      // 当 UniIOData 发送变化时执行该函数
      observer(newVal, oldVal) {
        // console.log("newVal:", newVal)
        // console.log("oldVal:", oldVal)
        // console.log("observer")

        this.RefreshEChart()
      },
      type: Object,
      value: {
        UniIO_Id: 'Default_UniIO_Id',
        UniIO_ExternalName: '示例 UniIO 名称',
        SmartLinkGroup_Name: '示例智联组名称',
        Device_Name: '示例设备名称',
        UniIO_Type: 'actor',
        Cmd_Config: {
          Is_Enum: true,
          Enum: [
            {
              Value: 0,
              Mean: '关闭',
              Main_Color: '#e64340',
            },
            {
              Value: 1,
              Mean: '启动',
              Main_Color: '#1bbb1b',
            },
          ],
          Value_Max: 100,
          Value_Min: 0,
        },
        UniIO_Value_Unit: 'DefaultUnit',
        UniIO_Value_Mean_Pair: {
          0: "空闲",
          1: "运行",
          2: "错误",
        },
        LateastRecord: {
          event_time: 'default_event_time',
          value: 'DefaultValue',
        },
        EChartData: {
          xAxis: {
            data: [
              '00:00',
              '00:01',
              '00:08',
              '00:11',
              '10:00',
              '15:00',
              '16:00',
              '18:01',
            ]
          },
          series: [{
            name: '负载率',
            data: [5, 20, 500, 5, 20, 500, 5, 20],
            itemStyle: { color: '#FFEC71' }, // 线条颜色
          }]
        }
      },
    }
  },

  // 组件的初始数据
  data: {
    // ec: {
    //   onInit: this.initChart
    // }
    Cmd_Ipt_Value: undefined,
  },
  
  async attached() {
    const UniIOData = this.properties.UniIOData
    const that = this; // 保存 this 的引用
    this.setData({
      ec: {
        onInit: function(canvas, width, height, dpr) {
          return that.initChart(canvas, width, height, dpr);
        }
      }
    })

    this.throttledOnSendCommand = throttle(this.OnSendCommand, 2000) // 时间单位: ms
  },

  async ready() {
    this.setData({
      Is_VALUE_MEAN_PAIR_ON: false
    })

    // console.log("this.properties.UniIOData.UniIO_Value_Mean_Pair:", this.properties.UniIOData.UniIO_Value_Mean_Pair)
    if(this.properties.UniIOData.UniIO_Value_Mean_Pair !== undefined) {
      // console.log("VALUE_MEAN_PAIR_ON")
      this.setData({
        Is_VALUE_MEAN_PAIR_ON: true
      })
    }
  },

  // 组件的方法列表
  methods: {

    initChart(canvas, width, height, dpr) {
      const UniIOData = this.properties.UniIOData;

      const chart = echarts.init(canvas, null, { width, height, devicePixelRatio: dpr });
      canvas.setChart(chart);
      const option = {
        // title: { text: '折线图示例' },
        tooltip: {
          trigger: 'axis' // 触发方式为坐标轴触发
        },
        xAxis: {
          type: 'category',
          data: UniIOData.EChartData.xAxis.data,
          axisLine: { show: false }, // 隐藏X轴线
          axisTick: { show: false }  // 隐藏刻度线
        },
        yAxis: {
          type: 'value',
          splitLine: { lineStyle: { color: '#e0e0e0' } } // Y轴分割线样式
        },
        series: [{
          name: UniIOData.EChartData.series[0].name,
          type: 'line',
          smooth: true, // 平滑曲线
          data: UniIOData.EChartData.series[0].data,
          itemStyle: UniIOData.EChartData.series[0].itemStyle, // 线条颜色
          lineStyle: { width: 2 }, // 线宽
          showSymbol: true, // 显示数据点标记
          symbol: 'circle', // 标记形状（circle/triangle等）
          symbolSize: 8     // 标记大小
        }]
      };
      chart.setOption(option);
      this.chart = chart; // 保存图表实例
      return chart;
    },

    async RefreshEChart(e) {
      const UniIOData = this.properties.UniIOData;
      if (this.chart) {
        const option = {
          xAxis: {
            data: UniIOData.EChartData.xAxis.data
          },
          series: [{
            data: UniIOData.EChartData.series[0].data
          }]
        };
        this.chart.setOption(option); // 调用调用 initChart 时保存到 Component 的 chart 实例更新图表配置
      }
    },

    OnBindTapCmdBtn(e) {
      // console.log("e.currentTarget.dataset:", e.currentTarget.dataset)
      const CmdValue = e.currentTarget.dataset.cmdvalue // data-value 属性传递的键值对的键名只有小写字母
      this.throttledOnSendCommand(CmdValue)
    },

    async OnBindTapCmdIptConfirm(e) {
      let Cmd_Ipt_Value
      if(typeof this.data.Cmd_Ipt_Value === 'string') { // typeof 获取类型得到的是字符串
        Cmd_Ipt_Value = this.data.Cmd_Ipt_Value.trim()  // trim() 不修改调用它的字符串，只是返回新字符串
        if( (Cmd_Ipt_Value === '') || (Cmd_Ipt_Value === ' ') ) {
          wx.showToast({
            title: '参数无效',
            duration: 1500,
            icon: 'none',
            mask: false,
          })
          return
        }
      }
      Cmd_Ipt_Value = Number(this.data.Cmd_Ipt_Value)
      // 确认发送后清除输入框
      await this.setData({
        Cmd_Ipt_Value: '',
      }, () => {
        // console.log("输入框刷新 this.data.Cmd_Ipt_Value:", this.data.Cmd_Ipt_Value)
      })
      // console.log("Cmd_Ipt_Value:", Cmd_Ipt_Value)
      // 双重 校验参数格式 取值范围
      // NaN === NaN 永远不成立
      if( (Cmd_Ipt_Value === undefined) || (Cmd_Ipt_Value === null) || (isNaN(Cmd_Ipt_Value)) || (Cmd_Ipt_Value < this.properties.UniIOData.Cmd_Config.Value_Min) || (Cmd_Ipt_Value > this.properties.UniIOData.Cmd_Config.Value_Max) ) {
        wx.showToast({
          title: '参数无效',
          duration: 1500,
          icon: 'none',
          mask: false,
        })
        return
      }

      await this.throttledOnSendCommand(Cmd_Ipt_Value)
    },

    async OnSendCommand(CmdValue) {
      // 收集参数
      // console.log("this.data.UniIOData:", this.data.UniIOData)
      // console.log("this.properties.UniIOData:", this.properties.UniIOData)
      const Para = {
        uniIO_id: this.properties.UniIOData.UniIO_Id,
        value: Number(CmdValue),
      }
      console.log("OnSendCommand Para:", Para)

      // 校验参数格式  取值范围
      if( (Para.uniIO_id === undefined) || (Para.uniIO_id === null) || (Para.uniIO_id.trim().length <= 0) || (Para.value === undefined) || (Para.value === null) || (Para.value === NaN) || (Para.value < this.properties.UniIOData.Cmd_Config.Value_Min) || (Para.value > this.properties.UniIOData.Cmd_Config.Value_Max) ) {
        wx.showToast({
          title: '参数无效',
          duration: 1500,
          icon: 'none',
          mask: false,
        })
        return
      }

      // 调用 API
      let resData
      try{
        resData = await requestWithLafToken('GET', '/iot2/uniIO/sendCommand', {uniIO_id: Para.uniIO_id, value: Para.value})
      } catch(err) {
        console.log("API sendCommand err:", err)
        switch(err.runCondition) {
          case 'laf_token error':
            on_laf_token_Invalid()
            return
          default:
            on_common_error(err)
            return
        }
      }
      console.log("resData:", resData)
      wx.showToast({
        title: '命令发送成功',
        duration: 1500,
        // icon: icon,
        mask: false,
      })
    }



  } // methods
})  // Component

// 函数节流的高阶函数
function throttle(fn, limit) {
  let lastCallTime = 0

  return function(...args) {
    const now = Date.now()

    if(now - lastCallTime >= limit) {
      fn.apply(this, args)  // this 指向调用 throttleFn() 时的 this , 本例中为 Component 实例。 args 作为 fn 的参数传入 fn
      lastCallTime = now
    } else{ // 两次调用的间隔时间如果没有达到 limit 则不调用
      console.log("请勿频繁操作")
      wx.showToast({
        title: '请勿频繁操作',
        duration: 1000,
        icon: 'none',
        mask: false,
      })
    }
  
  }
}