# DuckSandboxDetect
 沙箱测试,用于测评国内常见沙箱的代码
 
# 本次测评分别测评如下性能:
1. icebp指令模拟
https://key08.com/index.php/2021/05/13/1086.html:
> mov ss会产生一个异常，但是这个异常会在下一个指令执行的时候再抛出.因此 他首先mov ss了,这样下一个指令才会执行异常, 到icebp的时候, mov ss造成的异常就应该被抛出了。但是同时icebp也会造成一个异常，而且异常优先级哎比mov ss的高,你说巧不巧.这样CPU就会处理icebp的异常而不处理mov ss的异常，但是mov ss的异常是必须要求dr6的single_instruction bit位(也就叫做DR6.BS位)为1的,要不然就炸。很遗憾的是icebp的异常他不会设置这个bs位,导致直接炸虚拟机.很多时候虚拟机在2004跑起来了结果一阵子就虚拟机guest机异常了就是这个毛病

2. 时间延迟检测
> 由于经过虚拟化的vmexit阶段,沙箱的退出时间会比真实的早

3. cpuid检测
https://secret.club/2020/04/13/how-anti-cheats-detect-system-emulation.html
> 一种类似于保留 MSR 地址范围的快速方法是检查保留CPUID响应与它们通常的值。例如，40000000h是由架构标记为保留的 CPUID 叶子，最常用于 VMM 的报告功能。有两个选项可以检查无效的eax或返回相同数据的eax。

4.单步检测
https://howtohypervise.blogspot.com/2019/01/a-common-missight-in-most-hypervisors.html

# 检测结果:

腾讯哈勃
https://habo.qq.com/file/showdetail?pk=ADcGYF1vB24IOFs8U2s%3D

奇安信:
https://sandbox.ti.qianxin.com/sandbox/page/detail?type=file&id=AXprRYp1qO1C-f6P9JVe&env=&time=&sha1=0666775c8eb67fbd87c99a54cb7968084573c709

freebuf:
https://sandbox.freebuf.com/reportDetail?fileSha1=0666775c8eb67fbd87c99a54cb7968084573c709

微步在线:
https://s.threatbook.cn/report/file/6d244d4978f8b8b67aa338ee3fef23e30198fc554a3ed225fea734efa118ea59/?env=win7_sp1_enx64_office2013

virustotal:
https://www.virustotal.com/gui/file/fbdbf6716b19de8607064abaf69fb5f157be34b328dd62cbbf0d6f874d9e04d3/behavior/Microsoft%20Sysinternals%20Sysmon

其中,freebuf和微步在线的沙箱均无法分析出icebp指令之后的内容(可能是不支持这个指令),因此我多次测试了几次,
https://s.threatbook.cn/report/file/18fc971fd8b097331322c0fc815bfba26f8427a15ffb16f34ee869065d3cabc4/?env=win7_sp1_enx64_office2013

https://sandbox.freebuf.com/reportDetail?fileSha1=529fc26cc6e8c9ff4158a9b45755279502afdc9b
均崩溃或者无法继续执行:
![](https://key08.com/usr/uploads/2021/07/2918362663.png)

![](https://key08.com/usr/uploads/2021/07/315797688.png)

奇安信与VT的沙箱对于cpuid和rdtsc模拟不够好,但没有在icebp指令崩溃:
![](https://key08.com/usr/uploads/2021/07/1932825589.png)

![](https://key08.com/usr/uploads/2021/07/1626874842.png)

腾讯哈勃表现最好,只有rdtsc指令模拟不够好,其他的均不错
![](https://key08.com/usr/uploads/2021/07/2370001926.png)

# 暂时测试的结论:
freebuf = 微步在线 < 奇安信 = VirtusTotal < 腾讯哈勃
**仅仅代表本次测试结果,不排除测试不严谨导致的问题**