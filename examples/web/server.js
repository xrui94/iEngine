import express from 'express';
import chalk from 'chalk';
import { program } from 'commander'

import path from "path";

function getPageFile(filename) {
  return filename.length > 0 ? filename : './index.html';
}

function getPort(port) {
  return port.length > 0 && !isNaN(port) ? port : 3000;
}

function getMode(mode) {
  return (mode !== 'dev' && mode !== 'prod') ? 'prod' : mode;
}

// 解析获取命令行输入的参数
program
  .name('server-util')
  .description('CLI to run serve')
  .version('0.1.0')
  .option('-f, --file <string>', 'Specify the HTML file to run.', getPageFile)
  .option('-p, --port <number>', 'Set service port', getPort)
  .option('-m, --mode <string>', 'Get development mode.', getMode)

program.parse();
const options = program.opts();
const htmlPage = options.file;
const port = options.port;

// 创建express应用
const app = express();

// 配置Express
// ES6 Module模式下，直接使用“__dirname”，或报错：ReferenceError: __dirname is not defined in ES module scope
const __dirname = path.resolve();
app.use(express.static(__dirname, {
    setHeaders: (res) => {
        // 设置响应头，以启用SharedArrayBuffer，从而使用多线程
        res.set('Cross-Origin-Opener-Policy', 'same-origin');
        res.set('Cross-Origin-Embedder-Policy', 'require-corp');
    }
}));

// 获取html页面
app.get('/', (req, res) => {
    // express跨域设置
    res.setHeader("Access-Control-Allow-Origin", "*");
    res.setHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    res.setHeader("Access-Control-Allow-Methods", "PUT,POST,GET,DELETE,OPTIONS");
    res.setHeader('Access-Control-Allow-Credentials', true); // 是否允许发送Cookie，默认false

    // 设置响应头，以启用SharedArrayBuffer，从而使用多线程
    res.setHeader("Cross-Origin-Opener-Policy", "same-origin");
    res.setHeader("Cross-Origin-Embedder-Policy", "require-corp");

    //
    res.sendFile(path.join(__dirname, htmlPage));    // 注意：路径不能以点开头!!!
});

// 监听端口
app.listen(port, () => {
    console.log('Server is running at:', chalk.blue(`http://localhost:${port}`));
});