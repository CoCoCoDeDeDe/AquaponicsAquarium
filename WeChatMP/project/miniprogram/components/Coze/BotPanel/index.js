// components/Coze/BotPanel/index.js

import { formatUnixTime_Type1, formatUnixTime_Type2, convertObjToArray, remainInArray } from "../../../utils/common"
import { requestWithLafToken, on_laf_token_Invalid, on_common_error, requestBotInfo, requestConversationList } from "../../../apis/laf"

Component({

  /**
   * 组件的属性列表
   */
  properties: {

  },

  /**
   * 组件的初始数据
   */
  data: {
    bot_info: {
      model_info: {
        max_tokens : 4096,
        model_id : "1737521813",
        model_name : "豆包·1.5·Pro·32k",
        cache_type : "closed",
        temperature : 0.8,
        context_round : 3
      },
      plugin_info_list: [
        {
          plugin_id: "7526138885580161075",
          name: "基元智联智能体入口",
          description: "被智能体调用，可使用基元智联平台上的部分API，主要用于获取用户的传感器的数据和控制用户所拥有的执行器的运行。",
          icon_url: "https://lf3-appstore-sign.oceancloudapi.com/ocean-cloud-tos/plugin_icon/default_icon.png?lk3s=cd508e2b&x-expires=1755268153&x-signature=GgG5PlPkva0XxVa6FvgdZJjUR08%3D"
        }
      ],
      owner_user_id: "2094992681874336",
      bot_id: "7525815471376465929",
      description: "",
      version: "1752658906814",
      variables: [
        {
          description: "用户在基元智联平台的token",
          enable: true,
          prompt_enable: true,
          keyword: "Authorization",
          default_value: "",
          variable_type: "KVVariable",
          channel: "custom"
        }
      ],
      prompt_info: {
        prompt: '## 术语：\n1. {#InputSlot placeholder="需要 AI 理解的本平台相关的术语" mode="input"#}基元智联：智能体部署到的物联网平台，基于分布式概念将设备与传感器\\执行器解耦，让传感器\\执行器自由成组，提高资源利用效率和智慧联动性能。{#/InputSlot#}\n2. {#InputSlot placeholder="需要 AI 理解的本平台相关的术语" mode="input"#}UniIO：是基元智联平台上传感器和执行器的统一概念，并且在软件组织上与设备解耦，可自由成组。{#/InputSlot#}\n3. {#InputSlot placeholder="需要 AI 理解的本平台相关的术语" mode="input"#}智联组：不同设备的UniIO智联可相互组成智联组，同一个设备的UniIO可能存在于不同的智联组中。不存在设备存在于智联组中的概念，智联组只可以由UniIO组成。{#/InputSlot#}\n\n# 角色：{#InputSlot placeholder="角色名称" mode="input"#}西瓜{#/InputSlot#}\n{#InputSlot placeholder="角色概述和主要职责的一句话描述" mode="input"#}基元智联平台的AI管家——西瓜，负责帮助用户管理平台上的设备和UniIO等资源。{#/InputSlot#}\n\n## 目标：\n{#InputSlot placeholder="角色的工作目标，如果有多目标可以分点列出，但建议更聚焦1-2个目标" mode="input"#}帮助用户组织和管理基元智联平台上用户的设备和资源。帮助用户查询物联网和设备相关知识。{#/InputSlot#}\n\n## 访问基元智联的规则：\n1. {#InputSlot placeholder="需要 AI 知道的在使用基元智联平台的插件、工作流、API 时的规则" mode="input"#}在访问大部分涉及用户信息和用户所拥有的资源（如设备、UniIO）的插件、工作流、API 时，需要在 Header 中传入字段 Authorization ，即在对话的用户的基元智联平台的 token，该 token 需要智能体（西瓜）在收到用户消息同时接受，这样智能体（西瓜）旧有了和用户相同的访问基元智联 API 的权限。{#/InputSlot#}\n\n## 技能：\n1.  {#InputSlot placeholder="为了实现目标，角色需要具备的技能1" mode="input"#}用插件{#LibraryBlock id="7526138885580161075" uuid="LOgQcwRFS2IE7Ux0x32qu" type="plugin" apiId="7526180813537411118"#}getUserProfile{#/LibraryBlock#}查询用户的基本信息。{#/InputSlot#}\n2. {#InputSlot placeholder="为了实现目标，角色需要具备的技能2" mode="input"#}用插件{#LibraryBlock id="7526138885580161075" uuid="LoBrielPrA1QfaZeonjZA" type="plugin" apiId="7526143998856249386"#}GetDeviceProfileListOfUser{#/LibraryBlock#}获取用户所有的设备的信息。{#/InputSlot#}\n3. {#InputSlot placeholder="为了实现目标，角色需要具备的技能3" mode="input"#}用插件{#LibraryBlock id="7526138885580161075" uuid="pr0eNxkeXS-Uto2ZTbomq" type="plugin" apiId="7526173433709805620"#}GetSLGroupProfileList{#/LibraryBlock#}获取用户所有的智联组的信息。{#/InputSlot#}\n\n## 限制：\n- {#InputSlot placeholder="描述角色在互动过程中需要遵循的限制条件1" mode="input"#}言语要友好且风趣幽默{#/InputSlot#}\n- {#InputSlot placeholder="描述角色在互动过程中需要遵循的限制条件2" mode="input"#}要让用户知道对用户的资源进行了那些操作{#/InputSlot#}\n',
        prompt_mode : "standard"
      },
      create_time: "1752481129",
      update_time: "1752658907",
      name: "基元智联AI管家西瓜",
      icon_url : "https://lf9-appstore-sign.oceancloudapi.com/ocean-cloud-tos/FileBizType.BIZ_BOT_ICON/2094992681874336_1752332578980342463_pSvshsBxqi.png?lk3s=50ccb0c5&x-expires=1753280953&x-signature=GOGfa0EIm3Q1TeNW5a9ArzQfz88%3D"
    },
    conversations_info: {
      has_more: false,
      conversations: [
        {
          id: "7527613724430123034",
          meta_data: {
            "uuid": "newid1234"
          },
          last_section_id: "7527613724430123034",
          created_at: 1752659155
        }
      ]
    }
  },

  lifetimes: {
    async attached(options) {
      try {
        // 获取在线 bot 数据
        const res_requestBotInfo = await requestBotInfo()
        this.setData({
          bot_info: res_requestBotInfo
        })
        console.log("this.data.bot_info:", this.data.bot_info)

        // 获取在线 conversation 数据
        const res_requestConversationList = await requestConversationList()
        this.setData({
          conversations_info: res_requestConversationList
        })
        console.log("this.data.conversations_info:", this.data.conversations_info)

      } catch(err) {
        console.log("attached err:", err)
      }
    }
  },

  /**
   * 组件的方法列表
   */
  methods: {

  }
})