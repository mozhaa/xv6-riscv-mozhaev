.data
buf: .space 32
input_error_message: .asciz "Input error!\n"
buffer_overflow_message: .asciz "Buffer overflow!\n"
invalid_format_message: .asciz "Invalid format!\n"
format_string: .asciz "%d\n"

.text
.global main

main:
    // gets(buf, 32);
    la a0, buf 
    li a1, 32
    call gets

    // if (ret_value == 0) -> input_error
    beq ra, zero, input_error_failure 

    // if (ret_value - buf > BUF_SIZE) -> buffer_overflow
    addi t1, a0, 32
    blt t1, ra, buffer_overflow_failure 

    la a0, buf
    call atoi

    mv t3, ra

    la t1, buf

// loop through all numbers in the beginning of the buffer
sprint_numbers: 
    mv t0, t1
    li t5, 48
    li a0, 58
    blt t0, t5, continue_second_number // < '0'
    bge t0, a0, continue_second_number // > '9'
    addi t0, t0, 1
    beq zero, zero, sprint_numbers

continue_second_number:

    la a0, t1
    call atoi

    mv t4, ra

    la a0, format_string
    ld a1, t4
    call printf

    // exit(0);
    li a7, 2 
    li a0, 0
    ecall

input_error_failure:
    la a0, input_error_message
    call printf

    li a7, 2 // exit(2);
    li a0, 2
    ecall

buffer_overflow_failure:
    la a0, buffer_overflow_message
    call printf

    li a7, 2 // exit(1);
    li a0, 1
    ecall

invalid_format_failure:
    la a0, invalid_format_message
    call printf

    li a7, 2 // exit(3);
    li a0, 3
    ecall