**NothingOS** 32-bitový víceuživatelský multitaskingový operační systém. Vycházeli jsme z toho, že nebudeme používat standardní služby BIOSu a vše si uděláme sami, což jsme dodrželi a s hardwarem počítače komunikujeme pouze pomocí portů.

Vzniklo jako **ročníková práce** (2001) na **Střední Průmyslové Škole v Rožnově pod Radhoštěm**.

Autoři **Petr Hučík** ([@tyruk](https://github.com/tyruk)) a **Alois Bělaška** ([@abelaska](https://github.com/abelaska)).

[Full documentation](https://abelaska.github.io/NothingOS)

**Trocha historie:**

Vývoj započal **14.9.2000**.

*   Verze **v1.0**, je první a byla postavena na kostře programu, který se spouštěl z MS-DOSu jako normální program, po spuštění se přepl do protected modu a nakonec zpět do real modu a DOSu.Zde se odladili rutiny pro práci s ATA zařízeními a ASCII klávesnicí a hlavně Memory Managment.  
    *   Verze **v1.1** se datuje ke dni **10.10.2000**. Tato verze již byla spouštěna ze své vlastní partition, pomocí BootLoaderu jménem [OSLO](./nos-tools/OSLO.V12/OSLO.V12). Obsahovala rutiny pro práci s diskem a klávesnicí.Tato verze byla revoluční ve svém skoku ze spouštění z příkazové řádky na zavádění pomocí OSLA, boot loaderu.  
    
*   Verze **v1.2** je ještě více revoluční než **v1.1**, protože jsme zde implementovali obrovskou spoustu věcí, jako je práce s drivery a knihovnami, které mohli být jak interní(virtuální), tak díky rutinám pro práci s diskem a s EXT2 FS i externí.
*   Verze **v1.3** je hodně rozvinutá verze **v1.2** a byla přidána detekce procesoru, přerušení pro volání systémových funkcí, byl rozvinut systém virtuálních knihoven a DELAY rutin.
*   Verze **v1.4** vznikla spontánně nedopatřením:) dne **18.2.2001**.V ní je již implementován přístup k datovým zařízením(disk,...) skrze pamět, byly položeny základy pro spouštění programů, Virtual file systému, a vše ostatní se tomu přizpůsobilo, ať už je to práce s drivery, knihovnami či celý memory managment. Dále byly přidány rutiny pro práci s CMOS.
*   Verze **v1.5** vznikla **23.3.2001**, je to již plně funkční beta verze Nothing OS, funguje již 10 konzolí, spouštění programů, vytváření zařízení, což je jen jeden krůček před zápisem do souboru, jsou hotovy první programy, interpret příkazového řádku [Nosh](./nos-programs/NOSH), program na výpis všech spuštěných procesů [Ps](./nos-programs/PS), a program na vytvoření zařízení [Mknod](./nos-programs/MKNOD)

**Enjoy!**