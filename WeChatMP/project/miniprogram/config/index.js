// config\index.js
// 配置出口
// 可提交到 git

// 根据环境变量切换配置（默认开发环境）
// const env = process.env.NODE_ENV || "dev";
const env = "prod";
let config;

try {
  config = require(`./${env}`);
} catch (e) {
  // 兜底：如果环境配置文件不存在，提示错误
  console.error(`未找到${env}环境的配置文件`);
  config = {};
}

module.exports = config;