.text               # 代码段
main:
    # 加载立即数到寄存器
    li $t0, 10       # $t0 = 10
    li $t1, 5        # $t1 = 5

    # 乘法
    mult $t0, $t1    # $t0 * $t1, 结果在HI和LO寄存器中
    mflo $t2         # 获取乘法结果的低32位

    # 除法
    div $t0, $t1     # $t0 / $t1
    mflo $t3         # 获取商
    mfhi $t4         # 获取余数

    # 使用nor指令
    nor $t5, $t0, $t1 # $t5 = ~($t0 | $t1)

    # 使用slt指令
    slt $t6, $t0, $t1 # 如果$t0 < $t1, $t6 = 1, 否则 $t6 = 0

    # 结束程序
    li $v0, 10       # 加载退出系统调用的代码
    syscall          # 系统调用退出
