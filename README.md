# Z80Assembler

# lldb

 lldb ./a.out token\_txt.txt

## command

### 開始

 (lldb) run

### breakpoint

関数名を登録

 (lldb) breakpoint set --name nextTkn()

### local variables

現在のスコープでローカル変数を参照する

 (lldb) watchpoint set variable i

### rerun etc

nextはステップオーバー、stepはステップイン

 (lldb) n
 (lldb) next
 (lldb) s
 (lldb) step

## ref

* [gdb-lldb対比表](http://lldb.llvm.org/lldb-gdb.html)

