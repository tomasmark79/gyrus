// Local Chromium CDP smoke check. Start the web server and a dedicated browser
// with --remote-debugging-port=9444 first. No Playwright dependency is required.
import {writeFile} from 'node:fs/promises';

const base = process.env.GYRUS_CDP || 'http://127.0.0.1:9444';
const page = await (await fetch(`${base}/json/new?about:blank`, {method:'PUT'})).json();
const socket = new WebSocket(page.webSocketDebuggerUrl);
await new Promise((resolve,reject) => {socket.onopen=resolve; socket.onerror=reject;});
let next = 0;
const requests = new Map();
const errors = [];
const views = [];
socket.onmessage = ({data}) => {
  const msg=JSON.parse(data);
  if (msg.id) {
    const request=requests.get(msg.id);
    if (request) { requests.delete(msg.id); msg.error ? request.reject(msg.error) : request.resolve(msg.result); }
  } else if (msg.method==='Runtime.exceptionThrown') errors.push(msg.params.exceptionDetails);
  else if (msg.method==='Runtime.consoleAPICalled' && msg.params.type==='error')
    errors.push(msg.params.args.map(x=>x.value ?? x.description).join(' '));
};
function send(method,params={}) {
  const id=++next;
  return new Promise((resolve,reject)=>{requests.set(id,{resolve,reject});socket.send(JSON.stringify({id,method,params}));});
}
const delay=ms=>new Promise(resolve=>setTimeout(resolve,ms));
const evaluate=async expression=>(await send('Runtime.evaluate',{expression,returnByValue:true})).result.value;
async function shot(name) {
  views.push({name,view:await evaluate('({scrollY,active:document.activeElement?.id,fullscreen:!!document.fullscreenElement,canvas:document.getElementById("canvas").getBoundingClientRect().toJSON()})')});
  const {data}=await send('Page.captureScreenshot',{format:'png'});
  await writeFile(`/tmp/gyrus-${name}.png`,Buffer.from(data,'base64'));
}
await send('Page.enable');
await send('Runtime.enable');
await send('Emulation.setDeviceMetricsOverride',{width:1040,height:820,deviceScaleFactor:1,mobile:false});
await send('Page.navigate',{url:process.env.GYRUS_URL || 'http://127.0.0.1:8785/Gyrus.html'});
let ready=false;
for(let i=0;i<100;i++) {
  if(await evaluate('document.getElementById("status")?.textContent === "Ready"')) {ready=true;break;}
  await delay(100);
}
if(!ready) throw new Error('Browser runtime did not become ready: '+JSON.stringify(errors));
await delay(300);
await shot('browser-title');
const button = await evaluate('(()=>{const r=document.getElementById("play").getBoundingClientRect();return {x:r.x+r.width/2,y:r.y+r.height/2};})()');
await send('Input.dispatchMouseEvent',{type:'mousePressed',...button,button:'left',clickCount:1});
await send('Input.dispatchMouseEvent',{type:'mouseReleased',...button,button:'left',clickCount:1});
await delay(100);
await shot('browser-player');
await delay(3100); // Original stage message gates projectile spawning for150ticks.
await evaluate(`(()=>{
  window.audioEvidence={callbacks:0,peak:0};
  const node=Module.SDL2?.audio?.scriptProcessorNode;
  if(!node)return;
  const original=node.onaudioprocess;
  node.onaudioprocess=function(event){
    original.call(this,event);
    ++audioEvidence.callbacks;
    for(const value of event.outputBuffer.getChannelData(0))
      audioEvidence.peak=Math.max(audioEvidence.peak,Math.abs(value));
  };
})()`);
await send('Input.dispatchKeyEvent',{type:'keyDown',key:'ArrowRight',code:'ArrowRight',windowsVirtualKeyCode:39});
await delay(400);
await send('Input.dispatchKeyEvent',{type:'keyUp',key:'ArrowRight',code:'ArrowRight',windowsVirtualKeyCode:39});
await shot('browser-moved');
await send('Input.dispatchKeyEvent',{type:'keyDown',key:' ',code:'Space',windowsVirtualKeyCode:32});
await delay(80);
await shot('browser-shot');
await send('Input.dispatchKeyEvent',{type:'keyUp',key:' ',code:'Space',windowsVirtualKeyCode:32});
await delay(Number(process.env.GYRUS_WAVE_WAIT_MS || 6000));
await shot('browser-wave');
const audio=await evaluate('({...audioEvidence,state:Module.SDL2?.audioContext?.state})');
if(audio.state!=='running'||!audio.callbacks||!(audio.peak>0))errors.push({audio});
console.log(JSON.stringify({ready,errors,views,audio,canvas:await evaluate('({width:Module.canvas.width,height:Module.canvas.height})'),
  screenshots:['title','player','moved','shot','wave'].map(x=>`/tmp/gyrus-browser-${x}.png`)},null,2));
await send('Page.close');
socket.close();
if(errors.length) process.exitCode=1;
