# SIC-XE-Assembler
將SIC及SIC/XE的語法做檢查，並翻成object code。

## 開發平台
Win10
<br>
<br>

## 使用開發環境
Dev C++
<br>
<br>

## 使用方式
將input檔放至當前目錄<br>
並且將Table1.table, Table2.table, Table3.table, Table4.table也放至當前目錄<br>
編譯

    SP_project2.cpp  
或

    SP_project2_v2.cpp

之後執行程式<br>
<br>
選擇要執行的選項<br>

      0. Quit
      1. SIC assembler
      2. SIC/XE assembler
      
<br>
選擇後輸入input檔名<br>
程式會產出結果於 input檔名_output.txt 檔案中<br>
<br> 
<br>


## input檔格式
SIC或SIC/XE的程式碼<br> 
<br> 
<br>

## output檔格式
由左到右的欄位分別是<br> 
Line、Loc、Source statement、Object code<br> 
Loc為根據指令大小做計算，以16進位表示
Object code則是根據指令類型及label的Loc去做計算
<br>
<br>


## 規則細節說明
<br>
以下為實作之語法內容<br>
<br>

#### SIC語法

<br>
其中{}表示可寫可不寫

    standard：<INS> <m>
    <m>： <direct>|<index>
    <direct>： <symbol>
    <index>： <symbol>, X
<br>

#### SICXE語法

<br>
其中{}表示可寫可不寫

    format1：<INS>
    format2：<INS> r1 {, r2} 
    format3：<INS> <m> 
    format4：+<INS> <m>
    <m>： <imm>|<direct>|<indirect>|<index>|<literal>
    <imm>： # <symbol> | # dec_num
    <direct>： <symbol> | address
    <indirect>： @ <symbol>
    <index>： <symbol>, X
    <literal>： = X''… | C''…
<br>

#### PSEUDO的語法

<br>
其中{}表示可寫可不寫

    {label} START hex_num
    {label} END {label}
    label EQU label | dec_num | *  四則運算限定於label與label
    {label} BYTE X''… | C''… | dec_num
    {label} WORD X''… | C''… | dec_num
    {label} LTORG
    {label} BASE dec_num | symbol
    {label} RESB dec_num
    {label} RESW dec_num
<br>

* literal hash 至 literal table，並由table依序(依table前後次序)定地址。

      = x'b1'  只用b1 hash
      = c'EOF' 只用EOF hash
      = 3277   只用3277 hash

* 此project沒有支援macro, 但有literal

* 有支援的pseudo code列表

      START/END

      EQU/ORG

      Define constant/ storage( BYTE,WORD)

      LTORG

      USE BASE register


* pseudo code中EQU沒有支援導致multi-passes的指令
